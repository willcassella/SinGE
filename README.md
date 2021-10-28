# SinGE - A game engine

![Demersi](https://user-images.githubusercontent.com/4460452/138816933-74fc5c52-7d11-49a4-b9e0-9d1690789e0f.png)

SinGE was largely inspired by ideas I had while developing WillowEngine and using other game engines and tools. Demersi 
is a game developed with SinGE and was the primary motivator of engine direction and development, beyond the first 
prototypes (it had initially been conceived as an engine for building doom-like games, hence the name). Demersi may be 
downloaded from [my website](http://willcassella.com).

## Compiling

### Windows
Building on Windows requires Visual Studio 2017. Simply clone the repository, open SinGE.sln, and build all. 
Dependencies will be automatically downloaded and cached during the build process.

### Linux
Building on Linux has only been tested on Ubuntu. It requires a C++14 compiler and the following packages:
- `cmake`
- `pkg-config`
- `rapidjson-dev`
- `libfreeimage-dev`
- `libbullet-dev`
- `libglfw3-dev`
- `libglew-dev`

The cmake script in the root directory can be used to build the engine, without any special arguments.

## Development Roadmap
I've mostly ceased development on this iteration of the engine, though I'm still occasionally doing small maintenance 
changes for preservation purposes. I'm toying around with incrementally building a v2 in Rust, though that's effectively 
a rewrite so for now I'm doing that in a different repo.
