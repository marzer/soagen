# Contributing to Soagen

Firstly: thanks! Any help is greatly appreciated.

For most situations the easiest way for you to contribute is to simply let me know what's going on:

-   Reporting issues or requesting features: [Issues]
-   Chat: [Gitter]

If you'd like to contribute more directly via a pull request, see below.

-   [Pull Requests](#pull-requests)
    -   [Getting started](#getting-started)
    -   [Code style](#code-style)

## Pull Requests

### Getting started

A prerequisite of working on Soagen with the intention of making a pull request is to have it installed
as 'editable' from a clone of the repository:

```sh
git clone <your soagen fork>
cd soagen
pip install -r requirements.txt
pip install -e .
```

### Code style

It's Python. I'm primarily a C++ programmer. I really don't care that much.
If you want to be consistent, the codebase is configured for use with [yapf], so you can point your editor
to that as an autoformatter.

I'm not too fussy though. I'm unlikely to reject a PR on the basis of style unless you do something truly horrendous.

<br /><br />

[issues]: https://github.com/marzer/soagen/issues
[gitter]: https://gitter.im/marzer/community
[yapf]: https://github.com/google/yapf
