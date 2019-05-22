//#include <avr/eeprom.h>
//#include <avr/io.h>
//EPPROM
	//unsigned char s;
	//unsigned char eeprom_var EEMEM; // определяем переменную в EEPROM
	//eeprom_write_byte(&eeprom_var, Sec); // записываем "Sec" в EEPROM
	//s = eeprom_read_byte(&eeprom_var); // считываем байт из EEPROM и помещаем его в "s"
//
// Чтение из EEPROM
static unsigned char eeprom_read(unsigned int address){
	while(EECR & (1 << EEWE)); // Ждем готовности
	EEAR = address; // Загружаем требуемый адрес в регистр EEAR
	EECR |= (1 << EERE); // Устанавливаем в «1» разряд EERE регистра EECR
	return EEDR; // Возвращаем данные из регистра данных
}
// Запись в EEPROM
static void eeprom_write(unsigned int address, unsigned char data){
	// Проверяем данные
	if (eeprom_read(address) != data){
	while(EECR & (1 << EEWE)); // Ждем готовности
	cli(); // Запрещаем прерывания
	EEAR = address; // Загружаем требуемый адрес в регистр EEAR
	EEDR = data; // Загружаем данные в регистр EEAR
	EECR |= (1 << EEMWE); // Устанавливаем в «1» разряд EEMWE регистра EECR
	EECR |= (1 << EEWE); // Устанавливаем в «1» разряд EEWE регистра EECR
	sei(); // Разрешаем прерывания
	}
}
//unsigned char read_eeprom_bits(unsigned int address, unsigned char bitnum){
//	unsigned char tmp,i;
//	tmp=0;for(i=0;i<bitnum;i++){if(eeprom_read(address+i)==0){tmp &= ~(1<<i);}else{tmp |= 1<<i;}}
//	return tmp;
//}