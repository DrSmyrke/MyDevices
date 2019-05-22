#include <avr/eeprom.h>
#include <avr/io.h>
// Чтение из EEPROM
uint8_t eeprom_read(uint32_t address){
	while(EECR & (1 << EEWE)); // Ждем готовности
	EEAR = address; // Загружаем требуемый адрес в регистр EEAR
	EECR |= (1 << EERE); // Устанавливаем в «1» разряд EERE регистра EECR
	return EEDR; // Возвращаем данные из регистра данных
}
// Запись в EEPROM
void eeprom_write(uint32_t address, uint8_t data){
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
uint8_t read_eeprom_bits(uint32_t address, uint8_t bitnum){
	uint8_t tmp,i;
	tmp=0;for(i=0;i<bitnum;i++){if(eeprom_read(address+i)==0){tmp &= ~(1<<i);}else{tmp |= 1<<i;}}
	return tmp;
}
