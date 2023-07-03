# soagen

Struct-of-Arrays generator for C++ projects.

[![Sponsor](https://img.shields.io/static/v1?label=sponsor&message=%E2%9D%A4&logo=GitHub&color=%23fe8e86&style=flat-square)][sponsor]
[![Gitter](https://badges.gitter.im/marzer/community.svg)][gitter]

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
usage: soagen [-h] [-v] [--version] [--werror | --no-werror] [--clang-format | --no-clang-format] [--bug-report] files [files ...]

  ___  ___   __ _  __ _  ___ _ __
 / __|/ _ \ / _` |/ _` |/ _ \ '_ \
 \__ \ (_) | (_| | (_| |  __/ | | |
 |___/\___/ \__,_|\__, |\___|_| |_|
                   __/ |
                  |___/   v0.1.0 - github.com/marzer/soagen

Struct-of-Arrays generator for C++ projects.

positional arguments:
  files

options:
  -h, --help            show this help message and exit
  -v, --verbose         enable very noisy diagnostic output
  --version             print the version and exit
  --werror, --no-werror
                        treat warnings as errors (default: False)
  --clang-format, --no-clang-format
                        run clang-format on generated code if it is available (default: True)
  --bug-report          captures all inputs and output in a zip file for easier bug reporting.
```

## Config file options

See the [Configuration options] wiki page.

<br><br>

## License and Attribution

This project is published under the terms of the [MIT license](https://github.com/marzer/soagen/blob/main/LICENSE.txt).

[configuration options]: https://github.com/marzer/soagen/wiki/Configuration-options
[feature request]: https://github.com/marzer/soagen/issues/new
[gitter]: https://gitter.im/marzer/community
[sponsor]: https://github.com/sponsors/marzer
