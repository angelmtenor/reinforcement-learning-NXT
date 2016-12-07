/**-----------------------------------------------------------------------------
 * 	Title:      NXT_io.h: NXT in/out library				
 *   Project:   Cognitive Robotics. Mechatronics Engineering 2014-2015  
 *     			University of Málaga  
 *	@author:    Angel Martinez-Tenor                                                     
 *  Language:   NXC
 *	@version	1.5 (April 16, 2015)  
------------------------------------------------------------------------------*/


/* Brick buttons --------------------------------------------------------------- 
 *	LEFT ARROW:		Debug (learning step by learning step) 
 *	ORANGE SQUARE: 	Exploit Policy
 *	RIGHT ARROW:	Save Data & Stop Program  
 ---------------------------------------------------------------------------- */


/* References ----------------------------------------------------------------*/
#include "NXCDefs.h"
#include "NBCCommon.h"


/* Robot Setup----------------------------------------------------------------*/

/* Standard differential drive mobile setup with ultrasonic sensor as
 *  detailed in Lego 9797 kit manual. 2 contact-sensors with 2 bumpers must
 *  be added at the front of the robot as shown in 'NXT_learning_setup.png'
 */

// Actuators
#define LEFT_WHEEL        OUT_C
#define RIGHT_WHEEL        OUT_B

// Sensors ( check also NXT_mapSensors() )
#define ULTRASONIC        S4
#define LEFT_BUMPER     SENSOR_3
#define RIGHT_BUMPER    SENSOR_2

#define DEFAULT_MOTOR_POWER    80 // Pointless when MOTOR_POWER(TASK.h) is used


/* Global variables ----------------------------------------------------------*/
mutex msgmutex;        // Mutex needed for NXT display


/* Functions prototypes-------------------------------------------------------*/
/* Note: Older Bricx Command Center versions will throw error here.
 *      In that case, erasing this section will solve the problem.
 *      NBC 1.2.1.r4 compiler under Linux works OK in any case.
  */
void NXT_mapSensors(void);

byte getObservationUltrasonic(void);

void executeNXT(string command, byte power = DEFAULT_MOTOR_POWER);

void display(const string &m);

void displayForceRow(const string &m, byte row);

bool NXTcheckMemory(long memoryNeeded);

void pauseNXT(void);

bool debugButton(void);

bool exploitPolicyButton(void);

bool saveAndExitButton(void);

byte getButtonPressed(void);

void playStartSound(void);

void playEndingSound(void);

void playErrorSound(void);

void playExploitationSound(void);

void playStepSound(void);

void playStepOptimalSound(void);

void playPauseSound(void);

void playDebugSound(void);


/* Functions definitions------------------------------------------------------*/


void NXT_mapSensors() {
/* 	Assigns NXT sensors	*/
    SetSensorUltrasonic(ULTRASONIC);
    SetSensorTouch(IN_2);
    SetSensorTouch(IN_3);
}


//------------------------------------------------------------------------------
byte getObservationUltrasonic(void) {
/* 	Returns NXT UltraSonic observation (cm)	*/
    return SensorUS(ULTRASONIC);
}


//------------------------------------------------------------------------------
void executeNXT(string command, byte power = DEFAULT_MOTOR_POWER) {
/*	Maps motion commands into NXT motors.
 *		command: "stop", "turnLeft", "turnRight", "forward", "back" ...
 *		power:	 Power applied to the motors [0-100] 						
 */

    if (power < 0 || power > 100)
        return;

    switch (command) {
        case "stop": {        // Mandatory. Used in pauses, ending ...
            Off(LEFT_WHEEL);
            Off(RIGHT_WHEEL);
            break;
        }
        case "turnLeft": {
            OnRev(LEFT_WHEEL, power);
            OnFwd(RIGHT_WHEEL, power);
            break;
        }
        case "turnRight": {
            OnFwd(LEFT_WHEEL, power);
            OnRev(RIGHT_WHEEL, power);
            break;
        }
        case "forward": {
            OnFwd(LEFT_WHEEL, power);
            OnFwd(RIGHT_WHEEL, power);
            break;
        }
        case "forwardLeft": {
            OnFwd(LEFT_WHEEL, power);
            Off(RIGHT_WHEEL);
            break;
        }
        case "forwardRight": {
            Off(LEFT_WHEEL);
            OnFwd(RIGHT_WHEEL, power);
            break;
        }
        case "backLeft": {
            OnRev(LEFT_WHEEL, power);
            Off(RIGHT_WHEEL);
            break;
        }
        case "backRight": {
            Off(LEFT_WHEEL);
            OnRev(RIGHT_WHEEL, power);
            break;
        }
        case "back": {
            OnRev(LEFT_WHEEL, power);
            OnRev(RIGHT_WHEEL, power);
            break;
        }
    }
}


//------------------------------------------------------------------------------
void display(const string &m) {
/*	Shows a message in the NXT Display
 *	INPUT	m:		Text to show 	
 */
    byte aux, i;
    string line;

    ClearScreen();

    Acquire(msgmutex);
    aux = StrLen(m) / 16;  // NXT LCD screen is 16 characters wide

    for (i = 0; i <= aux; i++) {
        line = SubStr(m, 16 * i, 16);
        TextOut(0, 58 - 7 * i, line);
    }
    Release(msgmutex);
}

//------------------------------------------------------------------------------
void displayForceRow(const string &m, byte row) {
/*	Shows a message in the NXT Display
 *	INPUT	m:		Text to show 
 *  		row:    Desired display's row to show the text
 */

    Acquire(msgmutex);
    TextOut(0, 58 - 7 * row, m);

    Release(msgmutex);
}


//------------------------------------------------------------------------------
bool NXTcheckMemory(long memoryNeeded) {
/*	INPUT   NXT required memory 
 * 	OUTPUT  True =  NXT required memory available 	
 */
    unsigned long aux;
    string lin;

    aux = FreeMemory();

    lin = StrCat(NumToStr(memoryNeeded), " of ", NumToStr(aux));
    display(lin);
    if (aux <= memoryNeeded) {
        display("Not enough Memory. Delete unnecessary NXT files");
        Wait(1000);
        return (false);
    }
    return (true);
}


//------------------------------------------------------------------------------
void pauseNXT(void) {
/*	Pauses NXT until any button is pressed */
    displayForceRow("-- PAUSED --", 0);
    while (getButtonPressed() != 0) {
        Wait(50);
    }
}


//------------------------------------------------------------------------------
bool debugButton(void) {
    if (ButtonPressed(BTNLEFT, true) == 0)
        return false;
    else return true;
}

//------------------------------------------------------------------------------
bool exploitPolicyButton(void) {
    if (ButtonPressed(BTNCENTER, true) == 0)
        return false;
    else return true;
}

//------------------------------------------------------------------------------
bool saveAndExitButton(void) {
    if (ButtonPressed(BTNRIGHT, true) == 0)
        return false;
    else return true;
}

//------------------------------------------------------------------------------
byte getButtonPressed(void) {
    /* Return  	0: 	No button pressed
     *		   	1:	Left button pressed
     *			2:	Enter button pressed
     *			3:	Right button pressed
     */

    byte button = 0;
    if (debugButton())
        button = 1;
    else if (exploitPolicyButton())
        button = 2;
    else if (saveAndExitButton())
        button = 3;
    return button;
}



/* NXT Loudspeaker parameters ------------------------------------------------*/

// volume
#define VOL 1

// Musical Notes
#define C5  523
#define C#5 554
#define D5  587
#define D#5 622
#define E5  659
#define F5  698
#define F#5 740
#define G5    784
#define G#5 831
#define A5    880
#define A#5 932
#define B5    988
#define C6    1047
#define C#6 1109
#define D6    1175
#define D#6 1245
#define E6    1319
#define F6    1397
#define F#6 1480
#define G6    1568
#define G#6 1661
#define A6    1760
#define A#6 1865
#define B6    1976
#define C7    2093
#define C#7 2217
#define D7  2349
#define E7    2489

// Musical Note lengths
#define HALF        250
#define QUARTER     125
#define EIGHTH      63
#define SIXTEENTH   32

void playStartSound(void) {
/*	Play Start Sound */
    PlayToneEx(C5, QUARTER, VOL, FALSE);
    Wait(EIGHTH);
    PlayToneEx(E5, QUARTER, VOL, FALSE);
    Wait(EIGHTH);
    PlayToneEx(G5, QUARTER, VOL, FALSE);
    Wait(EIGHTH);
}

void playEndingSound(void) {
/*	Play Ending Sound */
    PlayToneEx(E5, HALF, VOL, FALSE); // Ending notification
    Wait(HALF);
    PlayToneEx(G5, HALF, VOL, FALSE);
    Wait(HALF);
    PlayToneEx(B5, HALF, VOL, FALSE);
    Wait(HALF);
}

void playErrorSound(void) {
/*	Play Error Sound */
    PlayToneEx(E5, QUARTER, VOL, FALSE);
    Wait(HALF);
    PlayToneEx(E5, QUARTER, VOL, FALSE);
    Wait(HALF);
}

void playExploitationSound(void) {
/*	Play Exploitation Sound */
    PlayToneEx(B6, 10, VOL, FALSE);
}

void playStepSound(void) {
/*	Play normal step sound */
    PlayToneEx(E5, 10, VOL, FALSE);
}

void playStepOptimalSound(void) {
/*	Play step sound when the optimal policy has been achieved */
    PlayToneEx(B5, EIGHTH, VOL, FALSE);
}

void playPauseSound(void) {
/*	PLay pause state sound */
    PlayToneEx(B5, QUARTER, VOL, FALSE);
    Wait(HALF);
    PlayToneEx(B5, QUARTER, VOL, FALSE);
    Wait(HALF);
}

void playDebugSound(void) {
/*	Play debug sound  */
    PlayToneEx(C5, QUARTER, VOL, FALSE);
    Wait(HALF);
    PlayToneEx(D5, QUARTER, VOL, FALSE);
    Wait(HALF);
}
