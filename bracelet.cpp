#include <iostream>
#include "oled/Edison_OLED.h"
#include "gpio/gpio.h"
#include "math.h"
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>

using namespace std;


void initOLED();
void startScreen();
void cleanUp();

int main(int argc, char * argv[])
{
	initOLED();
	startScreen();
}

// Initialize OLED
void initOLED() {
	oled.begin();
	oled.clear(PAGE);
	oled.display();
	oled.setFontType(0);
}

void startScreen()
{
	
	// Draw the splash screen:
	oled.clear(PAGE);
	oled.setCursor(0, 0);
	oled.print("SEWAR DAL");
	oled.print("Press A");
	oled.setCursor(0, 8);
	oled.print("# ");
	oled.setCursor(11,8);
	oled.print("0");
	oled.display();
	oled.setCursor(11,8);
	oled.print("1");
	// Call display to actually draw it on the OLED:
	oled.display();

	
}

void cleanUp()
{
	oled.clear(PAGE);
	oled.display();
}