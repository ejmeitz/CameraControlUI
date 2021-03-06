#include <SPI.h>
#include <HighPowerStepperDriver.h>

const uint8_t CS_Pin = 4;
const uint8_t Limit_Switch_Pin = 0;
const uint8_t powerPin = 7; //just using to power breadboard rail

volatile int flag = 0;
float currentPos = 0;
float previousPos = 0;
float minPos = 5; //[mm]
float maxPos = 200; //[mm]
bool isCalibrating = false; //used to ignore hardward interrupt during calibration routine

const HPSDStepMode microStepSetting = HPSDStepMode::MicroStep2; //1:1 is fastest, but can't use stall detection
const int microStepFactor = 2; //just wahtever the above value is but thats an enum class so can't use it directly
const int stepsPerRevolution = 200; 

// This period is the length of the delay between steps, which controls the
// stepper motor's speed.  You can increase the delay to make the stepper motor
// go slower.  If you decrease the delay, the stepper motor will go faster, but
// there is a limit to how fast it can go before it starts missing steps.
const uint16_t StepPeriodUs = 2000;

HighPowerStepperDriver sd;

void setup() {
  Serial.begin(9600);
  //Serial.setTimeout(250);

  SPI.begin();
  sd.setChipSelectPin(CS_Pin);
  pinMode(Limit_Switch_Pin,INPUT_PULLUP);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  // Give the driver some time to power up.
  delay(1);

  // Reset the driver to its default settings and clear latched status
  // conditions.
  sd.resetSettings();
  sd.clearStatus();

  // Select auto mixed decay.  TI's DRV8711 documentation recommends this mode
  // for most applications, and we find that it usually works well.
  sd.setDecayMode(HPSDDecayMode::AutoMixed);

  // Set the current limit. You should change the number here to an appropriate
  // value for your particular system.
  sd.setCurrentMilliamps36v4(1650);

  // Set the number of microsteps that correspond to one full step.
  sd.setStepMode(microStepSetting);

  // Enable the motor outputs.
  sd.enableDriver();

}

String serialData = ""; 
void loop() {
  if (Serial.available() > 1) {    //length of values from Qt will always be >=3 bytes???? do i want to make this >2??
      serialData += Serial.readString();
      int result = parseAndMove(serialData);
      if (result == 0){
        serialData = "";
        previousPos = currentPos;
        Serial.write("Success,");
      }else if(result == -2){ //invalid position
        serialData = "";
        currentPos = previousPos;
        Serial.write("Invalid,");
      }else{
        serialData = "";
        Serial.write("Unknown,");
      }
    }
 }


int parseAndMove(String data){
  if (data == "+10"){
    if(checkValidPosition(10)){
       sd.setDirection(1);
      for(unsigned int x = 0; x < 250 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos += 10;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "-10"){
    if(checkValidPosition(-10)){
       sd.setDirection(0);
      for(unsigned int x = 0; x < 250 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos -= 10;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "+01"){
    if(checkValidPosition(1)){
      sd.setDirection(1);
      for(unsigned int x = 0; x < 25 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos += 1;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "-01"){
    if(checkValidPosition(-1)){
      sd.setDirection(0);
      for(unsigned int x = 0; x < 25 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos -= 1;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "+11"){
     if(checkValidPosition(0.12)){
      sd.setDirection(1);
      for(unsigned int x = 0; x < 3 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos += 0.12;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "-11"){
    if(checkValidPosition(-0.12)){
      sd.setDirection(0);
      for(unsigned int x = 0; x < 3 * microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos -= 0.12;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "+04"){
    if(checkValidPosition(0.04)){
      sd.setDirection(1);
      for(unsigned int x = 0; x < microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos += 0.04;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "-04"){
    if(checkValidPosition(-0.04)){
      sd.setDirection(0);
      for(unsigned int x = 0; x < microStepFactor; x++)
      {
        sd.step();
        delayMicroseconds(StepPeriodUs);
      }
      currentPos -= 0.04;
      return 0;
    }else{
      return -2;
    }
  }else if(data == "Cal"){
    isCalibrating = true;
    calibrate();
    isCalibrating = false;
    return 0;
  }else{ //assume its a custom movement
    char firstChar = data.charAt(0);
    if(firstChar == '+' && data.endsWith("+")){
      data.remove(0,1);
      data.remove(data.length() - 1,1);
      float movement = data.toFloat();
      int steps = int(movement/0.04); 
      if(checkValidPosition(movement)){
        sd.setDirection(1);
        for(unsigned int x = 0; x < steps * microStepFactor; x++)
        {
          sd.step();
          delayMicroseconds(StepPeriodUs); 
        }
        currentPos += (steps * 0.04);
        return 0;
      }else{
        return -2;
      }
      
    }else if(firstChar == '-' && data.endsWith("-")){
      data.remove(0,1);
      data.remove(data.length() - 1,1);
      float movement = data.toFloat();
      int steps = int(movement/0.04); 
      if(checkValidPosition(-1*movement)){
        sd.setDirection(0);
        for(unsigned int x = 0; x < steps * microStepFactor; x++)
        {
          sd.step();
          delayMicroseconds(StepPeriodUs);
        }
        currentPos -= (steps * 0.04);
        return 0;
      }else{
        return -2;
      }
      
    }else{ //if it isnt a custom movement return a new failure value
      return -3;
    }
  }
  
}


bool checkValidPosition(float movement){
  float nextPos = currentPos + movement;
  if(nextPos > maxPos || nextPos < minPos){
    return false;
  }
  return true;
}


void calibrate(){
  sd.setDirection(0); //set direction towards limit switch
  //hit limit switch in big steps
  while(digitalRead(Limit_Switch_Pin) == HIGH){
     for(unsigned int x = 0; x < 100 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
  }
  
  delay(50);
  
  //back off 10mm
  sd.setDirection(1);
    for(unsigned int x = 0; x < 250 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
  sd.setDirection(0);
  
  delay(50); 
  
  //once we know we're close approach slower
  while(digitalRead(Limit_Switch_Pin) == HIGH){
     for(unsigned int x = 0; x < 10 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
  }

  delay(50); 
  
  //back off 5mm so limit switch isnt triggered
  sd.setDirection(1);
    for(unsigned int x = 0; x < 125 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
  
  currentPos = minPos; //since we backed off 5 this is now the position
}
