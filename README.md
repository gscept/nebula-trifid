# Nebula Trifid

[![Join the chat at https://gitter.im/gscept/nebula-trifid](https://badges.gitter.im/gscept/nebula-trifid.svg)](https://gitter.im/gscept/nebula-trifid?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Nebula Trifid is a open source, free to use, free to redistribute and free to modify game engine. It's originally based on the Nebula 3 Device used by RadonLabs to produce games.

Nebula Trifid features a fully capable OpenGL4 rendering system, a modular component based entity system, networking through RakNet, several physics implementations (Bullet or Havok), an FBX content pipeline which is subversion friendly, interactive audio using Fmod, game UI using LibRocket, and toolkit UI using Qt, sophisticated AI and pathfinding using Detour and Recast and physics using either Bullet, PhysX or Havok and finally GLFW for window and OpenGL context management. The engine is designed as a programmers tool, meaning we intend that the game development is done in a programming environment, and not entirely in-engine. The exception to this is the content creation and level editing, which is done in the tools provided.

Nebula Trifid is intended to be both an SDK as well as a programming platform to write games with. Games implemented in Nebula Trifid can either be entirely scripted, using LUA, or written in C++, where the latter allows for a greater freedom than the first.

Nebula Trifid also contains a complete toolkit which allows for content management in a game project, as well as a level editor which is used to design levels and script events. It's also easy to set up, all you need is CMake. 

Check out the [Changelog](https://github.com/gscept/nebula-trifid/wiki/Changelog) as well.

A more detailed list of features in Nebula Trifid can be seen here:

# Foundation
- Windows, Linux (OS X, but only the foundation part).
- Threading.
- Heap-grouped memory allocation.
- SSE based math library.
- Thread pool system.
- HTTP server for online debugging.
- Performance monitoring using profiling timers.
- Custom memory allocation. 
- Cross thread singleton interfaces.
- Very fast RTTI system.
- Smart pointers.
- Messaging infrastructure.
- Call stack information on assertions.
- Code generation for flyweight classes such as messages and script hooks.
- FourCC system which allows classes to be instantiated by name. 

# Rendering
- OpenGL 4.
- Custom shader language built on GLSL.
- Particles.
- Deferred rendering.
  * Physically based.
  * IBL.
- HBAO.
- Screen space subsurface scattering.
- DoF.
- Skinning and animation.
- Sprites.
- Lightmapping.
- Light scattering through crepuscular rays.
- Per-pixel picking.
- Real-time shadows.
  * Spot lights using VSM.
  * Global lights using CSM with VSM.
- HDR.
- 16 bit+ textures.
  * Cubemaps.
  * DDS - DXT1, DXT3, DXT5.
- LOD.
- Bloom.
- Subroutines (instead of pesky über shaders).
- Persistently mapped buffers.
- Per-shader batch drawing.
- Visibility resolving.
- Dynamic and asynchronous resource management.
- Instanced rendering.
- Job execution for skeletal animations, visibility queries and particle calculation.
- Dynamically resizable render context.
- Scriptable rendering paths through XML. 
- Tessellation.
- Render plugins.

# Game
- Modular entity system.
- Modular game functionality system.
- Several ready-to-use modules for general purpose game making.
- SQLite backend for loading and saving the state of a game.
- Scripting using LUA.

With Nebula Trifid comes three main tools which should be used by game developers/level designers/graphics artists in order to feed a game with graphics and level related content. 

# Content browser
Is not really a browser as much as it is a *content editor*. This tool is used to get an overview of all the content currently present in the working project, as well as give a preview of said content. The content browser is also used to modify information related to shading, particles, texture compression and filtering, as well as information extraction from FBX based models. 

![alt tag](http://nebulatrifid.org/wp-content/uploads/2015/03/cb.png)

# Level editor
The level editor is exactly what it sounds like. It allows you to create levels by placing game entities in a scene, alter their values (called attributes). It can also place entities which are purely graphical (called environment entities) lights as well as light probes for reflection and irradiance gathering. The level editor also allow you to modify level-specific post effects such as bloom, DoF, ambient occlusion, fog and skybox. The level can then be played by pressing the play button, which will open a new window in which the level can be tested. 

![alt tag](http://nebulatrifid.org/wp-content/uploads/2015/03/LE.png)

# Batch exporter
The batch exporter is used to keep a built version of your game project up to date. Whenever you setup a new project, or get new assets from someone else, the batch exporter is there to make sure the models, meshes, animations, sounds, levels, game data and shaders are all using the final version.

# Getting started
We currently do not have a complete documentation about how to setup your projects, but there is some documentation available at http://nebulatrifid.org/documentation/wiki/ that should get you started.

# What's left to do?
We already have a comprehensive list of the stuff we want to implement or fix. The list contains the following:
- Forward lighting for alpha rendered materials.
  * Translucent materials
- Detach materials from models, and make materials into a reusable resource.
  * This indirectly allow us to switch materials on models without having to rewrite the entire model node. Like live.
- Integrate content browser with level editor to have one singular application.
- Convert all the different resource browsers (in the tree view) into cascaded file browsers with thumbnails.
- Redesign how content is stored, instead of having a work/textures and work/gfxlib, we should have the content grouped by asset package.
- And a lot more... Which is one of the beauties with this project, it doesn't have a definite end.

# NOTE
Have in mind that Nebula Trifid is constantly under development, which might mean that some of the features doesn't behave as intended, might not be implemented yet, or might be misleading in how they are supposed to work. The development can be followed on the blog at: http://nebulatrifid.org/developer-blog/
