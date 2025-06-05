#pragma  once

namespace nnk128
{
	
	

//-eu=mV
typedef struct __attribute__((packed))
{
	//- name= ��
	uint16_t DAC1;
	//- name= ��
	uint16_t DAC2;
} eep_nnk_dac_t;

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


// ����������� ��������� EEPROM ROM RAM

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
	//- password=DAC_setup_enable
	//- name=���������_DAC
	eep_nnk_dac_t dac;
} EepData_t;

//- name = ���
//- metr = NNK128
typedef struct
{
	//- name=��
	uint16_t nk1; 
	//- name=��
	uint16_t nk2; 
} nnk_data_t;

//-name = accel
//-metr=CLA1
typedef struct __attribute__((packed))
{
	int16_t X;
	int16_t Y;
	int16_t Z;
} accel_t;

typedef  struct __attribute__((packed))
{
	//- name=�������
	//- metr=AU
	uint8_t AppState;
	//- name=�����
	//- metr=WT
	int32_t time;
	//-structname
	charge_t charge;
	//-structname
	accel_t accel;
	//-structname
	nnk_data_t nnk;
} WorkData_t;

typedef struct __attribute__((packed))
{
	//- name=�����
	//- metr=WT
	int32_t ramtime;
	//-structname
	charge_t charge;
	//-structname
	accel_t accel;
	//-structname
	nnk_data_t nnk;
} RamData_t;

//- adr = 5
//- info = "__DATE__ __TIME__  NNK128.5 BOOTEEP ChargerINT Accel Password QzTst EepSvStINT"
//- chip = 6
//- serial = 1
//- name = Nnk128
//- SupportUartSpeed = 0xFA
// 9-charge 12-accel
//- NoPowerDataCount = 10
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

