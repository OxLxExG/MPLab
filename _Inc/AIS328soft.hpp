#pragma once

#include <tools.h>
//#include <util/delay.h>
#include <string.h>

template<uint8_t MOSIPORT, uint8_t MOSIPIN,
		uint8_t MISOPORT, uint8_t MISOPIN,
		uint8_t CLKPORT, uint8_t CLKPIN, 
		uint8_t CSPORT, uint8_t CSPIN>
class ais328_t
{
	#define mosi (*(PORT_t*) (0x0400 + 0x20 * MOSIPORT))
	#define mosim _BV(MOSIPIN)
	#define miso (*(PORT_t*) (0x0400 + 0x20 * MISOPORT))
	#define misom _BV(MISOPIN)
	#define clk (*(PORT_t*) (0x0400 + 0x20 * CLKPORT))
	#define clkm _BV(CLKPIN)
	#define cs (*(PORT_t*) (0x0400 + 0x20 * CSPORT))
	#define csm _BV(CSPIN)
public:
	enum 
	{
		CTRL_REG1       = 0x20,
		CTRL_REG2       = 0x21,
		CTRL_REG3       = 0x22,
		CTRL_REG4       = 0x23,
		CTRL_REG5       = 0x24,
		STAT_REG        = 0x27,
		A_X             = 0x28,
		A_Y             = 0x2A,
		A_Z             = 0x2C,
		INT1_CFG        = 0x30,
		INT1_SOURCE     = 0x31,
		INT1_THS        = 0x32,
		INT1_DURATION   = 0x33,
		INT2_CFG        = 0x34,
		INT2_SOURCE     = 0x35,
		INT2_THS        = 0x36,
		INT2_DURATION   = 0x37,
		NORMAL_MODE     = 0x27,
		LOW_POWER       = 0x47,
		POWER_DOWN      = 0x07
	};

	struct xyz_data_t
	{
		int16_t a_x;
		int16_t a_y;
		int16_t a_z;
	} __attribute__((packed, aligned(1)));

	struct ctrl1
	{
		uint8_t ax_en           : 1;
		uint8_t ay_en           : 1;
		uint8_t az_en           : 1;
		uint8_t data_rate       : 2;
		uint8_t pwr_mod         : 3;
	};

	struct ctrl2
	{
		uint8_t hp_filt_cutoff  : 2;
		uint8_t hp_filt_ch2en   : 1;
		uint8_t hp_filt_ch1en   : 1;
		uint8_t data_filt_en    : 1;
		uint8_t hp_filt_mode    : 2;
		uint8_t boot            : 1;
	};

	struct ctrl3
	{
		uint8_t i1_cfg          : 2;
		uint8_t lir1            : 1;
		uint8_t i2_cfg          : 2;
		uint8_t lir2            : 1;
		uint8_t pp_od           : 1;
		uint8_t IHL             : 1;
	};

	struct ctrl4
	{
		uint8_t spi_mode        : 1;
		uint8_t self_tst_en     : 1;
		uint8_t unused          : 1;
		uint8_t STsign          : 1;
		uint8_t FS              : 2;
		uint8_t BLE             : 1;
		uint8_t BDU             : 1;
	};

	struct ctrl5
	{
		uint8_t turn_on         : 2;
		uint8_t unused          : 6;
	};

	struct stat_reg
	{
		uint8_t x_new_data      : 1;
		uint8_t y_new_data      : 1;
		uint8_t z_new_data      : 1;
		uint8_t xyz_new_data    : 1;
		uint8_t x_overrun       : 1;
		uint8_t y_overrun       : 1;
		uint8_t z_overrun       : 1;
		uint8_t xyz_overrun     : 1;
	};

private:
	
	struct {
		union {
			struct
			{
				int32_t x,y,z;
			};
			int32_t arr[3];
		};
		int16_t cnt;		
	};
		
	INLN void cs_lo(void){cs.OUTCLR = csm;}
	INLN void cs_hi(void){cs.OUTSET = csm;}
	void write8(uint8_t bt)
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			clk.OUTCLR = clkm;
			if (bt & 0x80) mosi.OUTSET = mosim;
			else mosi.OUTCLR = mosim;
			clk.OUTSET = clkm;
			bt <<= 1;
		}
	}
	uint16_t read16(void)
	{
		uint16_t acc16 = 0;
		for (uint8_t i = 0; i < 16; i++)
		{
			clk.OUTCLR = clkm;
			acc16 <<= 1;         //000000x0
			clk.OUTSET = clkm;
			if (miso.IN & misom) acc16 |=1;
		}
		return acc16;
	}
	void write(uint8_t adr, uint8_t data)  //функция записи данных
	{
		cs_lo();
		write8(adr);
		write8(data);
		cs_hi();
	}
	//функция считывания данных с акселерометра
	int16_t read(uint8_t axis)
	{
		cs_lo();
		write8(axis | 0xc0);
		int16_t out = read16();
		cs_hi();
		return out;// >> 4;
	}

	public:
	
	ais328_t() 
	{				
		cs.DIRSET |= csm;
		cs_hi();

		mosi.DIRSET = mosim;
		mosi.OUTCLR = mosim;		
		clk.DIRSET = clkm;
		clk.OUTCLR = clkm;

		volatile uint8_t* port_pin_ctrl = &miso.PIN0CTRL;
		port_pin_ctrl += MISOPIN;
		// MISO
		*port_pin_ctrl = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;					
	}
	
	void sample(void)
	{
		x += read(A_X);
		z += read(A_Y);
		y += read(A_Z);
		cnt++;
	}
	
	void get_data(void* buff)
	{
		int16_t* ptr = reinterpret_cast<int16_t*>(buff);
		
		for (uint8_t i = 0; i < 3; i++) *ptr++ = arr[i]/cnt;
		memset(&x, 0, sizeof(arr) + sizeof(cnt));
	}

	void norm_mode_2g_50hz(void)
	{
		union
		{
			uint8_t cmd;
			ctrl1 ctrl1_reg;
		};

		union
		{
			uint8_t cmd1;
			ctrl4 ctrl4_reg;
		};

		ctrl1_reg.ax_en = 1;
		ctrl1_reg.ay_en = 1;
		ctrl1_reg.az_en = 1;
		ctrl1_reg.pwr_mod = 1;
		ctrl1_reg.data_rate = 1;

		ctrl4_reg.FS = 0;
		ctrl4_reg.BDU = 1;
		ctrl4_reg.BLE = 1;

		write(CTRL_REG2, 0);
		write(CTRL_REG4, cmd1);
		write(CTRL_REG1, cmd);
	}

	void norm_mode_8g_1000hz(void)
	{
		union
		{
			uint8_t cmd;
			ctrl1 ctrl1_reg;
		};

		union
		{
			uint8_t cmd1;
			ctrl4 ctrl4_reg;
		};

		union
		{
			uint8_t cmd2;
			ctrl2 ctrl2_reg;
		};

		ctrl1_reg.ax_en = 1;
		ctrl1_reg.ay_en = 1;
		ctrl1_reg.az_en = 1;
		ctrl1_reg.pwr_mod = 1;
		ctrl1_reg.data_rate = 3;

		ctrl2_reg.data_filt_en = 1;
		ctrl2_reg.hp_filt_cutoff = 3;

		ctrl4_reg.FS = 3;
		ctrl4_reg.BDU = 1;

		write(CTRL_REG2, cmd2);
		write(CTRL_REG4, cmd1);
		write(CTRL_REG1, cmd);

	}
};

//extern ais328_t<AIS328SOFT> ais328;


