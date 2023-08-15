# Changelog

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
-   Added const propagation to `soagen::row` member functions
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
