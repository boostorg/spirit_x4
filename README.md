# Spirit.X4 is currently under heavy development. It is considered NOT USABLE in applications yet.

**This library is official, developed under organization-wide consensus, but not part of the `boostorg/boost` superproject (yet).**

**We will not accept contributions from outside collaborators until the X3 -> X4 transition is complete.**

For background information, see:

- <https://lists.boost.org/archives/list/boost@lists.boost.org/thread/K3EQLEQJHEUBROB6ODUKTZHFP2FDUS2E/?sort=date>
- <https://github.com/boostorg/spirit/issues/795>
- <https://github.com/boostorg/spirit/pull/807>


# ... WIP content follows ...


# Spirit.X4 [![Build Status](https://github.com/boostorg/spirit/actions/workflows/ci.yml/badge.svg)](https://github.com/boostorg/spirit/actions/workflows/ci.yml)

Spirit is a set of C++ libraries for parsing and output generation implemented as
Domain Specific Embedded Languages (DSEL) using Expression templates and Template
Meta-Programming. The Spirit libraries enable a target grammar to be written
exclusively in C++. Inline grammar specifications can mix freely with other
C++ code and, thanks to the generative power of C++ templates, are immediately
executable.

## Spirit.X4 (4th generation)

Stay tuned!

### Supported Environments

- C++23 and C++26
- GCC 14
- Clang 21
- MSVC 2022

## How to use Spirit

### Step 1. Setup the Boost superproject

#### For applications:

```console
cd my_app

git submodule add https://github.com/boostorg/boost.git modules/boost
cd modules/boost

# Not required after X4 is officially shipped
git submodule add https://github.com/boostorg/spirit_x4.git libs/spirit_x4
```

#### For Spirit development:

```console
git clone https://github.com/boostorg/boost.git
cd boost

# Not required after X4 is officially shipped
git clone https://github.com/boostorg/spirit_x4.git libs/spirit_x4
```

### Step 2. Setup the dependent Boost libraries

We're trying hard to reduce these dependencies. We aim to remove these entirely in the future.

```console
git submodule update --init --depth 1 --recursive -- \
    tools/build tools/boost_install libs/assert libs/bind libs/config \
    libs/container_hash libs/core libs/describe libs/detail libs/endian \
    libs/function libs/function_types libs/functional libs/fusion \
    libs/integer libs/io libs/mp11 libs/mpl libs/predef libs/preprocessor \
    libs/static_assert libs/throw_exception libs/tuple libs/type_index \
    libs/type_traits libs/typeof libs/utility libs/variant

# Linux
./bootstrap.sh --with-toolset=gcc
./b2 headers

# Windows
.\bootstrap.bat
.\b2 headers
```

### Step 3. Start using Spirit

#### For applications:

Edit your `CMakeLists.txt`:

```cmake
add_subdirectory(modules/boost/libs/spirit_x4)
target_link_libraries(my_app PRIVATE Boost::spirit_x4)
```

#### For developing Spirit:

```console
cd libs/spirit_x4
cmake -B build
```


## Brief History

Date       | Boost | Commit   | Event
---------- | ----- | -------- | -----------------------------------------------
2014-03-18 | 1.56  | 8a353328 | Spirit.X3 is added
2013-12-14 | 1.56  | c0537c82 | Phoenix V2 is retired
2011-03-28 | 1.47  | 400a764d | [Phoenix V3] support added to Spirit V2
2009-04-30 | 1.41  | 5963a395 | [Spirit.Repository] is appeared
2008-04-13 | 1.36  | ffd0cc10 | Spirit V2 (Qi, Karma, Lex, Phoenix V2) is added
2006-08-23 | 1.35  | 2dc892b4 | Fusion V1 is retired
2003-01-31 | 1.30  | 81907916 | Spirit is the part of the Boost

[Phoenix V3]: http://boost.org/libs/phoenix
[Spirit.Repository]: http://www.boost.org/doc/libs/develop/libs/spirit/doc/html/spirit/repository.html
