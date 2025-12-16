#pragma  once
#include <avr/io.h>

namespace induc
{
 typedef struct __attribute__((packed))
 {
	 uint16_t page;  
	 uint16_t block;   
 } tst_RowAddrs;
    
typedef struct __attribute__((packed))
{
	uint16_t reis;  
	uint16_t block;  
} tst_reis_t;
	
//-readonly
typedef struct __attribute__((packed))
{
	uint16_t ResetCount;	
	uint8_t ResetFunction;	
	uint8_t ResetRegister;
	//- name=восстановленый_кадр
	//- metr=WT
	int32_t kadr_Reset;
} reset_t;

//-color = 0x000000FF
//-ShowHex
//-readonly
typedef struct __attribute__((packed))
{	
	//-name=Ошибки_устройств
	uint16_t ErrPerifMsk;
	//-name=Ошибки_NAND
	uint16_t ErrNandMsk;
} perif_errors_t;

typedef struct __attribute__((packed))
{
	//- name=Аномальный_сброс
	reset_t reset; //8
	//- name=время_qzErr
	//- metr=WT
	//- readonly
	int32_t kadr_qzErr;	//4
    //- name = Ошибки_битовая_маска
    perif_errors_t err; //4
} eep_errors_t; //16 


//-readonly
typedef struct __attribute__((packed))
{
	//- name=автомат
	//- metr=AU
	uint8_t AppState;
	//- name=время
	//- metr=WT
	int32_t time;	
} work_state_t;


typedef struct __attribute__((packed))
{
	//- ShowHex
	//-readonly
	uint16_t bad_blocks[40];
} flash_nand_bad_bloks_t;

//-readonly
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

//-readonly
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
} ereis_t;

//-readonly
typedef struct __attribute__((packed))
{
	//- name=оконченый_рейс
	ereis_t reises[4];   // 8*4
} eep_reis_history_t;

typedef struct __attribute__((packed))
{
    //-name=текущий_рейс
    ereis_t reis; //8
	//- name=состояние_и_кадр
	work_state_t State; //5
} eep_save_state30_t;

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
} eep_ind_t;

// виртуальная структура EEPROM ROM RAM

typedef struct __attribute__((packed))
{
	//- from=0
	// - password=виьклиеьщукщш
	//- name=Модель_NAND
	flash_nand_t NAND;
	//- from=16
	//- password=rwelgrtgh
	//-name=errors
	eep_errors_t errors;
	//- from=32
	//- password=итвкитентьгь
	//- name=сохраненное_состояние
	eep_save_state30_t state;
	//- from=64
	//- password=цукукерьа
	//- name=История_рейсов 
    eep_reis_history_t hist;	
	//- from=128
	//- password=виьклиеьщукщш
	//- name=Сбойные_Блоки
	flash_nand_bad_bloks_t badBlock;
  	//- from=1024
	//- name=Ind
	eep_ind_t metrInd;

} EepData_t;

typedef struct __attribute__((packed))
{
	tst_RowAddrs waddr;
	tst_reis_t wreis;
	int8_t WCursorStatus;
	uint16_t pageCnt;	
	ereis_t LastReis;
    uint16_t CurrReisStartBlock;
} tst_ram;

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
    tst_ram tstram;
} WorkData_t;

typedef struct __attribute__((packed))
{
	//- name=время
	//- metr=WT
	int32_t ramtime;
	ais21h_t accel;
	//-structname
	Induc_t dat;
    tst_ram tstram;
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

