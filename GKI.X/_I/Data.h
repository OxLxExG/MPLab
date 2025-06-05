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
	TrrT_t T[2];
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
	//-name=����_�����_�������_1
	uint8_t ResetFlag;
} eep_new_bat_t;

//-name = charge
typedef struct __attribute__((packed))
{
	//-readonly
	uint8_t status;
	//-color = 0x7F0000FF
	//-eu=Ah
	//-name=���������������_�������
	//- digits=10
	//- precision=7
	float AccCharge;
	//- name=����������_�������
	//-color = 0x7F0000FF
	//-eu=Ah
	//-readonly
	//- digits=5
	//- precision=2
	float remainCh;
	//-readonly
	//- name=������
	//-color = 0x7F0000FF
	//-eu=%
	uint8_t procent;
	//-readonly
	//- name=��������_�����
	//-color = 0x7F0000FF
	//-eu=h
	uint16_t HoursLleft;
	//-color = 0x7F1FFF00
	//- eu = C
	//- name = �����������
	//-readonly
	//- digits=5
	//- precision=1
	float T;
	//- name = ����������_�������
	//-readonly
	//- eu = V
	//- digits=5
	//- precision=2
	float V;
	//- name = ���
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
	//- name=��������������_����
	//- metr=WT
	//-readonly
	int32_t kadr_Reset;
} reset_t;

typedef struct __attribute__((packed))
{
	//- name=ResetErr
	reset_t reset; //8
	//- name=�����_qzErr
	//- metr=WT
	//-readonly
	int32_t kadr_qzErr;	//4
} eep_errors_t; //12

typedef struct __attribute__((packed))
{
	//-name=�������_�������
	//-color = 0x7F0000FF
	//-eu=Ah
	float volum;
} eep_batt_volum_t;

typedef struct __attribute__((packed))
{
	//- name=�������
	//- metr=AU
	//-readonly
	uint8_t AppState;
	//- name=�����
	//- metr=WT
	//-readonly
	int32_t kadr;
} save_state_t;

typedef struct __attribute__((packed))
{
	//- name=���������_�_����
	save_state_t kadr;
	//- name=������_�������
	charge_t charge;
} eep_save_state_and_charge_t;


typedef struct __attribute__((packed))
{
	//- name= knT
	float kmul;
} InclinNanoTesla_t;


typedef struct __attribute__((packed))
{
	//  - name=�������
	//  eep_charge_setup_t chasetup;	
	// - name=��
	// TrrGk_t GR1;
	TrrInclin_t Inclin;
	//- name=�����������
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
	//- name=�����_�������
	eep_new_bat_t chasetup;
	//- from=8
	//- password=BAT
	//- name=�����_�������
	eep_batt_volum_t batvol;
	//- from=16
	//- password=rwelgrtgh
	//-name=errors
	eep_errors_t errors;
	//- from=512
	//- password=Reset_BAT_enable
	//- name=�����������_���������
	eep_save_state_and_charge_t state;
	//- from=1024
	//- password=DAC
	//- name=���������_��
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
	
//- name=��
//- metr=GK1
typedef struct __attribute__((packed))
{
//- name= ��
	uint16_t gk; 
} Gk_t ;

//- name=Inclin
//- metr=INKLGK2
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
	//- eu  = ���
	//- digits=10
	//- precision=2
	float gk_m;
	//- name= �����������
	//- eu= grad
	//- digits=10
	//- precision=1
	float Gtf; 
	//- name= �����
	//- eu= grad
	//- digits=10
	//- precision=2
	float Zenit; 
	//- name= ������
	//- eu= grad
	//- digits=10
	//- precision=1
	float Azimut; 
	//- name= ���_������
	//- eu= grad
	//- digits=10
	//- precision=1
	float Mtf; 
	//- name= ������_accel
	//- eu = mG
	//- digits=10
	//- precision=0
	uint16_t Gtot; 
	//- name= ������_magnit
	// - eu = nT
	//- digits=10
	//- precision=0
	uint16_t Mtot; 
	//- name= ���_������
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
//- name=�������
//- metr=AU
	uint8_t AppState;
//- name=�����
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
//- name=�����
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
//- info = "__DATE__ __TIME__  ADXL356.9 GK EEP_INT BOOTEEP CHAGER.v3 NanoTesla WT>0"
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

