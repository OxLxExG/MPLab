#pragma  once
	
//-eu=mV/10
typedef struct __attribute__((packed))
{
	//- name= мз
	uint8_t DAC1;
	//- name= бз
	uint8_t DAC2;
} eep_ggkp_dac_t;


// виртуальная структура EEPROM ROM RAM

typedef struct __attribute__((packed))
{
	//- from=32
	//- password=DAC
	//- name=Настройка_DAC
	eep_ggkp_dac_t dac;
} EepData_t;

//- adr = 4
//- SubAdr = 0x11
//- info = "__DATE__ __TIME__  Tiny1616.2 SubAdr RefLevel=2.5V DAC(1) =10mV"
//- chip = 7
//- serial = 1
//- name = GgkpTiny1616
//- export
typedef struct __attribute__((packed))
{
	// - EEP
	// - noname
	// EepData_t eep;
} AllDataStruct_t;


