/*********************************************************************/
/************************ Cameron Fulton *****************************/
/************************ Copyright 2015 *****************************/
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

// State of the map to be drawn
enum state { UNINITIALIZED,
			 OBSTACLES_ONLY,
			 FULL_MAP };

// Each grid cell initialized to a node
struct Node{
	// Data objects
	bool isOpenSet;		// in open set
	bool isClosedSet;	// in closed set
	bool isObstacle;	// is an obstacle
	Node *parent;		// pointer to its parent node
	float g;			// movement cost
	float f;			// total cost
	float h;			// distance from goal
	Coord location;		// coordinates in grid

	// Functions
	Node();
};

// Full map of explorable nodes
class m_map{
	public:
		// Open set of explorable nodes subclass
		struct OpenSet {
			// Data objects
			std::vector<Node*> set;	// list of unexplored nodes

			// Functions
			void Insert(Node &n);
			int FindInsertPosition(Node n); // put the node with the least movement cost on the back of the open set
			void DeleteNode(Node &n);
			void DeleteSet();
			void PrintSet();
		};

		// Data objects
		Coord copyMaxCoord();
		Coord copyStartCoord();
		Coord copyEndCoord();
		std::vector<Coord> copyOptPath();
		state MapState;
		bool MapInitialized;
		enum HEADING { NORTH = 1, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST };

		// Functions
		m_map();
		void WritePathToFile(std::string fileString);
		bool SolveOptimalPath(std::string inputFileString, std::string outputFileString);
		std::vector<Coord> copyObstacles();
		bool InitMap(std::string fileString);
		void ClearMap();
		~m_map();

	protected:
		// Data objects
		Coord m_startCoord;		// start coordinates
		Coord m_endCoord;		// goal coordinates
		Coord m_maxXY;			// max coordinates
		Node **m_nodeGrid;		// grid of nodes
		OpenSet m_openSet;		// list of unexplored nodes
		std::vector<Coord> m_optPath;	// optimal path
		std::vector<Coord> m_obstacles;	// obstacles

		// Functions	
		bool ExploreSurroundingNodes(); // add nodes to open/closed set
		void DeleteNodeFromClosedSet(Node &n);
		void UpdateExplorableSets(Coord desiredCoord,Coord cost); // manage the adding of nodes to the open set based
																  // on movement cost parameters
		Coord GetLocalMovementCost(int position);
		bool NodeIsInsideGrid(Coord coord);
		bool NodeIsValid(Coord coord);
		bool NodeIsObstacle(Coord coord);
		void DeleteNodeGrid();
};
