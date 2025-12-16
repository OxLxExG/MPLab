#pragma  once
#include <avr/io.h>
#include <Data.h>

extern bool NAND_ReadBadBlocks(void* n, uint8_t cnt);
extern bool NAND_ReadModel(void* n);
//extern bool NAND_Check(void);


