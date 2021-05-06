#include <Servo.h>
#include "types.h"

// Change the mode for testing

volatile int sonar=0;
volatile int md_mode=MD_PID;
volatile int md_data=S_FORW;
volatile int mode=MODE_LINE;

int pos[16]={0};
int prev=0;
int curr=0;
float integral=0;
float derivative=0;
int output=0;
float prev_error,error;
float Kp=1,Ki=0,Kd=0;
int dt=50;
Servo left,right;

int orders[]={S_LEFT, S_LEFT, S_LEFT, S_LEFT};

int this_order=0;



void setup() {
  // go conditions
  pos[0b0001]=2;
  pos[0b0011]=1;
  pos[0b0010]=1;
  pos[0b0110]=0;
  pos[0b0100]=-1;
  pos[0b1100]=-1;
  pos[0b1000]=-2;

  // stop conditions
  pos[0b1110]=0.5;
  pos[0b0111]=-0.5;
  pos[0b1111]=255;
  
  pinMode(IR0, INPUT);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  
  pinMode(IRF, INPUT);
  pinMode(IRR, INPUT);
 
  Serial.begin(9600);
  
  left.attach(ML);
  right.attach(MR);
  
  pinMode(ML, OUTPUT);
  pinMode(MR, OUTPUT);
  
  
  // graph
  initGraph();
   
  Direction head;
  cur_heading = H_SOUTH ;
  
  for (int i=0; i<=62; i++) {
    //if(i%7 == 0 || i%7 == 1){
      available[i] = UNOCCUPIED;
      //continue;
    //}
    //available[i] = UNREVEALED;
  }
  
  //available[29] = OCCUPIED;
  
  findPath(0,23);
  //findPath(23,9);
  //findPath(36,8);
  //findPath(8,9);
  //findPath(9,37);


  init_gripper();
  //Serial.println("S");
  //Serial.println(pathIndex);
}

void loop() {
  switch (mode){
    case MODE_LINE:
    {
      line_follow();
      break;
    }
    case MODE_STOP:
    {
      stationary();
      break;
    }
    case MODE_TURN:
    {
      turn_test();
      break;
    }
    
  }
}

inline void forward()
{
  // never remove delay! see turn_until_white for details
  left.writeMicroseconds(M_STOP + M_FORW);
  right.writeMicroseconds(M_STOP - M_FORW);
}

inline void turn_left_inplace()
{
  // never remove delay! see turn_until_white for details
  left.writeMicroseconds(M_STOP - M_FORW);
  right.writeMicroseconds(M_STOP - M_FORW);
  delay(dt);
}
inline void turn_right_inplace()
{
  left.writeMicroseconds(M_STOP + M_FORW);
  right.writeMicroseconds(M_STOP + M_FORW);
  delay(dt);
}

void turn_until_white_inplace(int signal)
{
  // do not remove delay(in rotate)! when motors start to work the amperage drop
  // causes them to 0 therefore registering false data, always let the
  // sensors recover from the drop
  
  int next_white=0;
  if (signal == S_RIGHT)
  {
    while ( digitalRead(IRF) == 0 )  // if start is white neglect it
    {
      turn_right_inplace();
    }
    while ( digitalRead(IRF) == 1 )
    {
      turn_right_inplace();
    }
  } else if (signal == S_LEFT) {
    while ( digitalRead(IRF) == 0 )  // if start is white neglect it
    {
      turn_left_inplace();
    }
    while ( digitalRead(IRF) == 1 )
    {
      turn_left_inplace();
    }
  }
  left.writeMicroseconds(M_STOP);
  right.writeMicroseconds(M_STOP);
        
}



inline void turn_left()
{
  // never remove delay! see turn_until_white for details
  left.writeMicroseconds(M_STOP);
  right.writeMicroseconds(M_STOP - M_FORW);
  delay(dt);
}
inline void turn_right()
{
  left.writeMicroseconds(M_STOP + M_FORW);
  right.writeMicroseconds(M_STOP);
  delay(dt);
}


void turn_until_white(int signal)
{
  // do not remove delay(in rotate)! when motors start to work the amperage drop
  // causes them to 0 therefore registering false data, always let the
  // sensors recover from the drop
  
  int next_white=0;
  if (signal == S_RIGHT)
  {
    while ( digitalRead(IRF) == 0 )  // if start is white neglect it
    {
      turn_right();
    }
    while ( digitalRead(IRF) == 1 )
    {
      turn_right();
    }
  } else if (signal == S_LEFT) {
    while ( digitalRead(IRF) == 0 )  // if start is white neglect it
    {
      turn_left();
    }
    while ( digitalRead(IRF) == 1 )
    {
      turn_left();
    }
  }
  left.writeMicroseconds(M_STOP);
  right.writeMicroseconds(M_STOP);
        
}
void turn_test()
{
  turn_until_white(S_LEFT);
  left.writeMicroseconds(M_STOP + M_SLOW_FORW);
  right.writeMicroseconds(M_STOP + M_SLOW_FORW);
  delay(500);
  turn_until_white(S_RIGHT);
  left.writeMicroseconds(M_STOP - M_SLOW_FORW);
  right.writeMicroseconds(M_STOP - M_SLOW_FORW);
  delay(500);
}
void line_follow(){
  
  int iXL = 1-digitalRead(IRXL);
  int iXR = 1-digitalRead(IRXR);
  int iXF = 1-digitalRead(IRF);
  int iML = 1-digitalRead(IR0);
  int iMR = 1-digitalRead(IR3);
  
  int iB = 1-digitalRead(IRR);
  
  int iR = 1-digitalRead(IR2);
  int iL = 1-digitalRead(IR1);
  
  if (md_mode == MD_PID) {
    int pos_val=0;
    curr= (1-digitalRead(IR1))<<2 | (1-digitalRead(IR2))<<1;
    pos_val=pos[curr];
    md_data = 0;
    
    if ((iXR || iXL) && curr == 0b0110) {
      int steps=0;
      
      while (steps<4) {
        iXL = 1-digitalRead(IRXL);
        iXR = 1-digitalRead(IRXR);
        iXF = 1-digitalRead(IRF);
        iML = 1-digitalRead(IR0);
        iMR = 1-digitalRead(IR3);
        
        iB = 1-digitalRead(IRR);
        
        iR = 1-digitalRead(IR2);
        iL = 1-digitalRead(IR1);
  
  
        left.writeMicroseconds(M_STOP + M_SLOW_FORW);
        right.writeMicroseconds(M_STOP - M_SLOW_FORW);
        delay(dt);
        
        if (iXR && iMR) {
          md_data|=S_RIGHT+S_FORW*(1-digitalRead(IRF));  
          md_mode = MD_PENDING;
        } 
        if (iXL && iML) {
          md_data|=S_LEFT+S_FORW*(1-digitalRead(IRF));
          md_mode = MD_PENDING;
        }
        
        
        steps++;
      }
      
      
    }

    if (md_mode==MD_PID) {
      // detect block
     
      error = -1.0*pos_val;
      integral+=error*dt;
      derivative=((error) - (prev_error))/dt;
      output = (Kp*error) + (Ki*integral) + (Kd*derivative);
   
      left.writeMicroseconds(M_STOP + M_FORW - output*(M_FORW+5));
      right.writeMicroseconds(M_STOP - M_FORW - output*(M_FORW+5));
      
      prev_error=error;
      delay(dt);
      prev=curr;
      
      sonar=(int)get_sonar();
      //Serial.println(get_sonar());
      if (sonar < 10) {
        while(sonar < 10) {
          left.writeMicroseconds(M_STOP - M_SLOW_FORW );
          right.writeMicroseconds(M_STOP + M_SLOW_FORW );
          delay(1);
          sonar=(int)get_sonar();
        }
        stationary();
        if (sonar <=10){
          grabBox(BOTTOM);
        }
        
        stationary();
      }
      
    } 
  } else if (md_mode == MD_PENDING) {
    if (pathIndex -1 == this_order)
    {
      stationary();
      dropBox(DOWN);
      while(1) {}
    }
    int this_dir=getNextHeading(path[this_order],path[this_order+1]);
    //if (this_order == 0) {
   // Serial.println((this_dir));
   // Serial.println(path[tgrahis_order]);
    Serial.println("---------------");
    
    if (this_dir == S_TURN180) {
      // forward a bit
      int steps=0;
      while (steps<3){
        left.writeMicroseconds(M_STOP + M_FORW );
        right.writeMicroseconds(M_STOP - M_FORW );
        delay(dt);
        steps++;
      }
      
      turn_until_white_inplace(S_LEFT);
      if (md_data & S_LEFT)
        turn_until_white_inplace(S_LEFT);
      stationary();
      md_mode = MD_PID;
      //forward();
      
    } else if (/*(this_dir == S_LEFT) || (this_dir == S_RIGHT) ||*/ md_data & this_dir == this_dir ){
//      if (md_data & S_FORW != S_FORW) {
//        while (digitalRead(IRF)==0){
//          left.writeMicroseconds(M_STOP + M_FORW );
//          right.writeMicroseconds(M_STOP - M_FORW );
//        }
//      }
      stationary();
      //Serial.print("MD");
      //
      //Serial.println(md_data);
      //while (1) {}
      turn_until_white(this_dir);
      
      int steps=0;
      while (steps<8){
        left.writeMicroseconds(M_STOP + M_FORW );
        right.writeMicroseconds(M_STOP - M_FORW );
        delay(dt);
        steps++;
      }
      // give control back to PID
      
      
      
      md_mode = MD_PID;
      
      stationary();     
    } 
    
    this_order = this_order +1;        
  } else if (md_mode == MD_BLOCK) {
    Serial.print("XX");
    //stationary();
  }
}

void stationary(){
  left.writeMicroseconds(M_STOP);
  right.writeMicroseconds(M_STOP);
}
