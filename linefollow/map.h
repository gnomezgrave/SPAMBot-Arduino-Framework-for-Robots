 


#define NUM_NODES   63
#define UNREVEALED  11
#define UNOCCUPIED  12
#define OCCUPIED    13

#define START       7			// West of 7
#define END         49			// West of 49
#define UNLOAD1     13			// East of 13
#define UNLOAD2     27			// East of 27
#define UNLOAD3     41			// East of 41
#define UNLOAD4     55			// East of 55

extern int pathIndex;
extern int path[NUM_NODES];
extern int p[NUM_NODES];


extern Headings cur_heading;

struct node {
    int north;
    int south;
    int east;
    int west;
};



Headings nodePos(int n1,int n2);