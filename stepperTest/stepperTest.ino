#include <SPI.h>
#include <HighPowerStepperDriver.h>

const uint8_t CS_Pin = 4;
const uint8_t Limit_Switch_Pin = 0;
const uint8_t powerPin = 7; //just using to power breadboard rail

volatile int flag = 0;
float pos = 0;
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
  attachInterrupt(digitalPinToInterrupt(Limit_Switch_Pin), ISR_LimitSwitch, FALLING); 

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
  if (Serial.available() > 0) {    //length of values from Qt will always be 3 bytes???? do i want to make this >2??
      serialData += Serial.readString();   
      if (parseAndMove(serialData)== 0){
        serialData = "";
        Serial.write("Success,");
      }
    }
 }


int parseAndMove(String data){
  if (data == "+10"){
    sd.setDirection(1);
    for(unsigned int x = 0; x < 250 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos += 10;
    return 0;
  }else if(data == "-10"){
    sd.setDirection(0);
    for(unsigned int x = 0; x < 250 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos -= 10;
    return 0;
  }else if(data == "+01"){
    sd.setDirection(1);
    for(unsigned int x = 0; x < 25 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos += 1;
    return 0;
  }else if(data == "-01"){
    sd.setDirection(0);
    for(unsigned int x = 0; x < 25 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos -= 1;
    return 0;
  }else if(data == "+11"){
    sd.setDirection(1);
    for(unsigned int x = 0; x < 3 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos += 0.12;
    return 0;
  }else if(data == "-11"){
    sd.setDirection(0);
    for(unsigned int x = 0; x < 3 * microStepFactor; x++)
    {
      sd.step();
      delayMicroseconds(StepPeriodUs);
    }
    pos -= 0.12;
    return 0;
  }else if(data == "Cal"){
    Serial.println("Calibrating");
    isCalibrating = true;
    calibrate();
    isCalibrating = false;
    return 0;
  }
  else{
    return -1;
  }
  
}


void checkLimitSwitch(){
   if( (digitalRead(Limit_Switch_Pin) == LOW) && (flag == 0) ) 
  {
    Serial.println("door is closed"); 
    flag = 1; 
    
  }
  
    if( (digitalRead(Limit_Switch_Pin) == HIGH) && (flag == 1) ) 
  {
    Serial.println("door is opened"); 
    flag = 0;
 
  }
}

void ISR_LimitSwitch(){
  if(isCalibrating == false){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    // If interrupts come faster than 200ms, assume it's a bounce and ignore
    if (interrupt_time - last_interrupt_time > 200)
    {
      Serial.println("Triggered not during calibration");
    }
    last_interrupt_time = interrupt_time;
  }
}

void ISR_Fault(){
  //TO-DO 
  //Generic fault thrown by stepper driver
  //kill program
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
  
  pos = 5.0; //since we backed off 5 this is now the position
}
