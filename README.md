# soagen

Struct-of-Arrays generator for C++ projects.

[![Sponsor](docs/images/badge-sponsor.svg)][sponsor]
[![Gitter](docs/images/badge-gitter.svg)][gitter]

-   [Installation](#installation)
-   [Usage](#usage)
-   [Config file options](#config-file-options)
-   [License and Attribution](#license-and-attribution)

<br><br>

## Installation

### Prerequisites:

-   Python 3.9+

### Then:

```
pip install soagen
```

<br><br>

## Usage

Soagen is a command-line application.

```
usage: soagen [-h] [-v] [--version] [--install <dir>] [--werror | --no-werror]
              [--color | --no-color] [--clang-format | --no-clang-format]
              [--doxygen | --no-doxygen] [--natvis | --no-natvis] [--bug-report]
              [configs ...]

  ___  ___   __ _  __ _  ___ _ __
 / __|/ _ \ / _` |/ _` |/ _ \ '_ \
 \__ \ (_) | (_| | (_| |  __/ | | |
 |___/\___/ \__,_|\__, |\___|_| |_|
                   __/ |
                  |___/   v0.0.2 - marzer.github.io/soagen

Struct-of-Arrays generator for C++ projects.

positional arguments:
  configs               zero or more .toml files describing your structures-of-arrays
                        (wildcards are accepted, e.g. soa/*.toml)

options:
  -h, --help            show this help message and exit
  -v, --verbose         enable very noisy diagnostic output
  --version             print the version and exit
  --install <dir>       install soagen.hpp into a directory
  --werror, --no-werror
                        treat warnings as errors (default: False)
  --color, --no-color   use colors in terminal output (default: True)
                        (the British spelling "colour" is also accepted)
  --clang-format, --no-clang-format
                        attempt to run clang-format on generated code (default: True)
  --doxygen, --no-doxygen
                        include doxygen markup in the generated code (default: False)
  --natvis, --no-natvis
                        generate .natvis files for Visual Studio (default: True)
  --bug-report          capture all inputs and outputs in a bug-report zip file
```

<br><br>

## Config file options

See the [Configuration options] wiki page.

<br><br>

## License and Attribution

This project is published under the terms of the [MIT license](https://github.com/marzer/soagen/blob/main/LICENSE.txt).

[configuration options]: https://github.com/marzer/soagen/wiki/Configuration-options
[feature request]: https://github.com/marzer/soagen/issues/new
[gitter]: https://gitter.im/marzer/community
[sponsor]: https://github.com/sponsors/marzer
