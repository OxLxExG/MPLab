#pragma  once

namespace adxl356gk_v2
{
	
typedef struct __attribute__((packed))
{
	int32_t X;
	int32_t Y;
	int32_t Z;
} Dat_t;

typedef struct __attribute__((packed))
{
	float X;
	float Y;
	float Z;
} Dat_m_t;

typedef struct __attribute__((packed))
{
	//- digits=10
	//- precision=5
	float  kGK;
//-color = 0x00FF0000	
	uint16_t DAC;
} TrrGk_t;

// - digits = 12
// - precision=8
//typedef struct __attribute__((packed))
//{
//	float  k1;
//	float  k2;
//	float  k3;
//	float  k4;
//	float  k5;
//	float  k6;
//	float  k7;
//	float  k8;
//	float  k9;
//	float  k10;
//	float  k11;
//	float  k12;
//	float  k13;
//	float  k14;
//	float  k15;
//	float  k16;
//} k_t;

typedef struct __attribute__((packed))
{
    uint8_t Model[3];
	float X[14];
	float Y[14];
	float Z[14];
} TrrSensor_t;


typedef struct __attribute__((packed))
{
	TrrSensor_t accel;
	TrrSensor_t magnit;
} Poly_t;


typedef struct __attribute__((packed))
{
    Poly_t Poly;
} TrrInclin_t;

//- ShowHex
typedef struct __attribute__((packed))
{
	//-readonly
	uint16_t id;
	//-readonly
	uint16_t status;
	//-readonly
	uint16_t mode;
	//-readonly
	uint16_t clock;
	//-readonly
	uint16_t gain1;
	//-readonly
	uint16_t gain2;
	//-readonly
	uint16_t cfg;	
} eep_ads131_t;

typedef struct __attribute__((packed))
{
	//-name=Флаг_смены_батареи_1
	uint8_t ResetFlag;
} eep_new_bat_t;

//-name = charge
typedef struct __attribute__((packed))
{
	//-readonly
	uint8_t status;
	//-color = 0x7F0000FF
	//-eu=Ah
	//-name=Израсходованная_емкость
	//- digits=10
	//- precision=7
	float AccCharge;
	//- name=Оставшаяся_емкость
	//-color = 0x7F0000FF
	//-eu=Ah
	//-readonly
	//- digits=5
	//- precision=2
	float remainCh;
	//-readonly
	//- name=Ресурс
	//-color = 0x7F0000FF
	//-eu=%
	uint8_t procent;
	//-readonly
	//- name=Осталось_часов
	//-color = 0x7F0000FF
	//-eu=h
	uint16_t HoursLleft;
	//-color = 0x7F1FFF00
	//- eu = C
	//- name = Температура
	//-readonly
	//- digits=5
	//- precision=1
	float T;
	//- name = Напряжение_батареи
	//-readonly
	//- eu = V
	//- digits=5
	//- precision=2
	float V;
	//- name = Ток
	//-readonly
	//- eu = mA
	//- digits=5
	//- precision=1
	float I;
} charge_t;

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

typedef struct __attribute__((packed))
{
	//-name=Ёмкость_батареи
	//-color = 0x7F0000FF
	//-eu=Ah
	float volum;
} eep_batt_volum_t;

typedef struct __attribute__((packed))
{
	//- name=автомат
	//- metr=AU
	//-readonly
	uint8_t AppState;
	//- name=время
	//- metr=WT
	//-readonly
	int32_t kadr;
} save_state_t;

typedef struct __attribute__((packed))
{
	//- name=состояние_и_кадр
	save_state_t kadr;
	//- name=Разряд_батареи
	charge_t charge;
} eep_save_state_and_charge_t;


typedef struct __attribute__((packed))
{
	//- name= knT
	float kmul;
} InclinNanoTesla_t;


typedef struct __attribute__((packed))
{
	//  - name=Батарея
	//  eep_charge_setup_t chasetup;	
	// - name=ГК
	// TrrGk_t GR1;
	TrrInclin_t Inclin;
	//- name=Магнитометр
	InclinNanoTesla_t toNanoTesla;
} eep_t;

typedef struct __attribute__((packed))
{
	//- from=0
	// - password=BAT
	//- name=Смена_батареи
	eep_new_bat_t chasetup;
	//- from=8
	//- password=BAT
	//- name=Объём_батареи
	eep_batt_volum_t batvol;
	//- from=16
	//- password=rwelgrtgh
	//-name=errors
	eep_errors_t errors;
	//- from=512
	//- password=Reset_BAT_enable
	//- name=сохраненное_состояние
	eep_save_state_and_charge_t state;
	//- from=1024
	//- password=DAC
	//- name=Настройка_ГК
	TrrGk_t dac;
	
	//- from=2000
	//- password=STDM
	eep_t stdm;
	//- password=rwelgrtgh
	//- from=4000
	eep_ads131_t ads313regs;
} EepData_t;
	
//- name=ГК
//- metr=GK1
typedef struct __attribute__((packed))
{
//- name= гк
	uint16_t gk; 
} Gk_t ;

//- name=Inclin
//- metr=INKLGK4
typedef struct __attribute__((packed))
{
	//- eu = mG
	//- digits=10
	//- precision=1
	Dat_m_t accel_m;
	//- eu = nT
	//- digits=10
	//- precision=1
	Dat_m_t magnit_m;
	//- eu  = мкР
	//- digits=10
	//- precision=2
	float gk_m;
	//- name= отклонитель
	//- eu= grad
	//- digits=10
	//- precision=1
	float Gtf; 
	//- name= зенит
	//- eu= grad
	//- digits=10
	//- precision=2
	float Zenit; 
	//- name= азимут
	//- eu= grad
	//- digits=10
	//- precision=1
	float Azimut; 
	//- name= маг_отклон
	//- eu= grad
	//- digits=10
	//- precision=1
	float Mtf; 
	//- name= амплит_accel
	//- eu = mG
	//- digits=10
	//- precision=0
	uint16_t Gtot; 
	//- name= амплит_magnit
	// - eu = nT
	//- digits=10
	//- precision=0
	uint16_t Mtot; 
	//- name= маг_наклон
	//- eu= grad
	//- digits=10
	//- precision=1
	float Mn; 
	//- eu= grad.C
	//- digits=10
	//- precision=3
	float T;
	Dat_m_t accel;
	Dat_m_t magnit;
} InclW_t;

typedef  struct __attribute__((packed))
{
//- name=автомат
//- metr=AU
	uint8_t AppState;
//- name=время
//- metr=WT
	int32_t time;    
	//-structname
	InclW_t dat;	
	//-structname
	Gk_t gk;        
	//-structname
	charge_t charge;
} WorkData_t;

typedef struct __attribute__((packed))
{
//- name=время
//- metr=WT
	int32_t ramtime;    
	//-structname
	InclW_t dat;  
	//-structname
	Gk_t gk;        
	//-structname
	charge_t charge;
} RamData_t;

//- adr = 3
//- info = "__DATE__ __TIME__  ADXL356.20 4points Poly"
//- chip = 6
//- serial = 1
//- name = InclGK4
//- SupportUartSpeed = 0xFA
//- export
typedef struct __attribute__((packed))
{
 //- WRK
 //- noname
   WorkData_t Wrk; 
   //- RamSize = 32
   //- RAM
   //- noname
   RamData_t Ram; 
   //- EEP
   //- noname
   EepData_t eep;
} AllDataStruct_t; 
}

