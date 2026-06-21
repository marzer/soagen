# Changelog

## v0.8.0

-   Fixed the generator crashing on startup with recent versions of the `schema` package (#3, @ludditeongithub)
-   Fixed default option values (e.g. `attributes`, `mixins`) being shared between structs generated in a single invocation
-   Fixed the `param_type` column option being emitted into the wrong template slot, causing a compile error
-   Fixed the generated `.natvis` referencing a non-existent allocation member (debugger visualizers were broken)
-   Fixed table and span less-than comparisons (`<`, `<=`, `>`, `>=`) being non-lexicographic and intransitive
-   Fixed a compile error during copy-assignment when using a propagating or non-always-equal allocator
-   Fixed the `std::start_lifetime_as` fast-path for implicit-lifetime types never being used
-   Fixed SoA types failing to compile on recent Clang with "ambiguous partial specializations"
-   Fixed `soagen::compressed_pair` computing an incorrect alignment on MSVC
-   Fixed a compile error with `memmove` when instantiating an all-trivially-copyable SoA
-   Fixed a compile error calling `data()` on a `const` all-trivially-copyable SoA
-   Fixed a compile error in the strong-exception-guarantee reallocation path for element types that are copyable but not nothrow-movable
-   Fixed `at()` being marked as a pure function despite throwing on out-of-range access
-   Fixed `at()` being present even when exceptions are disabled
-   Fixed `insert()`/`emplace()` at a non-end position leaking an element when an element operation threw mid-operation
-   Fixed the generator updating file timestamps even if the content was unchanged
-   Fixed a generated file occasionally being read half-written by a concurrent build step
-   Fixed `reserve()`/`resize()`/`resize_for_overwrite()` allocation and alignment calculation bugs
-   Fixed the copy constructor taking its allocator from the destination's own uninitialised allocator instead of the source's
-   Fixed `max_size()` being computed with the wrong per-column alignment
-   Fixed `<`, `<=`, `>`, `>=` on tables/spans/SoAs being incorrectly `noexcept` in some circumstances
-   Fixed `emplace()`/`emplace_back()` failing to compile when two column names map to the same PascalCase template parameter
-   Fixed the generated `.natvis` casting column types unqualified
-   Fixed generated files silently overwriting each other when two configs output path
-   Fixed generated files being needlessly rewritten on every build when checked out with CRLF line endings
-   Added support for compiling with exceptions disabled (e.g. `-fno-exceptions`) (#5, @pgrAm)
-   Added support for passing a column enumerator (e.g. `my_soa::columns::id`) to `row::get<>()`, matching `column<>()`
-   Added comparison operators (`==`, `!=`, `<`, `<=`, `>`, `>=`) to `span`, and made SoA comparisons heterogeneous
-   Added `rvalue_row_type<>`
-   Added `rvalue_iterator_type<>`
-   Added `rvalue_span_type<>`
-   Added `source_offset` customization point for mix-ins
-   Added a `--output` (`-o`) command-line option to control where generated files are written (e.g. an ephemeral build directory)
-   Added `resize_for_overwrite()` for growing a table without initializing the new trivially-constructible elements (#4, @pgrAm)
-   Added validation for duplicate variable names, structs with no variables, and configs with no structs
-   Added support for invoking the generator as `python -m soagen`
-   Removed `tomli` requirement for Python 3.11 and later

## v0.7.0

-   Fixed rvalue row corruption bug on MSVC ([info](https://developercommunity.visualstudio.com/t/C:-Corrupt-references-when-creating-a/10446877))
-   Fuxed some SFINAE issues
-   Added `Cols...` selector template parameter to `at()`, `front()` and `back()`
-   Added `structs.mixins`
-   Added copy-based fallbacks for `unordered_erase()`, `insert()`, `emplace()` and `swap_columns()` (previously they required movability)

## v0.6.0

-   Fixed `swap_columns()` fast-path breaking `data()`
-   Added spans
    -   Added `span`
    -   Added `is_span<>`
    -   Added `table::subspan()` and `table::const_subspan()`
    -   Added `span_type<>` and `const_span_type<>`
-   Added `soa_type<>`
-   Added `const_iterator_type<>`
-   Added `const_row_type<>`
-   Added `column_indices<>`
-   Added `soagen::for_each_column()`
-   Added `row::for_each_column()`
-   Added generic names `first`, `second`, ..., `sixteenth` for unnamed columns 0-15
-   Binary size improvements
-   Documentation improvements

## v0.5.0

-   Added rows to `soagen::table`
-   Added iterators to `soagen::table`
-   Added `soagen::table::for_each_column()`

## v0.4.0

-   Fixed `soagen::is_table<>`
-   Added support for emplace-constructing column values by unpacking all `std::tuple`-like types (not just the `emplacer`)
-   Added support for taking `std::integral_constants` in `for_each_column()`
-   Added `soagen::same_table_type<>`
-   Added conversions between `soagen::row<>` specializations
-   Optimized instantiation overhead for most type-traits

## v0.3.0

-   Added `hpp.combined`
-   Added `std::integral_constant<size_t>` to the overload set used by `for_each_column()`
-   Added support for constructing rows from all `std::tuple`-like types
-   Optimized bulk-swap operations

## v0.2.0

-   Added `structs.annotations`
-   Added `structs.attributes`
-   Added `auto` option for `structs.default_constructible`
-   Added `soagen::row_base`
-   Added `soagen::table_base`
-   Added `soagen::iterator_base`
-   Added `Base` template argument to `soagen::table` for CRTP
-   Added `swap_columns<>()`
-   Made `column_indices` member struct into `enum class columns`

## v0.1.2

-   Minor refactors.

## v0.1.1

-   Minor refactors.

## v0.1.0

-   First public release 🎉&#xFE0F;
