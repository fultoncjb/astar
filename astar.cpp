/*********************************************************************/
/************************ Cameron Fulton *****************************/
/************************ Copyright 2014 *****************************/
/*********************************************************************/
/************************** astar.cpp    *****************************/
/* Given a JSON file of obstacles, a starting coordinate, and an     */
/* ending coordinate, output the optimal path to a file using A*.    */

#define VALID_NODE ( (desiredNode.x <= m_maxXY.x && desiredNode.x >= 0) && (desiredNode.y <= m_maxXY.y && desiredNode.y >= 0) )

#include"astar.h"

using namespace std;

bool m_map::SortByF (const node *node1, const node *node2) 
{ return node1->f > node2->f; }

m_map::m_map()
{

}


// Initialize the grid for analysis
bool m_map::m_initGrid(std::string fileString)
{
	// Hold the entire file in an object
	Json::Value mapValue;
	// Coordinate where the robot starts
	Json::Value startValue;
	// Coordinate where the robot ends
	Json::Value endValue;
	// Coordinates where the obstacles are
	Json::Value obsValue;

	std::ifstream mapFile(fileString.c_str());

	coord curCoord;

	if( mapFile.is_open() )
	{
		mapFile >> mapValue;

		startValue = mapValue.get("robotStart","UTF-8");
		endValue = mapValue.get("robotEnd","UTF-8");
		obsValue = mapValue.get("obstacles","UTF-8");

		// Set the start and end coordinates
		m_startCoord.x = startValue[0].asInt();
		m_startCoord.y = startValue[1].asInt();
		m_maxXY.x = m_startCoord.x;
		m_maxXY.y = m_startCoord.y;
		m_endCoord.x = endValue[0].asInt();
		m_endCoord.y = endValue[1].asInt();
		if(m_endCoord.x > m_maxXY.x)
			m_maxXY.x = m_endCoord.x;
		if(m_endCoord.y > m_maxXY.y)
			m_maxXY.y = m_endCoord.y;

		// Iterate through the obstacles
		for(Json::ValueIterator itr = obsValue.begin(); itr != obsValue.end(); ++itr)
		{
			curCoord.x = (*itr)[0].asInt();
			curCoord.y = (*itr)[1].asInt();
			m_obstacles.push_back(curCoord);

			if(curCoord.x > m_maxXY.x)
				m_maxXY.x = curCoord.x;
			if(curCoord.y > m_maxXY.y)
				m_maxXY.y = curCoord.y;

		}
	}
	else
		return false;
	
	mapFile.close();

	// Allocate a grid based on the input file
	// Dynamically allocate the array based on the size in the JSON
	// New initializes the memory values to zero
	m_nodeGrid = new node*[m_maxXY.x+1];
	for(int i=0;i<m_maxXY.x+1;++i)
		m_nodeGrid[i]=new node[m_maxXY.y+1];

	for(int ii=0;ii<=m_maxXY.x;ii++)
	{
		for(int jj=0;jj<=m_maxXY.y;jj++)
		{
			m_nodeGrid[ii][jj].g = 0;
			m_nodeGrid[ii][jj].h = sqrt( pow(double(ii-m_endCoord.x),2)+pow(double(jj-m_endCoord.y),2) );
			m_nodeGrid[ii][jj].f = m_nodeGrid[ii][jj].g + m_nodeGrid[ii][jj].h;
			m_nodeGrid[ii][jj].parent = &m_nodeGrid[0][0];
			m_nodeGrid[ii][jj].x = ii;
			m_nodeGrid[ii][jj].y = jj;
			m_nodeGrid[ii][jj].isObstacle = false;
			m_nodeGrid[ii][jj].isOpenSet = false;
			m_nodeGrid[ii][jj].isClosedSet = false;
				
		}
	}

	for(std::vector<coord>::iterator itr = m_obstacles.begin(); itr != m_obstacles.end(); ++itr)
		m_nodeGrid[(*itr).x][(*itr).y].isObstacle = true;

	
	return true;

}

// Print out the final optimal path
void m_map::printPath(std::string fileString)
{
	node *curNode = &m_nodeGrid[m_endCoord.x][m_endCoord.y];

	// Open an output file
	ofstream outputFile;
	outputFile.open( fileString.c_str() );

	if (outputFile.is_open() )
	{
		//cout << "********OPTIMAL PATH********" << endl;
		outputFile << "********OPTIMAL PATH********" << endl << endl;

		while( !(curNode->x == m_startCoord.x && curNode->y == m_startCoord.y) )
		{
			m_optPath.push_back(curNode);
			//cout << "X: " << curNode->x << endl;
			outputFile << "X: " << curNode->x << endl;
			//cout << "Y: " << curNode->y << endl;
			outputFile << "Y: " << curNode->y << endl;
			//cout << "F: " << curNode->f << endl;
			outputFile << "F: " << curNode->f << endl;
			//cout << endl;
			outputFile << endl;
			curNode = curNode->parent;
		}
	}

	// Print out the starting node
	m_optPath.push_back(curNode);
	//cout << "X: " << curNode->x << endl;
	outputFile << "X: " << curNode->x << endl;
	//cout << "Y: " << curNode->y << endl;
	outputFile << "Y: " << curNode->y << endl;
	//cout << "F: " << curNode->f << endl;
	outputFile << "F: " << curNode->f << endl;
	//cout << endl;
	outputFile << endl;

	outputFile.close();
}

// Debugging function to print out the heap 
void m_map::printHeap()
{
	cout << "********HEAP********" << endl;
	for(std::vector<node*>::iterator itr=m_openSet.begin();itr!=m_openSet.end();++itr)
	{
		cout << "X: " << (*itr)->x << endl;
		cout << "Y: " << (*itr)->y << endl;
		cout << "G: " << (*itr)->g << endl;
		cout << "H: " << (*itr)->h << endl;
		cout << "F: " << (*itr)->f << endl;
		cout << "Obstacle?: " << (*itr)->isObstacle << endl;
		cout << "Parent: " << (*itr)->parent->x << ",";
		cout << (*itr)->parent->y << endl;
		cout << "Closed set?: " << (*itr)->isClosedSet << endl;
		cout << "Open set?: " << (*itr)->isOpenSet << endl;
		cout << endl;
	}
	cout << endl << endl;
}

m_map::node* m_map::checkValidNode(int position, node *curNode)
{

	coord shift;
	switch(position)
	{
		case 1:
			shift.x = 0;
			shift.y = 1;
			break;
		case 2:
			shift.x = 1;
			shift.y = 1;
			break;
		case 3:
			shift.x = 1;
			shift.y = 0;
			break;
		case 4:
			shift.x = 1;
			shift.y = -1;
			break;
		case 5:
			shift.x = 0;
			shift.y = -1;
			break;
		case 6:
			shift.x = -1;
			shift.y = -1;
			break;
		case 7:
			shift.x = -1;
			shift.y = 0;
			break;
		case 8:
			shift.x = -1;
			shift.y = 1;
			break;
	}

	// Coordinate of test
	coord desiredNode;
	desiredNode.x = curNode->x + shift.x;
	desiredNode.y = curNode->y + shift.y;

	// Return a null pointer if the node is not valid
	node *nodePtr = NULL;

	float cost;

	// Check the coordinates are inside the grid and not an obstacle
	if(VALID_NODE)
	{
		if(!m_nodeGrid[desiredNode.x][desiredNode.y].isObstacle)
		{
			// Calculate movement cost
			cost = curNode->g + sqrt(double(shift.x*shift.x + shift.y*shift.y));

			// Movement cost less
			if(cost < m_nodeGrid[desiredNode.x][desiredNode.y].g )
			{
				// In open set
				if(m_nodeGrid[desiredNode.x][desiredNode.y].isOpenSet)
				{
					// Remove neighbor from open
					m_nodeGrid[desiredNode.x][desiredNode.y].isOpenSet = false;
					for(std::vector<node*>::iterator itr=m_openSet.begin();itr!=m_openSet.end();++itr)
					{
						if((*itr)->x==desiredNode.x && (*itr)->y==desiredNode.y)
							m_openSet.erase(itr);
					}
				}

				// In closed set
				if(m_nodeGrid[desiredNode.x][desiredNode.y].isClosedSet)
					// Remove neighbor from closed
					m_nodeGrid[desiredNode.x][desiredNode.y].isClosedSet = false;
			}
			
			if(!m_nodeGrid[desiredNode.x][desiredNode.y].isOpenSet && !m_nodeGrid[desiredNode.x][desiredNode.y].isClosedSet)
			{
				m_nodeGrid[desiredNode.x][desiredNode.y].g = cost;
				m_nodeGrid[desiredNode.x][desiredNode.y].f = m_nodeGrid[desiredNode.x][desiredNode.y].g + m_nodeGrid[desiredNode.x][desiredNode.y].h;
				m_nodeGrid[desiredNode.x][desiredNode.y].isOpenSet = true;
				nodePtr = &m_nodeGrid[desiredNode.x][desiredNode.y];
				m_nodeGrid[desiredNode.x][desiredNode.y].parent = &m_nodeGrid[curNode->x][curNode->y];
			}
		}
		
	}

	return nodePtr;
}

bool m_map::getBestNode()
{

	// Set the current node to the top of the open set
	// TODO: dereference this pointer so we can knock it off the open set
	node *curNode = m_openSet.back();

	// Add the best node to the closed set
	curNode->isClosedSet = true;
	m_closedSet.push_back(curNode);
	curNode->isOpenSet = false;

	// Initialize the possible nodes to be added to the heap
	std::vector<node> possibleNodes;

	// Cycle through the surrounding nodes
	for(int ii=1;ii<9;ii++)
	{ 
		// Check if node may be placed on heap, toggle heap flag
		node *desiredNode = checkValidNode(ii,curNode);
		if (desiredNode )
		{
			if(desiredNode->isOpenSet)
				possibleNodes.push_back(*desiredNode);
		}
	}	

	// Pop the current node off the open set
	m_openSet.pop_back();

	if(m_openSet.size() < 1 && possibleNodes.size()<1)
		return false;

	// Sort by f
	//std::sort( possibleNodes.begin(),possibleNodes.end(),SortByF );

	// Add the nodes on the open set
	for(std::vector<node>::iterator itr = possibleNodes.begin(); itr != possibleNodes.end(); ++itr)
		m_openSet.push_back(&m_nodeGrid[(*itr).x][(*itr).y]);

	// Put the lowest movement cost to on the back of open set
	leastCostToBack();
	
	return true;
	
}

// Get the path and print it to the screen
bool m_map::getPath(std::string fileString, std::string outputFileString)
{
	// Initialize the grid
	if( !m_initGrid(fileString) )
	{
		cout << "Could not open file." << endl;
		return false;
	}

	// Add the start node to the open list of nodes
	m_openSet.push_back(&m_nodeGrid[m_startCoord.x][m_startCoord.y]);
	m_nodeGrid[m_startCoord.x][m_startCoord.y].isOpenSet = true;

	// Current node is the start node
	node *curNode = m_openSet.front();
	
	int cycleCount=0;

	// Expand nodes until the goal node is reached
	while( !(curNode->x == m_endCoord.x && curNode->y == m_endCoord.y) && m_openSet.size()>0)
	{
		cycleCount++;
		if(cycleCount % 1000 == 1)
		{
			//cout << curNode->x << "," << curNode->y << endl;
			//cout << m_openSet.size() << endl;
			cout << ".";
		}		
		// Search around for nodes that you can explore, add appropriate ones to heap
		//printHeap();
		if( !getBestNode() )
		{
			cout << "No possible solution." << endl;
			return false;
		}

		if(!m_openSet.empty())
			curNode = m_openSet.back();
	}

	printPath(outputFileString);

	// Add the final node to the path
	m_optPath.push_back(curNode);

	//printPath(outputFileString);	
	cleanMem();

	return true;
}

void m_map::cleanMem()
{
	// Delete the map object
	delete m_nodeGrid;
}

// Move the node with the smallest f value to the back of the open set
void m_map::leastCostToBack()
{

	//node *minNode = m_openSet.back();
	float minF = (*m_openSet.back()).f;
	std::vector<node*>::iterator minItr = m_openSet.begin();

	for(std::vector<node*>::iterator itr = m_openSet.begin();itr!=m_openSet.end();++itr)
	{
		if( (*itr)->f < minF)
		{
			//minNode = *itr;
			minItr = itr;
			minF = (*itr)->f;
		}	
	}

	std::iter_swap(m_openSet.end()-1,minItr);	
}

// Copy make to an object for GUI purposes
std::vector<m_map::coord> m_map::copyObstacles()
{
	return m_obstacles;
}

// Copy make to an object for GUI purposes
m_map::coord m_map::copyMaxCoord()
{
	return m_maxXY;
}

// Copy make to an object for GUI purposes
m_map::coord m_map::copyEndCoord()
{
	return m_endCoord;
}

// Copy make to an object for GUI purposes
m_map::coord m_map::copyStartCoord()
{
	return m_startCoord;
}

std::vector<m_map::coord> m_map::copyOptPath()
{
	std::vector<coord> vecOptPath;
	coord curCoord;

	for(std::vector<node*>::iterator itr=m_optPath.begin();itr!=m_optPath.end();++itr)
	{
		curCoord.x = (*itr)->x;
		curCoord.y = (*itr)->y;
		vecOptPath.push_back(curCoord);
	}

	return vecOptPath;
}