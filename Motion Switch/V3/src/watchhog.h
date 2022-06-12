#define WDTO_15MS   WDTCR = (0 << WDP2) | (0 << WDP1) | (0 << WDP0)
#define WDTO_30MS   WDTCR = (1 << WDP0)
#define WDTO_60MS   WDTCR = (1 << WDP1)
#define WDTO_120MS  WDTCR = (1 << WDP1) | (1 << WDP0)
#define WDTO_250MS  WDTCR = (1 << WDP2)
#define WDTO_500MS  WDTCR = (1 << WDP2) | (1 << WDP0)
#define WDTO_1S     WDTCR = (1 << WDP2) | (1 << WDP1)
#define WDTO_2S     WDTCR = (1 << WDP2) | (1 << WDP1) | (1 << WDP0)

void wdt_init(void)
{
	asm("wdr");
	WDTO_2S;
	WDTCR |= (1 << WDE) | (1 << WDCE);
}
void wdt_off(void)
{
	WDTCR = 0x00;
}
