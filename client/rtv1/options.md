# RT options

## To do
* Scene file
  * Scene file using JSON or something advanced
* Integrated image saving
* Progress bar with render progress
* Real-time parameter changes
  * Preview renders should be low-resolution and unshaded
* Launch renders through the terminal
* Video to show off RT on YouTube/Vimeo
* A good GUI with a library (GTK/SDL)

* Textures
  * At least one object
  * All objects
  * Stretch texture on the object
  * Translate texture on the object
  * Use another library to read textures
* Bump mapping textures
* Red/blue 3D
* Skybox

## Done
* RTv1
* Ambient light
* Configurable ambient lighting
* Antialias
* Multithreaded or OpenCL rendering
* It's particularly fast ^_^
* Direct light (get blinded by spots in front of the camera)
* Color effects (sepia, grayscale, etc.)
* Object limits
  * Basic XYZ cutting
  * XYZ based on world/object coordinates
  * Transforms work properly after cutting
* Perturbation
  * Normal
  * Color (XY checkerboard)
  * Color (something more complex)
* Simple quadric object (paraboloid)
* Cartoon effect
* Reflection (with percentage)

## Probably won't do
* Object limits
  * Unique process for each object
  * Planes can be cut with a triangle or disc
* Parallel light
* Transparency textures
* Limit textures
* Shadows work with semi-transparent textures
* Negative objects
* Motion blur (we don't have motion)
* Clustered rendering
* Menger sponge
* Cloth
* Torus
* Equation in the config file (using GNU library)
* Fractal objects (mandelbulb for example)
* OBJ/mesh support
* 3D on a TV or VR headset
* Moebius
* Caustics
* Global illumination
* Perturbation
  * Color (Perlin noise)
* Groups
* Automatic object creation (torus with spheres, helix)
* Spherical light (soft shadows)
* Depth of field
* Transparency (with percentage)
* Refraction
* Shadows adapt based on transparency
