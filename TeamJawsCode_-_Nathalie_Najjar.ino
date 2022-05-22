//Team Jaws
/* Written by Nathalie Najjar
 * Date of Last edit: 03/13/2022
 * Purpose: 
 *   This code's purpose is for a rover to follow a line (through photoresistors), 
     detect a red cup (through pixy2 cam), go towards that cup and have the claw grab it (micro servo turning)
     After grabbing the cup the rover will continue line following
 */

#include <Pixy2.h>
#include <Servo.h>
Pixy2 pixy;
Servo Jaws;
Servo MiniJaws;
Servo Motor;

int PR_right = A1; //right photoresistor 
int PR_left = A2; //left photoresistor 
int largeservopin = 3; //Large servo pin: 3
int microservopin = 5; // micro servo pin : 5

int CupStatus = 1;

unsigned long time; 

void setup ()
{
  pinMode(PR_right, INPUT);
  pinMode(PR_left, INPUT);
  
  Serial.begin (115200);
  MiniJaws.attach (5); // micro servo pin
  MiniJaws.write (180); // micro servo initial position
  Jaws.attach (3); // large servo pin
  Jaws.write (90); // large servo initial position 
  Motor.attach (9); // DC motor servo pin 
  Motor.write (90); // initial speed = 0
  Serial.println ("Starting.../n");
  pixy.init ();
}


void loop ()
{

  //time = millis();
  
  int i; //grab blocks!
  float kp= 0.4; //  going to be used in calculations later (PID control)
  pixy.ccc.getBlocks();
  int width = pixy.ccc.blocks[i].m_width; // width of red cup on pixy mon 
  


  if (pixy.ccc.getBlocks () > 0 && pixy.ccc.blocks[1].m_signature == 1) //if cup detected
  {
      if (width > 170) // cup close to rover
      { 
          
          MiniJaws.write (90); //claw closes
          CupStatus = 2;  // Holding Cup       
          Motor.write (125); // goes forward 
          Serial.println ("grab");

      }
      else if (width < 170) // cup detected but far from rover
      {
          Motor.write (125); // rover moving
          MiniJaws.write (180);  // claw opens
          CupStatus = 1;  // Tracking Cup
          
      }
  }
  else
  {
    CupStatus = -1; // cup not detected  
  }

  if(CupStatus == 1)  // tracking Cup
  { 
    // kp control
    Motor.write (125); //moves 
    int error= 158 - pixy.ccc.blocks->m_x ;
    int angle= kp* error ;
    int value = angle +90 ;
    Jaws.write (value);
    Serial.println ("go to cup"); 
  }  
                 


   else  // not detected OR Cup Gripped 
  {
    int brightnessR = analogRead (PR_right);
    int brightnessL = analogRead (PR_left);
    Serial.println ("right brightness"); // right PR value below
    Serial.println (brightnessR);
    Serial.println ("left brightness"); //left PR value below
    Serial.println(brightnessL);


    Motor.write (125); // rover moves 
    
    if (brightnessR > 600 && brightnessL >400) // both photoresistors not on dark line 
    {
      Serial.println ("Moving straight");
      Jaws.write (90); //stays straight
    }
    
    else if (brightnessR <600 && brightnessL >400 ) //right PR on dark line
    {
      Serial.println ("Turn Right");
     
      Jaws.write (180); //turn right 
    }
    
    else if (brightnessR >600 && brightnessL <400 ) //left PR on dark line
    {
      Serial.println ("Turn Left");
      Jaws.write (0); // turn left 
    }

    else 
    {  
      Serial.println ("Moving straight");
      Jaws.write (90);
      }
  }


if (time >= 50000)
  {
    Motor.write (90); // rover stops
  }  
}// end of void loop 
