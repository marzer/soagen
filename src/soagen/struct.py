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
            Optional(r'brief', default=''): Stripped(str),
            Optional(r'copyable', default=True): bool,
            Optional(r'default_constructible', default=True): bool,
            Optional(r'details', default=''): Stripped(str),
            Optional(r'epilogue', default=''): Stripped(str),
            Optional(r'footer', default=''): Stripped(str),
            Optional(r'header', default=''): Stripped(str),
            Optional(r'includes', default=dict): {object: object},
            Optional(r'iterators', default=True): bool,
            Optional(r'movable', default=True): bool,
            Optional(r'prologue', default=''): Stripped(str),
            Optional(r'reverse_iterators', default=False): bool,
            Optional(r'rvalue_iterators', default=True): bool,
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
            raise SchemaError(rf"name: '{self.name}': {valid[1]}", None)
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
        self.meta.push(r'struct::name', self.name)
        self.meta.push(r'struct::type', self.type)
        self.meta.push(r'struct::scope', '')

        if not self.allocator:
            self.allocator = self.config.allocator
        if self.allocator == r'std::allocator':
            self.allocator = r'std::allocator<std::byte>'

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
        with MetaScope(self.config.meta_stack, self.meta):
            o(rf'class {self.type};')

    def write_soagen_specializations(self, o: Writer):
        o(
            rf'''
        template <>
        inline constexpr bool is_soa<{self.qualified_name}> = true;
        '''
        )

    def write_soagen_detail_specializations(self, o: Writer):
        max_length = 0
        for col in self.columns:
            max_length = max(len(col.name), max_length)
        with StringIO() as buf:
            buf.write('table_traits<\n')
            for i in range(len(self.columns)):
                if i:
                    buf.write(f',\n')
                col = self.columns[i]
                buf.write(f'{o.indent_str*4}/* {col.name:>{max_length}} */ make_column<{col.type}')
                if col.param_type:
                    buf.write(rf', {col.param_type}')
                if col.alignment > 0:
                    if not col.param_type:
                        buf.write(rf', param_type<{col.type}>')
                    buf.write(rf', {col.alignment}')
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
            '''
            )

            for col in self.columns:
                o(rf'SOAGEN_MAKE_COL({self.qualified_name}, {col.index}, {col.name});')

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
        with MetaScope(self.config.meta_stack, self.meta) as meta:
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
            with ClassDefinition(
                o,
                f'class SOAGEN_EMPTY_BASES {self.name}',
                hidden_base_classes=[
                    rf'public soagen::mixins::resizable<{self.name}>',
                    rf'public soagen::mixins::equality_comparable<{self.name}>',
                    rf'public soagen::mixins::less_than_comparable<{self.name}>',
                    rf'public soagen::mixins::data_ptr<{self.name}>',
                    rf'public soagen::mixins::const_data_ptr<{self.name}>',
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
                    static constexpr size_t column_count = soagen::table_traits_type<{self.name}>::column_count;

                    {doxygen(r"@brief Gets the soagen::column_traits for a specific column of the table.")}
                    template <size_type Column>
                    using column_traits = typename table_traits::template column<Column>;

                    {doxygen(r"@brief Gets the type of a specific column in the table.")}
                    template <size_type Column>
                    using column_type = typename column_traits<Column>::value_type;

                    '''
                    )

                    if self.iterators:
                        o(
                            rf'''
                            {doxygen(r"@brief Row iterators returned by iterator functions.")}
                            using iterator = soagen::iterator_type<{self.name}&>;

                            {doxygen(r"@brief Row iterators returned by const-qualified iterator functions.")}
                            using const_iterator = soagen::iterator_type<const {self.name}&>;
                            '''
                        )

                        if self.rvalue_iterators:
                            o(
                                rf'''
                                {doxygen(r"@brief Row iterators returned by rvalue-qualified iterator functions.")}
                                using rvalue_iterator = soagen::iterator_type<{self.name}&&>;
                                '''
                            )

                    if self.iterators and self.reverse_iterators:
                        o(
                            rf'''
                            {doxygen(r"@brief Reverse lvalue row iterator.")}
                            using reverse_iterator = std::reverse_iterator<iterator>;

                            {doxygen(r"@brief Reverse const lvalue row iterator.")}
                            using const_reverse_iterator = std::reverse_iterator<const_iterator>;


                            '''
                        )
                        if self.rvalue_iterators:
                            o(
                                rf'''
                                {doxygen(r"@brief Reverse rvalue row iterator.")}
                                using rvalue_reverse_iterator = std::reverse_iterator<rvalue_iterator>;
                                '''
                            )

                    o(
                        rf'''
                    {doxygen(r"@brief Regular (lvalue-qualified) row type used by this class.")}
                    using row_type = soagen::row_type<{self.name}&>;

                    {doxygen(r"@brief Const row type used by this class.")}
                    using const_row_type = soagen::row_type<const {self.name}&>;

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
                        using column_indices = POXY_IMPLEMENTATION_DETAIL(struct dummy_t);
                        #else'''
                        )

                    with ClassDefinition(o, 'struct column_indices'):
                        for col in self.columns:
                            o(rf'static constexpr size_type {col.name} = {col.index};')

                    if o.doxygen:
                        o('#endif')

                    o(
                        rf'''

                    {doxygen(r"@brief Gets the name of the specified column as a string.")}
                    template <size_type Column> static constexpr auto& column_name = soagen::detail::col_name_<{self.name}, Column>::value;

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
                    o(
                        rf'''

                    {doxygen(r"@brief Default constructor.")}
                    {ctor_attrs if self.default_constructible else ""} {self.name}() = {"default" if self.default_constructible else "delete"};

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
                        '''
                        )

                with DoxygenMemberGroup(o, 'Capacity'):
                    with Public(o):
                        o(
                            rf'''
                        {doxygen(r"@brief Returns true if the number of rows is zero.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr bool empty() const noexcept
                        {{
                            return table_.empty();
                        }}

                        {doxygen(r"@brief Returns the current number of rows.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr size_type size() const noexcept
                        {{
                            return table_.size();
                        }}

                        {doxygen(r"@brief Returns the maximum possible number of rows.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr size_type max_size() const noexcept
                        {{
                            return table_.max_size();
                        }}

                        {doxygen(r"""
                        @brief Returns the size of the current underlying buffer allocation in bytes.

                        @warning This value is `capacity() * (sizeof() for every column) + (alignment padding)`.
                                 It is **not** based on `size()`! If you are using the value returned by this function
                                 in conjunction with `data()` to do serialization, hashing, etc, use `shrink_to_fit()` first.""")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr size_type allocation_size() const noexcept
                        {{
                            return table_.allocation_size();
                        }}

                        {doxygen(r"@brief Reserves storage for (at least) the given number of rows.")}
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& reserve(size_type new_cap) //
                            noexcept(noexcept(std::declval<table_type&>().reserve(size_type{{}})))
                        {{
                            table_.reserve(new_cap);
                            return *this;
                        }}

                        {doxygen(r"@brief Returns the number of rows that can be held in currently allocated storage.")}
                        SOAGEN_PURE_INLINE_GETTER
                        constexpr size_type capacity() const noexcept
                        {{
                            return table_.capacity();
                        }}

                        {doxygen(r"@brief Frees unused capacity.")}
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& shrink_to_fit() //
                            noexcept(noexcept(std::declval<table_type&>().shrink_to_fit()))
                        {{
                            table_.shrink_to_fit();
                            return *this;
                        }}
                        '''
                        )

                with DoxygenMemberGroup(o, 'Modifiers'):
                    with Public(o):
                        o(
                            rf'''
                        {doxygen("@brief Removes all rows from table.")}
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& clear() noexcept
                        {{
                            table_.clear();
                            return *this;
                        }}

                        {doxygen(r"""
                        @brief Erases the row at the given position.

                        @availability This method is only available when all the column types are move-assignable.""")}
                        SOAGEN_HIDDEN(template <bool sfinae = soagen::has_erase_member<table_type, size_type>>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T({self.name}&, sfinae) erase(size_type pos) //
                            noexcept(soagen::has_nothrow_erase_member<table_type, size_type>)
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
                        SOAGEN_HIDDEN(template <bool sfinae = soagen::has_unordered_erase_member<table_type, size_type>>)
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        SOAGEN_ENABLE_IF_T(soagen::optional<size_type>, sfinae) unordered_erase(size_type pos) //
                            noexcept(soagen::has_nothrow_unordered_erase_member<table_type, size_type>)
                        {{
                            return table_.unordered_erase(pos);
                        }}
                        '''
                        )

                        if self.iterators:
                            for const in ('', 'const_'):
                                o(
                                    rf'''
                                    {doxygen(rf"""
                                    @brief Erases the row at the given iterator.

                                    @returns    An iterator to the row immediately following the one which was removed,
                                                or #{"c" if const else ""}end() if the one removed was the last row in the table.

                                    @availability This method is only available when all the column types are move-assignable.""")}
                                    SOAGEN_HIDDEN(template <bool sfinae = soagen::has_erase_member<table_type, size_type>>)
                                    SOAGEN_ALWAYS_INLINE
                                    SOAGEN_CPP20_CONSTEXPR
                                    SOAGEN_ENABLE_IF_T({const}iterator, sfinae) erase({const}iterator pos) //
                                        noexcept(soagen::has_nothrow_erase_member<table_type, size_type>)
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
                                    SOAGEN_HIDDEN(template <bool sfinae = soagen::has_unordered_erase_member<table_type, size_type>>)
                                    SOAGEN_ALWAYS_INLINE
                                    SOAGEN_CPP20_CONSTEXPR
                                    SOAGEN_ENABLE_IF_T(soagen::optional<{const}iterator>, sfinae) unordered_erase({const}iterator pos) //
                                        noexcept(soagen::has_nothrow_unordered_erase_member<table_type, size_type>)
                                    {{
                                        if (auto moved_pos = table_.unordered_erase(static_cast<size_type>(pos)); moved_pos)
                                            return {const}iterator{{ *this, static_cast<difference_type>(*moved_pos) }};
                                        return {{}};
                                    }}



                                '''
                                )

                        o(
                            rf'''

                        {doxygen(r"@brief Removes the last row(s) from the table.")}
                        SOAGEN_ALWAYS_INLINE
                        SOAGEN_CPP20_CONSTEXPR
                        {self.name}& pop_back(size_type num = 1) //
                            noexcept(noexcept(std::declval<table_type&>().pop_back(size_type{{}})))
                        {{
                            table_.pop_back(num);
                            return *this;
                        }}

                        #if SOAGEN_DOXYGEN

                        {doxygen(r"""
                        @brief Resizes the table to the given number of rows.

                        @availability This method is only available when all the column types are default-constructible.""")}
                        {self.name}& resize(size_type new_size) //
                            noexcept(soagen::has_nothrow_resize_member<table_type, size_type>);

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
                                for row_overload in (False, True):
                                    if row_overload and func in (r'emplace_back', r'emplace'):
                                        continue
                                    for rvalue_overload in (False, True):
                                        if rvalue_overload and (row_overload or func in (r'emplace_back', r'emplace')):
                                            continue
                                        for iterator_overload in (None, r'iterator', r'const_iterator'):
                                            if iterator_overload and func in (r'push_back', r'emplace_back'):
                                                continue
                                            if iterator_overload and not self.iterators:
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

                                            if row_overload:
                                                template_params.append(r'typename Table')
                                                template_params.append(r'size_t... Columns')
                                                template_defaults += ['', '']
                                                types.append(r'const soagen::row<Table, Columns...>&')
                                                names.append(r'row_')
                                                deduced.append(True)
                                                defaults.append('')
                                                forwards.append(names[-1])
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
                                            if row_overload:
                                                sfinae.append(r'soagen::is_soa<soagen::remove_cvref<Table>>')
                                                sfinae_template_dependent = True
                                            if rvalue_overload:
                                                sfinae.append(r'table_traits::rvalue_type_list_is_distinct')
                                            if func in (r'insert', r'emplace'):
                                                sfinae.append(r'table_traits::all_move_constructible')
                                                sfinae.append(r'table_traits::all_move_assignable')
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
                                                if row_overload:
                                                    s += rf'table_traits::row_push_back_is_nothrow<table_type&, {types[0]}>'
                                                else:
                                                    s += rf'table_traits::{"rvalue_" if rvalue_overload else ""}push_back_is_nothrow<table_type&>'
                                            elif func == 'insert':
                                                if row_overload:
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

                with DoxygenMemberGroup(
                    o,
                    'Equality',
                    availability=r'These operators are only available when all the column types are equality-comparable.',
                ):
                    with Public(o):
                        o(
                            rf'''

                        #if SOAGEN_DOXYGEN

                        {doxygen(r"""
                        @brief Returns true if all of the elements in two tables are equal.""")}
                        friend constexpr bool operator==(const {self.name}& lhs, const {self.name}& rhs) //
                            noexcept(soagen::is_nothrow_equality_comparable<table_type>);

                        {doxygen(r"""
                        @brief Returns true if not all of the elements in two tables are equal.""")}
                        friend constexpr bool operator!=(const {self.name}& lhs, const {self.name}& rhs) //
                            noexcept(soagen::is_nothrow_equality_comparable<table_type>);

                        #endif
                        '''
                        )

                with DoxygenMemberGroup(
                    o,
                    'Comparison',
                    availability=r'These operators are only available when all the column types are less-than-comparable.',
                ):
                    with Public(o):
                        o(
                            rf'''

                        #if SOAGEN_DOXYGEN

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

                        #endif
                        '''
                        )

                with DoxygenMemberGroup(o, 'Column access'):
                    with Public(o):
                        o(
                            rf'''
                        #if SOAGEN_DOXYGEN

                        {doxygen(r"""
                        @brief Returns a pointer to the raw byte backing array.

                        @availability This method is only available when all the column types are trivially-copyable.""")}
                        constexpr std::byte* data() //
                            noexcept(soagen::has_nothrow_data_member<table_type>);

                        {doxygen(r"""
                        @brief Returns a pointer to the raw byte backing array.

                        @availability This method is only available when all the column types are trivially-copyable.""")}
                        constexpr const std::byte* const data() //
                            noexcept(soagen::has_nothrow_data_member<const table_type>);

                        #endif

                        {doxygen(r"@brief Returns a pointer to the elements of a specific column.")}
                        template <size_type Column>
                        SOAGEN_ALIGNED_COLUMN(Column)
                        constexpr column_type<Column>* column() noexcept
                        {{
                            static_assert(Column < table_traits::column_count, "column index out of range");

                            return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, Column>>(table_.template column<Column>());
                        }}

                        {doxygen(r"@brief Returns a pointer to the elements of a specific column.")}
                        template <size_type Column>
                        SOAGEN_ALIGNED_COLUMN(Column)
                        constexpr std::add_const_t<column_type<Column>>* column() const noexcept
                        {{
                            static_assert(Column < table_traits::column_count, "column index out of range");

                            return soagen::assume_aligned<soagen::detail::actual_column_alignment<table_traits, allocator_type, Column>>(table_.template column<Column>());
                        }}'''
                        )
                        for i in range(len(self.columns)):
                            o(
                                rf'''
                             {doxygen(rf"@brief Returns a pointer to the elements in column [{i}]: {self.columns[i].name}.")}
                            SOAGEN_ALIGNED_COLUMN({i})
                            constexpr {self.columns[i].pointer_type} {self.columns[i].name}() noexcept
                            {{
                                return column<{i}>();
                            }}

                            {doxygen(rf"@brief Returns a pointer to the elements in column [{i}]: {self.columns[i].name}.")}
                            SOAGEN_ALIGNED_COLUMN({i})
                            constexpr {self.columns[i].const_pointer_type} {self.columns[i].name}() const noexcept
                            {{
                                return column<{i}>();
                            }}
                            '''
                            )
                        for const in ('', 'const '):
                            o(
                                rf'''
                            {doxygen(r"""
                            @brief Invokes a function once for each column data pointer.

                            @tparam Func A callable type compatible with one of the following signatures:<br>
                                     - `void(auto*, size_type)`
                                     - `void(size_type, auto*)`
                                     - `void(auto*)`

                            @param func The callable to invoke.""")}
                            template <typename Func>
                            constexpr void for_each_column(Func&& func) {const}//
                                noexcept(table_traits::for_each_column_ptr_nothrow_invocable<Func&&{', true'if const else ''}>)
                            {{
                                {rf'{NEWLINE}{o.indent_str*8}'.join([rf'soagen::invoke_with_optarg(static_cast<Func&&>(func), this->template column<{col.index}>(), size_type{{ {col.index} }});' for col in self.columns])}
                            }}
                            '''
                            )

                with DoxygenMemberGroup(o, 'Row access'):
                    with Public(o):
                        for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
                            move = 'std::move' if ref == '&&' else ''
                            move_l = 'std::move(' if ref == '&&' else ''
                            move_r = ')' if ref == '&&' else ''
                            row_type = ('rvalue_' if ref == '&&' else ('const_' if const else '')) + r'row_type'
                            o(
                                rf'''
                                {doxygen(r"""
                                @brief Returns the row at the given index.

                                @tparam Columns Indices of the columns to include in the row. Leave the list empty for all columns.""")}
                                template <size_type... Columns>
                                SOAGEN_PURE_GETTER
                                SOAGEN_CPP20_CONSTEXPR
                                soagen::row_type<{const}{self.name}{ref}, Columns...> row(size_type index) {const}{ref} noexcept
                                {{
                                    if constexpr (sizeof...(Columns))
                                    {{
                                        return {{ {{ {move_l}this->template column<Columns>()[index]{move_r} }}... }};
                                    }}
                                    else
                                    {{
                                        return {move}(*this).template row<{self.column_indices}>(index);
                                    }}
                                }}

                                {doxygen(r"""@brief Returns the row at the given index.""")}
                                SOAGEN_PURE_INLINE_GETTER
                                SOAGEN_CPP20_CONSTEXPR
                                {row_type} operator[](size_type index) {const}{ref} noexcept
                                {{
                                    return {move}(*this).row(index);
                                }}

                                {doxygen(r"""
                                @brief Returns the row at the given index.

                                @throws std::out_of_range""")}
                                SOAGEN_PURE_GETTER
                                SOAGEN_CPP20_CONSTEXPR
                                {row_type} at(size_type index) {const}{ref}
                                {{
                                    #if SOAGEN_HAS_EXCEPTIONS
                                    if (index >= size())
                                        throw std::out_of_range{{ "bad element access" }};
                                    #endif
                                    return {move}(*this).row(index);
                                }}

                                {doxygen(r"""@brief Returns the very first row in the table.""")}
                                SOAGEN_PURE_INLINE_GETTER
                                SOAGEN_CPP20_CONSTEXPR
                                {row_type} front() {const}{ref} noexcept
                                {{
                                    return {move}(*this).row(0u);
                                }}

                                {doxygen(r"""@brief Returns the very last row in the table.""")}
                                SOAGEN_PURE_INLINE_GETTER
                                SOAGEN_CPP20_CONSTEXPR
                                {row_type} back() {const}{ref} noexcept
                                {{
                                    return {move}(*this).row(size()-1u);
                                }}

                            '''
                            )

                if self.iterators:
                    with Public(o):
                        for funcs, group in ((('', 'c'), r'Iterators'), (('r', 'cr'), r'Iterators (reverse)')):
                            reverse = 'r' in funcs
                            if reverse and not self.reverse_iterators:
                                continue
                            with DoxygenMemberGroup(o, group):
                                for func in funcs:
                                    for const, ref in (('', '&'), ('', '&&'), ('const ', '&')):
                                        if 'c' in func and (not const or ref == '&&'):
                                            continue
                                        if ref == '&&' and not self.rvalue_iterators:
                                            continue

                                        move_l = 'std::move(' if ref == '&&' else ''
                                        move_r = ')' if ref == '&&' else ''
                                        begin = rf'{func}end()' if reverse else rf'{move_l}*this{move_r}, 0'
                                        end = (
                                            rf'{func}begin()'
                                            if reverse
                                            else rf'{move_l}*this{move_r}, static_cast<difference_type>(size())'
                                        )

                                        an = 'a reverse' if reverse else 'an'
                                        past = 'before' if reverse else 'past'
                                        first = 'last' if reverse else 'first'
                                        last = 'first' if reverse else 'last'
                                        iterator = rf'soagen::iterator_type<{const}{self.name}{ref}, Columns...>'

                                        o(
                                            rf'''

                                        {doxygen(rf"""@brief Returns {an} iterator to the {first} row in the table.""")}
                                        template <size_type... Columns>
                                        SOAGEN_PURE_INLINE_GETTER
                                        constexpr {iterator} {func}begin() {const}{ref if self.rvalue_iterators else ''} noexcept
                                        {{
                                            return {{ {begin} }};
                                        }}

                                        {doxygen(rf"""@brief Returns {an} iterator to one-{past}-the-{last} row in the table.""")}
                                        template <size_type... Columns>
                                        SOAGEN_PURE_INLINE_GETTER
                                        constexpr {iterator} {func}end() {const}{ref if self.rvalue_iterators else ''} noexcept
                                        {{
                                            return {{ {end} }};
                                        }}
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
