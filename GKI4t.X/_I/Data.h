#pragma  once

namespace adxl356gk_v2
{
	
#define TEMPR_POINTS 3	
    
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

// - name=m3x4
//- digits = 12
//- precision=8
typedef struct __attribute__((packed))
{
	float  m11;
	float  m12;
	float  m13;
	float  m14;
	float  m21;
	float  m22;
	float  m23;
	float  m24;
	float  m31;
	float  m32;
	float  m33;
	float  m34;
} m3x4_t;

typedef struct __attribute__((packed))
{
	m3x4_t m3x4;
} TrrSensor_t ;

/*
 T-
   T1
    t0, t1
    accel
	     X
	     k, d
	     Y
	     k, d
	     Z
	     k, d
	magnit
	     X
		  k, d
		 Y
		  k, d
		 Z
		  k, d
   T2
*/
typedef struct __attribute__((packed))
{
	float kOld;
	float k;
	float d;
	float d0;
} koeff_t;

typedef struct __attribute__((packed))
{
	koeff_t X;
	koeff_t Y;
	koeff_t Z;	
} sensorTrrT_t;

typedef struct __attribute__((packed))
{
	float t0;
	float t1;
	sensorTrrT_t accel;
	sensorTrrT_t magnit;
} TrrT_t;

typedef struct __attribute__((packed))
{
	TrrT_t T[TEMPR_POINTS];
} TrrTemp_t;

typedef struct __attribute__((packed))
{
	TrrSensor_t accel;
	TrrSensor_t magnit;
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
	//-color = 0x0000FF
	//-eu=Ah
	//-name=Израсходованная_емкость
	//- digits=10
	//- precision=7
	float AccCharge;
	//- name=Оставшаяся_емкость
	//-color = 0x0000FF
	//-eu=Ah
	//-readonly
	//- digits=5
	//- precision=2
	float remainCh;
	//-readonly
	//- name=Ресурс
	//-color = 0x0000FF
	//-eu=%
	uint8_t procent;
	//-readonly
	//- name=Осталось_часов
	//-color = 0x0000FF
	//-eu=h
	uint16_t HoursLleft;
	//-color = 0x1FFF00
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
	//-color = 0x0000FF
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
	TrrTemp_t T;
} eep_inclT_t;


typedef struct __attribute__((packed))
{
	eep_inclT_t Inclin;
} eep_512_t;

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
	//- from=3000
	//- password=EXM
	eep_512_t metrEx;		
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
//- metr=INKLGK3
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
//- info = "__DATE__ __TIME__  ADXL356.10 4points 1024buf"
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

