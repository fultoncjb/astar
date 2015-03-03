/*********************************************************************/
/************************ Cameron Fulton *****************************/
/************************ Copyright 2015 *****************************/
/*********************************************************************/
/*************************   astar.cpp   *****************************/
/* Given a JSON file of obstacles, a starting coordinate, and an     */
/* ending coordinate, output the optimal path to a file using A*.    */

// Debugging for diagnosing memory leaks
//static int dynamicMemCounter = 0;

#include"astar.h"

using namespace std;

m_map::m_map()
{
	m_startCoord = {0,0};
	m_endCoord = {0,0};
	m_maxXY = {0,0};
	m_nodeGrid = NULL;
}

m_map::~m_map()
{
}

// Node construct
Node::Node()
{
	isObstacle = false;
	isOpenSet = false;
	isClosedSet = false;
	g = 0;
	f = 0;
	h = 0;
	parent = NULL;
}

// ----------------------------------------------------------- //
// --------------------------InitMap-------------------------- //
// Description: read a JSON file with robotStart, robotEnd,    //
// and obstacles. Allocate m_nodeGrid on the heap. Initialize  //
// nodes. Return false if the file does not contain the JSON   //
// objects.                                                    //
// ----------------------------------------------------------- //
bool m_map::InitMap(std::string fileString)
{
	// Hold the entire file in an object
	Json::Value mapValue;
	// Coordinate where the robot starts
	Json::Value startValue;
	// Coordinate where the robot ends
	Json::Value endValue;
	// Coordinates where the obstacles are
	Json::Value obsValue;

	// Try to open chosen input file
	std::ifstream mapFile(fileString.c_str());

	// Coordinate type to initialize obstacle flags
	Coord curCoord;

	if( mapFile.is_open() )
	{
		// Get the data from the JSON file
		try
		{
			mapFile >> mapValue;
			startValue = mapValue.get("robotStart","UTF-8");
			endValue = mapValue.get("robotEnd","UTF-8");
			obsValue = mapValue.get("obstacles","UTF-8");
		}
		// File is not formatted correctly
		catch(std::exception &fileFormat)
		{
			MapInitialized = false;
			return false;
		} 

		// JSONCPP don't throw exceptions when the file is formatted but doesn't contain the strings indicated
		if(startValue.size() == 0 || endValue.size() == 0 || obsValue.size() == 0)
		{
			MapInitialized = false;
			return false;
		}

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

		// Populate the obstacles vector
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
	// File unsuccessfully opened
	else
	{
		MapInitialized = false;
		return false;
	}
	
	mapFile.close();

	// Allocate a grid based on the input file
	m_nodeGrid = new Node*[m_maxXY.x+1];
	//dynamicMemCounter++;
	// Check if grid was allocated successfully
	if(!m_nodeGrid)
	{
		MapInitialized = false;
		return false;
	}

	for(int i=0;i<m_maxXY.x+1;++i)
	{
		//dynamicMemCounter++;
		m_nodeGrid[i]=new Node[m_maxXY.y+1];
		if(!m_nodeGrid[i])
		{
			MapInitialized = false;
			return false;
		}
	}

	// Initialize nodes
	for(int ii=0;ii<=m_maxXY.x;ii++)
	{
		for(int jj=0;jj<=m_maxXY.y;jj++)
		{
			m_nodeGrid[ii][jj].h = sqrt( pow(double(ii-m_endCoord.x),2)+pow(double(jj-m_endCoord.y),2) );
			m_nodeGrid[ii][jj].f = m_nodeGrid[ii][jj].g + m_nodeGrid[ii][jj].h;
			// Initialize all parents to (0,0)
			m_nodeGrid[ii][jj].parent = &m_nodeGrid[0][0];
			m_nodeGrid[ii][jj].location.x = ii;
			m_nodeGrid[ii][jj].location.y = jj;
				
		}
	}

	// Populate the obstacles vector
	for(std::vector<Coord>::iterator itr = m_obstacles.begin(); itr != m_obstacles.end(); ++itr)
		m_nodeGrid[itr->x][itr->y].isObstacle = true;


	// Map successfully initialized
	MapInitialized = true;
	return true;

}

// ----------------------------------------------------------- //
// 						WritePathToFile 					   //
// Description: output optimal path to the given file.         //
// ----------------------------------------------------------- //
void m_map::WritePathToFile(std::string fileString)
{
	Node *curNode = &m_nodeGrid[m_endCoord.x][m_endCoord.y];
	Coord currentCoord = m_endCoord;

	// Open an output file
	ofstream outputFile;
	outputFile.open( fileString.c_str() );

	// File successfully opened
	if (outputFile.is_open() )
	{
		outputFile << "********OPTIMAL PATH********" << endl << endl;

		// Start at the end node and use parent pointers until the start node is reached
		while( !(curNode->location.x == m_startCoord.x && curNode->location.y == m_startCoord.y) )
		{
			currentCoord = { curNode->location.x,curNode->location.y };
			m_optPath.push_back(currentCoord);
			outputFile << "X: " << curNode->location.x << endl;
			outputFile << "Y: " << curNode->location.y << endl;
			outputFile << "F: " << curNode->f << endl;
			outputFile << endl;
			curNode = curNode->parent;
		}
	}

	// Print out the starting node
	m_optPath.push_back(currentCoord);
	outputFile << "X: " << curNode->location.x << endl;
	outputFile << "Y: " << curNode->location.y << endl;
	outputFile << "F: " << curNode->f << endl;
	outputFile << endl;

	outputFile.close();
}

// ----------------------------------------------------------- //
//  						PrintSet                           //
// Description: debugging function to print out the open set.  //
// ----------------------------------------------------------- //
void m_map::OpenSet::PrintSet()
{
	cout << "********HEAP********" << endl;
	for(std::vector<Node*>::iterator itr=set.begin();itr!=set.end();++itr)
	{
		cout << "X: " << (*itr)->location.x << endl;
		cout << "Y: " << (*itr)->location.y << endl;
		cout << "G: " << (*itr)->g << endl;
		cout << "H: " << (*itr)->h << endl;
		cout << "F: " << (*itr)->f << endl;
		cout << "Obstacle?: " << (*itr)->isObstacle << endl;
		cout << "Parent: " << (*itr)->parent->location.x << ",";
		cout << (*itr)->parent->location.y << endl;
		cout << "Closed set?: " << (*itr)->isClosedSet << endl;
		cout << "Open set?: " << (*itr)->isOpenSet << endl;
		cout << endl;
	}
	cout << endl << endl;
}

// ----------------------------------------------------------- //
//  				GetLocalMovementCost                       //
// Description: return the rectilinear cost needed to move in  //
// certain directions.                                         //
// ----------------------------------------------------------- //
Coord m_map::GetLocalMovementCost(int position)
{

	Coord shift;
	switch(position)
	{
		case NORTH:
			shift.x = 0;
			shift.y = 1;
			break;
		case NORTHEAST:
			shift.x = 1;
			shift.y = 1;
			break;
		case EAST:
			shift.x = 1;
			shift.y = 0;
			break;
		case SOUTHEAST:
			shift.x = 1;
			shift.y = -1;
			break;
		case SOUTH:
			shift.x = 0;
			shift.y = -1;
			break;
		case SOUTHWEST:
			shift.x = -1;
			shift.y = -1;
			break;
		case WEST:
			shift.x = -1;
			shift.y = 0;
			break;
		case NORTHWEST:
			shift.x = -1;
			shift.y = 1;
			break;
	}

	return shift;

}


// ----------------------------------------------------------- //
//  					NodeIsInsideGrid                       //
// Description: check whether the surrounding node in question //
// is within the bounds of the grid as defined by start        //
// coordinates, goal coordinates, and obstacle coordinates.    //
// ----------------------------------------------------------- //
bool m_map::NodeIsInsideGrid(Coord coord)
{
	return ( (coord.x <= m_maxXY.x && coord.x >= 0) && (coord.y <= m_maxXY.y && coord.y >= 0) );
}

// ----------------------------------------------------------- //
//  					NodeIsObstacle                         //
// Description: check whether the surrounding node in question //
// is an obstacle.                                             //
// ----------------------------------------------------------- //
bool m_map::NodeIsObstacle(Coord coord)
{
	return m_nodeGrid[coord.x][coord.y].isObstacle;
}

// ----------------------------------------------------------- //
//  						NodeIsValid                        //
// Description: check whether the surrounding node in question //
// is not an obstacle and within the bounds of the grid.       //
// ----------------------------------------------------------- //
bool m_map::NodeIsValid(Coord coord)
{
	if( NodeIsInsideGrid(coord) && !NodeIsObstacle(coord) )
		return true;

	return false;
}

// ----------------------------------------------------------- //
//  				ExploreSurroundingNodes                    //
// Description: examine 8 nodes surrounding the current node   //
// and update the open and closed sets based on the movement   //
// cost. Return false if explorable nodes have been exhausted. //
// ----------------------------------------------------------- //
bool m_map::ExploreSurroundingNodes()
{

	// Set the current node to the end of the open set (i.e. lowest cost)
	Node *curNode = m_openSet.set.back();
	Coord currentPosition;
	currentPosition.x = curNode->location.x;
	currentPosition.y = curNode->location.y;

	// Add the best node to the closed set
	curNode->isClosedSet = true;

	// Node to hold potential nodes to be added to open set
	Coord desiredCoord;

	// Directional movement cost
	Coord localMovementCost;

	// Pop the current node off the open set
	curNode->isOpenSet = false;
	m_openSet.set.pop_back();

	// Cycle through the surrounding nodes
	for(int ii=NORTH;ii<=NORTHWEST;ii++)
	{ 
		// Calculate distance formula for particular directional movement
		localMovementCost = GetLocalMovementCost(ii);
		desiredCoord.x = currentPosition.x + localMovementCost.x;
		desiredCoord.y = currentPosition.y + localMovementCost.y;

		// Node is within bounds of grid and not an obstacle
		if( NodeIsValid(desiredCoord) )
			UpdateExplorableSets(desiredCoord,localMovementCost);
	}	

	// Run out of nodes to explore, no possible solution
	if(m_openSet.set.size() < 1)
		return false;

	return true;
}

// ----------------------------------------------------------- //
//  				UpdateExplorableSets                       //
// Description: add/remove nodes from the open and closed sets.//
// If the current movement cost is less than the stored cost   //
// of the surrounding node and the node is on the open set     //
// then update the cost and insert it in the correct position  //
// on the open set. Remove the node from the closed set if     //
// the current movement cost is less than the stored cost.     //
// If the surrounding node is not currently on the open or     //
// closed set, add it to the open set.                         //
// ----------------------------------------------------------- //
void m_map::UpdateExplorableSets(Coord desiredCoord,Coord cost)
{
	Coord currentCoord = { desiredCoord.x-cost.x,desiredCoord.y-cost.y };
	float desiredCost = m_nodeGrid[currentCoord.x][currentCoord.y].g + sqrt(cost.x*cost.x+cost.y*cost.y);
	Node *desiredNode = &m_nodeGrid[desiredCoord.x][desiredCoord.y];

	// The potential movement cost is lower than the stored value
	if(desiredCost < desiredNode->g)
	{
		// Surrounding node is already on the open set
		if( desiredNode->isOpenSet )
		{
			// Delete the node from the open set
			m_openSet.DeleteNode(*desiredNode);

			// Update the movement costs and parent
			desiredNode->g = desiredCost;
			desiredNode->f = desiredNode->h + desiredNode->g;
			desiredNode->parent = &m_nodeGrid[currentCoord.x][currentCoord.y];

			// Add the node back to the open set in the updated position
			m_openSet.Insert(*desiredNode);
		}

		if(desiredNode->isClosedSet)
			// Delete from closed set
			DeleteNodeFromClosedSet(*desiredNode);
	}

	if(!desiredNode->isOpenSet && !desiredNode->isClosedSet)
	{
		//Update the movement costs and parent
		desiredNode->g = desiredCost;
		desiredNode->f = desiredNode->h + desiredNode->g;
		desiredNode->parent = &m_nodeGrid[currentCoord.x][currentCoord.y];

		//InsertNodeInOpenSet(*desiredNode);
		m_openSet.Insert(*desiredNode);
	}
}

// ----------------------------------------------------------- //
//  			    	SolveOptimalPath                       //
// Description: public function to solve the map.			   //
// ----------------------------------------------------------- //
bool m_map::SolveOptimalPath(std::string inputFileString, std::string outputFileString)
{
	// Initialize the grid
	if( !MapInitialized )
	{
		if( !InitMap(inputFileString) )
		{
			cout << "Could not open file." << endl;
			return false;
		}
	}

	// Add the start node to the open list of nodes
	m_openSet.Insert(m_nodeGrid[m_startCoord.x][m_startCoord.y]);

	// Current node is the start node
	Node *curNode;

	// Explore nodes until the end coordinate is reached or there are no explorable nodes left
	do {
		curNode = m_openSet.set.back();

		// Search around for nodes that you can explore, add appropriate ones to open set
		if( !ExploreSurroundingNodes() )
		{
			cout << "No possible solution." << endl;
			return false;
		}
	} while( !(curNode->location.x == m_endCoord.x && curNode->location.y == m_endCoord.y) && m_openSet.set.size()>0);

	// Write output to file
	WritePathToFile(outputFileString);

	// Add the final node to the path
	Coord currentCoord = { curNode->location.x,curNode->location.y };
	m_optPath.push_back(currentCoord);

	// Set the map solved flag
	MapState = FULL_MAP;

	// Delete the node grid
	DeleteNodeGrid();

	// Open set no longer needed
	m_openSet.DeleteSet();

	return true;
}

// Public function for accessing obstacles
std::vector<Coord> m_map::copyObstacles()
{
	return m_obstacles;
}

// Public function for accessing maximum coordinates
Coord m_map::copyMaxCoord()
{
	return m_maxXY;
}

// Public function for accessing goal coordinates
Coord m_map::copyEndCoord()
{
	return m_endCoord;
}

// Public function for accessing start coordinates
Coord m_map::copyStartCoord()
{
	return m_startCoord;
}

// Public function for accessing optimal path
std::vector<Coord> m_map::copyOptPath()
{
	return m_optPath;
}

// ----------------------------------------------------------- //
//  			    		DeleteNode                         //
// Description: remove a node from the open set.			   //
// ----------------------------------------------------------- //
void m_map::OpenSet::DeleteNode(Node &n)
{
	if(n.isOpenSet)
	{
		// Remove the desired node from open set
		n.isOpenSet = false;

		// Find the reference to this node in the open set
		std::vector<Node*>::iterator itr = find(set.begin(),set.end(),&n);

		// Erase this node from the open set
		if( itr != set.end() )
			set.erase(itr);
	}
}

void m_map::DeleteNodeFromClosedSet(Node &n)
{
	n.isClosedSet = false;
}

// ----------------------------------------------------------- //
//  			    	SolveOptimalPath                       //
// Description: insert the node into the open set at the       //
// correct position based on movement cost.					   //
// ----------------------------------------------------------- //
void m_map::OpenSet::Insert(Node &n)
{
	// Already on the open set
	if(n.isOpenSet)
		return;
	else
	{
		set.insert(set.begin()+FindInsertPosition(n),&n);
		n.isOpenSet = true;
	}
}

// ----------------------------------------------------------- //
//  			    	FindInsertPosition                     //
// Description: pseudo binary search for finding the position  //
// at which the current node should be inserted into the open  //
// set. 													   //
// ----------------------------------------------------------- //
int m_map::OpenSet::FindInsertPosition(Node n)
{
	// Find the indices the current node will be inserted in between
	int upperBoundIdx = set.size() / 2;
	int lowerBoundIdx = upperBoundIdx - 1;
	// Amount of indices to traverse at once
	int currentInterval = set.size() / 2;

	// Open set is empty, just add the node at the 0 position
	if( set.size() <= 0 )
		return 0;

	// Check boundary conditions
	if( n.f >= set.front()->f )
		return 0;
	if( n.f <= set.back()->f )
		return set.size();

	// Execute until the current node cost is sandwiched correctly between two nodes
	do{
		// Update the interval at which indices are traversed
		currentInterval /= 2;
		if( currentInterval <= 0 )
			currentInterval = 1;

		// Enforce the indices must be within the size of the vector
		if(lowerBoundIdx < 0)
			lowerBoundIdx = 0;
		if( upperBoundIdx >= set.size() )
			upperBoundIdx = set.size() - 1;

		// Current node cost is greater than the lower bound, decrease index
		if( n.f > set.at(lowerBoundIdx)->f )
		{
			lowerBoundIdx -= currentInterval;
			upperBoundIdx -= currentInterval;
		}
		// Current node cost is less than the upper bound, increase index
		else if( n.f < set.at(upperBoundIdx)->f )
		{
			lowerBoundIdx += currentInterval;
			upperBoundIdx += currentInterval;
		}

	} while( !(n.f >= set.at(upperBoundIdx)->f && n.f <= set.at(lowerBoundIdx)->f) );

	return upperBoundIdx;
}

// Delete the dynamically allocated node grid
void m_map::DeleteNodeGrid()
{
	for(unsigned int ii=0;ii<m_maxXY.x+1;ii++)
		delete [] m_nodeGrid[ii];

	delete [] m_nodeGrid;

	m_nodeGrid = NULL;
}

// Delete the open set
void m_map::OpenSet::DeleteSet()
{
	set.clear();
}

// Clear the map of the obstacles and the optimal path
void m_map::ClearMap()
{
	m_obstacles.clear();
	m_optPath.clear();
}
