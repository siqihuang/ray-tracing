Thank you for testing this program

PART I required
1 the omp structure is implemented in the main.cpp. In the outputImage function,
which accelerates the program. Using this structure to render 1 MC picture
without anti-aliasing and with only 1 light sample it takes 2800ms in average,
and it takes 9100ms in average.All tests are using cornell.txt. The instruction
says to put the omp in the outer loop. When I implemented the MC, it is the new
outer loop, so the omp is outer loop for 1 MC, I hope it is fine.
2 anti-aliasing is set to 1 because it will greatly reduce the speed with the
amount of MC iteration is large(2000 for instance). For testing you can change
the SAMPLE_NUM in main.cpp. It is defined in the "#define" block.
3 for the area light, it is also set to 1 for speed when the number of MC 
iteration is large. You can test it by changing the LIGHT_SAMPLE in main.cpp,
which is also defined in "#define" block.

PART II optional
1 MC ray tracing
I have only implemented the indirect illumniation. To test this part you should
better set anti-aliasing and LIGHT_SAMPLE to 1 or it will be too slow. The 
MC_ITER in main.cpp is the number of iteration for MC. on my computer it takes 2.7
second in average to run one iteration using cornell.txt. In moore environment, the
program should be finished in 200s. Also after 10 iteration there is an output image
in SAMPLE.bmp.
2 Acceleration structure
a. Octree tree
The octree is set to 7 max depth. And when a triangle is in two space, put
it in the parent node. I used the hw3_dragon.txt to test this file. The 
construction time is 100ms, and run time is 35s. If setting the depth to 1,
which means all mesh are in one cube. The construction time is 16ms and run
time is 607ms. The second situation can be considered as running without the octree
but only a bounding box.
b. k-d tree
The k-d Tree is divided recursively when there is only one triangle in a node.
Also using the hw3_dragon.txt the construction time is 250ms, and running time
is 2.4s in average. The construction structure have used a improved fast-sorting
to improve the efficiency. The max tree depth is not recorded, but because
the whole mesh is simple divided into two parts separately, it can be up to
16 layer for 100000 points. Every triangle is in the leaf node.When spliting
the mesh, the left tree and right tree will both have half elements.
When comparing these two structures, it is obviously that the kd tree is much
better. Because it divede the mesh into halves not the space.

3 Motion Blur
To see the effect of it, input y when asked. Notice, to get a output image.
You need first push 'n' to select the object to move. The yellow one is the 
selected one. It will move -0.05 in x direction in every MC iteration. You cannot
change the moving direction and step. It is just a demonstration.