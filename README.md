# GIRadianceRegressionFunction
This software is our final year study project, this is an implementation of the paper "Global Illumination with Radiance Regression Functions" available at
http://www.jiapingwang.com/files/shadebot_sig13.pdf

It is decomposed in 3 modules:

## Extraction module _(Latin Hypercube + PBRT)_
This module will extract learning datas from a scene (.pbrt file)

#### Latin Hypercube
The goal of this program is to generate several position of cameras in the scene, and for each position of camera, generate several positions of lights. Positions are generated using Latin Hypercube sampling.

#### PBRT
This version of PBRT can use a spherical camera that will generate samples around it, using Fibonacci sampling. We also added an integrator that only computes indirect lighting.

## Learning module
This module will create a NeuralNetwork and learn extracted datas, and save the NeuralNetwork.

## Evaluation module
This module uses a 3D OpenGL engine to compute direct lighting, and evaluate in real time the NeuralNetwork to get the indirect lighting.
