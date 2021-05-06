   #include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include <wiringPi.h>

#include "stlastar.h" // See header for copyright and usage information
#include "mapsearchnode.h"
#include <iostream>
#include <stdio.h>
#include <list>

// The Arduimo board i2c address
#define ADDRESS 0x04

using namespace std;

typedef struct node {
    int x;
    int y;
    int occupied;
    int cubeNumber;
} Node;

enum Params {
    DIRECTIONS = 0,
    SONAR = 1,	// Front
    TURN = 2,	//
    SONAR2 = 3,	// Left
    SONAR3 = 4,	// Right
    UNKNOWN = -1
};

enum Values {
    LEFT = 10,
    RIGHT = 11,
    STRAIGHT = 12,
    EMPTY = -1
};

enum Headings {
    H_LEFT = 100,
    H_RIGHT = 101,
    H_FORWARD = 102,
    H_BACKWARD = 103,
    H_UNKNOWN = 104
};

void interrupt(void);
void sendData(int[]);
int requestSonar();
Headings nextTurn(Headings c_head, int x1,int y1,int x2,int y2);
void traverse(int x1,int y1, int x2,int y2, AStarSearch<MapSearchNode>* astarsearch,list<MapSearchNode*> lst);

// The I2C bus: This is for V2 pi's. For V1 Model B you need i2c-0
static const char *devName = "/dev/i2c-1";

Headings cur_heading;
int x, y;
int cur_dest[2];

int file;

bool isHalt = false;

Node graph[7][9];

list<Values> orders;

AStarSearch<MapSearchNode>* astarsearch;

int cur_sonar;

int volatile interruptControl;

void interrupt(void){
    unsigned char cmd[2];
	char buf[2];
	int param = -1;
	int value = -1;

	if (read(file, buf, 2) == 1) {
		// Now wait else you could crash the Arduino by sending requests too fast
		usleep(10000);
		param = (Params) buf[0];
		value = (Values) buf[1];

		if (param == DIRECTIONS) {
			if(value == 0b0111){
				// cross
			}else if(value == 0b0101){
				// T
			}else if(value == 0b0001){
				// Right L
			}else if(value == 0b0100){
				// Left L
			}else if(value == 0b0011){
				// T to Right
			}else if(value == 0b0110){
				// T to Left
			}

		} else if (param == SONAR) {
			int a=x;
			int b=y;
			if(value < 30){
				if(cur_heading == H_FORWARD){
					b = y<8 ? y+1:y;
				}else if(cur_heading == H_BACKWARD){
					b = y>1 ? y-1:y;
				}else if(cur_heading == H_LEFT){
					a = x>1 ? x-1:x;
				}else if(cur_heading == H_RIGHT){
					a = x<8 ? x+1:x;
				}
				graph[a][b].occupied = 1;
				cur_sonar = 1;
			}else{
				cur_sonar = 0;
				graph[a][b].occupied = 0;
			}
		} else if (param == TURN) {

		}
		interruptControl = 1;
	}

}

void sendData(int data[]){
	if (write(file, data, 2) == 1) {
		usleep(10000);
	}
}

void block(){
	while(!interruptControl){
		usleep(10000);
	}
	interruptControl = 0;
	
}

int main(int argc, char** argv) {

    if ((file = open(devName, O_RDWR)) < 0) {
        fprintf(stderr, "I2C: Failed to access %d\n", devName);
        exit(1);
    }

    printf("I2C: acquiring buss to 0x%x\n", ADDRESS);

    if (ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
        fprintf(stderr, "I2C: Failed to acquire bus access/talk to slave 0x%x\n", ADDRESS);
        exit(1);
    }

    x = 0;
    y = 0;
    
    interruptControl = 0;

    wiringPiSetup () ;

    wiringPiISR (0, INT_EDGE_RISING, &interrupt) ;
    
    astarsearch = new AStarSearch<MapSearchNode>();

    for (int i = 0; i <7; i++) {
        for (int j = 1; j < 9; j++) {
            Node n;
            n.x=i;
            n.y=j;
            n.occupied = -1;
            n.cubeNumber = -1;
            graph[i][j] = n;
        }
    }

    cur_heading = H_FORWARD;
    
	int buf[2];
	
	// go to first cross
	buf[0] = TURN;
	buf[1] = STRAIGHT;
	sendData(buf);
	
	x = 0;
	y = 1;
	
	block();
	
	buf[0] = SONAR;
	buf[1] = 1;
	
	sendData(buf);
	
	list<MapSearchNode*> lst;
	
	traverse(0,1,0,8,astarsearch,lst);
	
	while(lst.size()>0)
	{
		MapSearchNode* el = lst.back();
		lst.pop_back();
		Headings turn = nextTurn(cur_heading,x,y,el->x,el->y);
		
		buf[0] = TURN;
		buf[1] = turn;
		
		sendData(buf);
		
		block();
		usleep(50000);
	}

    close(file);
    return (EXIT_0SUCCESS);
}



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

int requestSonar() {
    unsigned char cmd[1];
    cmd[0] = SONAR;
    if (write(file, cmd, 1) == 1) {
        usleep(10000);
        char buf[2];

        if (read(file, buf, 2) == 1) {
            if (buf[0] == (int) SONAR) {
                return (int) buf[1];
            }
        }
    }
    return -1;
}

Headings nextTurn(Headings c_head,int x1,int y1,int x2,int y2){
	if(x1 - x2 > 0){
		//left
		return getNextHeading(cur_heading,H_LEFT);
	}else if(x1 - x2 <0){
		//right
		return getNextHeading(cur_heading,H_RIGHT);
	}
	if(y1 - y2 >0){
		// back
		return getNextHeading(cur_heading,H_BACKWARD);
	}else if(y1 - y2 <0){
		//forward
		return getNextHeading(cur_heading,H_FORWARD);
		
	}
}

void traverse(int x1,int y1, int x2,int y2, AStarSearch<MapSearchNode>* astarsearch,list<MapSearchNode*> lst){
	//AStarSearch<MapSearchNode> astarsearch;
	
	unsigned int SearchCount = 0;
	
	// Create a start state
	MapSearchNode nodeStart;
	nodeStart.x = x1;
	nodeStart.y = y1; 
	
	// Define the goal state
	MapSearchNode nodeEnd;
	nodeEnd.x = x2;						
	nodeEnd.y = y2; 
	
	// Set Start and goal states
	cout<<nodeStart.x<<","<<nodeStart.y<<endl;
	astarsearch->SetStartAndGoalStates( nodeStart, nodeEnd );
	
	unsigned int SearchState;
	unsigned int SearchSteps = 0;
	
	do
	{
		SearchState = astarsearch->SearchStep();
		SearchSteps++;
		
	}
	while( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SEARCHING );
	
	if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_SUCCEEDED )
	{
		MapSearchNode *node = astarsearch->GetSolutionStart();
		while (node)
		{
			node->PrintNodeInfo();
			lst.push_back(node);
			node = astarsearch->GetSolutionNext();
		};
		// Once you're done with the solution you can free the nodes up
		astarsearch->FreeSolutionNodes();
	}
	else if( SearchState == AStarSearch<MapSearchNode>::SEARCH_STATE_FAILED ) 
	{

		// fail
	}
	
	
	astarsearch->EnsureMemoryFreed();
}

