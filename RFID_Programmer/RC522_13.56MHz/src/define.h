#define ORANGE_LED 5
#define GREEN_LED 6
#define RED_LED 4

#define ORANGE_LED_ON digitalWrite(ORANGE_LED,0)
#define ORANGE_LED_OFF digitalWrite(ORANGE_LED,1)
#define GREEN_LED_ON digitalWrite(GREEN_LED,0)
#define GREEN_LED_OFF digitalWrite(GREEN_LED,1)
#define RED_LED_ON digitalWrite(RED_LED,0)
#define RED_LED_OFF digitalWrite(RED_LED,1)

void myInit()
{
	pinMode(ORANGE_LED,OUTPUT);
	pinMode(GREEN_LED,OUTPUT);
	pinMode(RED_LED,OUTPUT);

	GREEN_LED_ON;
	ORANGE_LED_ON;
	RED_LED_ON;
	delay(1000);

	RED_LED_OFF;
	ORANGE_LED_OFF;
	GREEN_LED_OFF;
}
