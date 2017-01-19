# RT options

## To do

* Scene file
  * Scene file using JSON or something advanced
* Perturbation
  * Color (XY based)
  * Color (something more complex)
  * Color (Perlin noise)
* Reflection (with percentage)
* Transparency (with percentage)
* Refraction
* Shadows adapt based on transparency
* Textures
  * At least one object
  * All objects
  * Stretch texture on the object
  * Translate texture on the object
  * Use another library to read textures
* Bump mapping textures
* Groups
* Cartoon effect
* Integrated image saving
* Progress bar with render progress
* A good GUI with a library (GTK/SDL)
* Real-time parameter changes
  * Preview renders should be low-resolution and unshaded
* Launch renders through the terminal
* Automatic object creation (torus with spheres, helix)
* Video to show off RT on YouTube/Vimeo
* Spherical light (soft shadows)
* Red/blue 3D
* Depth of field
* Skybox
* Simple quadric object

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
