#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from io import StringIO
import re

from . import cpp, log, utils
from .column import *
from .configurable import *
from .includes import *
from .metavars import *
from .schemas import *
from .type_list import *
from .variable import *
from .writer import *

NEWLINE = '\n'
TAB = '\t'
TAB_SPACES = '    '


class Struct(Configurable):
    __schema = Schema(
        {
            Optional(r'allocator', default=''): Stripped(str),
            Optional(r'annotations', default=[]): And(
                ValueOrArray(str, name=r'annotations'),
                Use(lambda x: utils.remove_duplicates([s.strip() for s in x if s.strip()])),
            ),
            Optional(r'attributes', default=[]): And(
                ValueOrArray(str, name=r'attributes'),
                Use(lambda x: utils.remove_duplicates([s.strip() for s in x if s.strip()])),
            ),
            Optional(r'brief', default=''): Stripped(str),
            Optional(r'copyable', default=True): bool,
            Optional(r'default_constructible', default=True): Or(bool, 'auto'),
            Optional(r'details', default=''): Stripped(str),
            Optional(r'epilogue', default=''): Stripped(str),
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'includes', default=dict): {object: object},
            Optional(r'mixins', default=[]): And(
                ValueOrArray(str, name=r'mixins'),
                Use(lambda x: utils.remove_duplicates([s.strip() for s in x if s.strip()])),
            ),
            Optional(r'movable', default=True): bool,
            Optional(r'prologue', default=''): Stripped(str),
            Optional(r'reverse_iterators', default=False): bool,
            Optional(r'static_variables', default=list): [object],
            Optional(r'swappable', default=True): bool,
            Optional(r'variables', default=list): [object],
        }
    )

    def __init__(self, cfg, name, vals):
        super().__init__(cfg)

        vals = Struct.__schema.validate(vals)
        self.__dict__.update(vals)

        self.name = name
        valid = cpp.is_valid_identifier(self.name)
        if not valid[0]:
            raise SchemaError(rf"name {valid[1]}", None)
        self.type = name

        if self.reverse_iterators:
            log.w(rf'{current_schema_context()}reverse_iterators: not currently implemented')
            self.reverse_iterators = False

        self.qualified_type = rf'{self.config.namespace}::{self.type}' if self.config.namespace else self.type
        self.qualified_name = self.qualified_type
        self.index = -1  # set by the config

        self.meta = MetaVars()
        self.meta.push(r'name', self.name)
        self.meta.push(r'type', self.type)
        self.meta.push(r'qualified_name', self.qualified_name)
        self.meta.push(r'qualified_type', self.qualified_type)
        self.meta.push(r'struct::name', self.name)
        self.meta.push(r'struct::type', self.type)
        self.meta.push(r'struct::qualified_name', self.qualified_name)
        self.meta.push(r'struct::qualified_type', self.qualified_type)
        self.meta.push(r'struct::scope', '')
        self.meta.push(r'struct::row_type', r'row_type')
        self.meta.push(r'struct::lvalue_row_type', r'row_type')
        self.meta.push(r'struct::rvalue_row_type', r'rvalue_row_type')
        self.meta.push(r'struct::const_row_type', r'const_row_type')
        self.meta.push(r'struct::span_type', r'span_type')
        self.meta.push(r'struct::lvalue_span_type', r'span_type')
        self.meta.push(r'struct::rvalue_span_type', r'rvalue_span_type')
        self.meta.push(r'struct::const_span_type', r'const_span_type')

        if not self.allocator:
            self.allocator = self.config.allocator
        if self.allocator == r'std::allocator':
            self.allocator = r'std::allocator<std::byte>'

        self.annotations = utils.remove_duplicates(self.config.all_structs.annotations + self.annotations)
        self.attributes = utils.remove_duplicates(self.config.all_structs.attributes + self.attributes)
        if r'SOAGEN_EMPTY_BASES' not in self.attributes:
            self.attributes.append(r'SOAGEN_EMPTY_BASES')
        if r'__declspec(empty_bases)' in self.attributes:
            self.attributes.remove(r'__declspec(empty_bases)')

        self.mixins = utils.remove_duplicates(
            [
                rf'soagen::mixins::size_and_capacity',
                rf'soagen::mixins::resizable',
                rf'soagen::mixins::equality_comparable',
                rf'soagen::mixins::less_than_comparable',
                rf'soagen::mixins::data_ptr',
                rf'soagen::mixins::columns',
                rf'soagen::mixins::rows',
                rf'soagen::mixins::iterators',
                rf'soagen::mixins::spans',
            ]
            + ([rf'soagen::mixins::swappable'] if self.swappable else [])
            + self.mixins
        )

        with SchemaContext('includes'):
            self.includes = Includes(cfg, self.includes)

        has_defaults = False
        for i in range(len(self.variables)):
            with SchemaContext(
                rf"variable '{self.variables[i]['name']}'" if r'name' in self.variables[i] else rf"variable [{i}]"
            ):
                v = Variable(self, self.variables[i])
                self.variables[i] = v
                if v.name == self.name:
                    raise SchemaError(rf"name: may not be the same as the struct", None)

                self.meta.push(rf'{v.name}::type', v.type)
                self.meta.push(rf'{v.name}::default', v.default if v.default else '{}')
                has_default = bool(v.default)
                if not has_default and has_defaults:
                    log.w(rf'{current_schema_context()}has no default but previous variables do')
                has_defaults = has_defaults or has_default

        self.columns = []
        index = 0
        for v in self.variables:
            v.index = index
            index += 1
            self.columns += v.columns

        index = 0
        for c in self.columns:
            c.index = index
            index += 1
        self.column_indices = ", ".join([str(col.index) for col in self.columns])

        self.prologue = rf'''
        {self.config.all_structs.prologue}

        {self.prologue}
        '''.strip()

        self.header = rf'''
        {self.config.all_structs.header}

        {self.header}
        '''.strip()

        self.footer = rf'''
        {self.config.all_structs.footer}

        {self.footer}
        '''.strip()

        self.epilogue = rf'''
        {self.config.all_structs.epilogue}

        {self.epilogue}
        '''.strip()

        if not self.brief:
            self.brief = name
        self.details += rf'''

        @remark Models the <a href="https://en.wikipedia.org/wiki/AoS_and_SoA">Structure-of-arrays</a>
                equivalent of: @code{{.cpp}}
        struct {self.name}
        {{'''
        for col in self.columns:
            self.details += f'\n{{% tab %}}{col.type} {col.name};'
        self.details += '''
        };
        @endcode
        '''

        static_vars = [v for v in self.config.all_structs.static_variables]
        for i in range(len(self.static_variables)):
            with SchemaContext(
                rf"static variable '{self.static_variables[i]['name']}'"
                if r'name' in self.static_variables[i]
                else rf"static variable [{i}]"
            ):
                static_vars.append(StaticVariable(self.config, self.static_variables[i]))
        static_vars = [(v, v.access) for v in static_vars]
        self.static_variables = {'public': [], 'protected': [], 'private': []}
        for v, access in static_vars:
            self.static_variables[access].append(v)
        assert len(self.static_variables) == 3

    def set_index(self, index):
        assert self.index == -1
        assert isinstance(index, int)
        assert index >= 0
        self.index = index
        self.meta.push('index', index)
        self.meta.push('struct::index', index)

    def write_forward_declarations(self, o: Writer):
        with MetaScope(self):
            o(rf'class {self.type};')

    def write_impl_namespace(self, o: Writer):
        with MetaScope(self):
            namespace = rf'soagen_struct_impl::{self.config.namespace}::{self.name}'
            namespace = namespace.replace(r':', r'_').strip(r'_')
            namespace = re.sub(r'__+', r'_', namespace)
            self.global_impl_namespace = namespace
            with Namespace(o, self.global_impl_namespace):
                if self.config.namespace:
                    o(r'SOAGEN_DISABLE_WARNINGS;')
                    o(rf'using namespace {self.config.namespace};')
                    o(r'SOAGEN_ENABLE_WARNINGS;')
                    o()
                max_length = 0
                for col in self.columns:
                    max_length = max(len(col.name), max_length)
                left_padding = max(0, 32 - (max_length + 6)) * ' '
                with StringIO() as buf:
                    buf.write('soagen::table_traits<\n')
                    for i in range(len(self.columns)):
                        if i:
                            buf.write(f',\n')
                        col = self.columns[i]
                        buf.write(f'{left_padding}/* {col.name:>{max_length}} */ soagen::column_traits<{col.type}')
                        if col.alignment > 0:
                            buf.write(rf', soagen::max(std::size_t{{ {col.alignment}u }}, alignof({col.type}))')
                        if col.param_type:
                            if not col.param_type:
                                buf.write(rf', alignof({col.type})')
                            buf.write(rf', {col.param_type}')
                        buf.write(rf'>')
                    buf.write(rf'>')
                    o(rf'using soagen_table_traits_type = {buf.getvalue()};')
                o()
                o(rf'using soagen_allocator_type = {self.allocator};')

    def write_soagen_detail_specializations(self, o: Writer):
        with MetaScope(self):
            for col in self.columns:
                o(rf'SOAGEN_MAKE_NAMED_COLUMN({self.qualified_name}, {col.index}, {col.name});')
            o(
                rf'''
            template <>
            struct is_soa_<{self.qualified_name}> : std::true_type
            {{}};

            template <>
            struct table_traits_type_<{self.qualified_name}>
            {{
                using type = {self.global_impl_namespace}::soagen_table_traits_type;
            }};

            template <>
            struct allocator_type_<{self.qualified_name}>
            {{
                using type = {self.global_impl_namespace}::soagen_allocator_type;
            }};

            template <>
            struct table_type_<{self.qualified_name}>
            {{
                using type = table<table_traits_type<{self.qualified_name}>, allocator_type<{self.qualified_name}>>;
            }};
            '''
            )

    def write_class_definition(self, o: Writer):
        with MetaScope(self):
            if self.prologue:
                o(
                    f'''
                {self.prologue}
                '''
                )

            def doxygen(s: str) -> str:
                nonlocal o
                if not o.doxygen:
                    return ''
                s = [x.rstrip() for x in s.split('\n')]
                popped_start = 0
                while len(s) and not s[0]:
                    s.pop(0)
                    popped_start += 1
                leading_spaces = None
                for i in range(len(s)):
                    if not s[i]:
                        continue
                    lstripped = s[i].lstrip()
                    if s[i] != lstripped:
                        leading_spaces = min(
                            len(s[i]) - len(lstripped), 999999 if leading_spaces is None else leading_spaces
                        )
                    s[i] = lstripped
                leading_spaces = ' ' * leading_spaces if leading_spaces is not None else ''
                s = f'{popped_start*NEWLINE}{leading_spaces}/// {rf"{NEWLINE}{leading_spaces}/// ".join(s)}'
                return s

            o(
                doxygen(
                    rf'''
                @brief {self.brief}

                @details {self.details}

                @note The code and documentation for this class were generated by soagen - https://marzer.github.io/soagen'''
                )
            )
            for annotation in self.annotations:
                o(annotation)
            with ClassDefinition(
                o,
                f'class {" ".join(self.attributes)} {self.name}',
                hidden_base_classes=[rf'public {m}<{self.name}>' for m in self.mixins],
            ):
                with Public(o):
                    o(
                        rf'''
                    {doxygen(r"@brief The unsigned integer size type used by this class.")}
                    using size_type = std::size_t;

                    {doxygen(r"@brief The signed integer difference type used by this class.")}
                    using difference_type = std::ptrdiff_t;

                    {doxygen(r"@brief The allocator type used by this class.")}
                    using allocator_type = soagen::allocator_type<{self.name}>;

                    {doxygen(r"@brief This class's underlying soagen::table type.")}
                    using table_type = soagen::table_type<{self.name}>;

                    {doxygen(r"@brief The soagen::table_traits for the underlying table.")}
                    using table_traits = soagen::table_traits_type<{self.name}>;

                    {doxygen(r"@brief The number of columns in the table.")}
                    static constexpr size_type column_count = table_traits::column_count;

                    {doxygen(r"@brief Gets the soagen::column_traits for a specific column of the table.")}
                    template <auto Column>
                    using column_traits = POXY_IMPLEMENTATION_DETAIL(typename table_traits::template column<static_cast<size_type>(Column)>);

                    {doxygen(r"@brief Gets the type of a specific column in the table.")}
                    template <auto Column>
                    using column_type = POXY_IMPLEMENTATION_DETAIL(typename column_traits<static_cast<size_type>(Column)>::value_type);

                    {doxygen(r"@brief Row iterators returned by iterator functions.")}
                    using iterator = soagen::iterator_type<{self.name}>;

                    {doxygen(r"@brief Row iterators returned by rvalue-qualified iterator functions.")}
                    using rvalue_iterator = soagen::rvalue_iterator_type<{self.name}>;

                    {doxygen(r"@brief Row iterators returned by const-qualified iterator functions.")}
                    using const_iterator = soagen::const_iterator_type<{self.name}>;

                    {doxygen(r"@brief Regular (lvalue-qualified) span type.")}
                    using span_type = soagen::span_type<{self.name}>;

                    {doxygen(r"@brief Rvalue-qualified span type.")}
                    using rvalue_span_type = soagen::rvalue_span_type<{self.name}>;

                    {doxygen(r"@brief Const-qualified span type.")}
                    using const_span_type = soagen::const_span_type<{self.name}>;
                    '''
                    )

                    if self.reverse_iterators:
                        o(
                            rf'''
                        {doxygen(r"@brief Reverse lvalue row iterator.")}
                        using reverse_iterator = std::reverse_iterator<iterator>;

                        {doxygen(r"@brief Reverse rvalue row iterator.")}
                        using rvalue_reverse_iterator = std::reverse_iterator<rvalue_iterator>;

                        {doxygen(r"@brief Reverse const lvalue row iterator.")}
                        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
                        '''
                        )

                    o(
                        rf'''
                    {doxygen(r"@brief Regular (lvalue-qualified) row type used by this class.")}
                    using row_type = soagen::row_type<{self.name}>;

                    {doxygen(r"@brief Rvalue row type used by this class.")}
                    using rvalue_row_type = soagen::rvalue_row_type<{self.name}>;

                    {doxygen(r"@brief Const row type used by this class.")}
                    using const_row_type = soagen::const_row_type<{self.name}>;

                    {doxygen(r"""
                    @brief   The number of rows to advance to maintain the requested `alignment` for every column.

                    @details The stride size you need to use when iterating through rows of this table such that
                             the starting element for each batch in each column would have the same memory alignment as the
                             value specified for the column-specific `alignment`.

                    @note    Typically you can ignore this; column elements are always aligned correctly according to their
                             type. This is for over-alignment scenarios where you need to do things in batches (e.g. SIMD).""")}
                    static constexpr size_type aligned_stride = table_traits::aligned_stride;

                    '''
                    )

                    if o.doxygen:
                        o(
                            rf'''
                         #if SOAGEN_DOXYGEN
                        /// @brief Named index constants for all of the columns in the table.
                        using columns = POXY_IMPLEMENTATION_DETAIL(struct dummy_t);
                        #else'''
                        )

                    with ClassDefinition(o, 'enum class columns : size_type'):
                        for col in self.columns:
                            o(rf'{col.name} = {col.index},')

                    if o.doxygen:
                        o('#endif')

                    o(
                        rf'''

                    {doxygen(r"@brief Gets the name of the specified column as a null-terminated string.")}
                    template <auto Column> static constexpr auto& column_name = soagen::detail::column_name<{self.name}, static_cast<size_type>(Column)>::value;

                    {self.header}

                    '''
                    )

                for access in (r'public', r'protected', r'private'):
                    if self.static_variables[access]:
                        o()
                        o.cpp_access_level = access
                        for var in self.static_variables[access]:
                            var.write(o)

                with Private(o):
                    o(
                        rf'''
                    table_type table_;
                    '''
                    )

                with Public(o):
                    ctor_attrs = 'SOAGEN_NODISCARD_CTOR'

                    if isinstance(self.default_constructible, bool) or self.default_constructible != 'auto':
                        o(
                            rf'''
                        {doxygen(r"@brief Default constructor.")}
                        {ctor_attrs if self.default_constructible else ""} {self.name}() = {"default" if self.default_constructible else "delete"};
                        '''
                        )

                    o(
                        rf'''
                    {doxygen(r"@brief Move constructor.")}
                    {ctor_attrs if self.movable else ""} {self.name}({self.name}&&) = {"default" if self.movable else "delete"};

                    {doxygen(r"@brief Move-assignment operator.")}
                    {self.name}& operator=({self.name}&&) = {"default" if self.movable else "delete"};

                    {doxygen(r"@brief Copy constructor.")}
                    {ctor_attrs if self.copyable else ""} {self.name}(const {self.name}&) = {"default" if self.copyable else "delete"};

                    {doxygen(r"@brief Copy-assignment operator.")}
                    {self.name}& operator=(const {self.name}&) = {"default" if self.copyable else "delete"};

                    {doxygen(r"@brief Destructor.")}
                    ~{self.name}() = default;

                    {doxygen(r"@brief Constructs with the given allocator.")}
                    {ctor_attrs}
                    constexpr explicit {self.name}(const allocator_type& alloc) noexcept //
                        : table_{{ alloc }}
                    {{}}

                    {doxygen(r"@brief Constructs with the given allocator.")}
                    {ctor_attrs}
                    constexpr explicit {self.name}(allocator_type&& alloc) noexcept //
                        : table_{{ static_cast<allocator_type&&>(alloc) }}
                    {{}}

                    {doxygen(r"@brief Returns the allocator being used by the table.")}
                    SOAGEN_INLINE_GETTER
                    SOAGEN_CPP20_CONSTEXPR
                    allocator_type get_allocator() const noexcept
                    {{
                        return table_.get_allocator();
                    }}

                    '''
                    )

                # note: most of the doxygen member groups here are based on those from the cppreference.com page for
                # std::vector: https://en.cppreference.com/w/cpp/container/vector

                with DoxygenMemberGroup(o, 'Underlying table'):
                    with Public(o):
                        o(
                            rf'''
                        {doxygen(r"@brief Returns an lvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr table_type& table() & noexcept
                        {{
                            return table_;
                        }}

                        {doxygen(r"@brief Returns an rvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr table_type&& table() && noexcept
                        {{
                            return static_cast<table_type&&>(table_);
                        }}

                        {doxygen(r"@brief Returns a const lvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr const table_type& table() const & noexcept
                        {{
                            return table_;
                        }}

                        {doxygen(r"@brief Returns an lvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        explicit constexpr operator table_type& () noexcept
                        {{
                            return table_;
                        }}

                        {doxygen(r"@brief Returns an rvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        explicit constexpr operator table_type&& () noexcept
                        {{
                            return static_cast<table_type&&>(table_);
                        }}

                        {doxygen(r"@brief Returns a const lvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        explicit constexpr operator const table_type& () const noexcept
                        {{
                            return table_;
                        }}
                        '''
                        )

                with DoxygenMemberGroup(o, 'Modifiers'):
                    with Public(o):
                        o(
                            rf'''
                        {doxygen(r"""
                        @brief Erases the row at the given position.

                        @availability This method is only available when all the column types are move-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_erase_member<table_type>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR //
                        SOAGEN_ENABLE_IF_T({self.name}&, sfinae) erase(size_type pos) //
                            noexcept(soagen::has_nothrow_erase_member<table_type>) //
                        {{
                            table_.erase(pos);
                            return *this;
                        }}

                        {doxygen(r"""
                        @brief	Erases the row at the given position without preserving order.

                        @details	This is much faster than #erase() because it uses the swap-and-pop idiom:
                                    Instead of shifting all the higher rows downward, the last row is moved into the
                                    position of the erased one and the size of the table is reduced by 1.

                        @note		If you are tracking row indices in some other place and need to maintain that invariant,
                                    you can use the return value to update your data accordingly.

                        @returns	The position of the row that was moved into the erased row's position, if any.

                        @availability This method is only available when all the column types are move-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_unordered_erase_member<table_type>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR //
                        SOAGEN_ENABLE_IF_T(soagen::optional<size_type>, sfinae) unordered_erase(size_type pos) //
                            noexcept(soagen::has_nothrow_unordered_erase_member<table_type>) //
                        {{
                            return table_.unordered_erase(pos);
                        }}
                        '''
                        )

                        for const in ('', 'const_'):
                            o(
                                rf'''
                                {doxygen(rf"""
                                @brief Erases the row at the given iterator.

                                @returns    An iterator to the row immediately following the one which was removed,
                                            or #{"c" if const else ""}end() if the one removed was the last row in the table.

                                @availability This method is only available when all the column types are move-assignable.""")}
                                SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_erase_member<table_type>)
                                SOAGEN_ALWAYS_INLINE
                                SOAGEN_CPP20_CONSTEXPR
                                SOAGEN_ENABLE_IF_T({const}iterator, sfinae) erase({const}iterator pos) //
                                    noexcept(soagen::has_nothrow_erase_member<table_type>) //
                                {{
                                    table_.erase(static_cast<size_type>(pos));
                                    return pos;
                                }}

                                {doxygen(r"""
                                @brief	Erases the row at the given position without preserving order.

                                @details	This is much faster than #erase() because it uses the swap-and-pop idiom:
                                            Instead of shifting all the higher rows downward, the last row is moved into the
                                            position of the erased one and the size of the table is reduced by 1.

                                @note		If you are tracking row indices in some other place and need to maintain that invariant,
                                            you can use the return value to update your data accordingly.

                                @returns	The position of the row that was moved into the erased row's position, if any.

                                @availability This method is only available when all the column types are move-assignable.""")}
                                SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = soagen::has_unordered_erase_member<table_type>)
                                SOAGEN_ALWAYS_INLINE
                                SOAGEN_CPP20_CONSTEXPR
                                SOAGEN_ENABLE_IF_T(soagen::optional<{const}iterator>, sfinae) unordered_erase({const}iterator pos) //
                                    noexcept(soagen::has_nothrow_unordered_erase_member<table_type>) //
                                {{
                                    if (auto moved_pos = table_.unordered_erase(static_cast<size_type>(pos)); moved_pos)
                                        return {const}iterator{{ *this, static_cast<difference_type>(*moved_pos) }};
                                    return {{}};
                                }}
                            '''
                            )

                        o(
                            rf'''
                        {doxygen(r"""
                        @brief Swaps two columns.

                        @availability The two columns must have the same underlying value_type.""")}
                        template <auto A, auto B>
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& swap_columns() //
                            noexcept(noexcept(std::declval<table_type&>().template swap_columns<static_cast<size_type>(A), static_cast<size_type>(B)>()))
                        {{
                            table_.template swap_columns<static_cast<size_type>(A), static_cast<size_type>(B)>();
                            return *this;
                        }}

                        #if SOAGEN_DOXYGEN

                        {doxygen(r"@brief Removes the last row(s) from the table.")}
                        {self.name}& pop_back(size_type num = 1) noexcept(...);

                        {doxygen("@brief Removes all rows from table.")}
                        {self.name}& clear() noexcept;

                        {doxygen(r"""
                        @brief Resizes the table to the given number of rows.

                        @availability This method is only available when all the column types are default-constructible.""")}
                        {self.name}& resize(size_type new_size) //
                            noexcept(soagen::has_nothrow_resize_member<table_type>);

                        '''
                        )

                        if self.swappable:
                            o(
                                rf'''
                            {doxygen(r"""
                            @brief Swaps the contents of the table with another.

                            @availability This method is only available when #allocator_type is swappable or non-propagating.""")}
                            constexpr void swap({self.name}& other) //
                                noexcept(soagen::has_nothrow_swap_member<table_type>);

                            #endif
                            '''
                            )

                # figure out defaults for function + template params
                value_defaults = []
                type_defaults = []
                defaults_ok = True
                for i in range(len(self.columns) - 1, -1, -1):
                    col = self.columns[i]
                    if not col.default:
                        defaults_ok = False
                    if defaults_ok:
                        value_defaults.append(col.default)
                        type_defaults.append(rf'column_traits<{col.index}>::default_emplace_type')
                    else:
                        value_defaults.append('')
                        type_defaults.append('')
                value_defaults.reverse()
                type_defaults.reverse()

                # param lists etc
                lvalue_param_list = []
                lvalue_forward_list = []
                rvalue_param_list = []
                rvalue_forward_list = []
                template_type_names = []
                template_type_list = []
                template_params = []
                template_param_list = []
                template_forward_list = []
                for i in range(len(self.columns)):
                    col = self.columns[i]
                    # push_back + insert:
                    value_default = rf' = {value_defaults[i]}' if value_defaults[i] else ''
                    lvalue_param_list.append(rf'column_traits<{i}>::param_type {col.name}{value_default}')
                    lvalue_forward_list.append(rf'static_cast<column_traits<{i}>::param_forward_type>({col.name})')
                    rvalue_param_list.append(rf'column_traits<{i}>::rvalue_type {col.name}{value_default}')
                    rvalue_forward_list.append(rf'static_cast<column_traits<{i}>::rvalue_forward_type>({col.name})')
                    # emplace_back + emplace:
                    type_default = rf' = {type_defaults[i]}' if type_defaults[i] else ''
                    typename = utils.to_pascal_case(col.name)
                    if typename == col.name:
                        typename = rf'{typename}T'
                    template_type_names.append(typename)
                    template_params.append(typename + '&&')
                    template_type_list.append(rf'typename {typename}{type_default}')
                    template_param_list.append(rf'{typename}&& {col.name}{value_default}')
                    template_forward_list.append(rf'static_cast<{typename}&&>({col.name})')

                with DoxygenMemberGroup(o, 'Adding rows'):
                    with Public(o):
                        o(
                            rf'''
                        // ------ push_back() --------------------------------------------------------------------------

                        {doxygen('@brief Adds a new row at the end of the table.')}
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& push_back({", ".join(lvalue_param_list)}) //
                            noexcept(table_traits::push_back_is_nothrow<table_type>)		//
                        {{
                            table_.emplace_back({", ".join(lvalue_forward_list)});
                            return *this;
                        }}

                        {doxygen('@brief Adds a new row at the end of the table (rvalue overload).')}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = table_traits::rvalues_are_distinct)
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& push_back({", ".join(rvalue_param_list)}) //
                            noexcept(table_traits::rvalue_push_back_is_nothrow<table_type>)	 //
                        {{
                            table_.emplace_back({", ".join(rvalue_forward_list)});
                            return *this;
                        }}

                        // ------ emplace_back() -----------------------------------------------------------------------

                        {doxygen('@brief Constructs a new row directly in-place at the end of the table.')}
                        SOAGEN_CONSTRAINED_TEMPLATE((table_traits::row_constructible_from<{", ".join(template_params)}>), //
                                                    {", ".join(template_type_list)}) //
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& emplace_back({", ".join(template_param_list)}) //
                            noexcept(table_traits::emplace_back_is_nothrow<table_type, {", ".join(template_params)}>) //
                        {{
                            table_.emplace_back({", ".join(template_forward_list)});
                            return *this;
                        }}

                        {doxygen('@brief Constructs a new row directly in-place at the end of the table by unpacking a tuple-like object.')}
                        SOAGEN_CONSTRAINED_TEMPLATE(table_traits::row_constructible_from<Tuple>, typename Tuple)
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& emplace_back(Tuple&& tuple_)								 //
                            noexcept(table_traits::emplace_back_is_nothrow<table_type, Tuple&&>) //
                        {{
                            table_.emplace_back(static_cast<Tuple&&>(tuple_));
                            return *this;
                        }}

                        '''
                        )

                with DoxygenMemberGroup(
                    o,
                    'Inserting rows',
                    availability=r'These overloads are only available when all the column types are move/copy-constructible and move/copy-assignable.',
                ):
                    with Private(o):
                        o(
                            r'''
                            static constexpr bool can_insert_ = table_traits::all_move_or_copy_constructible
                                                            && table_traits::all_move_or_copy_assignable;

                            static constexpr bool can_insert_rvalues_ = can_insert_ && table_traits::rvalues_are_distinct;
                          '''
                        )

                    with Public(o):
                        o(
                            rf'''

                        // ------ insert(size_type) --------------------------------------------------------------------

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T({self.name}&, sfinae) insert(size_type index_, {", ".join(lvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(index_, {", ".join(lvalue_forward_list)});
                            return *this;
                        }}

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table (rvalue overload).

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_rvalues_) //
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& insert(SOAGEN_ENABLE_IF_T(size_type, sfinae) index_, {", ".join(rvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(index_, {", ".join(rvalue_forward_list)});
                            return *this;
                        }}


                        // ------ insert(iterator) ---------------------------------------------------------------------

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(iterator, sfinae) insert(iterator iter_, {", ".join(lvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(lvalue_forward_list)});
                            return iter_;
                        }}

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(const_iterator, sfinae) insert(const_iterator iter_, {", ".join(lvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(lvalue_forward_list)});
                            return iter_;
                        }}

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table (rvalue overload).

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_rvalues_) //
                        SOAGEN_CPP20_CONSTEXPR
                        iterator insert(SOAGEN_ENABLE_IF_T(iterator, sfinae) iter_, {", ".join(rvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(rvalue_forward_list)});
                            return iter_;
                        }}

                        {doxygen("""@brief Inserts a new row at an arbitrary position in the table (rvalue overload).

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_HIDDEN_CONSTRAINT(sfinae, bool sfinae = can_insert_rvalues_) //
                        SOAGEN_CPP20_CONSTEXPR
                        const_iterator insert(SOAGEN_ENABLE_IF_T(const_iterator, sfinae) iter_, {", ".join(rvalue_param_list)}) //
                            noexcept(table_traits::insert_is_nothrow<table_type>)             //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(rvalue_forward_list)});
                            return iter_;
                        }}

                        // ------ emplace(size_type) -------------------------------------------------------------------

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, {", ".join(template_type_list)}
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<{", ".join(template_params)}> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T({self.name}&, sfinae) emplace(size_type index_, {", ".join(template_param_list)}) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, {", ".join(template_params)}>) //
                        {{
                            table_.emplace(index_, {", ".join(template_forward_list)});
                            return *this;
                        }}

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table by unpacking a tuple-like object.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, typename Tuple
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<Tuple&&> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& emplace(SOAGEN_ENABLE_IF_T(size_type, sfinae) index_, Tuple&& tuple_) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, Tuple&&>) //
                        {{
                            table_.emplace(index_, static_cast<Tuple&&>(tuple_));
                            return *this;
                        }}

                        // ------ emplace(iterator) --------------------------------------------------------------------

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, {", ".join(template_type_list)}
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<{", ".join(template_params)}> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(iterator, sfinae) emplace(iterator iter_, {", ".join(template_param_list)}) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, {", ".join(template_params)}>) //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(template_forward_list)});
                            return iter_;
                        }}

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table by unpacking a tuple-like object.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, typename Tuple
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<Tuple&&> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        iterator emplace(SOAGEN_ENABLE_IF_T(iterator, sfinae) iter_, Tuple&& tuple_) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, Tuple&&>) //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), static_cast<Tuple&&>(tuple_));
                            return iter_;
                        }}

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, {", ".join(template_type_list)}
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<{", ".join(template_params)}> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(const_iterator, sfinae) emplace(const_iterator iter_, {", ".join(template_param_list)}) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, {", ".join(template_params)}>) //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), {", ".join(template_forward_list)});
                            return iter_;
                        }}

                        {doxygen("""@brief Constructs a new row directly in-place at an arbitrary position in the table by unpacking a tuple-like object.

                        @availability This overload is only available when all the column types are move/copy-constructible and move/copy-assignable.""")}
                        SOAGEN_CONSTRAINED_TEMPLATE(sfinae, typename Tuple
                                                    SOAGEN_HIDDEN_PARAM(bool sfinae = table_traits::row_constructible_from<Tuple&&> && can_insert_)) //
                        SOAGEN_CPP20_CONSTEXPR
                        const_iterator emplace(SOAGEN_ENABLE_IF_T(const_iterator, sfinae) iter_, Tuple&& tuple_) //
                            noexcept(table_traits::emplace_is_nothrow<table_type, Tuple&&>) //
                        {{
                            table_.emplace(static_cast<size_type>(iter_), static_cast<Tuple&&>(tuple_));
                            return iter_;
                        }}

                        '''
                        )

                with DoxygenMemberGroup(o, 'Columns'):
                    with Public(o):
                        o(
                            rf'''
                        {doxygen(r"@brief Returns a pointer to the elements of a specific column.")}
                        template <auto Column>
                        SOAGEN_COLUMN({self.name}, Column)
                        constexpr column_type<Column>* column() noexcept
                        {{
                            static_assert(static_cast<size_type>(Column) < table_traits::column_count, "column index out of range");

                            return soagen::assume_aligned<soagen::actual_alignment<{self.name}, Column>>(table_.template column<Column>());
                        }}

                        {doxygen(r"@brief Returns a pointer to the elements of a specific column.")}
                        template <auto Column>
                        SOAGEN_COLUMN({self.name}, Column)
                        constexpr std::add_const_t<column_type<Column>>* column() const noexcept
                        {{
                            static_assert(static_cast<size_type>(Column) < table_traits::column_count, "column index out of range");

                            return soagen::assume_aligned<soagen::actual_alignment<{self.name}, Column>>(table_.template column<Column>());
                        }}

                        '''
                        )

                        if o.doxygen:
                            o(
                                rf'''
                            #if SOAGEN_DOXYGEN

                            {doxygen(r"""
                            @brief Returns a pointer to the raw byte backing array.

                            @availability This method is only available when all the column types are trivially-copyable.""")}
                            constexpr std::byte* data() noexcept;

                            {doxygen(r"""
                            @brief Returns a pointer to the raw byte backing array.

                            @availability This method is only available when all the column types are trivially-copyable.""")}
                            constexpr const std::byte* const data() noexcept;

                            '''
                            )
                            for i in range(len(self.columns)):
                                o(
                                    rf'''
                                {doxygen(rf"@brief Returns a pointer to the elements in column [{i}]: {self.columns[i].name}.")}
                                constexpr {self.columns[i].pointer_type} {self.columns[i].name}() noexcept;

                                {doxygen(rf"@brief Returns a pointer to the elements in column [{i}]: {self.columns[i].name}.")}
                                constexpr {self.columns[i].const_pointer_type} {self.columns[i].name}() const noexcept;
                                '''
                                )
                            for const in ('', 'const '):
                                o(
                                    rf'''
                                {doxygen(r"""
                                @brief Invokes a function once for each column data pointer.

                                @tparam Func A callable type compatible with one of the following signatures:<ul>
                                        <li> `void(auto*, std::integral_constant<size_type, N>)`
                                        <li> `void(auto*, size_type)`
                                        <li> `void(std::integral_constant<size_type, N>, auto*)`
                                        <li> `void(size_type, auto*)`
                                        <li> `void(auto*)`
                                </ul>
                                Overload resolution is performed in the order listed above.

                                @param func The callable to invoke.""")}
                                template <typename Func>
                                constexpr void for_each_column(Func&& func) {const} noexcept(...);
                                '''
                                )
                            o(
                                '''

                            #endif

                            '''
                            )

                if o.doxygen:  # everything here is exposition only
                    with PreprocessorRegion(o, 'SOAGEN_DOXYGEN'), Public(o):
                        # Capacity
                        with DoxygenMemberGroup(o, 'Capacity'):
                            with Public(o):
                                o(
                                    rf'''
                                {doxygen(r"@brief Returns true if the number of rows is zero.")}
                                SOAGEN_PURE_INLINE_GETTER
                                constexpr bool empty() const noexcept;

                                {doxygen(r"@brief Returns the current number of rows.")}
                                SOAGEN_PURE_INLINE_GETTER
                                constexpr size_type size() const noexcept;

                                {doxygen(r"@brief Returns the maximum possible number of rows.")}
                                SOAGEN_PURE_INLINE_GETTER
                                constexpr size_type max_size() const noexcept;

                                {doxygen(r"""
                                @brief Returns the size of the current underlying buffer allocation in bytes.

                                @warning This value is `capacity() * (sizeof() for every column) + (alignment padding)`.
                                        It is **not** based on `size()`! If you are using the value returned by this function
                                        in conjunction with `data()` to do serialization, hashing, etc, use `shrink_to_fit()` first.""")}
                                constexpr size_type allocation_size() const noexcept;

                                {doxygen(r"@brief Reserves storage for (at least) the given number of rows.")}
                                {self.name}& reserve(size_type new_cap)noexcept(...)

                                {doxygen(r"@brief Returns the number of rows that can be held in currently allocated storage.")}
                                constexpr size_type capacity() const noexcept;

                                {doxygen(r"@brief Frees unused capacity.")}
                                {self.name}& shrink_to_fit() noexcept(...);
                                '''
                                )

                        # Equality
                        with DoxygenMemberGroup(
                            o,
                            'Equality',
                            availability=r'These operators are only available when all the column types are equality-comparable.',
                        ):
                            o(
                                rf'''
                            {doxygen(r"""
                            @brief Returns true if all of the elements in two tables are equal.""")}
                            friend constexpr bool operator==(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_equality_comparable<table_type>);

                            {doxygen(r"""
                            @brief Returns true if not all of the elements in two tables are equal.""")}
                            friend constexpr bool operator!=(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_equality_comparable<table_type>);
                            '''
                            )

                        # Comparsion
                        with DoxygenMemberGroup(
                            o,
                            'Comparison',
                            availability=r'These operators are only available when all the column types are less-than-comparable.',
                        ):
                            o(
                                rf'''
                            {doxygen("""
                            @brief Returns true if the LHS table is ordered lexicographically less-than the RHS table.""")}
                            friend constexpr bool operator<(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_less_than_comparable<table_type>);

                            {doxygen("""
                            @brief Returns true if the LHS table is ordered lexicographically less-than-or-equal-to the RHS table.""")}
                            friend constexpr bool operator<=(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_less_than_comparable<table_type>);

                            {doxygen("""
                            @brief Returns true if the LHS table is ordered lexicographically greater-than the RHS table.""")}
                            friend constexpr bool operator>(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_less_than_comparable<table_type>);

                            {doxygen("""
                            @brief Returns true if the LHS table is ordered lexicographically greater-than-or-equal-to the RHS table.""")}
                            friend constexpr bool operator>=(const {self.name}& lhs, const {self.name}& rhs) //
                                noexcept(soagen::is_nothrow_less_than_comparable<table_type>);
                            '''
                            )

                        # Iterators
                        for funcs, group in ((('', 'c'), r'Iterators'), (('r', 'cr'), r'Iterators (reverse)')):
                            reverse = 'r' in funcs
                            if reverse and not self.reverse_iterators:
                                continue
                            with DoxygenMemberGroup(o, group):
                                for func in funcs:
                                    for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
                                        if func.startswith('c') and (not const or ref == '&&'):
                                            continue

                                        func_ref = ref if not func.startswith('c') else ''
                                        an = 'a reverse' if reverse else 'an'
                                        past = 'before' if reverse else 'past'
                                        first = 'last' if reverse else 'first'
                                        last = 'first' if reverse else 'last'
                                        iterator = rf'soagen::{"const_" if const else ""}iterator_type<{self.name}{"&&" if ref == "&&" else ""}, Cols...>'

                                        o(
                                            rf'''

                                        {doxygen(rf"""@brief Returns {an} iterator to the {first} row in the table.""")}
                                        template <auto... Cols>
                                        constexpr {iterator} {func}begin() {const}{func_ref} noexcept;

                                        {doxygen(rf"""@brief Returns {an} iterator to one-{past}-the-{last} row in the table.""")}
                                        template <auto... Cols>
                                        constexpr {iterator} {func}end() {const}{func_ref} noexcept;
                                        '''
                                        )

                        # Rows
                        with DoxygenMemberGroup(o, 'Rows'):
                            for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
                                rvalue = '&&' if ref == '&&' else ''
                                row_type = rf'soagen::{"const_" if const else ""}row_type<{self.name}{rvalue}, Cols...>'
                                row_alias = ('rvalue_' if ref == '&&' else ('const_' if const else '')) + r'row_type'
                                o(
                                    rf'''
                                    {doxygen(r"""
                                    @brief Returns the row at the given index.

                                    @tparam Cols Indices of the columns to include in the row. Leave the list empty for all columns.""")}
                                    template <auto... Cols>
                                    {row_type} row(size_type index) {const}{ref} noexcept;

                                    {doxygen(r"""@brief Returns the row at the given index.""")}
                                    {row_alias} operator[](size_type index) {const}{ref} noexcept;

                                    {doxygen(r"""
                                    @brief Returns the row at the given index.

                                    @throws std::out_of_range""")}
                                    template <auto... Cols>
                                    {row_type} at(size_type index) {const}{ref};

                                    {doxygen(r"""@brief Returns the very first row in the table.""")}
                                    template <auto... Cols>
                                    {row_type} front() {const}{ref} noexcept;

                                    {doxygen(r"""@brief Returns the very last row in the table.""")}
                                    template <auto... Cols>
                                    {row_type} back() {const}{ref} noexcept;

                                '''
                                )

                        # Spans
                        with DoxygenMemberGroup(o, 'Spans'):
                            o(
                                rf'''
                                {doxygen(rf"""@brief Returns a span of (some part of) the table.""")}
                                span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) & noexcept;

                                {doxygen(rf"""@brief Returns an rvalue-qualified span of (some part of) the table.""")}
                                rvalue_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) && noexcept;

                                {doxygen(rf"""@brief Returns a const-qualified span of (some part of) the table.""")}
                                const_span_type subspan(size_type start, size_type count = static_cast<size_type>(-1)) const& noexcept;

                                {doxygen(rf"""@brief Returns a const-qualified span of (some part of) the table.""")}
                                const_span_type const_subspan(size_type start, size_type count = static_cast<size_type>(-1)) const noexcept;
                                '''
                            )
                o(
                    rf'''
                {self.footer}
                '''
                )

            if self.swappable:
                o(
                    rf'''
                {doxygen(f"""
                @brief Swaps the contents of two instances of #{self.qualified_name}.

                @availability	This overload is only available when #{self.qualified_name}::allocator_type
                                is swappable or non-propagating.""")}
                SOAGEN_HIDDEN_CONSTRAINT(sfinae, auto sfinae = soagen::has_swap_member<{self.name}>)
                SOAGEN_ALWAYS_INLINE
                constexpr void swap({self.name}& lhs, {self.name}& rhs) //
                    noexcept(soagen::has_nothrow_swap_member<{self.name}>)
                {{
                    lhs.swap(rhs);
                }}
                '''
                )

            if self.epilogue:
                o(
                    f'''
                {self.epilogue}
                '''
                )

    def write_outline_member_implementations(self, o: Writer):
        pass


__all__ = [r'Struct']
