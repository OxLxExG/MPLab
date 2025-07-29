#pragma once


#ifdef BT2
	//��� ������������� � BT1
	#define ADDRESS 0
    #define CMD_POS 1
	#define HEADER_LEN 2
	#define DATA_POS 2
#else
	//����� ���������� � ����
	#define ADDRESS (ADDRESS_PROCESSOR << 4)
    #define CMD_POS 0
	#define HEADER_LEN 1
	#define DATA_POS 1
#endif // BT2
	#define CRC_LEN 2

//********* ������� �������������� ����������� **************

// ��������� ������� �� ����������� ���� 0 - �� ���������� ������� 
  #define CMD_TIME_SYNC 0xF5

// ������������� ������� �� ����� ������ � ��������
	#define CMD_BEACON 0xFA

// ����� ��������� ��������
	#define CMD_TURBO 0xFD

// ������ ������� ������
#define CMD_ERAM 0x1 | ADDRESS
// ������ ���������� �� ���������� (���������� ����������)
#define CMD_INFO 0x2 | ADDRESS

// ��������� ������
#define CMD_ERR 0xE | ADDRESS // ���� 6 ������� �������� �  CMD_WRITE_EE

// ������ EEPROM ������
#define CMD_READ_EE 0x5 | ADDRESS
// ������ EEPROM ������
#define CMD_WRITE_EE 0x6 | ADDRESS

// ������� ��������� � ������ (�������� ����� ��� ������ ��������� ����������� ������ � ������� ������)
#define CMD_WORK 0x7 | ADDRESS
// bootloader
#define CMD_BOOT 0x8 | ADDRESS
// ������������� NAND
#define CMD_ERAM_CLEAR 0x0A | ADDRESS
// bootloader commands
#define CMD_BOOT_TEST	0x8
#define CMD_BOOT_READ   0xD 
#define CMD_BOOT_EXIT	0xE 
#define CMD_BOOT_WRITE  0xF 

// ������ ������� ������ ������ � �������� �����
#define CMD_ERAM_WRITE 0x9 | ADDRESS
// �������� ������� ������ ������ � �������� �����
//#define CMD_ERAM_CLEAR 0xA | ADDRESS
// ��������� �������� ��� ������
#define CMD_ERAM_SET_BASE 0xC | ADDRESS

// ��������� �������
enum
{
	APP_SET_TIME,
	APP_CLEAR_RAM,
	APP_DELAY,
	APP_WORK,
	APP_IDLE,
	APP_MAX
};


// ����������� ������ ������
typedef  struct  __attribute__((packed))
{
	uint8_t AppState; ///  �������|AU
	int32_t time;    ///  �����|WT
} WorkDataStd_t;

// CMD_ERAM ������ ������ ������� 
typedef struct  __attribute__((packed)) 
{
	uint32_t addr;
	uint32_t len;
} ram_read_t;

