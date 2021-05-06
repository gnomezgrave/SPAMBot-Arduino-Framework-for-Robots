#include <map.h>

int pathIndex;
int path[NUM_NODES];
int p[NUM_NODES];


Headings cur_heading;

node nodes[NUM_NODES];

bool seen[NUM_NODES];
int available[NUM_NODES];

int sourceNode[NUM_NODES];

int reset=0;
int len=0;


char *DirectionsString[]={"FORWARD","RIGHT","BACKWARD","LEFT","TURN180"};
char *getDirectionString(int signal)
{
  if (signal==S_LEFT)
    return "Left";
  else if (signal==S_RIGHT)
    return "Right";
  else if (signal==S_FORW)
    return "Forward";
  else if (signal==S_TURN180)
    return "Turn180";
}

Headings nodePos(int  n1, int n2){
	if(nodes[n1].north == n2)
		return H_NORTH;
	else if(nodes[n1].east == n2)
		return H_EAST;
	else if(nodes[n1].south == n2)
		return H_SOUTH;
	else if(nodes[n1].west == n2)
		return H_WEST;
	else
		return H_UNKNOWN;
}



Direction getNextHeading(int n1, int n2) {
	Headings nextPos = nodePos(n1,n2);
	//printf("NExt Pos : %i\n",nextPos);
	switch(cur_heading){
	  case H_NORTH:
		switch(nextPos){
			case H_NORTH:
				return FORWARD;
			case H_EAST:
				cur_heading = H_EAST;
				return RIGHT;
			case H_SOUTH:
				cur_heading = H_SOUTH;
				return TURN180;
			case H_WEST:
				cur_heading = H_WEST;
				return LEFT;
		}
		 break;
	  case H_EAST:
		  switch(nextPos){
			case H_NORTH:
				cur_heading = H_NORTH;
				return LEFT;
			case H_EAST:
				cur_heading = H_EAST;
				return FORWARD;
			case H_SOUTH:
				cur_heading = H_SOUTH;
				return RIGHT;
			case H_WEST:
				cur_heading = H_WEST;
				return TURN180;
		}
		 break;
	  case H_SOUTH:
		  switch(nextPos){
			case H_NORTH:
				cur_heading = H_NORTH;
				return TURN180;
			case H_EAST:
				cur_heading = H_EAST;
				return LEFT;
			case H_SOUTH:
				cur_heading = H_SOUTH;
				return FORWARD;
			case H_WEST:
				cur_heading = H_WEST;
				return RIGHT;
		}
		 break;
	  case H_WEST:
		  switch(nextPos){
			case H_NORTH:
				cur_heading = H_NORTH;
				return RIGHT;
			case H_EAST:
				cur_heading = H_EAST;
				return TURN180;
			case H_SOUTH:
				cur_heading = H_SOUTH;
				return LEFT;
			case H_WEST:
				cur_heading = H_WEST;
				return FORWARD;
		}
		 break;
  }
}



/* int stationNodes[] = {
  7,  // Start
  49,  // End
  13, // Unloading Bay 1
  27, // Unloading Bay 2
  41, // Unloading Bay 3
  55, // Unloading Bay 4
};*/

void setEdges(int i, int n, int s, int e, int w) {
  nodes[i].north = n;
  nodes[i].south = s;
  nodes[i].east = e;
  nodes[i].west = w;
}
int getNorth(int n){
  return (n<7?-1:n-7);
}

int getSouth(int n){
  return (n>55?-1:n+7);
}

int getEast(int n){
  return (n%7==6?-1:n+1);
}

int getWest(int n){
  return (n%7==0?-1:n-1);
}

void initGraph() {
  for(int i=0; i<63; i++){
      setEdges(i,getNorth(i),getSouth(i),getEast(i),getWest(i));
  }
}

void findPath(int startNode, int endNode) {
  memset(seen, 0, sizeof(seen));
  
  for (int i=0; i<NUM_NODES; i++) {
    sourceNode[i] = -1;
  }
     
  int q[NUM_NODES+1];
  int q_top = 0, q_front = 0;
  q[q_top++] = startNode;
  seen[startNode] = true;
  
  while (q_front < q_top) {
    int cur = q[q_front++];
    
    //printf("cur %d\n", cur);
    
    if (nodes[cur].north >= 0 && !seen[nodes[cur].north] && available[nodes[cur].north] == UNOCCUPIED) {
      q[q_top++] = nodes[cur].north;
      seen[nodes[cur].north] = true;
      sourceNode[nodes[cur].north] = cur;
    }
    if (nodes[cur].south >= 0 && !seen[nodes[cur].south] && available[nodes[cur].south] == UNOCCUPIED) {
      q[q_top++] = nodes[cur].south;
      seen[nodes[cur].south] = true;
      sourceNode[nodes[cur].south] = cur;
    }
    if (nodes[cur].east >= 0 && !seen[nodes[cur].east] && available[nodes[cur].east] == UNOCCUPIED) {
      q[q_top++] = nodes[cur].east;
      seen[nodes[cur].east] = true;
      sourceNode[nodes[cur].east] = cur;
    }
    if (nodes[cur].west >= 0 && !seen[nodes[cur].west] && available[nodes[cur].west] == UNOCCUPIED) {
      q[q_top++] = nodes[cur].west;
      seen[nodes[cur].west] = true;
      sourceNode[nodes[cur].west] = cur;
    }
    
  }
  
  pathIndex = 0;
  int c = endNode;
  path[pathIndex++] = endNode;
  
  while (sourceNode[c] != -1 && sourceNode[c] != startNode) {
    c = sourceNode[c];
    path[pathIndex++] = c;
  }
  
  path[pathIndex++]=startNode;
  
  /*for (int i=0;i<pathIndex;i++){
    //head=getDirection(head,path[i],path[i+1]);
    printf("%d\n",path[i]);
  }*/
  
  for (int i=pathIndex-1;i>pathIndex/2-1;i--){
    int tmp=path[i];
    path[i]=path[pathIndex-i-1];
    path[pathIndex-i-1]=tmp;
  }
  
  /*printf("\n");
  for (int i=0;i<pathIndex;i++){
    //head=getDirection(head,path[i],path[i+1]);
    printf("%d\n",path[i]);
  }*/
  
  if(reset==0){
   memcpy(&p[len],path,pathIndex*sizeof(int));
  }
  
}