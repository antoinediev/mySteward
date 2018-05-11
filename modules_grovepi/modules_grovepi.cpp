// GrovePi Example for using the Grove Rotary Angle Sensor (Potentiometer) and the Grove LED to create LED sweep
//
// Modules:
//	 http://www.seeedstudio.com/wiki/Grove_-_Rotary_Angle_Sensor
//	 http://www.seeedstudio.com/wiki/Grove_-_LED_Socket_Kit
// The GrovePi connects the Raspberry Pi and Grove sensors.  You can learn more about GrovePi here:  http://www.dexterindustries.com/GrovePi
//
// Have a question about this example?  Ask on the forums here:  http://forum.dexterindustries.com/c/grovepi
//
/*
## License

   The MIT License (MIT)

   GrovePi for the Raspberry Pi: an open source platform for connecting Grove Sensors to the Raspberry Pi.
   Copyright (C) 2017  Dexter Industries

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/

#include "grovepi.h"
#include "grovepi.h"
#include "grove_rgb_lcd.h"
#include <stdlib.h>
#include <stdio.h>

int read_potentiometre();
int buzz();
int print_lcd(char * msg, char* color);

using namespace GrovePi;

// sudo g++ -Wall grovepi.cpp grove_rotary_angle_sensor.cpp -o grove_rotary_angle_sensor.out -> without grovepicpp package installed
// sudo g++ -Wall -lgrovepicpp grove_rotary_angle_sensor.cpp -o grove_rotary_angle_sensor.out -> with grovepicpp package installed

int main(int argc, char ** argv){
	try
	{
		initGrovePi();
	}
	catch(I2CError &error)
	{
		printf(error.detail());

		return -1;
	}
if(argc < 1) return -1;
	
if(strcmp(argv[1], "print_lcd")==0 ){
	print_lcd( argv[2],argv[3] );
}
else if(strcmp(argv[1], "buzz")==0 ){
	buzz();
}
else if(strcmp(argv[1], "getPot")==0 ){
	return read_potentiometre();
}
	/*
	printf("[degrees = %d\n", read_potentiometre() );
	//buzz();
	char msg[300]="UTILISEZ la poubelle machin bidule", color[10]="green", colorr[10]="red", colorrr[10]="BLUE";
	print_lcd(msg, color); delay(1000);
	print_lcd(msg, colorr); delay(1000);
	print_lcd(msg, colorrr); delay(1000);
	*/
}

int print_lcd(char * msg, char* color)
{
	LCD lcd; // initialize new Grove LCD RGB device
	
	try
	{
		// connect to the i2c-line
		lcd.connect();

		// set text and RGB color on the LCD
		lcd.setText(msg );
		if( strcmp(color, "red")==0 || strcmp(color, "RED")==0)
			lcd.setRGB(255, 0, 0);
		else if( strcmp(color, "blue")==0 || strcmp(color, "BLUE")==0)
			lcd.setRGB(0, 0, 255);
		else if( strcmp(color, "green")==0 || strcmp(color, "GREEN")==0)
			lcd.setRGB(0, 255, 0);
		else if( strcmp(color, "black")==0 || strcmp(color, "BLACK")==0)
			lcd.setRGB(0, 0, 0);
		
	}
	catch(I2CError &error)
	{
		printf(error.detail());

		return -1;
	}

	return 1;
}

int read_potentiometre()
{
	int potentiometer_pin = 1; // potentiometer is connected to A1 port
	int adc_ref = 5; // reference voltage of ADC is 5V
	int grove_ref_vcc = 5; // Grove's reference voltage is 5V, regularly
	int full_angle = 100; // max turning angle for the potentiomater (almost a complete turn)

	try
	{
		pinMode(potentiometer_pin, INPUT);

			// start reading potentiometer's values
			int sensor_value = analogRead(potentiometer_pin);
			// calculate voltage
			float voltage = (float)(sensor_value) * adc_ref / 1023;

			// calculate rotation in degrees (0 to 300)
			float degrees = voltage * full_angle / grove_ref_vcc;
			return (int)degrees;
	}
	catch
	(I2CError &error)
	{
		printf(error.detail());

		return -1;
	}

	return -1; //should not happen
}

int buzz()
{
	int buzzer_pin = 8; // Grove Buzzer is connected to digital port D8 on the GrovePi

	try
	{
		pinMode(buzzer_pin, OUTPUT); // set the buzzer_pin as OUTPUT (we have a buzzer)
		
			// turn ON the buzzer for 1000 ms (1 sec)
			// and put the state on the screen
			digitalWrite(buzzer_pin, HIGH);
			printf("[pin %d][buzzer ON]\n", buzzer_pin);
			delay(1000);

			// and then OFF for another 1000 ms (1 sec)
			// and put the state on the screen
			digitalWrite(buzzer_pin, LOW);
			printf("[pin %d][buzzer OFF]\n", buzzer_pin);
			delay(1000);
			
				// turn ON the buzzer for 1000 ms (1 sec)
			// and put the state on the screen
			digitalWrite(buzzer_pin, HIGH);
			printf("[pin %d][buzzer ON]\n", buzzer_pin);
			delay(1000);

			// and then OFF for another 1000 ms (1 sec)
			// and put the state on the screen
			digitalWrite(buzzer_pin, LOW);
			printf("[pin %d][buzzer OFF]\n", buzzer_pin);
			
	}
	catch(I2CError &error)
	{
		printf(error.detail());

		return -1;
	}

	return 0;
}