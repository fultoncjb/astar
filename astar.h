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
#include"json/json.h"
#include<math.h>
#include<vector>
#include <sys/time.h>

// Coordinate struct
struct Coord{
	int x;
	int y;
};

enum state { UNINITIALIZED,
			 OBSTACLES_ONLY,
			 FULL_MAP };

// Each grid cell initialized to a node
struct Node{
	Node();
	bool isOpenSet;		// in open set
	bool isClosedSet;	// in closed set
	bool isObstacle;	// is an obstacle
	Node *parent;		// pointer to its parent node
	float g;		// movement cost
	float f;		// total cost
	float h;		// distance from goal
	Coord location;		// coordinates in grid
};

// Full map of explorable nodes
class m_map{
	public:

		// Allow windows access to private/protected members
		friend class MapDrawArea;

		m_map();
		void printPath(std::string fileString);
		void printHeap();
		bool SolveOptimalPath(std::string fileString, std::string outputFileString);
		std::vector<Coord> copyObstacles();
		Coord copyMaxCoord();
		Coord copyStartCoord();
		Coord copyEndCoord();
		std::vector<Coord> copyOptPath();
		state MapState;
		bool MapInitialized;
		bool InitMap(std::string fileString);

		~m_map();
	protected:
		// Functions	
		bool AddExplorableNodes();	// put the node with the least movement cost on back of the open set
		void DeleteNodeFromOpenSet(Node &n);
		void DeleteNodeFromClosedSet(Node &n);
		void AddNodeToOpenSet(Coord desiredCoord,Coord cost);
		void InsertNodeInOpenSet(Node &n);
		int FindOpenSetPosition(Node n);

		Coord GetLocalMovementCost(int position);
		bool NodeIsInsideGrid(Coord coord);
		bool NodeIsValid(Coord coord);
		bool NodeIsObstacle(Coord coord);

		// Data objects
		Coord m_startCoord;		// start coordinates
		Coord m_endCoord;		// goal coordinates
		Coord m_maxXY;			// max coordinates
		Node **m_nodeGrid;		// grid of nodes
		std::vector<Node*> m_openSet;	// list of unexplored nodes
		std::vector<Node*> m_optPath;	// optimal path
		std::vector<Coord> m_obstacles;	// obstacles
};

enum POSITION { NORTH = 1, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };

