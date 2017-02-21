#include "mbed.h"
#include "codecool/codecool_lcd.h"
#include "codecool/codecool_i2c.h"
#include "codecool/codecool_serial.h"
#include "codecool/codecool_joystick.h"
#include "codecool/codecool_pwm.h"
#include "codecool/codecool_shield_names.h"

#define LM75_ADDRESS 0x90

void sender(){
	uint8_t buffer[16];
    float temp;

    buffer[0] = 0x00;
    I2C_WRITE(LM75_ADDRESS, buffer, 1);

    memset(buffer, 0x00, sizeof (buffer)); //fill buffer with zeros to remove any unnecessary data
    I2C_READ(LM75_ADDRESS, buffer, 2); //send a request for the IC to read two bytes of data


    int8_t _int_part = (int8_t)buffer[0]; //create an 8-bit wide signed variable
    temp = _int_part + 0.5f * ((buffer[1]&0x80)>>7);

	SERIAL_SEND(buffer,16);
	LCD_CLS();
	LCD_LOCATE(0, 0);
	LCD_PRINTF("Temperature: %0.1f sent.\n",temp);
	wait(1);
	LCD_CLS();
}

void receiver(){
	uint8_t buffertwo[16];
	float temp2;

    buffertwo[0] = 0x00;
    I2C_WRITE(LM75_ADDRESS, buffertwo, 1);


    memset(buffertwo, 0x00, sizeof (buffertwo));
    I2C_READ(LM75_ADDRESS, buffertwo, 2);

    SERIAL_RECV(buffertwo, 16);
    int8_t _int_parttwo = (int8_t)buffertwo[0];
    temp2 = _int_parttwo + 0.5f * ((buffertwo[1]&0x80)>>7);
    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Temperature: %0.1f received.\n",temp2);
    wait(1);
    LCD_CLS();

}

int main() {
	I2C_FREQ(100000); //setup 100kHz for I2C communication

    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING(); //set baud rate of serial communication

    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Thermometer started"); //clear and start screen
    wait(2);
    LCD_CLS();

    while (true) {
    	if (JOYSTICK_PUSHED && SERIAL_AVAILABLE() > 0) {
    		receiver();
    		wait(1);
    	}
    	else if (JOYSTICK_PUSHED) {
    		sender();
    		wait(1);
    	}
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Push the button.");

    }

}

/*int main() {
	I2C_FREQ(100000); //setup 100kHz for I2C communication

    SERIAL_BAUD(9600);
    SERIAL_SET_NON_BLOCKING(); //set baud rate of serial communication

    LCD_CLS();
    LCD_LOCATE(0, 0);
    LCD_PRINTF("Thermometer started"); //clear and start screen
    wait(2);

    uint8_t buffer[16];
    uint8_t buffertwo[16];
    float temp;
    float temp2;



    while (true) {
        buffer[0] = 0x00;
        I2C_WRITE(LM75_ADDRESS, buffer, 1);

        memset(buffer, 0x00, sizeof (buffer)); //fill buffer with zeros to remove any unnecessary data
        I2C_READ(LM75_ADDRESS, buffer, 2); //send a request for the IC to read two bytes of data


        int8_t _int_part = (int8_t)buffer[0]; //create an 8-bit wide signed variable
        temp = _int_part + 0.5f * ((buffer[1]&0x80)>>7);

        LCD_CLS();
        LCD_LOCATE(0, 0);
        LCD_PRINTF("Temperature: %0.1f\n",
        		temp);

        if (JOYSTICK_PUSHED && SERIAL_AVAILABLE() == 0) {
        	SERIAL_SEND(buffer,16);
        	LCD_CLS();
        	LCD_LOCATE(0, 0);
        	LCD_PRINTF("Temperature: %0.1f sent.\n",temp);
        	wait(2);
        }
        else if (JOYSTICK_PUSHED && SERIAL_AVAILABLE() != 0) {
            buffertwo[0] = 0x00;
            I2C_WRITE(LM75_ADDRESS, buffertwo, 1);

            memset(buffertwo, 0x00, sizeof (buffertwo));
            I2C_READ(LM75_ADDRESS, buffertwo, 2);
            SERIAL_RECV(buffertwo, 16);
            int8_t _int_parttwo = (int8_t)buffertwo[0];
            temp2 = _int_parttwo + 0.5f * ((buffertwo[1]&0x80)>>7);
            LCD_CLS();
            LCD_LOCATE(0, 0);
            LCD_PRINTF("Temperature: %0.1f received.\n",temp2);
            wait(2);
        }



        wait(0.2);
    }
}*/
