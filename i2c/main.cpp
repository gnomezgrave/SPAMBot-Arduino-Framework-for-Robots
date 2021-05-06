////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// STL A* Search implementation
// (C)2001 Justin Heyes-Jones
//
// Finding a path on a simple grid maze
// This shows how to do shortest path finding using A*

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stlastar.h" // See header for copyright and usage information

#include <iostream>
#include <stdio.h>
#include <list>
#include "mapsearchnode.h"

#define DEBUG_LISTS 0
#define DEBUG_LIST_LENGTHS_ONLY 0

using namespace std;

// Global data

// The world map

const int MAP_WIDTH = 7;
const int MAP_HEIGHT = 7;

int world_map[ MAP_WIDTH * MAP_HEIGHT ] = 
{
	
// 0001020304050607
	1,1,1,1,1,1,1,  // 00
	1,1,1,1,1,1,1,  // 01
	1,1,1,1,1,1,1,  // 02
	1,1,1,1,1,1,1,  // 03
	1,1,1,1,1,1,1,  // 04
	1,1,1,1,1,1,1,  // 05
	1,1,1,1,1,1,1   // 06
	
};

// map helper functions

int GetMap( int x, int y )
{
	if( x < 0 ||
		x >= MAP_WIDTH ||
		y < 0 ||
		y >= MAP_HEIGHT
	)
	{
		return 9;	 
	}
	
	return world_map[(y*MAP_WIDTH)+x];
}

// Definitions

bool MapSearchNode::IsSameState( MapSearchNode &rhs )
{
	
	// same state in a maze search is simply when (x,y) are the same
	if( (x == rhs.x) &&
		(y == rhs.y) )
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

void MapSearchNode::PrintNodeInfo()
{
	char str[100];
	sprintf( str, "Node position : (%d,%d)\n", x,y );
	
	cout << str;
}

// Here's the heuristic function that estimates the distance from a Node
// to the Goal. 

float MapSearchNode::GoalDistanceEstimate( MapSearchNode &nodeGoal )
{
	float xd = float( ( (float)x - (float)nodeGoal.x ) );
	float yd = float( ( (float)y - (float)nodeGoal.y) );
	
	return xd + yd;
	
}

bool MapSearchNode::IsGoal( MapSearchNode &nodeGoal )
{
	
	if( (x == nodeGoal.x) &&
		(y == nodeGoal.y) )
	{
		return true;
	}
	
	return false;
}

// This generates the successors to the given Node. It uses a helper function called
// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
// is done for each node internally, so here you just set the state information that
// is specific to the application
bool MapSearchNode::GetSuccessors( AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node )
{
	
	int parent_x = -1; 
	int parent_y = -1; 
	
	if( parent_node )
	{
		parent_x = parent_node->x;
		parent_y = parent_node->y;
	}
	
	
	MapSearchNode NewNode;
	
	// push each possible move except allowing the search to go backwards
	
	if( (GetMap( x-1, y ) < 9) 
		&& !((parent_x == x-1) && (parent_y == y))
	) 
	{
		NewNode = MapSearchNode( x-1, y );
		astarsearch->AddSuccessor( NewNode );
	}	
	
	if( (GetMap( x, y-1 ) < 9) 
		&& !((parent_x == x) && (parent_y == y-1))
	) 
	{
		NewNode = MapSearchNode( x, y-1 );
		astarsearch->AddSuccessor( NewNode );
	}	
	
	if( (GetMap( x+1, y ) < 9)
		&& !((parent_x == x+1) && (parent_y == y))
	) 
	{
		NewNode = MapSearchNode( x+1, y );
		astarsearch->AddSuccessor( NewNode );
	}	
	
	
	if( (GetMap( x, y+1 ) < 9) 
		&& !((parent_x == x) && (parent_y == y+1))
	)
	{
		NewNode = MapSearchNode( x, y+1 );
		astarsearch->AddSuccessor( NewNode );
	}	
	
	return true;
}

// given this node, what does it cost to move to successor. In the case
// of our map the answer is the map terrain value at this node since that is 
// conceptually where we're moving

float MapSearchNode::GetCost( MapSearchNode &successor )
{
	return (float) GetMap( x, y );
	
}


// Main
/*
int main( int argc, char *argv[] )
{
	
	// Our sample problem defines the world as a 2d array representing a terrain
	// Each element contains an integer from 0 to 5 which indicates the cost 
	// of travel across the terrain. Zero means the least possible difficulty 
	// in travelling (think ice rink if you can skate) whilst 5 represents the 
	// most difficult. 9 indicates that we cannot pass.
	
	// Create an instance of the search class...
	
	list<MapSearchNode*> lst;
	
	traverse(0,0,5,5,&lst);
	cout<<lst.size()<<endl;
	return 0;
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
