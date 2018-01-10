# PushNotifier (v2) for C++

A Class to use [PushNotifier](https://pushnotifier.de/) in C++ projects.


## Installation

Just clone this repository to your sourcefolder or add it as submodule in git. You need to do this recursively as this SDK has submodules itself.

You will need the CURl development library for successful compilation.


## Compilation

Before compilation you need to uncomment and insert **APP_TOKEN** and **APP_PACKAGE** in PushNotifier.h, if you don't it won't work. Alternatively you can (if you are using g++) use `-DAPP_TOKEN=\"testtoken\"` during compilation.

```bash
g++ -std=c++11 -o test PushNotifier.cpp main.cpp `pkg-config libcurl --cflags --libs`
```
Note that this class needs C++11 or higher to function.


## Documentation
The SDK is fully documented using doxygen.

```bash
doxygen docs.doxy
```
