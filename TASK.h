/**-----------------------------------------------------------------------------
 * 	Title:      Learning PARAMETERS & TASK definition							
 * 	Project:    Cognitive Robotics. Mechatronics Engineering 2014-2015 
    			University of Málaga  
 *	@author:    Angel Martinez-Tenor                                                     
 *  Language:   NXC
 *	@version	1.5 (April 16, 2015)  NON-FIXED POINT!
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
* TASK:     	Wander evading obstacles: Simple Wander
* STATES:		4: no contact, left contact, right contact, both contacts  
* ACTIONS:      4: Stop, Spin Left, Spin Right, Forward        
------------------------------------------------------------------------------*/


/* References ----------------------------------------------------------------*/
// #include "NXT_io.h"  // Already included in 'learning.nxc'


/* Constants definition ------------------------------------------------------*/

// FileSystem parameters
#define NAME            "Simple_1"   //  LIMIT: 15 CHARACTERS
// resulting filename will be  NAME+".log"
#define ENVIRONMENT     "square_70x100 (with obstacle 29x9)"

// Q-learning algorithm parameters
#define N_STATES        4            // from 1 to N_STATES
#define N_ACTIONS       4            // from 1 to N_ACTIONS

#define GAMMA            0.9  // NXT OPTIMAL
#define INITIAL_ALPHA   0.02  // NXT OPTIMAL

// Experiment parameters
#define STEP_TIME       250     // (ms)
#define N_STEPS         1000    // Exp_Time > STEP_TIME * N_STEPS
#define INITIAL_POLICY  1       // 1: stop

#define MOTOR_POWER            50   // motor power (from 0 to 100)  TUNE
#define    THRESHOLD_DEGREES   25   // used in reward function      TUNE
#define    THRESHOLD_DISTANCE  25   // used in state encoding       TUNE


/* Functions prototypes ------------------------------------------------------*/
/* Note: Older Bricx Command Center versions will throw error here.
 *      In that case, erasing this section will solve the problem.
 *      NBC 1.2.1.r4 compiler under Linux works OK in any case.
  */

byte selectActionStrategy(void);
void executeAction(byte a);
byte observeState(void);
float obtainReward(byte s, byte a, byte sp);


// -----------------------------------------------------------------------------
byte selectActionStrategy(void) {
    /* OUTPUT	0: Greedy (exploitation)
	 * 			1: Random 
	 *			2: Least Explored 
	 * Note:	Refer to selectAction(byte s, byte strategy) in 'Q_LEARNING.nxc'
	 *			or SARSA.nxc' for further details
	 */

    byte strategy;

    strategy = 0;

    if (Random(100) < 70)     // 70 % exploiting
        strategy = 0; //
    else if (Random(100) < 30)
        // Improvement to simple e_Greedy: enhances exploring actions possibly
        //  not too often visited. When exploring there is a 30% probability of
        //  selecting the least explored action for the current state.
        strategy = 2;
    else
        strategy = 1;

    return (strategy);
}


//------------------------------------------------------------------------------
void executeAction(byte a) {
    /* INPUT: 	Selected action 
	   Note: 	Do not forget check N_ACTIONS */

    byte power = MOTOR_POWER;

    switch (a) {
        case 1:
            executeNXT("stop");    // mandatory (pause, end ...)
            break;
        case 2:
            executeNXT("turnLeft", power);
            break;
        case 3:
            executeNXT("turnRight", power);
            break;
        case 4:
            executeNXT("forward", power);
            break;

            /*
            case 5:
                executeNXT("forwardLeft",power);
                break;
            case 6:
                executeNXT("forwardRight",power);
                break;
            case 7:
                executeNXT("backLeft",power);
                break;
            case 8:
                executeNXT("backRight",power);
                break;
            case 9:
                executeNXT("back",power);
                break;
            */
    }
}

//------------------------------------------------------------------------------
byte observeState(void) {
    /*	Returns the state of the robot by encoding the information measured from
     *  its sensors (ultraSonic, contacts ...). In case the number of states or 
     *  their definitions change, this function  must be updated
     *  OUTPUT:	Observed State (sp)
     */

    byte state;

    /* NOTE: Use 'getObservationUltrasonic()' (NXT_io.h) to get the ultrasonic
     *  		observation (cm) and 'THRESHOLD_DISTANCE' to encode the states
     */

    state = 1 + LEFT_BUMPER + 2 * RIGHT_BUMPER;
    /* States discretization:
	 *	s1: 	no contacts
	 *	s2: 	left contact
	 *	s3:		right_contact
	 *	s4:  	both contacts
     */
    //display(NumToStr(state),); Wait(500)
    return (state);
}


// -----------------------------------------------------------------------------  
float obtainReward(byte s, byte a, byte sp) {
    /*  Returns the reward obtained by
     *	INPUT:		Previous State (s), Action executed (a), State Reached (s')
     *  OUTPUT:		Reward
     */

    /* INPUT: s,a,s' not used directly here since we look at the motion of
     * wheels and sensors (that would be a and s'). Encoders and contact sensors
     * result in a better function  R(s,a,s')
     */

    float R;  // Reward
    long threshold_degrees;

    threshold_degrees = THRESHOLD_DEGREES;
    //  Ref: 1 second at POWER 100 gives above 700 degrees with full battery 

    R = 0;

    // Alternative: using MotorBlockTachoCount()
    if (MotorRotationCount(LEFT_WHEEL) > threshold_degrees &&
        MotorRotationCount(RIGHT_WHEEL) > threshold_degrees)
        R = 10;
    else if ((MotorRotationCount(RIGHT_WHEEL) > threshold_degrees) ||
             (MotorRotationCount(LEFT_WHEEL) > threshold_degrees))
        R = 0.5;

    if (LEFT_BUMPER || RIGHT_BUMPER) {
        if (LEFT_BUMPER && RIGHT_BUMPER)
            R = -50;
        else
            R = -5;
    }

    ResetRotationCount(LEFT_WHEEL);
    ResetRotationCount(RIGHT_WHEEL);
    return (R);
}
