#ifndef _ws3_h_
#define _ws3_h_

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "Stream.h"
#include "Arduino.h"


#define WS3_SAMPLING_TIME           500UL       // 0.5s
#ifndef WS3_BUFFER_SERIAL_SIZE
    #define WS3_BUFFER_SERIAL_SIZE 148          // 2 * length of plain text 
#endif
enum class LANG{
    EN = 0,
    ID = 1,
};

static const char WS3_key[]="ABCDEFGHIJKLMN*";
enum class SENS{
            WIND_DIR_VAL 	= 0,	// A
            WIND_DIR_ANGLE	= 1,	// B
            WIND_VEL_HZ	    = 2,	// C
            WIND_VEL_M_S	= 3,	// D
            WIND_VEL_AVG	= 4,	// E
            WIND_VEL_MAX	= 5,	// F
            RAIN_VAL		= 6,	// G
            RAIN_AVG	    = 7,	// H
            RAIN_MINUTE	    = 8,	// I
            RAIN_HOUR	    = 9,	// J
            RAIN_DAY		= 10,	// K
            TEMP		    = 11,	// L
            HUMIDITY	    = 12,	//M
            ATM_PRESSURE	= 13,	//N
};

static const char wind_dir_en[][20] =  {
                                        "North",
                                        "North-northeast",
                                        "Northeast",
                                        "East-northeast",
                                        "East",
                                        "East-southeast",
                                        "Southeast",
                                        "South-southeast",
                                        "South",
                                        "South-southwest",
                                        "Southwest",
                                        "West-Southwest",
                                        "West",
                                        "West-northwest",
                                        "Northwest",
                                        "North-northwest",
                                    };
static const char wind_dir_id[][20] =  {
                                        "Utara",
                                        "Utara timur laut",
                                        "Timur laut",
                                        "Timur timur laut",
                                        "Timur",
                                        "Timur tenggara",
                                        "Tenggara",
                                        "Selatan tenggara",
                                        "Selatan",
                                        "Selatan Barat daya",
                                        "Barat daya",
                                        "Barat barat daya",
                                        "Barat",
                                        "Barat barat laut",
                                        "Barat laut",
                                        "Utara barat laut",
                                    };
class WS3{
    private :
        Stream *line;
        uint16_t baudrate;
        int bufferValue[14];
        char bufTxt[WS3_BUFFER_SERIAL_SIZE];
        uint16_t bufSize = WS3_BUFFER_SERIAL_SIZE;
        uint32_t timeMillis =   0UL;
        void (*halt)(uint32_t timeout);
        /**
         * @brief 
         *          delay function to held the system to do nothing. 
         *          if the pointer-function *halt is NULL, it will run the Arduino delay by default
         *          Proposed to :   - run your delay function if any
         *                          - run your task-delay if RTOS was used to giving other task chance to be executed
         * @param timeout : amount of time to held the system to do nothing
         */
        void held(uint32_t timeout);

        /**
         * @brief 
         *          key-parser, will extracting value from a plain-text to each key as integers
         *          e.g plain from the sensor : "A4095B000C0000D0000E0000F0000G0000H0000I0000J0000K0000L0281M576N10024 * 27"
         * @param plain     : text source
         * @param key       : delimiter as a key to parsing 
         * @param value     : series - of integer variable, would be use as storage for keep the value - parsed
         * @param size_key  : size of integer-series and key variables.
         */
        void parsing(const char* plain, const char* key, int* value, size_t size_key);

        void loadBuffer(uint32_t timeout = 1000);
        int getWindCompassIndex(void);
    public:
        WS3(Stream &line = Serial, uint16_t baudrate = 9600):line(&line),baudrate(baudrate){};
        ~WS3() {free(line);};

        void begin(uint32_t timeout_ = 1000UL );
        void attachHeld(void (*v)(uint32_t time_));

        void loop(void);
        
        // get data raw of sensor value based on index data
        int getRawValue(uint8_t idx = (uint8_t)SENS::WIND_DIR_VAL )  { return this->bufferValue[idx]; }

        // get value of each sensor
        float getSensorValue(uint8_t idx);
        float getSensorValue(SENS idx)                               { return this->getSensorValue((uint8_t)idx); };

        // get the wind direction, 16 directions
        const char * getWindCompass(uint8_t lang = 0 );
        const char * getWindCompass(LANG lang )                     { return this->getWindCompass((uint8_t)lang);} 
};

#endif