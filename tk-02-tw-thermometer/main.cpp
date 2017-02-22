#include "mbed.h"
#include "codecool/codecool_lcd.h"
#include "codecool/codecool_i2c.h"
#include "codecool/codecool_serial.h"
#include "codecool/codecool_joystick.h"
#include "codecool/codecool_pwm.h"
#include "codecool/codecool_shield_names.h"

#define LM75_ADDRESS 0x90
#define PRINT(temperature, state) { \
				LCD_CLS(); \
				LCD_LOCATE(0, 0); \
				LCD_PRINTF("Temperature: %0.1f %s.\n",temperature, state); \
} //defined print temperature

void sender(){
	uint8_t buffer[16];
    float temp;

    buffer[0] = 0x00;
    I2C_WRITE(LM75_ADDRESS, buffer, 1);

    memset(buffer, 0x00, sizeof (buffer)); //fill buffer with zeros to remove any unnecessary data
    I2C_READ(LM75_ADDRESS, buffer, 2); //send a request for the IC to read two bytes of data


    int8_t _int_part = (int8_t)buffer[0]; //create an 8-bit wide signed variable
    temp = _int_part + 0.5f * ((buffer[1]&0x80)>>7);

	SERIAL_SEND(buffer,16); // send the data
	PRINT(temp, "sent"); // display the value on the LCD display.
	wait(1);
	LCD_CLS();
	memset(buffer, 0x00, sizeof (buffer)); //fill buffer with zeros to remove any unnecessary data again
}

void receiver(){
	uint8_t buffertwo[16];
	float temp2;

    buffertwo[0] = 0x00;
    I2C_WRITE(LM75_ADDRESS, buffertwo, 1);


    memset(buffertwo, 0x00, sizeof (buffertwo));
    I2C_READ(LM75_ADDRESS, buffertwo, 2);

    SERIAL_RECV(buffertwo, 16); //receive the data
    int8_t _int_parttwo = (int8_t)buffertwo[0];
    temp2 = _int_parttwo + 0.5f * ((buffertwo[1]&0x80)>>7);


    if (temp2 >= 34.0) {							//red
    	PWM_SET_PULSE_WIDTH(LED_RED_SHIELD, 100);
    	PRINT(temp2, "received");
    }
    else if (temp2 >=26.0 && temp2 < 34.0) {		//orange
    	PWM_SET_PULSE_WIDTH(LED_RED_SHIELD, 100);
    	PWM_SET_PULSE_WIDTH(LED_GREEN_SHIELD, 100);
    	PRINT(temp2, "received");
    }
    else if (temp2 >= 18.0 && temp2 < 26.0) {		//green
    	PWM_SET_PULSE_WIDTH(LED_GREEN_SHIELD, 100);
    	PRINT(temp2, "received");
    }
    else if (temp2 >= 10.0 && temp2 < 18.0) {		//aqua blue
    	PWM_SET_PULSE_WIDTH(LED_BLUE_SHIELD, 100);
    	PWM_SET_PULSE_WIDTH(LED_GREEN_SHIELD, 100);
    	PRINT(temp2, "received");
    }
    else if (temp2 < 10.0) {						//dark blue
    	PWM_SET_PULSE_WIDTH(LED_BLUE_SHIELD, 100);
    	PRINT(temp2, "received");
    }

    wait(2);
    LCD_CLS();
    memset(buffertwo, 0x00, sizeof (buffertwo));

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

    PWM_INIT(LED_RED_SHIELD); //initialize the LEDs on the shield
    PWM_INIT(LED_GREEN_SHIELD);
    PWM_INIT(LED_BLUE_SHIELD);

	PWM_SET_PULSE_WIDTH(LED_RED_SHIELD, 0);
	PWM_SET_PULSE_WIDTH(LED_GREEN_SHIELD, 0);
	PWM_SET_PULSE_WIDTH(LED_BLUE_SHIELD, 0);

    while (true) {
    	if (JOYSTICK_PUSHED && SERIAL_AVAILABLE() > 0) {
    		receiver();	//if joystick is pushed and the number of bytes received is higher than 0, receive the data
    	}
    	else if (JOYSTICK_PUSHED) {
    		sender(); //if joystick is pushed, send the data
    	}
    	PWM_SET_PULSE_WIDTH(LED_RED_SHIELD, 0);  // set the LEDS to not glow
    	PWM_SET_PULSE_WIDTH(LED_GREEN_SHIELD, 0);
    	PWM_SET_PULSE_WIDTH(LED_BLUE_SHIELD, 0);

    LCD_LOCATE(0, 0);
    LCD_PRINTF("Push the button.");

    }
}
