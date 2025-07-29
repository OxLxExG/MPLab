#pragma  once
#include <avr/io.h>

namespace induc
{
	
// flash	
typedef struct __attribute__((packed))
{
	//- ShowHex
	uint16_t bad_blocks[40];
} flash_bad_bloks_t;


// flash	
typedef struct __attribute__((packed))
{
	//-color=0x000000FF
	char manufacturer[13];
	/** Device part number */
	//-color=0x0000FF00
	char model[21];
	/** Number of data bytes per page */
	//-ShowHex
	uint16_t data_bytes_per_page;
	/** Number of spare bytes per page */
	//-ShowHex
	uint16_t spare_bytes_per_page;
	/** Number of pages per block */
	//-ShowHex
	uint8_t pages_per_block;
	/** Number of blocks per unit */
	//-ShowHex
	uint16_t blocks_per_lun;
	/** Bad blocks maximum per unit */
	//-ShowHex
	uint8_t max_bad_blocks_per_lun;
	/** Guaranteed valid blocks at beginning of target */
	//-ShowHex
	uint8_t guarenteed_valid_blocks;
} flash_nand_t;

typedef struct __attribute__((packed))
{
	//-readonly
	uint16_t ResetCount;	
	//-readonly
	uint8_t ResetFunction;	
	//-readonly
	uint8_t ResetRegister;
	//- name=восстановленый_кадр
	//- metr=WT
	//-readonly
	int32_t kadr_Reset;
} reset_t;

typedef struct __attribute__((packed))
{
	//- name=ResetErr
	reset_t reset; //8
	//- name=время_qzErr
	//- metr=WT
	//-readonly
	int32_t kadr_qzErr;	//4
} eep_errors_t; //12 

 //- name=текущий_рейс
 typedef struct __attribute__((packed))
 {
	 //- name=рейс
	 uint16_t reis;
	 //- name=блок_памяти
	 uint16_t startBlock;
 } eep_current_reis_t;

typedef struct __attribute__((packed))
{
	//- name=рейс
	 	uint16_t reis;
	//- name=число_блоков
	 	uint16_t blocks;
	//- name=блок_памяти_начало
	 	uint16_t startBlock;
	//- name=блок_памяти_конец
	 	uint16_t endBlock;
} eep_reis_t;


typedef struct __attribute__((packed))
{
	//-name=текущий_рейс
	eep_current_reis_t Currentwreis;
	//- name=история_рейсов
	eep_reis_t reises[3]; 
} eep_t;


//-color = 0x000000FF
typedef struct __attribute__((packed))
{	
	//-name=акселерометр
	uint8_t accel;
	//-name=модуль_вик
	uint16_t induct;
	//-name=память_данные_не_записаны
	uint8_t reis;
	//-name=память_запрос_на_чтение_с_переполнением
	uint8_t cursor;
	//-name=память_форматирование
	uint8_t ramForm;
	//-name=память
	uint8_t ram;
} errors_t;

typedef struct __attribute__((packed))
{
	//-name=ошибки
	errors_t err;
	//-name=текущий_рейс
	eep_reis_t Current;
} eep_ram_t;

typedef struct __attribute__((packed))
{
	//-color = 0x00FF0000
	uint32_t signature;
	//-color = 0x001FFF00
	uint32_t serial;
	uint16_t L1[8];
	uint16_t L2[8];
	uint16_t F[8];
	int16_t Air_zz[8];
    //-name=сдвиг_между_квадратурами
	uint16_t PhaseOffsets[4];
	uint32_t D_sonde_mm;
	int16_t Air_zz_amt[8];//
	uint16_t service[70];//
} uart_ind_t;


typedef struct __attribute__((packed))
{
	//- from=0
	flash_nand_t NAND;
	//- from=512
	flash_bad_bloks_t BadBlocks;
	//- from=1024
	//- name=Ind
	uart_ind_t metrInd;
	//- from=2048
	eep_ram_t ramData;
	//- from=4096
	eep_t eep;
} EepData_t;

//typedef struct __attribute__((packed))
//{
//float Re;
//float Im;
//} complex_t;

//- name=Ind
//- metr=Ind1
typedef struct __attribute__((packed))
{
	uint32_t signature;
	uint32_t condition;
	//- digits=10
	//- precision=1
	float Ro_smt[8];
	//- digits=10
	//- precision=1
	float PH_smt[8];
	uint32_t frame;
	//- digits=4
	//- precision=1
	float temperature;
	
	//- digits=10
	//- precision=1
	float AM_RX_1[10];
	//- digits=10
	//- precision=1
	float AM_RX_2[10];
	//- digits=10
	//- precision=3
	float PH_RX_1[10];
	//- digits=10
	//- precision=3
	float PH_RX_2[10];
	
	//complex_t RX_1[10];
	//complex_t RX_2[10];
} Induc_t;

//- name=accel
typedef struct __attribute__((packed))
{
	int16_t X;
	int16_t Y;
	int16_t Z;
	int8_t T;	
} ais21h_t;

typedef  struct __attribute__((packed))
{
	//- name=автомат
	//- metr=AU
	uint8_t AppState;
	//- name=время
	//- metr=WT
	int32_t time;
	ais21h_t accel;
	//-structname
	Induc_t dat;
} WorkData_t;

typedef struct __attribute__((packed))
{
	//- name=время
	//- metr=WT
	int32_t ramtime;
	ais21h_t accel;
	//-structname
	Induc_t dat;
} RamData_t;

//- adr = 6
//- info = "__DATE__ __TIME__  NAND 294G UserFormating IND EEP BOOTEEP"
//- chip = 6
//- serial = 1
//- name = Ind
//- SupportUartSpeed = 0xFA
//- export
typedef struct __attribute__((packed))
{
	//- WRK
	//- noname
	WorkData_t Wrk;
	//- RamSize = 256
	//- RAM
	//- noname
	RamData_t Ram;
	//- EEP
	//- noname
	EepData_t eep;
} AllDataStruct_t;

}

