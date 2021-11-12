# TooManyPlanets

This is a planet gravity simulation. The main branch runs a simulation with 100,000 planets,
if you have a less than beefy computer I would recommend changing it to 20,000 or 50,000 in 
both the cpp file and the cl file.

## OpenGL -> OpenCL interoperability
So this is an implementation of OpenCL OpenGL interoperability if you want to have a look at it for that reason.
It took me like two weeks to figure out how to do it so good luck to anyone trying to do that. I would recommend,
leaving alone the ImageGL class in the OpenCL C++ library (in the C library things are a bit different).

## Versions
 - OpenCL 1.2
 - glfw 3.3.4
 - glew 2.1.0
