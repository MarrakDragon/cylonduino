
int cockpitLightsPin = 0;     // PWM  (on/off) for cockpit lights. Need to find a purple LED and check the resistor
int enginePin = 1; //;1       // PWM to ramp up, then flicker
int spotLightsPin = 2;        // digital
int Unused = 3;               // Digital - Has a 1.5ohm pullup resistor, so not going to use it.
int gunLightsPin = 4; ;       // PWM for guns. Could use digital, but I'd like to ramp them up (very) briefly before firing
int headLightsPin = 5;        // Digital (on/off) for headlights

const int engineRampStartLevel = 0; // Dark
const int engineRampEndLevel = 255; // Fully Lit (20mA+, maybe 40. Make sure we have enough Amperage coming in.
int       engineRampLevel = engineRampStartLevel;
const int engineRampDelayMS = 20;     // Delay between ramp levels
long      engineRampLastMS = 0;  // Track the last time we did an engine ramp

long      engineFlickerDelayMS = 500; // ms
long      engineFlickerLastMS = 0;    // Track the last time we flickered the engine

const int laserRampStartLevel = 25; // Darkish
const int laserRampEndLevel = 200;  // 200 or less
const int laserRampIncrement = 25;  // How many to increase each 'frame'
int       laserRampLevel = laserRampStartLevel;
long      laserRampDelayMS = 500;   // Delay between ramp levels
long      laserRampLastMS = 0;      // Track the last time we did an engine ramp
long      laserFireDelayMS = random(1000) + 250;   // ms Next time the laser fires
long      laserFireLastMS = 0;      // ms Last time the laser fired
long      laserOnLengthMS = random(100) + 250;    // How long the laser stays on, in ms.
long      laserOnLastMS = 0;        // ms

// Gunstates
enum {gunOff = 2, gunRampUp, gunFired, gunRampDown };
int gunState;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the pins we care about as an output.

  pinMode(cockpitLightsPin, OUTPUT);
  pinMode(enginePin, OUTPUT); 
  pinMode(spotLightsPin, OUTPUT);
  pinMode(gunLightsPin, OUTPUT);
  pinMode(headLightsPin, OUTPUT);

  gunState = gunOff;
  laserFireDelayMS = random(1000) + 250;   // set the next laser fire time
  laserFireLastMS = 0; // ms
    
  // Always on
  digitalWrite( cockpitLightsPin, HIGH );
  digitalWrite( headLightsPin,    HIGH );
  digitalWrite( spotLightsPin,    HIGH );
}

void LightEngine()
{
  if (   (engineRampLevel < engineRampEndLevel ) ) 
  {
    if( (millis() - engineRampLastMS) > engineRampDelayMS )
    {
      // ramp engine
      analogWrite(enginePin, engineRampLevel++);
      engineRampLastMS = millis();
    }
  }
  else
  {
    if( (millis() - engineFlickerLastMS) > engineFlickerDelayMS )
    {
      // ramp engine
      analogWrite(enginePin, random(120)+135);
      engineFlickerDelayMS = random(500);     // Next flicker time
      engineFlickerLastMS = millis();         // Store the last flicker time
    }

  } 
}


//const int gofflight = 4;
//const int ggolight = 0;
//const int gramplight=5;
void LightGuns(  )
{
    switch ( gunState )
    {
      case gunOff:
//        analogWrite(gofflight, 200 );
//        analogWrite(ggolight, 0);
//        digitalWrite(gramplight, LOW);
        if( ( millis() - laserFireLastMS ) > laserFireDelayMS )
        {

          laserRampLastMS = millis();           // Reset last ramp
          laserRampDelayMS = random(30) + 20;    // Set ramp time
          laserRampLevel = laserRampStartLevel;  // Reset the ramp level
          gunState = gunRampUp;

        }
        break;
            
      case gunRampUp:
//        analogWrite(gofflight, 0 );
//        digitalWrite(gramplight, HIGH );
//        analogWrite(ggolight, 0 );
        if (   (laserRampLevel < laserRampEndLevel ) ) 
        {
          if( (millis() - laserRampLastMS) > laserRampDelayMS )
          {
            // ramp engine
            analogWrite(gunLightsPin, laserRampLevel%256);
            laserRampLevel += laserRampIncrement;
            laserRampLastMS = millis();
  
          }
        }
        else
        {
          // FIRE!
          analogWrite(gunLightsPin, (random(55)+laserRampEndLevel) % 256);
          laserOnLastMS = millis();            // Reset laserLastOn
          laserOnLengthMS = random(100) + 250;  // set long the laser is on
          gunState = gunFired;
        }
        break;
      
      case gunFired:
//        analogWrite(gofflight, 0 );
//        analogWrite(ggolight, 240 );
//        digitalWrite(gramplight, LOW );
        // Leave it on for a bit
        if( (millis() - laserOnLastMS) > laserOnLengthMS )
        {
          analogWrite(gunLightsPin, 0);            // Turn off the "laser"
          laserFireDelayMS = random(1000) + 250;   // set the next laser fire time
          laserFireLastMS = millis();             // Set last fire time
          gunState = gunOff;                       // Go back to gun off
        }

        break;
        
      default:
        break;
        
    };
}


// the loop routine runs over and over again forever:
void loop() 
{
  
  LightEngine();

  LightGuns();
 
}
