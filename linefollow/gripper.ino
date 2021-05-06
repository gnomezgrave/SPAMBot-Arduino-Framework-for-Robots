#include <Servo.h>

#define GRIP               10             // 0 - fully closed together         ///// 180 - fully open
#define SHOULDER           11            // 0 - back against the "shoulder"   ///// 180 - fully stretched out

#define DEFAULT_SHOULDR    20 //60           // default angle of the shoulder servo 
#define SHOULDR_BTM_GRAB   150           // angle of shoulder when grabbing bottom box 
#define SHOULDR_TOP_GRAB   125           // angle of shoulder when grabbing top box 
#define SHOULDR_LIFT       130//90//20 

#define DEFAULT_GRIP       20           // default angle of the grip servo (Grip Open)
#define GRIP_GRAB          160             // Angle for grip servo when grabbed (Grip Closed)
/*
#define BOTTOM             0
#define TOP                1
#define DOWN               2
#define UP                 3
#define NONE               4
*/
//#define WRIST            12            // 0 - twisted upwards               ///// 180 - wrist looking downwards
//#define ELBOW            7             // 0 - looking downwards             ///// 180 - rotating upwards
//#define BASE             9        

void grabBox(int box);
void dropBox(int where);

Servo shoulder, grip;                    // Servos for Shoulder and Grip movement      
int whichBox;                            // Which box has been lifted?
//int currentAngle; 
boolean isOccupied;

// Current Servo Position/Angle 

void init_gripper(){ 
  shoulder.attach(SHOULDER);            // shoulder servo attached to PIN denotedd by "SHOULDER"
  grip.attach(GRIP);                    // grip servo attached to PIN denotedd by "GRIP"
  
  //currentAngle = DEFAULT_SHOULDR;
  
  shoulder.write(DEFAULT_SHOULDR);     // Set shoulder to default position of 80 degrees (perpendicular to the robot)   
  grip.write(DEFAULT_GRIP);            // Set grip to default position of 160 degrees (fully open) 
  delay(3000);
  // Serial.begin(9600);                 // Set Serial Monitor for Debugging
}

/*
void loop(){
  delay(10000);
  grabBox(BOTTOM);
  delay(10000);
  //dropBox(DOWN);
  //delay(5000);
}*/


void grabBox(int box){
  
  switch(box){
    case TOP:                                                                  // Grap a box on the surface level
               for(int a = DEFAULT_SHOULDR; a <= SHOULDR_TOP_GRAB ; a += 1){     // Rotate gripper and set it at 125 degrees to grab the top box
                  shoulder.write(a);
                  delay(20);
                }
                
                delay(500);
                
                for(int a = DEFAULT_GRIP; a <= GRIP_GRAB ; a += 1){              // Fully close gripper to grab object
                  grip.write(a);
                  //delay(10);
                }
                
                delay(500);
                
                for(int a = SHOULDR_TOP_GRAB; a >= (SHOULDR_TOP_GRAB - SHOULDR_LIFT); a -= 1){    // Lift the grabbed object slightly (30 degrees)
                  shoulder.write(a);
                  delay(20);
                }
                
                whichBox = TOP;                                                  // set current position of the gripper to TOP
                delay(500);
                break;
                
    case BOTTOM:                                                                // Grap a box on the surface level
                for(int a = DEFAULT_SHOULDR; a <= SHOULDR_BTM_GRAB; a += 1){     // Rotate gripper and set it parallel to the ground    
                  shoulder.write(a);
                  delay(20);
                }
                
                delay(500);
                
                for(int a = DEFAULT_GRIP; a <= GRIP_GRAB ; a += 1){                // Fully close gripper to grab object
                  grip.write(a);
                 // delay(10);
                }
                
                delay(500);
                
                for(int a = SHOULDR_BTM_GRAB; a >= (SHOULDR_BTM_GRAB - SHOULDR_LIFT); a -= 1){    // Lift the grabbed object slightly (30 degrees) 
                  shoulder.write(a);
                  delay(20);
                }
                
                whichBox = BOTTOM;                                                              // set current position of the gripper to BOTTOM
                delay(500);
                break;
                
      default:
                //Serial.print("grabBox() method: parameter ");                                     // Print error if incorrect parameters are passed
                //Serial.println(box);
                //Serial.println("grabBox() method: should have parameters TOP or BOTTOM");
                break;
  }
  isOccupied=true;
}


void dropBox(int where){
  //check the current position of the grabbed box
  if(whichBox == TOP){      
    
    switch(where){                                                                                   // check where to place the current box (UP or DOWN)
      case UP:                                                                                     // Drop a box on TOP on TOP of another box
                 for(int a = (SHOULDR_TOP_GRAB - SHOULDR_LIFT); a <= SHOULDR_TOP_GRAB ; a += 1){     // Rotate gripper and set it at 125 degrees to drop the top box
                    shoulder.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = GRIP_GRAB; a >= DEFAULT_GRIP ; a -= 1){                               // Fully open gripper to drop object
                    grip.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = SHOULDR_TOP_GRAB; a >= DEFAULT_SHOULDR; a -= 1){                  // Send the shoulder back to its default position (perpendicular to the robot)
                    shoulder.write(a);
                    delay(20);
                  }
            
                  delay(500);
                  break;
                  
      case DOWN:                                                                                   // Drop a box that is on TOP on the GROUND
                 for(int a = (SHOULDR_TOP_GRAB - SHOULDR_LIFT); a <= SHOULDR_BTM_GRAB ; a += 1){     // Rotate gripper and set it at 160 degrees to drop the box on ground
                    shoulder.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = GRIP_GRAB; a >= DEFAULT_GRIP ; a -= 1){                               // Fully open gripper to drop object
                    grip.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = SHOULDR_BTM_GRAB; a >= DEFAULT_SHOULDR; a -= 1){                  // Send the shoulder back to its default position (perpendicular to the robot)
                    shoulder.write(a);
                    delay(20);
                  }
            
                  delay(500);
                  break;
                  
      default:
                //Serial.print("dropBox() method: parameter 'where': ");                         // Print error if incorrect parameters are passed
                //Serial.println(where);
                //Serial.println("dropBox() method: should be UP or DOWN");
                break;
          
    }

    whichBox = NONE;                                                                           // set current position of the gripper to NONE - DEFAULT (No PayLoad)
  
  }else if(whichBox == BOTTOM){
    
    switch(where){                                                                                   // check where to place the current box (UP or DOWN)
      case UP:                                                                                     // Drop a box in the groud level on TOP of another box
                 for(int a = (SHOULDR_BTM_GRAB - SHOULDR_LIFT); a >= (SHOULDR_TOP_GRAB - SHOULDR_LIFT) ; a -= 1){     // Rotate gripper and set it at 125 degrees to drop the top box
                    shoulder.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = GRIP_GRAB; a >= DEFAULT_GRIP ; a -= 1){                               // Fully open gripper to drop object
                    grip.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = SHOULDR_TOP_GRAB; a >= DEFAULT_SHOULDR; a -= 1){                  // Send the shoulder back to its default position (perpendicular to the robot)
                    shoulder.write(a);
                    delay(20);
                  }
            
                  delay(500);
                  break;
                  
      case DOWN:                                                                                   // Drop a box that is in the ground level on the groud
                 for(int a = (SHOULDR_BTM_GRAB - SHOULDR_LIFT); a <= SHOULDR_BTM_GRAB ; a += 1){     // Rotate gripper and set it at 160 degrees to drop the box on ground
                    shoulder.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = GRIP_GRAB; a >= DEFAULT_GRIP ; a -= 1){                               // Fully open gripper to drop object
                    grip.write(a);
                    delay(20);
                  }
                  
                  delay(500);
                  
                  for(int a = SHOULDR_BTM_GRAB; a >= DEFAULT_SHOULDR; a -= 1){                  // Send the shoulder back to its default position (perpendicular to the robot)
                    shoulder.write(a);
                    delay(20);
                  }
            
                  delay(500);
                  break;
                  
      default:
//                Serial.print("dropBox() method: parameter 'where': ");                          // Print error if incorrect parameters are passed
//                Serial.println(where);
//                Serial.println("dropBox() method: should be UP or DOWN");
                break;
    }
  
    whichBox = NONE;                                                                          // set current position of the gripper to NONE - DEFAULT (No PayLoad)
    
  } else {
//      Serial.print("dropBox() method: parameter 'whichBox': ");                                // Print error if incorrect parameters are passed
//      Serial.println(whichBox);
//      Serial.println("dropBox() method: should be TOP or BOTTOM");
  }
  isOccupied = false;
}



