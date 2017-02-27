# Atlas Engine

Atlas Engine is our 3D OpenGL engine. It is working on OpenGL 4.3, on Windows and Linux platforms.

## Main functionalities

### Direct lighting

It compute direct lighting in real time using Cook-Torrance method.

### Indirect lighting

Indirect lighting is computed by a neural network which has been trained by the learning module of the project. It use a Compute Shader that compute the neural network outputs corresponding to specific inputs (fragment position, camera position, light position and normals).