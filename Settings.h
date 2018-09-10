
//uncomment for debug serial out
//#define DEBUG

const long HOUR_MILLIS = 3600000L; // 3600 sec in hour, 1000 msec in sec
const byte CHANNELS_NUM = 6;

const byte BLYNC_PIN = 2;
const int BLYNC_INTERVAL_2S = 2000;
const int BLYNC_INTERVAL_200MS = 200;

//pins for SL_BOARD_0.0.1
const byte OUT_PINS[CHANNELS_NUM] = {11, 10, 9, 6, 5, 3}; //aviable 3, 5, 6, 9, 10, 11 (any of PWM pins)
const byte IN_PINS[CHANNELS_NUM] = {A6, A7, A0, A1, A2, A3}; //aviable A0, A1, A2, A3, A6, A7 (A4, A5 reserved for I2C)


/*
*  Dayli cicle
* 
*               SUNRISE          DAYLIGHT         SUNSET
* POWER_ON -> |_________|_______________________|________| -> WATING_FOR_POWER_OFF
*
*/
#ifdef DEBUG
    // debugging values
    const long SUNRISE_DURATION  =  10000L;
    const long SUNSET_DURATION   = 180000L;
    const long DAYLIGHT_DURATION =  60000L; 
#else
    // production values. Make changes here!
    const long SUNRISE_DURATION  = 2L * HOUR_MILLIS; // 3h
    const long SUNSET_DURATION   = 2L * HOUR_MILLIS; // 3h
    const long DAYLIGHT_DURATION = 7L * HOUR_MILLIS; // 9h
#endif


/*
*   Triming potentiometr border values
*
*   0   DOWN_TRIM     WORK BAND      UP_TRIM   1023
*   |.......|___________________________|.......|
*   
*/
const int DOWN_TRIM_LEVEL = 32; // min 0
const int UP_TRIM_LEVEL = 981; // max 1023





// end
