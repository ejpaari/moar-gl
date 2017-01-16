# moar-gl

A simple graphics engine which utilizes OpenGL 4.x and C++11. The emphasis is on implementing and learning graphic features. Other features are implemented if needed.

### Graphic features:
- Forward rendering with MSAA
- Deferred rendering with SSAO
- Diffuse, normal, bump and specular mapping
- Real-time hard shadows
- Point and directional lighting
- Bloom
- HDR
- Custom post-processing shaders
- Frustum culling (bounding sphere)
- Skybox

### Tools and libraries:

- GLEW 2.0.0 http://glew.sourceforge.net/
- Boost 1.57.0 https://sourceforge.net/projects/boost/files/boost/1.57.0/
- SOIL July 7, 2008 http://www.lonesock.net/soil.html
- AntTweakBar 1.16 http://anttweakbar.sourceforge.net/doc/tools:anttweakbar:download
- Qt 5.x / Qt Creator 3.x (optional, qmake is enough)

### Submodules:

- GLM
- GLFW
- Assimp

### Setup

1. Clone using "--recursive" to get the submodules
2. Build / install required tools, libraries and submodules
3. Build the myapp example project (two ways which are essentially the same)
 * With Qt / Qt Creator Build & Run
 * By running qmake and building with the Makefile

Following compilers have been tested (other compilers probably work fine as well)
- GCC 4.8
- VS14

### Assets:

- Sponza model from http://www.crytek.com/cryengine/cryengine3/downloads
- Attack droid model from http://www.turbosquid.com/FullPreview/Index.cfm/ID/637623

### Screenshots

http://imgur.com/a/IxZIF
