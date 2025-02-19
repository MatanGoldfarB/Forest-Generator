https://github.com/MatanGoldfarB/Graphics-HW2

To run the ray tracing, write "make" in the terminal and then ./raytracer <the file path>
The program will save the outputs into a folder name "output".
The output image name will be "my<the input file>.png" or "myAliasing<the input file>.png" for the aliasing case
For aliasing we will use the 4th coordinate in camera position, 1.0 no multi sampling. 0.0 for multi sampling.

In HW2.cpp in the function renderImage we can change the number of rays that will be used in Multi-sampling for anti-aliasing


