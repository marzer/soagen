#!/usr/bin/env python3
# This file is a part of marzer/soagen and is subject to the the terms of the MIT license.
# Copyright (c) Mark Gillard <mark.gillard@outlook.com.au>
# See https://github.com/marzer/soagen/blob/master/LICENSE for the full license text.
# SPDX-License-Identifier: MIT

from io import StringIO

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

    def write_soagen_detail_specializations(self, o: Writer):
        with MetaScope(self):
            max_length = 0
            for col in self.columns:
                max_length = max(len(col.name), max_length)
            with StringIO() as buf:
                buf.write('table_traits<\n')
                for i in range(len(self.columns)):
                    if i:
                        buf.write(f',\n')
                    col = self.columns[i]
                    buf.write(f'{o.indent_str*5}/* {col.name:>{max_length}} */ column_traits<{col.type}')
                    if col.alignment > 0:
                        buf.write(rf', soagen::max(size_t{{ {col.alignment} }}, alignof({col.type}))')
                    if col.param_type:
                        if not col.param_type:
                            buf.write(rf', alignof({col.type})')
                        buf.write(rf', {col.param_type}')
                    buf.write(rf'>')
                buf.write(rf'>')
                o(
                    rf'''
                template <>
                struct table_traits_type_<{self.qualified_name}>
                {{
                    using type = {buf.getvalue()};
                }};

                template <>
                struct allocator_type_<{self.qualified_name}>
                {{
                    using type = {self.allocator};
                }};

                template <>
                struct is_soa_<{self.qualified_name}> : std::true_type
                {{}};

                template <>
                struct columns_always_aligned_<{self.qualified_name}> : std::true_type
                {{}};
                '''
                )

                for col in self.columns:
                    o(rf'SOAGEN_MAKE_NAMED_COLUMN({self.qualified_name}, {col.index}, {col.name});')

            o(
                rf'''
            template <>
            struct table_type_<{self.qualified_name}>
            {{
                using type = table<table_traits_type<{self.qualified_name}>, {self.allocator}>;
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
                hidden_base_classes=[
                    rf'public soagen::mixins::size_and_capacity<{self.name}>',
                    rf'public soagen::mixins::resizable<{self.name}>',
                    rf'public soagen::mixins::equality_comparable<{self.name}>',
                    rf'public soagen::mixins::less_than_comparable<{self.name}>',
                    rf'public soagen::mixins::data_ptr<{self.name}>',
                    rf'public soagen::mixins::columns<{self.name}>',
                    rf'public soagen::mixins::rows<{self.name}>',
                    rf'public soagen::mixins::iterators<{self.name}>',
                ]
                + ([rf'public soagen::mixins::swappable<{self.name}>'] if self.swappable else []),
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
                    using column_traits = typename table_traits::template column<static_cast<size_type>(Column)>;

                    {doxygen(r"@brief Gets the type of a specific column in the table.")}
                    template <auto Column>
                    using column_type = typename column_traits<static_cast<size_type>(Column)>::value_type;

                    {doxygen(r"@brief Row iterators returned by iterator functions.")}
                    using iterator = soagen::iterator_type<{self.name}>;

                    {doxygen(r"@brief Row iterators returned by const-qualified iterator functions.")}
                    using const_iterator = soagen::iterator_type<const {self.name}>;

                    {doxygen(r"@brief Row iterators returned by rvalue-qualified iterator functions.")}
                    using rvalue_iterator = soagen::iterator_type<{self.name}&&>;
                    '''
                    )

                    if self.reverse_iterators:
                        o(
                            rf'''
                        {doxygen(r"@brief Reverse lvalue row iterator.")}
                        using reverse_iterator = std::reverse_iterator<iterator>;

                        {doxygen(r"@brief Reverse const lvalue row iterator.")}
                        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

                        {doxygen(r"@brief Reverse rvalue row iterator.")}
                        using rvalue_reverse_iterator = std::reverse_iterator<rvalue_iterator>;
                        '''
                        )

                    o(
                        rf'''
                    {doxygen(r"@brief Regular (lvalue-qualified) row type used by this class.")}
                    using row_type = soagen::row_type<{self.name}>;

                    {doxygen(r"@brief Const row type used by this class.")}
                    using const_row_type = soagen::row_type<const {self.name}>;

                    {doxygen(r"@brief Rvalue row type used by this class.")}
                    using rvalue_row_type = soagen::row_type<{self.name}&&>;

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

                with DoxygenMemberGroup(o, 'Underlying table access'):
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
                        explicit constexpr operator table_type& () & noexcept
                        {{
                            return table_;
                        }}

                        {doxygen(r"@brief Returns an rvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        explicit constexpr operator table_type&& () && noexcept
                        {{
                            return static_cast<table_type&&>(table_);
                        }}

                        {doxygen(r"@brief Returns a const lvalue reference to the underlying soagen::table.")}
                        SOAGEN_PURE_INLINE_GETTER
                        explicit constexpr operator const table_type& () const & noexcept
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
                        SOAGEN_HIDDEN(template <bool sfinae = soagen::has_erase_member<table_type>>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T({self.name}&, sfinae) erase(size_type pos) //
                            noexcept(soagen::has_nothrow_erase_member<table_type>)
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
                        SOAGEN_HIDDEN(template <bool sfinae = soagen::has_unordered_erase_member<table_type>>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(soagen::optional<size_type>, sfinae) unordered_erase(size_type pos) //
                            noexcept(soagen::has_nothrow_unordered_erase_member<table_type>)
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
                                SOAGEN_HIDDEN(template <bool sfinae = soagen::has_erase_member<table_type>>)
                                SOAGEN_ALWAYS_INLINE
                                SOAGEN_CPP20_CONSTEXPR
                                SOAGEN_ENABLE_IF_T({const}iterator, sfinae) erase({const}iterator pos) //
                                    noexcept(soagen::has_nothrow_erase_member<table_type>)
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
                                SOAGEN_HIDDEN(template <bool sfinae = soagen::has_unordered_erase_member<table_type>>)
                                SOAGEN_ALWAYS_INLINE
                                SOAGEN_CPP20_CONSTEXPR
                                SOAGEN_ENABLE_IF_T(soagen::optional<{const}iterator>, sfinae) unordered_erase({const}iterator pos) //
                                    noexcept(soagen::has_nothrow_unordered_erase_member<table_type>)
                                {{
                                    if (auto moved_pos = table_.unordered_erase(static_cast<size_type>(pos)); moved_pos)
                                        return {const}iterator{{ table_, static_cast<difference_type>(*moved_pos) }};
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

                with Public(o):
                    for funcs, group, availability in (
                        ((r'push_back', r'emplace_back'), r'Adding rows', ''),
                        (
                            (r'insert', r'emplace'),
                            r'Inserting rows',
                            r'These overloads are only available when all the column types are move-constructible and move-assignable.',
                        ),
                    ):
                        sfinae_variant = -1
                        sfinae_sig_hash = ''
                        with DoxygenMemberGroup(o, group, availability=availability):
                            for func in funcs:
                                for tuple_overload in (False, True):
                                    if tuple_overload and func not in (r'emplace_back', r'emplace'):
                                        continue
                                    for rvalue_overload in (False, True):
                                        if rvalue_overload and (
                                            tuple_overload or func in (r'emplace_back', r'emplace')
                                        ):
                                            continue
                                        for iterator_overload in (None, r'iterator', r'const_iterator'):
                                            if iterator_overload and func in (r'push_back', r'emplace_back'):
                                                continue

                                            template_params = []
                                            template_defaults = []
                                            types = []
                                            names = []
                                            defaults = []
                                            forwards = []
                                            deduced = []
                                            sig_hash = func

                                            if func in (r'insert', r'emplace'):
                                                defaults.append('')
                                                if iterator_overload:
                                                    types.append(iterator_overload)
                                                    names.append(r'iter_')
                                                    forwards.append(r'static_cast<size_type>(iter_)')
                                                    deduced.append(False)
                                                else:
                                                    types.append(r'size_type')
                                                    names.append(r'index_')
                                                    forwards.append(names[-1])
                                                    deduced.append(False)

                                            if tuple_overload:
                                                template_params.append(r'typename Tuple')
                                                template_defaults += ['']
                                                types.append(r'Tuple&&')
                                                names.append(r'tuple_')
                                                deduced.append(True)
                                                defaults.append('')
                                                forwards.append(rf'static_cast<{types[-1]}>({names[-1]})')
                                            else:
                                                for col in self.columns:
                                                    names.append(col.name)
                                                    defaults.append(col.default)
                                                    if func in (r'emplace_back', r'emplace'):
                                                        typename = utils.to_pascal_case(col.name)
                                                        if typename == col.name:
                                                            typename = rf'{typename}T'
                                                        template_params.append(rf'typename {typename}')
                                                        template_defaults.append(
                                                            rf'column_traits<{col.index}>::default_emplace_type'
                                                            if col.default
                                                            else ''
                                                        )
                                                        types.append(rf'{typename}&&')
                                                        forwards.append(rf'static_cast<{types[-1]}>({names[-1]})')
                                                        deduced.append(True)
                                                    else:
                                                        category = r'rvalue' if rvalue_overload else r'param'
                                                        types.append(rf'column_traits<{col.index}>::{category}_type')
                                                        forward_type = (
                                                            rf'column_traits<{col.index}>::{category}_forward_type'
                                                        )
                                                        forwards.append(rf'static_cast<{forward_type}>({names[-1]})')
                                                        deduced.append(False)

                                            sig_hash += '##'.join(types)
                                            if not sfinae_sig_hash or sfinae_sig_hash != sig_hash:
                                                sfinae_variant = 0 if rvalue_overload else -1
                                                sfinae_sig_hash = sig_hash

                                            sfinae = []
                                            sfinae_emitted = False
                                            sfinae_template_dependent = False
                                            sfinae_condition_string = ''
                                            if tuple_overload:
                                                sfinae.append(r'table_traits::row_constructible_from<Tuple&&>')
                                                sfinae_template_dependent = True
                                            if rvalue_overload:
                                                sfinae.append(r'table_traits::rvalue_type_list_is_distinct')
                                            if func in (r'insert', r'emplace'):
                                                sfinae.append(r'table_traits::all_move_constructible')
                                                sfinae.append(r'table_traits::all_move_assignable')
                                            if func in (r'emplace_back', r'emplace'):
                                                sfinae.append(
                                                    rf'table_traits::row_constructible_from<{",".join(types[-len(self.columns):])}>'
                                                )
                                                sfinae_template_dependent = True
                                            hidden_template = bool(sfinae) and not template_params
                                            if sfinae:
                                                if len(sfinae) > 2:
                                                    sfinae_condition_string = f'\n{o.indent_str*7}&& '.join(sfinae)
                                                else:
                                                    sfinae_condition_string = r' && '.join(sfinae)
                                                if len(sfinae) >= 2:
                                                    sfinae_condition_string = rf'({sfinae_condition_string})'
                                                if sfinae_template_dependent:
                                                    template_params.append(
                                                        rf'SOAGEN_ENABLE_IF({sfinae_condition_string}'
                                                    )
                                                    template_defaults.append(')')
                                                    sfinae_emitted = True
                                                else:
                                                    if hidden_template:
                                                        template_params.append(r'bool sfinae')
                                                        template_defaults.append(sfinae_condition_string)
                                                    else:
                                                        template_params.append(
                                                            rf'SOAGEN_HIDDEN_PARAM(bool sfinae = {sfinae_condition_string}'
                                                        )
                                                        template_defaults.append(')')

                                            defaults_ok = True
                                            assert len(template_params) == len(template_defaults)
                                            for i in range(len(template_defaults) - 1, -1, -1):
                                                if not defaults_ok:
                                                    template_defaults[i] = ''
                                                elif not template_defaults[i]:
                                                    defaults_ok = False

                                            defaults_ok = True
                                            assert len(names) == len(defaults)
                                            assert len(names) == len(types)
                                            for i in range(len(defaults) - 1, -1, -1):
                                                if not defaults_ok:
                                                    defaults[i] = ''
                                                elif not defaults[i]:
                                                    defaults_ok = False

                                            s = ''

                                            # doxygen
                                            if o.doxygen:
                                                s += '/// @brief '
                                                if func in (r'emplace_back', r'emplace'):
                                                    s += 'Constructs a new row directly in-place'
                                                elif func == r'insert':
                                                    s += 'Inserts a new row'
                                                elif func == r'push_back':
                                                    s += 'Adds a new row'
                                                if func in (r'insert', r'emplace'):
                                                    s += ' at an arbitrary position in the table'
                                                elif func.endswith(r'_back'):
                                                    s += ' at the end of the table'
                                                if tuple_overload:
                                                    s += ' by unpacking a tuple-like object'
                                                if rvalue_overload:
                                                    s += ' (rvalue overload)'
                                                s += '.\n'
                                                if func in (r'insert', r'emplace'):
                                                    s += '///\n'
                                                    s += '/// @availability This overload is only available when all'
                                                    s += ' the column types are move-constructible and move-assignable.\n'

                                            # template <>
                                            if template_params:
                                                if hidden_template:
                                                    s += r'SOAGEN_HIDDEN('
                                                s += r'template <'
                                                padding = f'\n{TAB_SPACES*2}  ' if len(template_params) >= 3 else ' '
                                                for i in range(len(template_params)):
                                                    if i:
                                                        if not template_params[i].startswith(
                                                            'SOAGEN_ENABLE_IF'
                                                        ) and not template_params[i].startswith('SOAGEN_HIDDEN'):
                                                            s += rf','
                                                        s += rf'{padding}'
                                                    s += template_params[i]
                                                    if template_defaults[i] == ')':
                                                        s += ')'
                                                    elif template_defaults[i]:
                                                        s += rf' = {template_defaults[i]}'
                                                s += r'>'
                                                if hidden_template:
                                                    s += r')'
                                                s += '\n'

                                            # attributes
                                            s += 'SOAGEN_CPP20_CONSTEXPR\n'

                                            # return type
                                            return_type = iterator_overload if iterator_overload else rf'{self.name}&'
                                            return_expression = r'iter_' if iterator_overload else r'*this'
                                            if sfinae and not sfinae_emitted and sfinae_variant == -1:
                                                return_type = rf'SOAGEN_ENABLE_IF_T({return_type}, sfinae)'
                                                sfinae_emitted = True
                                                sfinae_variant += 1
                                            s += rf'{return_type} '

                                            # name + args
                                            s += rf'{func}('
                                            if types:
                                                padding = ' ' * (len(func) + len(return_type) + 2)
                                                padding = f'\n{padding}' if len(types) >= 3 else ' '
                                                for i in range(len(types)):
                                                    if i:
                                                        s += rf',{padding}'
                                                    if (
                                                        sfinae
                                                        and not sfinae_emitted
                                                        and sfinae_variant == i
                                                        and types[i]
                                                        and not deduced[i]
                                                        in (
                                                            'size_t',
                                                            'size_type',
                                                            'iterator',
                                                            'const_iterator',
                                                            rf'column_traits<{i-1}>::param_type',
                                                            rf'column_traits<{i-1}>::rvalue_type',
                                                        )
                                                    ):
                                                        s += rf'SOAGEN_ENABLE_IF_T({types[i]}, sfinae)'
                                                        sfinae_emitted = True
                                                        sfinae_variant += 1
                                                    else:
                                                        s += rf'{types[i]}'
                                                    s += rf' {names[i]}'
                                                    if defaults[i]:
                                                        s += rf' = {defaults[i]}'
                                            s += ') //\n'

                                            # noexcept(...)
                                            backing_func = (
                                                r'emplace' if func in (r'insert', r'emplace') else r'emplace_back'
                                            )
                                            s += '\tnoexcept('
                                            if func == 'push_back':
                                                if tuple_overload:
                                                    s += rf'table_traits::row_push_back_is_nothrow<table_type&, {types[0]}>'
                                                else:
                                                    s += rf'table_traits::{"rvalue_" if rvalue_overload else ""}push_back_is_nothrow<table_type&>'
                                            elif func == 'insert':
                                                if tuple_overload:
                                                    s += rf'table_traits::row_insert_is_nothrow<table_type&, {types[-1]}>'
                                                else:
                                                    s += rf'table_traits::{"rvalue_" if rvalue_overload else ""}insert_is_nothrow<table_type&>'
                                            elif func == 'emplace_back':
                                                s += rf'table_traits::emplace_back_is_nothrow<table_type&, {", ".join(types)}>'
                                            elif func == 'emplace':
                                                s += rf'table_traits::emplace_is_nothrow<table_type&, {", ".join(types[1:])}>'
                                            else:
                                                assert False  # we should have all our bases covered by the table_traits
                                            s += ')\n'

                                            # requires(...)
                                            if sfinae:
                                                s += f'\tSOAGEN_REQUIRES({sfinae_condition_string.strip("()")}) //\n'

                                            # function body
                                            s += '{\n'
                                            s += f'\ttable_.{backing_func}('
                                            if forwards:
                                                padding = f'\n{TAB_SPACES*2}' if len(forwards) >= 3 else ' '
                                                if len(forwards) >= 3:
                                                    s += padding
                                                s += f',{padding}'.join(forwards)
                                            s += f');\n'
                                            if return_expression:
                                                s += f'\treturn {return_expression};\n'
                                            s += '}\n\n'

                                            assert not sfinae or sfinae_emitted
                                            o(s)

                with DoxygenMemberGroup(o, 'Column access'):
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
                                        iterator = rf'soagen::iterator_type<{const}{self.name}{"&&" if ref == "&&" else ""}, Columns...>'

                                        o(
                                            rf'''

                                        {doxygen(rf"""@brief Returns {an} iterator to the {first} row in the table.""")}
                                        template <auto... Columns>
                                        constexpr {iterator} {func}begin() {const}{func_ref} noexcept;

                                        {doxygen(rf"""@brief Returns {an} iterator to one-{past}-the-{last} row in the table.""")}
                                        template <auto... Columns>
                                        constexpr {iterator} {func}end() {const}{func_ref} noexcept;
                                        '''
                                        )

                        # Rows
                        with DoxygenMemberGroup(o, 'Row access'):
                            for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
                                row_type = ('rvalue_' if ref == '&&' else ('const_' if const else '')) + r'row_type'
                                o(
                                    rf'''
                                    {doxygen(r"""
                                    @brief Returns the row at the given index.

                                    @tparam Columns Indices of the columns to include in the row. Leave the list empty for all columns.""")}
                                    template <auto... Columns>
                                    soagen::row_type<{const}{self.name}{'&&' if ref == '&&' else ''}, Columns...> row(size_type index) {const}{ref} noexcept;

                                    {doxygen(r"""@brief Returns the row at the given index.""")}
                                    {row_type} operator[](size_type index) {const}{ref} noexcept;

                                    {doxygen(r"""
                                    @brief Returns the row at the given index.

                                    @throws std::out_of_range""")}
                                    {row_type} at(size_type index) {const}{ref};

                                    {doxygen(r"""@brief Returns the very first row in the table.""")}
                                    {row_type} front() {const}{ref} noexcept;

                                    {doxygen(r"""@brief Returns the very last row in the table.""")}
                                    {row_type} back() {const}{ref} noexcept;

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
