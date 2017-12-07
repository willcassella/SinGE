# SinGE - A game engine

![Demersi](http://willcassella.net/SinGE/demersi.png)

SinGE was largely inspired by ideas I had while developing WillowEngine and using other game engines and tools. Demersi was a game developed with SinGE and was the primary motivator of engine direction an development. Demersi may be downloaded on [my website](http://willcassella.net).

## Compiling

### Windows
Building on Windows requires Visual Studio 2017. Simply clone the repository, open SinGE.sln, and build all. Dependencies will be automatically downloaded and cached during the build process.

### Linux
Linux guide coming soon (aka, once I test it). It does work, though.

## Development Roadmap
I haven't made any major contributions to this in quite a while, since I'm busy with work and other projects. However, I still think about it a lot, and I'd like to write down some ideas for the (possibly distant) future.

### Renderer
- Move away from deferred rendering. It's been fun but very few of my projects have come close to justifying it and there's other things I want to try anyway.
- Overhaul material system. I'd like to have something like Unreal's Material/Material-Instance concept, since that not only decreases loading time but also decreases artist workload.
- Implement some nice effects like screen-space reflection, physically-based lens flare, real-time shadows (I got *this* close to finishing it for Demersi), and GPU-based particles.
- Rewrite the lightmapper to support image-based lighting, more efficient sampling methods, multiple light source, importance volumes, previewing (progressive lightmapping), and a more efficient storage/loading mechanism. This part of the engine is a good candidate for keeping as a seperate library, so I'd like to pursue that.
- Make rendering engine more generic, with some ideas I had for how that could be done without significant loss of performance.
- Do even more research into physically based shading.

### Engine: Short-term
- Rebuild scene system. Think: Less Unity-style components and more Godot-style nodes. I've thought a lot more about how things are layed out and how I'd like them to be, and I think this is a really good approach moving forward. It better satisfies my goals of predictability and composability.
- Prefabs! I've been putting this off for too long, mainly because I just want to do it right. This would include nesting and instancing.

### Engine: Long-term
- Rewrite physics integration on top of PhysX instead of Bullet. Bullet is perfectly fine in terms of features and performance, but it's a bit buggy and I really despise the API. It would be really cool to write my own physics engine, but that's a project for another day.
- Implement audio system, potentially using steam-audio? I haven't explored this space very much, which is dumb because I think sound design is an absolutely crucial component of game design.
- Implement animation system. This would include skeletal animation authored in a seperate program, as well as property animations authored in the editor(?).
- Build a good resource manager. Somehow I always end up having a really crappy runtime resource manager, or none at all. Neither is particularily ideal, but this isn't exactly a priority either. Even if loading of resources isn't handled any more intelligently than it is now, I at least need a smarter way of organizing and storing resources and metadata on-disk.

### Engine: Unknown-term
- Build prototype editor and game GUI with [Delta](https://github.com/willcassella/delta) or some other GUI library. Blender's great for hacking on top of, but it's not a very maintainable solution. I've gone without a decent GUI solution in my game projects for long enough, and I'd like to develop/find something I can stick with. This is a high priority, but it's not exactly something I think could be considered short-term.
- Build BSP/voxel level prototyping tool. Even if none of the results are used in production, it's great to have something for whipping out ideas in a short amount of time. Unreal is the only real game engine I can think of that's come close to doing this right, but I think it can be done even better.
