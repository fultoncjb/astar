astar
=====

C++ GUI for completing A* given a JSON file of obstacles and start/end coordinates. It is a simple GUI built on Gtkmm, a library of wrapper functions around gtk+ for use in C++. 

The JSON file must contain the  fields "obstacles," "robotStart," and "robotEnd." The program allocates memory on the heap to house a grid of nodes defined as obstacles or free-space. The grid dimensions are dynamically allocated based on the maximum value among the obstacles, robotStart, and robotEnd. Sample output maps contain the .png extension.
