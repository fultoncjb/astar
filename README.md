astar
=====

C++ GUI for completing A* given a JSON file of obstacles and start/end coordinates. It is a simple GUI built on Gtkmm, a library of wrapper functions around gtk+ for use in C++. 

A* is a path finding algorithm used for robots navigating through static maps (i.e. obstacle coordinates are static). Each node in the grid is defined by a movement cost (g) and the distance from the goal node (h). These two values are added to form "f" and nodes with the smallest "f" value are explored first. Starting from the "robotStart" coordinates, all the neighboring cells are processed and added on the "open set" if they are valid (i.e. not an obstacle, not already on the open set). Once all neighboring cells are processed, this node is placed on the "closed set." Its "g" value is only updated when it is a neighboring node of the node currently being explored and this node has a smaller "g" value than when previously encountered (i.e. the current path to this node takes fewer steps than when it was previously explored).

The JSON file must contain the  fields "obstacles," "robotStart," and "robotEnd." The program allocates memory on the heap to house a grid of nodes defined as either obstacles or free-space. The grid dimensions are dynamically allocated based on the maximum value among the obstacles, robotStart, and robotEnd coordinates. Sample output maps contain the .png extension.
