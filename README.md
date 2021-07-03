# [WORK IN PROGRESS] FaustGen

Livecode [Faust](faust.grame.fr/) in [SuperCollider](supercollider.github.io/) using an embedded [Faust compiler](https://faustdoc.grame.fr/manual/embedding/).

## Status

[Updates will be posted here](https://scsynth.org/t/wip-faustgen-a-ugen-for-interpreting-faust-code/4029)

### Requirements

- CMake >= 3.5
- LLVM
- Faust
- SuperCollider source code

Installing them on Arch based systems using an AUR helper:

```bash
paru -S faust llvm cmake
```

### TODO

See [todo.md](todo.md)

### Building

Clone the project:

    git clone https://github.com/madskjeldgaard/faustgen
	git submodule update --init --recursive
    cd faustgen
    mkdir build
    cd build

Then, use CMake to configure and build it:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cmake --build . --config Release --target install

You may want to manually specify the install location in the first step to point it at your
SuperCollider extensions directory: add the option `-DCMAKE_INSTALL_PREFIX=/path/to/extensions`.

It's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: add the option `-DSC_PATH=/path/to/sc/source`.
