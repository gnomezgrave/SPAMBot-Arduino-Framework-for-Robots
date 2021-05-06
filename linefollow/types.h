
#define MODE_LINE 0
#define MODE_STOP 1
#define MODE_TURN 2

#define IR0 2
#define IR1 3
#define IR2 4
#define IR3 5

#define IRF 6 // IR front
#define IRR 7 // IR rear

#define IRXL 8
#define IRXR 9

#define ML 13
#define MR 12

#define M_STOP 1500
#define M_FORW 300
#define M_SLOW_FORW 20

#define S_LEFT 0b0001
#define S_RIGHT 0b0010
#define S_FORW 0b0100
#define S_TURN180 0b1000

#define MD_PID 0
#define MD_PENDING 1
#define MD_BLOCK 2

enum Headings {
    H_NORTH = 100,
    H_EAST = 101,
    H_SOUTH = 102,
    H_WEST = 103,
    H_UNKNOWN = 104 
};

enum Direction {
    FORWARD = S_FORW,
    RIGHT = S_RIGHT,
    BACKWARD = -1,
    LEFT = S_LEFT,
    TURN180 = S_TURN180
};


#define BOTTOM             0
#define TOP                1
#define DOWN               2
#define UP                 3
#define NONE               4


void grabBox(int box);
void dropBox(int where);

