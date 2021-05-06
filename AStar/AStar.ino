#include <stdio.h>
#include <string.h>
#include "type.h"

#define NUM_NODES   63
#define UNREVEALED  11
#define UNOCCUPIED  12
#define OCCUPIED    13

#define START       7
#define END         49
#define UNLOAD1     13
#define UNLOAD2     27
#define UNLOAD3     41
#define UNLOAD4     55


typedef struct node {
    int north;
    int south;
    int east;
    int west;
} node;

node nodes[NUM_NODES];

bool seen[NUM_NODES];
int available[NUM_NODES];

int sourceNode[NUM_NODES];
int pathIndex;
int path[NUM_NODES];

Headings getNextHeading(Headings cur_heading, Headings nextTurn) {
    if (cur_heading == H_FORWARD) {
        cur_heading = nextTurn;
    } else if (cur_heading == H_BACKWARD) {
        if (nextTurn == H_FORWARD) {
            cur_heading = nextTurn;
        } else if (nextTurn == H_BACKWARD) {
            cur_heading = H_FORWARD;
        } else if (nextTurn == H_RIGHT) {
            cur_heading = H_LEFT;
        } else if (nextTurn == H_LEFT) {
            cur_heading = H_RIGHT;
        }
    } else if (cur_heading == H_RIGHT) {
        if (nextTurn == H_FORWARD) {
            cur_heading = nextTurn;
        } else if (nextTurn == H_BACKWARD) {
            cur_heading = H_LEFT;
        } else if (nextTurn == H_RIGHT) {
            cur_heading = H_BACKWARD;
        } else if (nextTurn == H_LEFT) {
            cur_heading = H_FORWARD;
        }
    } else if (cur_heading == H_LEFT) {
        if (nextTurn == H_FORWARD) {
            cur_heading = nextTurn;
        } else if (nextTurn == H_BACKWARD) {
            cur_heading = H_RIGHT;
        } else if (nextTurn == H_RIGHT) {
            cur_heading = H_FORWARD;
        } else if (nextTurn == H_LEFT) {
            cur_heading = H_BACKWARD;
        }
    }
    return cur_heading;
}

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

int leftNode(int cur, int next) {
  if (nodes[cur].north == next) {
    return nodes[next].west;
  } else if (nodes[cur].east == next) {
    return nodes[next].north;
  } else if (nodes[cur].south == next) {
    return nodes[next].east;
  } else if (nodes[cur].west == next) {
    return nodes[next].south;
  }
}

int rightNode(int cur, int next) {
  if (nodes[cur].north == next) {
    return nodes[next].east;
  } else if (nodes[cur].east == next) {
    return nodes[next].south;
  } else if (nodes[cur].south == next) {
    return nodes[next].west;
  } else if (nodes[cur].west == next) {
    return nodes[next].north;
  }
}

int straightNode(int cur, int next) {
  if (nodes[cur].north == next) {
    return nodes[next].north;
  } else if (nodes[cur].east == next) {
    return nodes[next].east;
  } else if (nodes[cur].south == next) {
    return nodes[next].south;
  } else if (nodes[cur].west == next) {
    return nodes[next].west;
  }
}

void findPath(int startNode, int endNode) {
  memset(seen, 0, sizeof(seen));
  for (int i=0; i<NUM_NODES; i++) {
    sourceNode[i] = -1;
  }
  
 /*for (int i=0; i<=62; i++) {
    if(i%7 == 0 ){
      seen[i] = UNOCCUPIED;
      continue;
    }
    seen[i] = UNREVEALED;
  }*/
   
  int q[NUM_NODES+1];
  int q_top = 0, q_front = 0;
  q[q_top++] = startNode;
  seen[startNode] = true;

  /*for(int i=0;i<63;i++){
    printf("%i - %i, ",i,seen[i]);
    
    if(i%7==6){
      printf("\n");
      
    }
  }*/
  
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
  while (sourceNode[c] != -1 && sourceNode[c] != startNode) {
    c = sourceNode[c];
    path[pathIndex++] = c;
    //printf("%d\n", c);
  }
  
  pathIndex--;
  
}

void setup(){
  initGraph();
  
  for (int i=0; i<=62; i++) {
    if(i%7 == 0 || i%7 == 1){
      available[i] = UNOCCUPIED;
      continue;
    }
    available[i] = UNREVEALED;
  }
  
  available[29] = OCCUPIED;
  
  findPath(57, 8);
}

void loop(){
  
}
/*
int main(){
  
  
  initGraph();
  
  for (int i=0; i<=62; i++) {
    if(i%7 == 0 || i%7 == 1){
      available[i] = UNOCCUPIED;
      continue;
    }
    available[i] = UNREVEALED;
  }
  
  available[29] = OCCUPIED;
  
  findPath(57, 8);


return 0;
}
*/
long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}

int sonar(){
  return rand()*50;
}
