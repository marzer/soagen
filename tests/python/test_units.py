#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

"""
Fast, deterministic unit tests for soagen's pure helpers and config validation. These need neither a C++
toolchain nor network access, so they run across the full python matrix as the project's primary safety net.
"""

from pathlib import Path

import pytest

from soagen import cpp, preprocessor, utils
from soagen.config import Config
from soagen.errors import Error
from soagen.main import check_for_output_collisions
from soagen.schemas import FixedArrayOf, SchemaError, Stripped, ValueOrArray
from soagen.version import VERSION, VERSION_STRING

# ----------------------------------------------------------------------------------------------------------------------
# version
# ----------------------------------------------------------------------------------------------------------------------


def test_version_is_a_three_int_tuple():
    assert isinstance(VERSION, tuple)
    assert len(VERSION) == 3
    assert all(isinstance(v, int) and v >= 0 for v in VERSION)


def test_version_string_matches_tuple():
    assert VERSION_STRING == '.'.join(str(v) for v in VERSION)


# ----------------------------------------------------------------------------------------------------------------------
# cpp.is_valid_identifier
# ----------------------------------------------------------------------------------------------------------------------


@pytest.mark.parametrize('name', ['foo', 'Bar9', 'a_b_c', 'x1'])
def test_valid_identifiers_are_accepted(name):
    assert cpp.is_valid_identifier(name)[0] is True


@pytest.mark.parametrize(
    'name',
    [
        '',  # blank
        '_leading',  # leading underscore
        'a__b',  # double underscore
        '9abc',  # leading digit
        'a-b',  # illegal character
        'class',  # C++ keyword
        'int',  # C++ keyword
    ],
)
def test_invalid_identifiers_are_rejected(name):
    valid, *rest = cpp.is_valid_identifier(name)
    assert valid is False
    assert rest and isinstance(rest[0], str) and rest[0]


# ----------------------------------------------------------------------------------------------------------------------
# cpp includes helpers
# ----------------------------------------------------------------------------------------------------------------------


def test_remove_implicit_includes_drops_implicit_and_keeps_others():
    out = cpp.remove_implicit_includes(['cstdint', 'memory', 'my/header.hpp', 'type_traits'])
    assert out == ['my/header.hpp']


def test_detect_includes_handles_empty_input():
    assert cpp.detect_includes('') == []
    assert cpp.detect_includes(None) == []


def test_detect_includes_maps_std_symbols_to_headers():
    assert 'cstddef' in cpp.detect_includes('std::size_t n = 0;')
    assert 'limits' in cpp.detect_includes('auto x = std::numeric_limits<int>::max();')


# ----------------------------------------------------------------------------------------------------------------------
# utils
# ----------------------------------------------------------------------------------------------------------------------


def test_make_divider_pads_to_line_length():
    line = utils.make_divider('section', line_length=120)
    assert line.startswith('//')
    assert len(line) == 120


def test_make_divider_does_not_truncate_long_text():
    text = 'x' * 200
    line = utils.make_divider(text, line_length=120)
    assert text in line


def test_write_file_is_write_if_changed_ignoring_line_endings_and_bom(tmp_path):
    p = tmp_path / 'out.txt'
    assert utils.write_file(p, 'a\nb\nc\n') is True  # first write
    assert utils.write_file(p, 'a\nb\nc\n') is False  # identical -> no rewrite
    p.write_bytes(b'a\r\nb\r\nc\r\n')  # a CRLF checkout (git autocrlf on Windows)
    assert utils.write_file(p, 'a\nb\nc\n') is False  # logically unchanged -> no rewrite
    p.write_bytes(b'\xef\xbb\xbf' + b'a\nb\nc\n')  # a BOM-prefixed copy
    assert utils.write_file(p, 'a\nb\nc\n') is False
    assert utils.write_file(p, 'a\nb\nc\nd\n') is True  # a genuine change still rewrites


# ----------------------------------------------------------------------------------------------------------------------
# schema validators
# ----------------------------------------------------------------------------------------------------------------------


def test_stripped_trims_whitespace():
    assert Stripped(str).validate('  hello  ') == 'hello'


def test_stripped_rejects_empty_when_disallowed():
    with pytest.raises(SchemaError):
        Stripped(str, allow_empty=False, name='thing').validate('   ')


def test_value_or_array_wraps_scalars():
    assert ValueOrArray(str).validate('x') == ['x']
    assert ValueOrArray(str).validate(['a', 'b']) == ['a', 'b']


def test_fixed_array_of_enforces_length():
    assert FixedArrayOf(int, 2).validate([1, 2]) == [1, 2]
    with pytest.raises(SchemaError):
        FixedArrayOf(int, 2).validate([1, 2, 3])


# ----------------------------------------------------------------------------------------------------------------------
# Config validation (regression guards for the schema-default crash and the new structural validation)
# ----------------------------------------------------------------------------------------------------------------------


def _config(tmp_path: Path, body: str) -> Config:
    toml = tmp_path / 'soa.toml'
    toml.write_text(body, encoding='utf-8')
    return Config(toml)


def test_empty_config_is_rejected_cleanly(tmp_path):
    # a config with no structs has nothing to generate: it must raise a clean SchemaError, not crash.
    with pytest.raises(SchemaError):
        _config(tmp_path, '')


def test_minimal_struct_builds_expected_columns(tmp_path):
    cfg = _config(
        tmp_path,
        "namespace = 'demo'\n[structs.point]\nvariables = [ {name='x', type='float'}, {name='y', type='float'} ]\n",
    )
    assert len(cfg.structs) == 1
    struct = cfg.structs[0]
    assert struct.name == 'point'
    assert [c.name for c in struct.columns] == ['x', 'y']


def test_duplicate_variable_name_is_rejected(tmp_path):
    with pytest.raises(SchemaError):
        _config(tmp_path, "[structs.foo]\nvariables = [ {name='a', type='int'}, {name='a', type='float'} ]\n")


def test_struct_without_variables_is_rejected(tmp_path):
    with pytest.raises(SchemaError):
        _config(tmp_path, '[structs.foo]\nvariables = []\n')


def test_defaults_are_not_shared_between_structs(tmp_path):
    # guards the shared-mutable-default aliasing bug: two structs that both omit `attributes` must each
    # get their own list, not a single aliased one that accumulates SOAGEN_EMPTY_BASES twice.
    cfg = _config(
        tmp_path,
        "[structs.a]\nvariables = [ {name='x', type='int'} ]\n[structs.b]\nvariables = [ {name='y', type='int'} ]\n",
    )
    a, b = cfg.structs
    assert a.attributes is not b.attributes
    assert a.attributes.count('SOAGEN_EMPTY_BASES') == 1
    assert b.attributes.count('SOAGEN_EMPTY_BASES') == 1


def test_output_defaults_to_the_toml_directory(tmp_path):
    cfg = _config(tmp_path, "[structs.point]\nvariables = [ {name='x', type='float'} ]\n")
    assert cfg.hpp[0].path.parent == tmp_path.resolve()
    assert cfg.hpp[0].path.name == 'soa.hpp'
    assert cfg.natvis.path.parent == tmp_path.resolve()
    assert cfg.natvis.path.name == 'soa.natvis'


def test_output_dir_redirects_generated_file_paths(tmp_path):
    # build-system integration: outputs go to an explicit (possibly ephemeral) directory, not next to the toml.
    out = tmp_path / 'build' / 'generated'
    toml = tmp_path / 'soa.toml'
    toml.write_text("[structs.point]\nvariables = [ {name='x', type='float'} ]\n", encoding='utf-8')
    cfg = Config(toml, output_dir=out)
    assert cfg.hpp[0].path.parent == out.resolve()
    assert cfg.hpp[0].path.name == 'soa.hpp'
    assert cfg.natvis.path.parent == out.resolve()
    assert cfg.natvis.path.name == 'soa.natvis'


def test_same_stem_configs_under_one_output_dir_collide(tmp_path):
    # the -o clobber: two like-named tomls in different dirs map to the same <out>/foo.hpp and would silently overwrite.
    out = tmp_path / 'out'
    (tmp_path / 'a').mkdir()
    (tmp_path / 'b').mkdir()
    (tmp_path / 'a' / 'foo.toml').write_text("[structs.x]\nvariables = [ {name='v', type='int'} ]\n", encoding='utf-8')
    (tmp_path / 'b' / 'foo.toml').write_text("[structs.y]\nvariables = [ {name='w', type='int'} ]\n", encoding='utf-8')
    configs = [Config(tmp_path / 'a' / 'foo.toml', output_dir=out), Config(tmp_path / 'b' / 'foo.toml', output_dir=out)]
    with pytest.raises(Error):
        check_for_output_collisions(configs)


def test_case_only_struct_names_collide_in_non_combined_mode(tmp_path):
    # non-combined mode names each header after its struct; Mesh.hpp and mesh.hpp collide on case-insensitive filesystems.
    toml = tmp_path / 'soa.toml'
    toml.write_text(
        "namespace = 'demo'\n[hpp]\ncombined = false\n"
        "[structs.Mesh]\nvariables = [ {name='v', type='int'} ]\n"
        "[structs.mesh]\nvariables = [ {name='w', type='int'} ]\n",
        encoding='utf-8',
    )
    with pytest.raises(Error):
        check_for_output_collisions([Config(toml)])


def test_distinct_outputs_do_not_collide(tmp_path):
    a = tmp_path / 'a.toml'
    b = tmp_path / 'b.toml'
    a.write_text("namespace = 'demo'\n[structs.x]\nvariables = [ {name='v', type='int'} ]\n", encoding='utf-8')
    b.write_text("namespace = 'demo'\n[structs.y]\nvariables = [ {name='w', type='int'} ]\n", encoding='utf-8')
    check_for_output_collisions([Config(a), Config(b)])


# ----------------------------------------------------------------------------------------------------------------------
# single-header macro hygiene (#undef post-process)
# ----------------------------------------------------------------------------------------------------------------------


def test_defined_macros_finds_object_and_function_macros():
    text = '#define FOO\n#define BAR(x) x\n  #  define BAZ 1\nint not_a_macro;\n'
    assert preprocessor.defined_macros(text) == {'FOO', 'BAR', 'BAZ'}


def test_referenced_soagen_macros_matches_both_cases_and_ignores_double_underscore():
    text = 'use SOAGEN_COLUMN and soagen_aligned_storage but not __SOAGEN_UNDEFS or UNRELATED'
    assert preprocessor.referenced_soagen_macros(text) == {'SOAGEN_COLUMN', 'soagen_aligned_storage'}


def test_internal_macros_undefs_only_unkept_soagen_names_sorted():
    text = '#define SOAGEN_PUBLIC\n#define SOAGEN_INTERNAL\n#define soagen_helper 0\n#define OTHER_MACRO 1\n'
    assert preprocessor.internal_macros(text, keep={'SOAGEN_PUBLIC'}) == ['SOAGEN_INTERNAL', 'soagen_helper']


def test_internal_macros_keeps_transitive_dependencies():
    text = '#define SOAGEN_PUBLIC SOAGEN_HELPER(1)\n#define SOAGEN_HELPER(x) x\n#define SOAGEN_UNUSED 1\n'
    assert preprocessor.internal_macros(text, keep={'SOAGEN_PUBLIC'}) == ['SOAGEN_UNUSED']


def test_internal_macros_unions_dependencies_across_redefinitions():
    # a macro redefined per-compiler must keep every branch's references, not just the last definition's
    text = (
        '#if A\n#define SOAGEN_PUBLIC SOAGEN_LEFT\n'
        '#elif B\n#define SOAGEN_PUBLIC SOAGEN_RIGHT\n'
        '#else\n#define SOAGEN_PUBLIC\n#endif\n'
        '#define SOAGEN_LEFT 1\n#define SOAGEN_RIGHT 2\n'
    )
    assert preprocessor.internal_macros(text, keep={'SOAGEN_PUBLIC'}) == []


def test_internal_macros_keeps_token_paste_prefix_family():
    text = (
        '#define SOAGEN_PUBLIC(v) SOAGEN_CONCAT(SOAGEN_IMPL_, v)\n'
        '#define SOAGEN_CONCAT(a, b) a##b\n'
        '#define SOAGEN_IMPL_9 9\n'
        '#define SOAGEN_IMPL_10 10\n'
        '#define SOAGEN_UNUSED 1\n'
    )
    assert preprocessor.internal_macros(text, keep={'SOAGEN_PUBLIC'}) == ['SOAGEN_UNUSED']
