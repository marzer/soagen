@mainpage soagen: A Structure-of-Arrays generator for C++

@tableofcontents

@section intro_tldr TL;DR

The leading section of this page is an overview of what [Structures-of-Arrays (SoA)](https://en.wikipedia.org/wiki/AoS_and_SoA)
are, what problems they solve, and the annoyances of working with them in C++. Following that is an overview of
**soagen** - a new Structure-Of-Arrays generator and library for C++17 and later.

@inline_success Skip to @ref intro_introducing_soagen if you already know all about SoA and want to go
straight to learning about **soagen** instead.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_motivation Motivation

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection intro_motivation_typical Typical data layouts (Array-of-Structures)

Data records in a typical C++ program will be organized into `structs` and/or `classes`, one per 'unit' type,
and multiples of these will be stored in arrays. For example, a program for managing employee records
might contain something akin to this:

```cpp
struct employee
{
	unsigned id;
	std::string	name;
	std::tuple<unsigned short, unsigned char, unsigned char> dob;
	int salary;
	// ... plus a bunch more
};
```

And elsewhere in the program you'd almost certainly find this:

```cpp
std::vector<employee> employees;
```

This paradigm is broadly called [Array-of-Structures](https://en.wikipedia.org/wiki/AoS_and_SoA) \(AoS\).
Stored this way, the all employee members are laid out sequentially in memory, so the `employees` array
would look like this:

@parblock
\[parent_add_class inset_table\]

\[parent_set_id table_employees_layout\]

<table>
<tr><th colspan="4"> employees[0] <th colspan="4"> employees[1] <th> &rarr;
<tr><td> `id` <td> `name` <td> `dob` <td> `salary`
    <td> `id` <td> `name` <td> `dob` <td> `salary`
	<td> &rarr;
</table>
@endparblock

When iterating over the `std::vector` depicted above, the CPU's cache will be mostly (or entirely) filled by
only a single `employee` at all times. This is likely fine - most tasks pertaining to `employee` objects are going to
want to access multiple data members, so the linear layout is useful. Rare would be the situation where you'd need to loop
over the `employees` and fetch only a single field.

What about scenarios where you _do_ want to only manipulate a single field from many objects, though? Let's say the
`std::vector` example above had 1,000,000 `employee` elements and you wanted to query the total labour cost of your
enormous company by summing up the `salary` member. Suddenly all those cache line misses start to add up and
your `calculate_labour_cost()` routine is now much slower than you'd like.

@inline_remark <i>Yes, yes, you wouldn't use C++ for this. It'd be in SQL or similar. This is an example.
Put down your pitchforks.</i>

Still, in the general case, the access patterns for an `employee` would benefit from the current layout
far more than any gains made by restructuring to solve this one specific problem (doing so is likely to pessimize the
rest of the program). Also consider that calls to `calculate_labour_cost()` would be infrequent
and the result could be cached, making this problem even less worth solving.

Let's have a look at a situation where the problem very much _is_ worth solving.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsection intro_motivation_soa Structure-of-Arrays

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsubsection intro_motivation_soa_naive A naïve implementation

Consider a game engine: game worlds are populated by entities, those entities have various characteristics (position,
orientation, mesh, id, name, et cetera). Suppose we had those encapsulated as an `entity` struct:

```cpp
struct entity
{
	unsigned id;
	std::string	name;
	vec3 pos;
	quaternion orient;
	// ... and so on
};
```

As before, lets have a look at what an array of these would look like in memory:

```cpp
std::vector<entity> entities;
```

@parblock
\[parent_add_class inset_table\]

\[parent_set_id table_entities_layout\]

<table>
<tr><th colspan="4"> entities[0] <th colspan="4"> entities[1] <th> &rarr;
<tr><td> `id` <td> `name` <td> `pos` <td> `orient`
    <td> `id` <td> `name` <td> `pos` <td> `orient`
	<td> &rarr;
</table>
@endparblock

Game worlds feature many thousands of entities. Rendering these can be expensive, so great effort goes into ensuring
the engine does not render anything unnecessarily. One technique for eliminating entities from the render list is to
cull those that do not intersect with the camera's [view frustum](https://en.wikipedia.org/wiki/Viewing_frustum). This
necessitates iterating over all the entities in the game world at least once every frame (assisted by a bounding volume
hierarchy or other acceleration structure). Unlike the `employee` program above, taking constant cache misses during
this iteration by reading in entire `entity` structs is potentially devastating to our frame time!

Ideally we want to _only_ read in the parts of the entities we care about (e.g. `entity::pos`), without dragging
anything else into the cache.

Enter: [emoji sparkles] [Structure-of-Arrays](https://en.wikipedia.org/wiki/AoS_and_SoA). [emoji sparkles]

If we restructured our entities into a series of parallel `std::vectors`, we could iterate over just the positions:

```cpp
struct entities
{
	std::vector<unsigned> id;
	std::vector<std::string> name;
	std::vector<vec3> pos;
	std::vector<quaternion> orient;
	// ... and so on
};
```

Now our data is conceptually more like a table, with columns for each characteristic (stored contiguously),
and rows for each individual element:

@parblock
\[parent_add_class inset_table\]

\[parent_set_id table_entities_layout_soa\]

<table>
<tr><th>            <th> id       <th> name       <th> pos      <th> orient
<tr><th> element 0  <td> `id[0]`  <td> `name[0]`  <td> `pos[0]` <td> `orient[0]`
<tr><th> element 1  <td> `id[1]`  <td> `name[1]`  <td> `pos[1]` <td> `orient[1]`
<tr><th> element 2  <td> `id[2]`  <td> `name[2]`  <td> `pos[2]` <td> `orient[2]`
<tr><th> &darr;     <td> &darr;   <td> &darr;     <td> &darr;   <td> &darr;
</table>
@endparblock

We've lost the explicit `entity` class for representing one single game world entity, and have instead transitioned to
an implicit object model where an entity is described by all the elements sharing the same index in the parallel
`std::vectors`. As well as being faster for single-element iteration, this also has another neat side-effect:
no structs means no padding between struct members, so lower memory usage!

[emoji warning] There are some big problems with this naïve SoA implementation, though.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@subsubsection intro_motivation_soa_multiple_allocation Problem #1: Multiple allocations

One heap allocation for each member `std::vector`. Sure we could come up with a custom `allocator` that works from the same
internal buffer, but that's not for the faint of heart.

@inline_remark <i>No doubt there's some solution for this in `std::pmr`. I'll leave that as an exercise for the reader.</i>

@subsubsection intro_motivation_soa_manual_sync Problem #2: Manual Synchronization

Now that we have multiple arrays, we must ensure that they are all updated in unison whenever we insert, modify,
or delete elements. Forgetting to do this at any point will mean the parallel members are no longer in sync and lead to
disastrous and hilarious effects.

@subsubsection intro_motivation_soa_iterators_weakly_typed Problem #3: Identities are weakly-typed

We no longer have the benefit of the strongly-typed `std::vector<entity>::iterator` (or even just the ability to take
a pointer, `entity*`), so any time we need to store an association between a specific entity and some other thing,
it needs to be done via an index (e.g. `size_t`). This is much more error-prone; it's all-too-easy to
accidentally treat an index into one collection as an index into another.

Using a 'strong type' library (like
[this one](https://github.com/rollbear/strong_type)) or using an `enum class` can help here, but that still means
you need `static_casts` (or some other conversion function) everywhere, which is _very annoying_.

@subsubsection intro_motivation_soa_struct_mode_hard Problem #4: AoS-style access is cumbersome

There will always be situations where you need to treat your SoA data as if it were AoS. Debug printing, for example.
Code that needs to work in AoS mode is now littered with `operator[]` invocations, making it uglier and more error prone.

@subsubsection intro_motivation_soa_idiomatic Problem #5: Not idiomatic C++

AoS-style data means we can call `std::vector<>::push_back()` to add an entirely new self-contained record. We can call
`std::sort()` on the elements in that vector. We can get the `begin()` and `end()` iterators. And so on. These are idioms
all C++ programmers are innately familiar with. SoA-style data can make these tasks much harder, typically necessitating
a new set of internal idioms and reducing the understandability of the code.

@subsubsection intro_motivation_soa_names Problem #6: Elegance or Names - pick one

There are a number of solutions for the problems described above floating around the internet. All of them boil down to
one of two approaches:

1. Most/all of the problems above are solved using a nice template syntax and some specialization tricks, but you lose
   the names of members and instead need to fall-back to the incredibly unfrendly std::tuple-like `entities.get<0>()[N]`.
2. You get names, yay! But now your codebase is filled with macros, boo.

Of the two options above, I generally prefer \#2, though having to update lists of template specialization macros whenever
you make changes to an SoA container is also a source of error. Without language-level reflection facilities, any solution
with names is going to have macros _somewhere_ (or make sacrifices elsewhere, e.g. mandating a very recent version of C++).

@parblock

\[parent_set_name aside\]

\[parent_set_class m-note m-special\]

We're done explaining AoS vs SoA, but if these concepts were new for you and you'd like to learn more about them before
you move on to read about **soagen**, here's some resources:

-   [AoS and SoA](https://en.wikipedia.org/wiki/AoS_and_SoA) - Wikipedia
-   [Implementing a semi-automatic structure-of-arrays data container](https://blog.molecular-matters.com/2013/10/22/implementing-a-semi-automatic-structure-of-arrays-data-container/) - Molecular Matters
-   [Memory Layout Transformations](https://www.intel.com/content/www/us/en/developer/articles/technical/memory-layout-transformations.html) - Intel
-   [Structure of Arrays vs Array of Structures in CUDA](https://saturncloud.io/blog/structure-of-arrays-vs-array-of-structures-in-cuda/) - SaturnCloud

@endparblock

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_introducing_soagen Introducing soagen

I'd to present my solution to the problems of working with Structure-of-Arrays in C++: **soagen**. Soagen is fundamentally two things:

1. [`soagen`], a command-line code utility for generating bespoke `std::vector`-like SoA containers, and
2. [`soagen.hpp`], a single-header backing library upon which the generated code depends.

Typically you only need to use the command-line tool [`soagen`], and don't need to know anything about [`soagen.hpp`]
beyond <i>"this needs to be on my include path somewhere"</i> (since it's largely an implementation detail).
You may need to learn more about the backing library if you're doing more advanced stuff, but we'll cover that later.

I'll go over how to use [`soagen`], and the code it generates, by demonstrating how to we'd use it to create a nice SoA
version of the `entity` class [described above](#intro_motivation_soa_naive).

@inline_remark <i>I stylize the name as 'soagen' (all lowercase), only capitalizing the S
when it's the first word in a sentence, but I don't feel strongly about it. Render it however you like, I'm not fussed.
SoAgen, Soagen, SOAgen, whatever.</i>

@inline_success If you have no interest in using a command-line tool to generate SoA classes and instead want to
build your own using [`soagen.hpp`] directly, skip to @ref intro_using_without_generator. The rest of the page is
probably worth a read just for context, though.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_getting_started Getting started

@subsubsection intro_getting_started_prerequisites Prerequisites

-   Using the generator: Python 3.9 or higher
-   Using the C++ library and/or generated code: C++17 or later

@subsubsection intro_getting_started_generator Installing soagen

[`soagen`] is a command-line application written in Python. Install it using `pip`:

@m_class{m-console}

```plaintext
> pip install soagen
```

After that, let's take a look at the help output:

@m_class{m-console}

```plaintext
> soagen --help

usage: soagen [-h] [-v] [--version] [--install <dir>] [--werror | --no-werror]
              [--color | --no-color] [--clang-format | --no-clang-format]
              [--doxygen | --no-doxygen] [--natvis | --no-natvis] [--bug-report]
              [configs ...]

  ___  ___   __ _  __ _  ___ _ __
 / __|/ _ \ / _` |/ _` |/ _ \ '_ \
 \__ \ (_) | (_| | (_| |  __/ | | |
 |___/\___/ \__,_|\__, |\___|_| |_|
                   __/ |
                  |___/   v0.2.0 - marzer.github.io/soagen

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

So we need to do two things:

1. Tell `soagen` about the structs we want via a `.toml` file
2. Install [`soagen.hpp`] somewhere (e.g. `my_game/src`)

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_creating_a_class Creating an SoA container class

`soagen` creates the output `XXXX.hpp` next to each `XXXX.toml` input you pass in. Given a game engine project with a fairly standard file structure:

```plaintext
docs/
include/
    quaternion.hpp
    vector.hpp
	...
src/
README.md
... etc
```

...if we want [`entities.hpp`] to end up in `src/` because it's an implementation detail, then that's where the `soagen`
configuration file `entities.toml` should go, too. Let's create `src/entities.toml` and populate it thus:

```toml
namespace = 'game'

[structs.entities]
variables = [
	{name = 'id',     type = 'unsigned' },
	{name = 'name',   type = 'std::string', default = '""' },
	{name = 'pos',    type = 'vec3',        default = '{}' },
	{name = 'orient', type = 'quaternion',  default = '{1, 0, 0, 0}' },
]
```

@inline_remark <i>There are many more options in the @ref schema but for now we'll just stick with the basics.</i>

Now run `soagen`:

@m_class{m-console}

```plaintext

> soagen src/*.toml

soagen v0.2.0
Reading src/entities.toml
Running clang-format for src/entities.hpp
Writing src/entities.hpp
Writing src/entities.natvis
All done!
```

We now have [`src/entities.hpp`] containing the definition of `game::entities`, a std::vector-like SoA container class
([here's the documentation](classsoagen_1_1examples_1_1entities.html) if you'd like to see the API).

@note Soagen has also generated a [`.natvis`] file for working with Visual Studio, though this isn't mandatory - it
can be disabled by passing `--no-natvis`.

Our project now looks like this:

```plaintext
docs/
include/
    quaternion.hpp
    vector.hpp
src/
    entities.hpp     <
    entities.natvis  <----- new
    entities.toml    <
README.md
```

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_installing_soagen_hpp Installing soagen.hpp

Inspecting the contents of [`entities.hpp`], we see that right near the top it expects to be able to
`#include <soagen.hpp>`. This means we need to 'install' it somewhere. The command-line utility is responsible for that,
too:

@m_class{m-console}

```plaintext
> soagen --install src

soagen v0.2.0
Copying soagen.hpp to src
All done!
```

Our project now looks like this:

```plaintext
docs/
include/
    quaternion.hpp
    vector.hpp
src/
    entities.hpp
    entities.natvis
    entities.toml
    soagen.hpp    <----- new
README.md
```

You can also combine the source generation and install steps in the same command invocation:

@m_class{m-console}

```plaintext
> soagen src/*.toml --install src
```

This is the same as doing them both separately.

@note [`soagen.hpp`] does not have to be installed inside your source tree, of course. You could install it wherever
you like. It does make sense to keep it side-by-side with your generated code, though, because the generated code is
always going make fairly exacting assumptions about the version of the header it was paired with.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_configuring_includes Configuring #includes

Taking another look at the contents of [`entities.hpp`], we see that `soagen` has detected the use of `std::string`
in the class interface and automatically added `#include <string>` for us. This will work for most standard library types
(please [file a bug report] if not!), but not for types specific to our codebase. This means that we'd need to add
an `#include` to make the `vec3` and `quaternion` types visible. Fortunately `soagen` knows how to do that.

Assuming that these math types live in `include/vector.hpp` and `include/quaternion.hpp`, and
`include/` is configured as one our include paths in our project build system, then we need to add the following to
our `entities.toml`:

```toml
[hpp]
includes.internal = [
	'vector.hpp',
	'quaternion.hpp'
]
```

After regenerating [`entities.hpp`] we'll see this:

```cpp
#include "vector.hpp"
#include "quaternion.hpp"
```

Now the compiler will be happy and we can start writing some code :)

@note `includes.internal` is, as the name suggests, for `#includes` that are internal to your project - they use the
double-quoted form of include directives (e.g. `#include "foo.hpp"`). There also exists `includes.external` for things
that are external to your project - these will use the angle-bracket form `#include <foo.hpp>`.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_nomenclature A brief aside: terminology

From here down there will be a slight change in terminology:

@parblock

\[parent_add_class inset_table\]

\[parent_set_id table_terminology\]

<table>
<tr><th> What I say <th> What it means
<tr><td> Table      <td> `soagen`-generated SoA container class
<tr><td> Column     <td> A contiguous subarray of one data type inside a `soagen`-generated SoA container class
<tr><td> Row        <td> A single 'element' in a `soagen`-generated SoA container class
</table>
@endparblock

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_adding_rows Adding rows

Now then, lets do what we came here to do: stick entities in our new container! Tables have all the same insertion
operations as std::vector, only with multiple parameters:

```cpp
// assuming you have #included "entities.hpp" somewhere:

game::entities e;

// push_back works as you'd expect
e.push_back(0, "foo", {0,0,0}, {1,0,0,0});

// it will also take advantage of the defaults
// we specified in the config, allowing us to elide
// some initializers:
e.push_back(1, "bar");

// insert has the same semantics too
e.insert(e.end(), 2, "qux", {1,2,3});

// ... but can also take a size_type as the position
// argument, not just iterators
e.insert(e.size(), 3, "kek");

for (auto&& row : e)
	std::cout << row.id << ": " << row.name << "\n";
```

@out
0: foo
1: bar
2: qux
3: kek
@eout

We also have `emplace()` and `emplace_back()` just like std::vector, but there's a catch: the `std::vector` versions
are variadic to allow for constructing a single value with any number of constructor arguments.
Since we have multiple values per row, there's no way we can have a variadic `emplace()`/`emplace_back()`
that somehow deduces which arguments go to which values. We need to add an extra level of indirection for that:
the soagen::emplacer:

```cpp
using soagen::emplacer;

e.emplace_back(4, emplacer{ 10, 'A' }, vec3{0,0,0}, quaternion{1,0,0,0});

// defaults also work with emplace and emplace_back()
e.emplace_back(5, emplacer{ 10, 'B' });

for (auto&& row : e)
	std::cout << row.id << ": " << row.name << "\n";
```

@out
0: foo
1: bar
2: qux
3: kek
4: AAAAAAAAAA
5: BBBBBBBBBB
@eout

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_removing_rows Removing rows

Soagen tables support `pop_back()`, `erase()` and `resize()` just like std::vector,
and also [`unordered_erase()`] for fast erasure when order doesn't matter:

```cpp

// erase BBBBBBBBBB
e.pop_back();

// erase AAAAAAAAAA
e.erase(e.end() - 1);

// erase kek (erase() also supports size_type)
e.erase(e.size() - 1);

// erase foo quickly using swap-and-pop:
e.unordered_erase(e.begin());

for (auto&& row : e)
	std::cout << row.id << ": " << row.name << "\n";
```

@out
2: qux <---- this was moved here by unordered_erase()
1: bar
@eout

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_capacity Capacity

Soagen tables have `capacity()`, `max_capacity()`, `reserve()`, and `shrink_to_fit()` as you'd expect:

```cpp

std::cout << "size: " << e.size() << "\n";
std::cout << "capacity: " << e.capacity() << "\n\n";

e.reserve(100);
std::cout << "size: " << e.size() << "\n";
std::cout << "capacity: " << e.capacity() << "\n\n";

e.shrink_to_fit();
std::cout << "size: " << e.size() << "\n";
std::cout << "capacity: " << e.capacity() << "\n\n";


```

@out
size: 2
capacity: 6

size: 2
capacity: 100

size: 2
capacity: 2
@eout

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_columns Working with columns

Recall that our table has four columns:

-   `id: unsigned`
-   `name: std::string`
-   `pos: vec3`
-   `orient: quaternion`

Since we don't have just one element type like a std::vector, we don't access these via `data()` - you use their names!
Each returns a pointer to their respective column's data:

```cpp

for (std::size_t i = 0; i < e.size(); i++)
	std::cout << "id " << i << ": " << e.id()[i] << "\n";

for (std::size_t i = 0; i < e.size(); i++)
	std::cout << "name " << i << ": " << e.name()[i] << "\n";

// ...and so on

```

@out
id 0: 2
id 1: 1
name 0: qux
name 1: bar
@eout

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_rows Working with rows and iterators

Soagen tables support treating rows as if they were regular AoS structs in their own right via #soagen::row.
This is what you get from `operator[]`, `at()`, and by dereferencing soagen::iterators.

**Rows have reference semantics**; if we retrieve a row from a non-`const` lvalue reference to our `entities` table, we'll
get a row filled with named lvalue references to the members. For example:

```cpp
// entities&, operator[]
auto row = e[0];

// fetches an instance of this:
struct row
{
	unsigned& id;
	std::string& name;
	vec3& pos;
	quaternion& orient;
};
```

Similarly, if we took a row from a `const`-qualified lvalue reference to our `entities`,
we'd get lvalue references to `const` members:

```cpp
// const entities&, at():
auto row = std::as_const(e).at(0);

// fetches an instance of this:
struct row
{
	const unsigned& id;
	const std::string& name;
	const vec3& pos;
	const quaternion& orient;
};
```

Same applies for rvalues:

```cpp
// entities&&, dereferencing an iterator:
auto row = *(std::move(e).begin());

// fetches an instance of this:
struct row
{
	unsigned&& id;
	std::string&& name;
	vec3&& pos;
	quaternion&& orient;
};
```

Note that **rows don't have to be all-or-nothing**; if you want a row view of just two of your members, you can call
[`row()`] to specify the columns you want:

```cpp
// columns 0 and 1 of row 0
auto row = e.row<0, 1>(0);

// fetches an instance of this:
struct row
{
	unsigned& id;
	std::string& name;
};
```

This is true of iterators as well:

```cpp
auto row = e.begin<0, 1>(0);
```

Any rows generated by dereferencing the iterator above will only have `id` and `name` members.

You can even rearrange the order of the columns if you need to do some swizzling:

```cpp
auto row = e.row<3,2,1>(0);

// fetches an instance of this:
struct row
{
	quaternion& orient; // column 3
	vec3& pos;          // column 2
	std::string& name;  // column 1
};
```

Of course, using the raw column indices is quite susceptible to human error (particularly if you change the table
members later), so tables all have a `columns` member enum with named constants for each column:

```cpp
auto row = e.begin<entities::columns::id, entities::columns::name>(0);
```

No matter the column configuration, a #soagen::row will implement the std::tuple protocol so you can use it with
structured bindings and other fun template stuff:

```cpp
auto r = e.row<3, 0>();

auto&& [orient, id] = r;

auto&& member_0 = r.get<0>(); // orient
auto&& member_1 = r.get<1>(); // id

auto&& column_0 = r.column<0>();    // id
// auto&& column_1 = r.column<1>(); // ERROR - r doesn't view column 1
// auto&& column_2 = r.column<2>(); // ERROR - r doesn't view column 2
auto&& column_3 = r.column<3>();    // orient
```

Finally, you can use them with `push_back()` and `insert()`:

```cpp
e.push_back(e[0]); // push a copy of row[0] onto the end of the table
```

@see <ul>

<li>soagen::iterator
<li>soagen::row</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_equality Equality

If all the column types implement the named requirement EqualityComparable \(i.e. they have `operator==`\), then so too
do your tables:

```cpp
game::entities e1;
game::entities e2;

e1.push_back(0, "cat");

std::cout << (e1 == e2) << "\n";

e2.push_back(0, "cat");

std::cout << (e1 == e2) << "\n";

e1.push_back(1, "dog");

std::cout << (e1 == e2) << "\n";


```

@out
false
true
false
@eout

The same is true of rows; if all of the viewed members are EqualityComparable, then so too are the rows:

```
std::cout << (e1[0] == e2[0]) << "\n";
std::cout << (e1[0] == e1[1]) << "\n";
```

@out
true
false
@eout

@note Rows do not have to have come from a table that is entirely EqualityComparable; it only depends on whether or not
their viewed columns are. You can take a row view of only a few columns of a much larger table, and so long as the
viewed columns are EqualityComparable, so too is the resulting #soagen::row type.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_comparison Comparison

Tables are comparable with operators `<`, `<=`, `>` and `>=` if all their column types implement the named
requirement LessThanComparable \(i.e. they have `operator<`\). Comparison is done row-wise, with row members
compared lexicographically:

```cpp
game::entities e1;
game::entities e2;

e1.push_back(0, "cat");
e1.push_back(1, "dog");

e2.push_back(0, "cat");
e2.push_back(1, "dog");

std::cout << (e1 <  e2) << "\n";
std::cout << (e1 <= e2) << "\n";
std::cout << (e1 >  e2) << "\n";
std::cout << (e1 >= e2) << "\n";

e2[1].name = "bird";

std::cout << (e1 <  e2) << "\n";
std::cout << (e1 <= e2) << "\n";
std::cout << (e1 >  e2) << "\n";
std::cout << (e1 >= e2) << "\n";

```

@out
true
true
false
false
@eout

@note Just as with EqualityComparable, rows are LessThanComparable if their viewed members are, too.
It does not depend on the source table.

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_custom_over_aligning_columns Over-aligning columns

Owing to its very nature, SoA-structured data is often used in contexts where memory alignment is important.
One common example is SIMD; you may have a column of floats that need to be aligned on a 16, 32 or 64-byte boundary,
according to whatever the minimum instruction set is you're targeting.

Soagen has support for this without you needing to come up with custom allocators or jump through any template hoops.
Let's re-visit our `entities.toml` file from earlier, only this time, we're going to over-align the `pos` column
by adding an `alignment`:

```toml
namespace = 'game'

[structs.entities]                                                 #    |
variables = [                                                      #    |
	{name = 'id',     type = 'unsigned' },                         #   \|/
	{name = 'name',   type = 'std::string', default = '""' },      #    V
	{name = 'pos',    type = 'vec3',        default = '{}', alignment = 32 },
	{name = 'orient', type = 'quaternion',  default = '{1, 0, 0, 0}' },
]
```

Re-generating the table now would see that the `entities::pos()` data pointer always returned a value that was aligned
on an (at least) 32-byte boundary.

@note This information is also propagated through to the compiler via `assume_aligned` hints all the way down.
<i>Vectorizer go brrrrrr!</i>

It can also be the case that you need to advance through the table in 'aligned batches', where the start of each batch
for each column has the same alignment as the value you specified in the config. Soagen has you covered here, too: tables
have [`aligned_stride`] as a public constant:

```cpp
constexpr auto stride = entities::aligned_stride;
for (auto ptr = e.pos(), end = ptr + e.size(); ptr < end; ptr += stride)
{
	// ...
}

```

Note that `<your table>::aligned_stride` **is across _every_ column**; if you have multiple over-aligned columns, this value
will be the lowest-common-multiple of <i>all</i> of their `aligned_strides`, such that stepping through by that amount would
have them <i>all</i> correctly aligned. This value may be larger than you like; if you're really just interested in one column,
you can fetch the `aligned_stride` for it specifically using `column_traits`:

```cpp

constexpr auto stride = entities::column_traits<2>::aligned_stride;
for (auto ptr = e.pos(), end = ptr + e.size(); ptr < end; ptr += stride)
{
	// ...
}

```

@see <ul>

<li>soagen::table_traits::aligned_stride
<li>soagen::column_traits::aligned_stride</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_custom_byo_allocator Using custom allocators

Soagen tables will use their own allocator by default: the #soagen::allocator. This is one of the mechanisms that allow
soagen to propagate the nice alignment hints all the way through to your user code.

It is the default, but you're not stuck with it. You can change the allocator for any struct in your config like this:

```toml
allocator = 'foo::allocator' # at root level to change it for all classes, or

[structs.entity]
allocator = 'foo::fancy_allocator' # ...can also be overridden set per-struct
```

The only requirements of your allocator are:

-   It must properly implement the named requirement Allocator, and
-   Have a `value_type` of `char`, `unsigned char` or `std::byte`.

@subsection intro_customizing_allocators Customizing allocators for soagen
Custom allocators may implement two soagen-specific extensions relating to memory alignment to assist the compiler
with generating performant code:

@subsubsection intro_customizing_allocators_min_alignment Specifying a min_alignment
If you know that your allocator will never align allocations on a boundary smaller than a particular value (say, 64
bytes), you can make soagen aware of this by adding a constexpr constant called `min_alignment`. Example:

```cpp
struct my_allocator
{
	inline constexpr std::size_t min_alignment = 64;
}
```

Soagen will then propagate this information to the tables where it may be of use to the compiler.

@subsubsection intro_customizing_allocators_aligned_allocate Alignment-aware allocate()
If your environment has some aligned-allocation facility you wish to make soagen aware of, you
can do so by providing an alignment-aware overload of `allocate()` taking a `std::align_val_t`
for alignment:

```cpp
struct my_allocator
{
	value_type* allocate(std::size_t size, std::align_val_t alignment)
	{
		return my_aligned_alloc_function(size, static_cast<std::size_t>(alignment));
	}
}
```

Soagen will choose this overload over any others if it is present.

@see soagen::allocator

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_access_underlying_buffer Accessing the underlying buffer

Soagen allocates one contiguous buffer for the entire table. If all the column types in your table are
TriviallyCopyable you'll be able to access the underlying buffer directly with `data()`, and determine it's size with
`allocation_size()`, allowing you to serialize/deserialize it directly, stream it, hash it, et cetera.

@see <ul>

<li>soagen::table::data()
<li>soagen::table::allocation_size()</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_integration_with_reflection Integrating with reflection systems

For lack of an in-language solution, C++ reflection systems are typically heavily based on stringification tricks,
magic macros and source-code scanners (e.g. Unreal Engine's UnrealHeaderTool). Soagen has functionality specifically
aimed at integration with these systems.

Firstly, each table class has a `constexpr` variable template for fetching the name of a column:

```cpp
std::cout << "column 0 name: " << entities::column_name<0> << "\n";
```

@out
column 0 name: id
@eout

Additionally, the generator supports adding custom annotations and attributes to the generated table class so you can
use things like UE's `UCLASS()`:

```toml
[structs.entities]
annotations = [ 'UCLASS()' ]
header = '''
GENERATED_BODY();
'''
default_constructible = 'auto' # let GENERATED_BODY create the default ctor
```

@see <ul>

<li>Schema: @ref schema_structs_annotations
<li>Schema: @ref schema_structs_attributes</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_using_without_generator Creating your own SoA types without the generator

What if you don't want to use a command-line tool to generate code, and instead want to build your own SoA
types? That's totally fine! [`soagen.hpp`] was written with that use-case in mind.

If you take a look at the source code for any `soagen`-generated table class you'll see that pretty much every function
call is a one-liner pass-through to the common underlying container type #soagen::table. The generated classes buy you
named members, default arguments to `push_back()` etc., and the ability to use rows and iterators, but most of those
could be adapted to custom types relatively simply using the existing machinery. Crack open one of the generated `.hpp`
files (e.g. [entities.hpp]) to see for yourself.

To use a #soagen::table directly, you need to express it terms of #soagen::table_traits, which is itself expressed in terms of
#soagen::column_traits. For example, to create a 'raw' verson of the entity table we've generated above:

```cpp

using entities = soagen::table<soagen::table_traits<
	soagen::column_traits<unsigned>,
	soagen::column_traits<std::string>,
	soagen::column_traits<vec3, 32>,
	soagen::column_traits<quaternion>
>>;

entities e;

e.emplace_back(/* ... */); // etc

```

@inline_success The interface of #soagen::table is a little bit less 'furnished' than it's `soagen`-generated brethren.
Making more features available without using the generator is on the [roadmap], so support for this use-case will
improve in future.

@see <ul>

<li>soagen::table
<li>soagen::table_traits
<li>soagen::column_traits</ul>

<!-- --------------------------------------------------------------------------------------------------------------- -->

@section intro_next_steps Next steps

If all of this sounds like it would solve a problem for you, please consider giving soagen a try! I'm eager to see
how people get on with it.

-   A loose development roadmap can be found [here](https://github.com/marzer/soagen/issues/1)
-   Bug reports and feature requests can be made here [here](https://github.com/marzer/soagen/issues)
-   A contribution guide can be found [here](https://github.com/marzer/soagen/blob/main/CONTRIBUTING.md)

Thanks!

<!-- --------------------------------------------------------------------------------------------------------------- -->

\[div class="m-block m-badge m-primary poxy-about-the-author"\]
\[img src="author.jpg" alt="The Author"\]
\[h3\]About the author\[/h3\]
\[p\]
I'm <a href="https://github.com/marzer">Mark</a>. You might know me as the <a href="https://marzer.github.io/tomlplusplus/">toml++</a> guy.
I write code. Some of it is alright. Almost all of it is C++.
\[span class="poxy-socials"\]
<a href="https://github.com/marzer">\[img src="poxy/poxy-icon-github.svg" class="poxy-icon"\]</a>
<a href="https://twitter.com/marzer8789">\[img src="poxy/poxy-icon-twitter.svg" class="poxy-icon"\]</a>
<a href="mailto:mark.gillard@outlook.com.au">\[img src="poxy/poxy-icon-email.svg" class="poxy-icon"\]</a>
\[/span]
\[/p\]
\[/div\]

[`soagen`]: https://pypi.org/project/soagen
[`soagen.hpp`]: https://github.com/marzer/soagen/blob/main/src/soagen/hpp/single/soagen.hpp
[`entities.hpp`]: https://github.com/marzer/soagen/blob/main/examples/entities.hpp
[entities.hpp]: https://github.com/marzer/soagen/blob/main/examples/entities.hpp
[`src/entities.hpp`]: https://github.com/marzer/soagen/blob/main/examples/entities.hpp
[natvis]: https://learn.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects?view=vs-2022#BKMK_Using_Natvis_files
[.natvis]: https://learn.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects?view=vs-2022#BKMK_Using_Natvis_files
[`.natvis`]: https://learn.microsoft.com/en-us/visualstudio/debugger/create-custom-views-of-native-objects?view=vs-2022#BKMK_Using_Natvis_files
[file a bug report]: https://github.com/marzer/soagen/issues
[`unordered_erase()`]: classsoagen_1_1examples_1_1entities.html#a117807c0fbe9e2ed2fbe39c9193ff231
[`row()`]: classsoagen_1_1examples_1_1entities.html#ac24830714a0cf3a0f677b77936a79e73
[`aligned_stride`]: structsoagen_1_1table__traits.html#a7b18454ef28aa4279e1f1fc61bd15381
[roadmap]: https://github.com/marzer/soagen/issues/1
