# OpenAT: Open Source Algorithmic Trading Library
[![Build Status](https://travis-ci.org/galeone/openat.svg?branch=master)](https://travis-ci.org/galeone/openat)


## Build

Clone the repository and make sure to clone the submodules too:

```
git clone --recursive https://github.com/galeone/openat
```

### Building on UNIX

```
mkdir build
cd build
cmake ..
CC=clang CXX=clang++ make
```

### Building on macOS

As of october 2017, the version of clang shipped with XCode 9 does not fully
support C++17. You will need to install gcc from HomeBrew; it will be available
as `gcc-7` and `g++-7`. Note that the gcc included in XCode is just a gcc 
frontend with an LLVM (clang) backend; it will not be able to build `at`.
```
brew install gcc
```

Install the needed dependencies and remember to link them:
```
brew install openssl sqlite
brew link sqlite --force
```

For security reasons, HomeBrew doesn't allow to symlink OpenSSL to /usr/local.
You will need to manually tell cmake where to find these libraries. Make sure
to provide cmake with the correct version of the OpenSSL (1.1).

You can now proceed and build `at`:
```
mkdir build && cd build
CC=gcc-7 CXX=g++-7 cmake \
    -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl@1.1/ \
    -DOPENSSL_INCLUDE_DIR=/usr/local/opt/openssl@1.1/include \
    -DOPENSSL_CRYPTO_LIBRARY=/usr/local/opt/openssl@1.1/lib/libcrypto.dylib ..
make
```
## Test

```
cd build
# ID is a test defined by gumbo-query that somehow appears into
# the tests of the current project. Exclue it using cmame -E (exclude) ID
ctest -E ID
```
