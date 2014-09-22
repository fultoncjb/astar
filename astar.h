/*********************************************************************/
/************************ Cameron Fulton *****************************/
/************************ Copyright 2014 *****************************/
/*********************************************************************/
/**************************  astar.h    ******************************/
/* Given a JSON file of obstacles, a starting coordinate, and an     */
/* ending coordinate, output the optimal path to a file using A*.    */

#include<cmath>
#include<string>
#include<algorithm>
#include<iostream>
#include<string>
#include<fstream>
#include<json/json.h>
#include<math.h>
#include<vector>

// Full map of explorable nodes
class m_map{
	public:
		// Coordinate struct
		struct coord{
			int x;
			int y;
		};

		// Each grid cell initialized to a node
		struct node{
			node();
			bool isOpenSet;		// in open set
			bool isClosedSet;	// in closed set
			bool isObstacle;	// is an obstacle
			node *parent;		// pointer to its parent node
			float g;		// movement cost
			float f;		// total cost
			float h;		// distance from goal
			coord location;		// coordinates in grid
		};

		
		m_map();
		void printPath(std::string fileString);
		void printHeap();
		bool getPath(std::string fileString, std::string outputFileString);
		void cleanMem();
		std::vector<coord> copyObstacles();
		coord copyMaxCoord();
		coord copyStartCoord();
		coord copyEndCoord();
		std::vector<coord> copyOptPath();
	protected:
		// Functions	
		bool m_initGrid(std::string fileString);
		node *checkValidNode(int position, node *curNode);
		bool putBestNodeOnBack();	// put the node with the least movement cost on back of the open set
		void leastCostToBack();

		// Data objects
		coord m_startCoord;		// start coordinates
		coord m_endCoord;		// goal coordinates
		coord m_maxXY;			// max coordinates
		node **m_nodeGrid;		// grid of nodes
		std::vector<node*> m_openSet;	// list of unexplored nodes
		std::vector<node*> m_optPath;	// optimal path
		std::vector<coord> m_obstacles;	// obstacles
};

bool SortByF (const m_map::node *node1, const m_map::node *node2);

