# Nebula Trifid

Nebula Trifid is an open source, free to use, free to redistribute and free to modify. It's originally based on the Nebula 3 Device used by RadonLabs to produce games.

Nebula Trifid features a fully capable OpenGL4 rendering system, a modular component based entity system, networking through RakNet, several physics implementations (Bullet or Havok), an FBX content pipeline which is subversion friendly, interactive audio using Fmod, game UI using LibRocket, and toolkit UI using Qt, sophisticated AI and pathfinding using Detour and Recast and finally GLFW for window and OpenGL context management.

Nebula Trifid is intended to be both an SDK as well as a programming platform to write games with. Games implemented in Nebula Trifid can either be entirely scripted, using LUA, or written in C++, where the latter allows for a greater freedom than the first.

Nebula Trifid also contains a complete toolkit which allows for content management in a game project, as well as a level editor which is used to design levels and script events. It's also easy to set up, all you need is CMake. 

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

# Rendering
- OpenGL 4.
- Particles.
- Deferred rendering.
  * Physically based.
- Reflections and irradiance.
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

# Game
- Modular entity system.
- Modular game functionality system.
- Several ready-to-use modules for general purpose game making.
- SQLite backend for loading and saving the state of a game.
- Scripting using LUA.

With Nebula Trifid comes three main tools which should be used by game developers/level designers/graphics artists in order to feed a game with graphics and level related content. 

# Content browser
Is not really a browser as much as it is a *content editor*. This tool is used to get an overview of all the content currently present in the working project, as well as give a preview of said content. The content browser is also used to modify information related to shading, particles, texture compression and filtering, as well as information extraction from FBX based models. 

# Level editor
The level editor is exactly what it sounds like. It allows you to create levels by placing game entities in a scene, alter their values (called attributes). It can also place entities which are purely graphical (called environment entities) lights as well as light probes for reflection and irradiance gathering. The level editor also allow you to modify level-specific post effects such as bloom, DoF, ambient occlusion, fog and skybox. The level can then be played by pressing the play button, which will open a new window in which the level can be tested. 

# NOTE
Have in mind that Nebula Trifid is constantly under development, which might mean that some of the features doesn't behave as intended, might not be implemented yet, or might be misleading in how they are supposed to work. The development can be followed on the blog at: http://nebulatrifid.org/developer-blog/
