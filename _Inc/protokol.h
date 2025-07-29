#pragma once


#ifdef BT2
	//Для совместимости с BT1
	#define ADDRESS 0
    #define CMD_POS 1
	#define HEADER_LEN 2
	#define DATA_POS 2
#else
	//адрес устройства в сети
	#define ADDRESS (ADDRESS_PROCESSOR << 4)
    #define CMD_POS 0
	#define HEADER_LEN 1
	#define DATA_POS 1
#endif // BT2
	#define CRC_LEN 2

//********* команды поддерживаемые устройством **************

// установка времени на поверхности если 0 - то выключение прибора 
  #define CMD_TIME_SYNC 0xF5

// синхронизация времени во время работы в скважине
	#define CMD_BEACON 0xFA

// режим повышеной скорости
	#define CMD_TURBO 0xFD

// чтение внешней памяти
#define CMD_ERAM 0x1 | ADDRESS
// чтение информации об устройстве (дескриптор устройства)
#define CMD_INFO 0x2 | ADDRESS

// обработка ошибок
#define CMD_ERR 0xE | ADDRESS // было 6 неверно конфликт с  CMD_WRITE_EE

// чтение EEPROM памяти
#define CMD_READ_EE 0x5 | ADDRESS
// запись EEPROM памяти
#define CMD_WRITE_EE 0x6 | ADDRESS

// текущие состоание и данные (Тестовый режим или выдача последниз сохраненных данных в рабочем режиме)
#define CMD_WORK 0x7 | ADDRESS
// bootloader
#define CMD_BOOT 0x8 | ADDRESS
// фрматирование NAND
#define CMD_ERAM_CLEAR 0x0A | ADDRESS
// bootloader commands
#define CMD_BOOT_TEST	0x8
#define CMD_BOOT_READ   0xD 
#define CMD_BOOT_EXIT	0xE 
#define CMD_BOOT_WRITE  0xF 

// запись внешней памяти ТОЛЬКО В ТЕСТОВЫХ ЦЕЛЯХ
#define CMD_ERAM_WRITE 0x9 | ADDRESS
// стирание внешней памяти ТОЛЬКО В ТЕСТОВЫХ ЦЕЛЯХ
//#define CMD_ERAM_CLEAR 0xA | ADDRESS
// установка страницы для записи
#define CMD_ERAM_SET_BASE 0xC | ADDRESS

// состояние прибора
enum
{
	APP_SET_TIME,
	APP_CLEAR_RAM,
	APP_DELAY,
	APP_WORK,
	APP_IDLE,
	APP_MAX
};


// стандартное начало пакета
typedef  struct  __attribute__((packed))
{
	uint8_t AppState; ///  автомат|AU
	int32_t time;    ///  время|WT
} WorkDataStd_t;

// CMD_ERAM чтение памяти прибора 
typedef struct  __attribute__((packed)) 
{
	uint32_t addr;
	uint32_t len;
} ram_read_t;

