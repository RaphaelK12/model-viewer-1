## Overview

This is a simple 3D model viewer for arbitrary [Wavefront .obj files](https://en.wikipedia.org/wiki/Wavefront_.obj_file), using the OpenGL graphics API. Models are rendered in one window using the simple (but deprecated) OpenGL fixed function pipeline, and in a second window using equivalent GLSL vertex/fragment shaders. Users can control the camera, lighting, and a variety of other rendering properties.


## Features & Controls

The model viewer is keyboard and mouse driven, with the following controls:

+ __Camera:__ translate with _WASD_ keys; rotate with mouse movement; zoom with scroll wheel; tilt with mouse buttons; reset with spacebar

<img src="https://raw.githubusercontent.com/clfm/Model-Viewer/master/images/camera.gif" alt="camera" width="350">

+ __Lighting:__ toggle lighting components with the _L_ key; change light color with _RGB_ keys

<img src="https://raw.githubusercontent.com/clfm/Model-Viewer/master/images/lighting.gif" alt="lighting" width="350">

+ __Rendering mode:__ toggle between normal rendering, polygon mesh, and point cloud with _123_ keys

<img src="https://raw.githubusercontent.com/clfm/Model-Viewer/master/images/rendering_mode.gif" alt="rendering_mode" width="350">

+ __Shading mode:__ toggle between flat and smooth (Gouraud) shading with the _P_ key

<img src="https://raw.githubusercontent.com/clfm/Model-Viewer/master/images/shading_mode.gif" alt="shading_mode" width="350">

+ __Clipping:__ change the near/far clipping distances with the _N_ and _F_ keys

<img src="https://raw.githubusercontent.com/clfm/Model-Viewer/master/images/clipping.gif" alt="clipping" width="350">


## Dependencies

The following libraries must be accessible to the project:
+ [OpenGL](https://www.opengl.org/)
+ [FreeGLUT](http://freeglut.sourceforge.net/)
+ [OpenGL Mathematics](https://glm.g-truc.net/0.9.9/index.html)
