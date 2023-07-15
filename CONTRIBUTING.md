# Contributing to Soagen

Firstly: thanks! Any help is greatly appreciated.

For most situations the easiest way for you to contribute is to simply let me know what's going on:

-   Reporting issues or requesting features: [Issues]
-   Chat: [Gitter]

If you'd like to contribute more directly via a pull request, see below.

<br/>

## Pull Requests

### Getting started

A prerequisite of working on Soagen with the intention of making a pull request is to have it installed
as 'editable' from a clone of the repository:

```sh
git clone <your soagen fork>
cd soagen
pip install -e .
```

<br/>

### Python contributions

#### Code style

It's Python. I'm primarily a C++ programmer. I really don't care that much.
If you want to be consistent, the codebase is configured for use with [black], so you can point your editor
to that as an autoformatter.

#### Source files

Python source files can be found in `src/soagen`.

<br/>

### C++ contributions

#### Code style

The codebase has a `.clang-format` config. So long as you make use of it, we're golden.

#### Source files

C++ source files can be found in `src/soagen/hpp`.

#### Modifying `soagen.hpp`

There are two versions of `soagen.hpp` in the codebase:

-   `src/soagen/hpp/single/soagen.hpp`:<br>
    This is the one used when a user invokes `--install`. It is preprocessed from other headers into one,
    to simply the end-user experience. **Do not edit it directly - your changes will be lost!**

-   `src/soagen/hpp/soagen.hpp`:<br>
    This one is the entry-point for the preprocessor that generates the single-header version, and doesn't contain
    any code. It's just a bunch of `#include` directives. There's nothing to edit! 😄&#xFE0F;

The _actual_ way to contribute changes to `soagen.hpp` is:

1. Near the code you wish to change in `soagen.hpp` will be a banner indicating the `.hpp` file responsible for that
   part of the file. Find it in `src/soagen/hpp` and edit it directly.
2. Regenerate the single-header version of `soagen.hpp` using the 'hidden' developer option `soagen --update`
   (working directory does not matter, this should work from anywhere).
3. _Somewhat-optinal step:_ There's some tests set up in `tests/`. They use Catch2. Any pull-requests that also update
   the tests are going to move much more quickly!

[issues]: https://github.com/marzer/soagen/issues
[gitter]: https://gitter.im/marzer/community
[black]: https://pypi.org/project/black/
