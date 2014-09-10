#include<cmath>
#include<string>
#include<algorithm>
#include<iostream>
#include<string>
#include<fstream>
#include<json/json.h>
#include<math.h>
#include<vector>

class m_map{
	public:
		struct coord{
			int x;
			int y;
		};

		struct node{
			bool isOpenSet;
			bool isClosedSet;	
			bool isObstacle;
			node *parent;
			float g;
			float f;
			float h;
			int x;
			int y;
		};

		
		m_map();
		bool SortByF (const node *node1, const node *node2);
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
		bool getBestNode();
		void leastCostToBack();

		// Data objects
		coord m_startCoord;
		coord m_endCoord;
		coord m_maxXY;
		node **m_nodeGrid;
		std::vector<node*> m_openSet;
		std::vector<node*> m_closedSet;
		std::vector<node*> m_optPath;
		std::vector<coord> m_obstacles;
};

