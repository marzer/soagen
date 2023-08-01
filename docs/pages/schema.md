@page schema Config File Schema

@tableofcontents

This is the config file schema for the `*.toml` configuration files that are used with `soagen`, a command-line tool
for generating SoA container classes for use in C++.

@see <ul>

<li>TOML language usage: [toml.io]
<li>Overview of how to use `soagen`: [Getting started]
</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section schema_root <root>

These options all belong at the root of the config file. **They do not appear under any `[headings]`.**

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_root_allocator allocator

The allocator your SoA classes will use. May be overridden on a per-class basis.

**Type:** string

**Required:** No

**Default:** `soagen::allocator`

**Example:**

```toml
allocator = 'foo::allocator'
```

@see soagen::allocator

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_root_namespace namespace

The namespace your generated code will belong to.

**Type:** string

**Required:** No (but strongly recommended!)

**Default:** None

**Example:**

```toml
namespace = 'game'
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section schema_hpp hpp

These options relate to the generated `.hpp` file. They belong under the `[hpp]` heading.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_banner banner

Text that will be injected _right_ at the top of the `.hpp` file as a block of comments.
Typically used for copyright notices and such.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
banner = 'Copyright (c) Codey McCodeface'
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_brief brief

A brief description of the file. Used to generate a `@brief` directive for Doxygen.

**Type:** string

**Required:** No

**Default:** A simple brief like <i>"Contains the definitions of foo and bar"</i> will be generated for you.

**Example:**

```toml
[hpp]
brief = 'This is where we keep the bodies.'
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_footer footer

Code to be injected at the bottom of the file, after all the other definitions.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
footer = '''
{% namespace::start %}

void foo()
{
	//...
}

{% namespace::end %}
'''
```

@note You _could_ use this to inject functions, but writing C++ code in a toml config file isn't exactly the best
user-experience. If you must do this, best practice would be to only use it for very simple functions like one-liner
conversions, et cetera.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_header header

Code to be injected near the top of the file, after the `#includes` and forward declarations.
Typically you'd use this for constants and type aliases that are used in your structs.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
header = '''
{% namespace::start %}

inline constexpr float default_mass = 1.0f;

{% namespace::end %}
'''
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_includes includes

File paths to be added as `#include` directives.

-   `internal` includes will use the quoted form `#include "foo"`
-   `external` includes will use the angle-bracket form `#include <foo>`

**Type:** string, or array of strings

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
includes.internal = 'quaternion.hpp'
includes.external = [ 'string_view', 'cfloat' ]
'''
```

@note You generally do not need to add includes for `std::` types that are used in your class interface or anywhere
in the generated `.hpp` file; `soagen` will detect these and add the necessary `#include` for you. (Please [file a bug report] if this does not work!)

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_hpp_prologue prologue

Code to be injected near the top of the file, immediately after the internal `#includes`, but before
external `#includes`, forward declarations, or any other definitions. Typically you'd use this to override any config
options that are configured via preprocessor macros.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
prologue = '''
#define SOAGEN_ALWAYS_INLINE inline // disable any forced-inlining
'''
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section schema_structs structs

These options all relate to individual SoA 'structs'. They belong under a heading matching the name of your struct, e.g.

```toml
[structs.particles]
# ...
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_allocator allocator

The allocator this particular struct will use. Overrides the root-level @ref schema_root_allocator.

**Type:** string

**Required:** No

**Default:** `soagen::allocator`

**Example:**

```toml
[structs.particles]
allocator = 'foo::allocator'
```

@see soagen::allocator

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_annotations annotations

Annotations that will be added immediately before your class (i.e. on the line before the C++ keyword `class`),
one per line. Use this to add things like Unreal Engine's `UCLASS()`.

**Type:** string, or array of strings

**Required:** No

**Default:** None

**Example:**

```toml
[structs.particles]
annotations = [ 'UCLASS()' ]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_attributes attributes

Attributes that will be added to your class definition, immediately after the C++ keyword `class`. Use this to add things
like `alignas(N)`.

**Type:** string, or array of strings

**Required:** No

**Default:** None

**Example:**

```toml
[structs.particles]
attributes = [ 'alignas(32)' ]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_brief brief

A brief description of the struct. Used to generate a `@brief` directive for Doxygen.

**Type:** string

**Required:** No

**Default:** A simple brief will be generated for you.

**Example:**

```toml
[structs.particles]
brief = 'Particles in the effects system.'
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_copyable copyable

Used to disable the copy constructor + assignment operator if necessary.

**Type:** boolean

**Required:** No

**Default:** `true` (they are both `= default;`)

**Example:**

```toml
[structs.particles]
copyable = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_default_constructible default_constructible

Used to disable the default constructor if necessary.

**Type:** boolean, or string value `auto`

**Required:** No

**Default:** `true` (default constructor is `= default;`)

**Example:**

```toml
[structs.particles]
default_constructible = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_details details

A detailed description of the struct. Used to generate a `@details` directive for Doxygen.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[structs.particles]
details = '''Lorem ipsum, et cetera.'''
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_epilogue epilogue

Code to be injected into the `.hpp` file directly after your class definition.
This code at @ref schema_root_namespace scope.

**Type:** string

**Required:** No

**Default:** None

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_footer footer

Code to be injected at the bottom of the class, after all the type + function definitions.
This code will be at class scope.

**Type:** string

**Required:** No

**Default:** None

@note You _could_ use this to inject functions, but writing C++ code in a toml config file isn't exactly the best
user-experience. If you must do this, best practice would be to only use it for very simple functions like one-liner
conversion operators, et cetera.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_header header

Code to be injected at the top of the class, after the internal typedefs and constants.
This code will be at class scope.

**Type:** string

**Required:** No

**Default:** None

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_iterators iterators

Used to disable the generation of all `iterator`-related functions and typedefs if necessary.

**Type:** boolean

**Required:** No

**Default:** `true`

**Example:**

```toml
[structs.particles]
iterators = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_movable movable

Used to disable the move constructor + assignment operator if necessary.

**Type:** boolean

**Required:** No

**Default:** `true` (they are both `= default;`)

**Example:**

```toml
[structs.particles]
movable = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_prologue prologue

Code to be injected into the `.hpp` file directly before your class definition.
This code will be at @ref schema_root_namespace scope.

**Type:** string

**Required:** No

**Default:** None

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_rvalue_iterators rvalue_iterators

Used to disable the generation of all `rvalue_iterator`-related functions and typedefs if necessary.

**Type:** boolean

**Required:** No

**Default:** `true`

**Example:**

```toml
[structs.particles]
rvalue_iterators = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_static_variables static_variables

Static variables to add as part of your class definition. See @ref schema_static_variables

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_swappable swappable

Used to disable the generation of member and non-member `swap()` functions and typedefs if necessary.

**Type:** boolean

**Required:** No

**Default:** `true`

**Example:**

```toml
[structs.particles]
swappable = false
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_structs_variables variables

Describes the variable 'columns' your struct will have. See @ref schema_variables

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section schema_static_variables static_variables

These options are for specifying static variables to add as part of your class definition. They are all KVP's in
individal tables in your struct's `static_variables`, e.g.:

```toml
[structs.particles]
static_variables = [
	{ name = '...', type = '...', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_access access

The access level of the static variable.

**Type:** string - one of `public`, `protected` or `private`.

**Required:** No

**Default:** `public`

**Example:**

```toml
[structs.particles]
static_variables = [
	{ ..., access = 'protected', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_brief brief

A brief description of the variable. Used to generate a `@brief` directive for Doxygen.

**Type:** string

**Required:** No

**Default:** None

**Example:**

```toml
[hpp]
	{ ..., brief = 'The default mass used for particles.', ... }
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_const const

The `const`-ness of the static variable.

**Type:** string - one of `const`, `constexpr`, or a boolean.

**Required:** No

**Default:** `constexpr`

**Example:**

```toml
[structs.particles]
static_variables = [
	{ ..., const = false, ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_name name

The name of your static variable.

**Type:** string

**Required:** Yes

**Example:**

```toml
[structs.particles]
static_variables = [
	{ ..., name = 'default_mass', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_type type

The type of the static variable.

**Type:** string

**Required:** Yes

**Example:**

```toml
[structs.particles]
static_variables = [
	{ ..., type = 'float', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_static_variables_value value

The value used to intialize the static variable.

**Type:** string, integer or float

**Required:** Yes

**Example:**

```toml
[structs.particles]
static_variable = [
	{ ..., value = 1.0, ... }
]
```

@attention If you specify this value as a string, it is used _verbatim_. The result must make syntactic sense!

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section schema_variables variables

These options are for specifying the variable 'columns' your SoA class will have. They are all KVP's in individal
tables in your struct's `variables`, e.g.:

```toml
[structs.particles]
variables = [
	{ name = '...', type = '...', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_variables_alignment alignment

The minimum memory alignment of this column's region in memory.

**Type:** integer

**Required:** No

**Default:** `alignof(value_type)`

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_variables_default default

The default value passed to parameters for this column in `push_back()` and friends.

**Type:** string, integer or float

**Required:** No

**Default:** None

**Example:**

```toml
[structs.particles]
variables = [
	{ ..., default = '{}', ... }
]
```

@attention If you specify this value as a string, it is used _verbatim_. The result must make syntactic sense!

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_variables_name name

The name of your variable 'column'.

**Type:** string

**Required:** Yes

**Example:**

```toml
[structs.particles]
variables = [
	{ ..., name = 'position', ... }
]
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_variables_param_type param_type

The `param_type` of your column.
Determines the type used to pass values for the column as a parameter to functions like `push_back()`.

**Type:** string

**Required:** No

**Default:** `soagen::param_type<value_type>`

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection schema_variables_type type

The `value_type` of your variable 'column'.

**Type:** string

**Required:** Yes

**Example:**

```toml
[structs.particles]
variables = [
	{ ..., type = 'vec3', ... }
]
```

@note Unlike std::vector, it OK to have a `const`-qualified `value_type` for a column.
`const` columns won't be stored as `const` internally, but all external access to them will be via `const`-qualified
pointers or references.

@note It's also OK to have move-only `value_types`; the only negative effect is that it inhibits the generation of some
member functions (e.g. `resize()`).

<!-- --------------------------------------------------------------------------------------------------------------- -->

[toml.io]: https://toml.io/en/
[Getting started]: index.html#intro_getting_started
[file a bug report]: https://github.com/marzer/soagen/issues
