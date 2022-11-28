#include "WS3.h"

void WS3::held(uint32_t timeout)
{
    if( this->halt )
        this->halt(timeout);
    else
        delay(timeout);
}

void WS3::parsing(const char* plain, const char* key, int* value, size_t size_key)
{
    char tmp_key[5] = "";
    for(uint8_t i=0; i<size_key; i++)
    {
        sprintf(tmp_key, "%c", key[i]);
        char *pointer = strstr(plain, (const char*)tmp_key);
        if( pointer != NULL )
        {
            char *str_p = strtok(pointer, (const char*)tmp_key);
            if( str_p != NULL)
            {
                value[i] = atoi(str_p);
                pointer = strtok(NULL, (const char *)tmp_key);
            }    
        }
    }
}

void WS3::begin(uint32_t timeout_)
{
    this->bufSize = WS3_BUFFER_SERIAL_SIZE;
    memset(this->bufferValue, 0, sizeof(int)*14);
    memset(this->bufTxt, '\0', this->bufSize);
    this->line->clearWriteError();
    uint32_t tiMillis=millis();
    while( this->line->available() && (millis() - tiMillis <= timeout_))
    {
        if( (char)this->line->read() == '\n' ); // clear the buffer and looking for the line-feed ( 0x0A )
            this->timeMillis = millis();
    }
}

void WS3::attachHeld(void (*v)(uint32_t time_)){
    this->halt = v;
}

float WS3::getSensorValue(uint8_t idx)
{
    if( idx >= 14 )
        return 0;
    float temp =  (float)this->bufferValue[idx];
    switch((SENS)idx){
        case SENS::WIND_VEL_M_S:
        case SENS::WIND_VEL_AVG:
        case SENS::WIND_VEL_MAX:
        case SENS::RAIN_MINUTE:
        case SENS::RAIN_HOUR:
        case SENS::RAIN_DAY:
        case SENS::TEMP:
        case SENS::HUMIDITY:
        case SENS::ATM_PRESSURE:
            temp /=10.f;
            break;
        case SENS::WIND_DIR_VAL:
        case SENS::WIND_DIR_ANGLE:
        case SENS::WIND_VEL_HZ:
        case SENS::RAIN_VAL:
        case SENS::RAIN_AVG:
        default :
                break;
    };
    return temp;
}

int WS3::getWindCompassIndex(void)
{
    float angle = -22.5, lastAngle = 0;
    for(uint8_t i = 0; i<16; i++)
    {
        // assume sensor has 16 point of compass = 360 / 16 = 22.5degree each step
        angle +=22.5;
        if( this->bufferValue[(uint8_t)SENS::WIND_DIR_ANGLE] >= (int)lastAngle && this->bufferValue[(uint8_t)SENS::WIND_DIR_ANGLE] <= (int)angle )
            return i;
        lastAngle = angle;
    }
    return 0;
}
const char * WS3::getWindCompass(uint8_t lang)
{
    int idx = this->getWindCompassIndex();
    if( lang == (uint8_t)LANG::EN )
        return wind_dir_en[idx];
    else
        return wind_dir_id[idx];
}

void WS3::loadBuffer(uint32_t timeout)
{
    uint32_t curMillis = millis();
    sprintf(this->bufTxt,(const char*)"");
    this->bufSize = 0;
    if( this->line )
    {
        while(millis() - curMillis < timeout)
        {
            uint32_t rollOver = millis();
            if(rollOver < curMillis)
                curMillis = rollOver;
            char c = (char) this->line->read();
            if( c == '\n' || c =='\r' || c == '*' )
            {
                this->bufTxt[this->bufSize] = '\0';
                this->timeMillis = millis();
                break;
            }
            else if( isAlphaNumeric(c) )
            {
                this->bufTxt[this->bufSize] = c;
                this->bufSize++;
                if( this->bufSize >= WS3_BUFFER_SERIAL_SIZE )
                    break;
            }
        }
    }
    Serial.println(this->bufTxt);
}

void WS3::loop(void)
{
    if( millis() - this->timeMillis >= 500UL)
        this->loadBuffer(1000UL);
    if( this->bufSize > 4 ) {
        parsing((const char*)this->bufTxt, (const char*)WS3_key, this->bufferValue, 14);
        sprintf(this->bufTxt,(const char*)"");
        this->bufSize = 0;
    }else
        this->held(50);
}