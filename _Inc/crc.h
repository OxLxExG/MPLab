#pragma once
#include <avr/io.h>

#ifdef __cplusplus
 extern "C" uint16_t crc16(uint8_t *buf, uint8_t count);
 extern "C" uint16_t crc16Long(uint8_t *buf, uint16_t count);
 extern "C" uint16_t crc16next(uint16_t crc_old, uint8_t data);
#else
 uint16_t crc16(uint8_t *buf, uint8_t count);
 uint16_t crc16Long(uint8_t *buf, uint16_t count);
 uint16_t crc16next(uint16_t crc_old, uint8_t data);
#endif
