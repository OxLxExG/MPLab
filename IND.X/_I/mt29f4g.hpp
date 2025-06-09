#ifndef MT294_HPP_
#define MT294_HPP_

//#define  TEST_RAM_EMULATE_BAD

#include <avr/io.h>
#include <util/delay.h>
#include <usarts.hpp>
#include <getarg.h>
#include <protokol.h>
#include <clock.hpp>
#include <tools.h>
#include <crc.h>
#include <Indicator.hpp>

#if !defined(MT29F4G)
# error "Global define MT29F4G=SPn,muxAlt,uart,PORTCS1,BITCS1,PORTCS2,BITCS2"
  // MT29F4G=SP0,DEF0,PA,7,PA,8,UART1
#endif

//#ifdef Uart
//#undef Uart
//#endif

typedef struct {
	uint8_t manufacturer_ID;
	uint8_t device_ID;
} NAND_ID;

#define NAND_REIS  (PAGE_DATA_SIZE + 0x40) //ECC protected

#define NAND_ID_MANUFACTURER    0x2C
#define NAND_ID_DEVICE          0x34
#define NUM_OF_PPAGE_BYTES		129

#define CFG2   (1<<7)
#define CFG1   (1<<6)
#define LOT_EN (1<<5)
#define ECC_EN (1<<4)
#define DS_S1  (1<<3)
#define DS_S0  (1<<2)
#define CFG0   (1<<1)
#define CONTI_RD (1<<0)


#define FLASH_WIDTH             8               // Flash data width 
#define FLASH_SIZE_BYTES        0x100000000      // Flash size in bytes 
#define NUM_BLOCKS              2048            // Total number of blocks in the device
#define NUM_PAGES_PER_BLOCK     64              // Number of pages per block
#define PAGE_SIZE               4352            // Page size in bytes 
#define PAGE_DATA_SIZE          4096            // Page data size in bytes 
#define PAGE_SPARE_SIZE         256             // Page spare size in bytes

#define BAD_BLOCK_BYTE          PAGE_DATA_SIZE
#define BAD_BLOCK_VALUE         0x00;

#define  SPI_UART_N GETARG_3(MT29F4G)
#if   SPI_UART_N == 0
#define RUart serial0
#elif SPI_UART_N == 1
#define RUart serial1
#elif SPI_UART_N == 2
#define RUart serial2
#elif SPI_UART_N == 3
#define RUart serial3
#elif SPI_UART_N == 4
#define RUart serial4
#endif

//#pragma message(VAR_NAME_VALUE(SPI_UART_N))

//#define PP_TIMAUT_MS(ms) (2*ms)

    typedef enum {
	    SPI_NAND_CRBSY = (1 << 7), // Cache Read Busy 
	    SPI_NAND_ECC   = (1 << 6) | (1 << 5) | (1 << 4),  // ECC bits 
	    SPI_NAND_PF    = (1 << 3), // program fail 
	    SPI_NAND_EF    = (1 << 2), // erase fail 
	    SPI_NAND_WEL   = (1 << 1), // write enable latch 
	    SPI_NAND_OIP   = (1 << 0), /* operation in progress */
    } StatusRegBits;
	
    #define CHECK_OIP(status_reg)       (status_reg & SPI_NAND_OIP) // returns 1 if OIP bit is 1 and device is busy

    typedef enum {
	    SPI_NAND_RESET                  = 0xFF,
	    SPI_NAND_GET_FEATURES           = 0x0F,
	    SPI_NAND_SET_FEATURES           = 0x1F,
	    SPI_NAND_READ_ID                = 0x9F,
	    SPI_NAND_PAGE_READ              = 0x13,
	    SPI_NAND_READ_PAGE_CACHE_RANDOM = 0x30,
	    SPI_NAND_READ_PAGE_CACHE_LAST   = 0x3F,
	    SPI_NAND_READ_CACHE_X1          = 0x03,
	    SPI_NAND_READ_CACHE_X2          = 0x3B, // dual wire I/O
	    SPI_NAND_READ_CACHE_X4          = 0x6B, // quad wire I/O
	    SPI_NAND_READ_CACHE_DUAL_IO     = 0xBB,
	    SPI_NAND_READ_CACHE_QUAD_IO     = 0xEB,
	    SPI_NAND_WRITE_ENABLE           = 0x06, // sets WEL bit in Status Register to 1
	    SPI_NAND_WRITE_DISABLE          = 0x04, // clears WEL bit
	    SPI_NAND_BLOCK_ERASE            = 0xD8,
	    SPI_NAND_PROGRAM_EXEC           = 0x10,
	    SPI_NAND_PROGRAM_LOAD_X1        = 0x02,
	    SPI_NAND_PROGRAM_LOAD_X4        = 0x32,
	    SPI_NAND_PROGRAM_LOAD_RANDOM_X1 = 0x84,
	    SPI_NAND_PROGRAM_LOAD_RANDOM_X4 = 0x34,
	    SPI_NAND_PERMANENT_BLK_LOCK     = 0x2C     // permanently protect a group of blocks
    } CommandCodes;

    // Using Get Feature command, we can access one of four registers 
    // Register addrs (see Datasheet page 37) 
    typedef enum {
	    SPI_NAND_BLKLOCK_REG_ADDR = 0xA0,
	    SPI_NAND_CFG_REG_ADDR     = 0xB0,
	    SPI_NAND_STATUS_REG_ADDR  = 0xC0,
	    SPI_NAND_DIE_SEL_REG_ADDR = 0xD0,
    } RegisterAddr;


 #define ROW_ADDRESS_BLOCK_BITS   11
 #define ROW_ADDRESS_PAGE_BITS    6
 #define ROW_ADDRESS_BITS         24
 
 #define COL_ADDRESS_BITS         13
 #define COL_DATA_BITS            12

 
 //typedef struct {
	 //uint16_t block       : ROW_ADDRESS_BLOCK_BITS;  // block number
	 //uint16_t page        : ROW_ADDRESS_PAGE_BITS;   // page number
	 //uint32_t rowAddr     : ROW_ADDRESS_BITS;        // block/page address
	 //uint32_t col     : COL_ADDRESS_BITS;        // starting address within a page for writes
 //} PhysicalAddrs;
 
 typedef struct {
	 uint32_t page:ROW_ADDRESS_PAGE_BITS;  // block number
	 uint32_t block:ROW_ADDRESS_BLOCK_BITS;   // page number
 } RowAddrs;
 
 typedef struct{
	 uint16_t col;//:COL_ADDRESS_BITS;
} loadCol;

 typedef union
 {
	 uint8_t B[2];
	 loadCol col;
 } uColAddrs;

 typedef union
 {
	 uint8_t B[4];
	 RowAddrs row;
 } uRowAddrs;

 	typedef struct __attribute__((packed))
 	{
	 	uint16_t reis;
	 	uint16_t blocks;
	 	uint16_t startBlock;
	 	uint16_t endBlock;
 	} lastReisInfo_t;

 //typedef struct {
        //uint16_t block;  // block number
        //uint8_t page;   // page number
        //uint16_t col;        // starting address within a page for writes
 //} PhysicalAddrs;
 
typedef struct {
	uint16_t reis;  
	uint16_t block;  
} reis_t;
 
// Parameter Page Data Structure 
typedef struct  {
	// Parameter page signature (ONFI) 
	char signature[5];

	// Revision number 
	uint16_t rev_num;

	// Features supported 
	uint16_t feature;

	// Optional commands supported 
	uint16_t command;

	// Device manufacturer 
	char manufacturer[13];

	// Device part number 
	char model[21];

	// Manufacturer ID (Micron = 2Ch) 
	uint8_t jedec_id;

	// Date code 
	uint16_t date_code;

	// Number of data bytes per page 
	uint32_t data_bytes_per_page;

	// Number of spare bytes per page 
	uint16_t spare_bytes_per_page;

	// Number of data bytes per partial page 
	uint32_t data_bytes_per_partial_page;

	// Number of spare bytes per partial page 
	uint16_t spare_bytes_per_partial_page;

	// Number of pages per block 
	uint32_t pages_per_block;

	// Number of blocks per unit 
	uint32_t blocks_per_lun;

	// Number of logical units (LUN) per chip enable 
	uint8_t luns_per_ce;

	// Number of address cycles 
	uint8_t num_addr_cycles;

	// Number of bits per cell (1 = SLC; >1= MLC) 
	uint8_t bit_per_cell;

	// Bad blocks maximum per unit 
	uint16_t max_bad_blocks_per_lun;

	// Block endurance 
	uint16_t block_endurance;

	// Guaranteed valid blocks at beginning of target 
	uint8_t guarenteed_valid_blocks;

	// Block endurance for guaranteed valid blocks 
	uint16_t block_endurance_guarenteed_valid;

	// Number of programs per page 
	uint8_t num_programs_per_page;

	// Partial programming attributes 
	uint8_t partial_prog_attr;

	// Number of bits ECC bits 
	uint8_t num_ECC_bits_correctable;

	// Number of interleaved address bits 
	uint8_t num_interleaved_addr_bits;

	// Interleaved operation attributes 
	uint8_t interleaved_op_attr;

} param_page_t;

static uint16_t __as_uint16(uint8_t byte1, uint8_t byte0) {
		return  ((uint16_t)byte0 << 8) | byte1;
}
static uint32_t __as_uint32(uint8_t byte3, uint8_t byte2, uint8_t byte1, uint8_t byte0) {
		return ((uint32_t) (((uint32_t)byte0 << 24) | ((uint32_t)byte1 << 16) | ((uint16_t)byte2 << 8) | byte3));
}
static void __as_string(uint8_t *src_ptr, char *dest_ptr, int start, int stop) {
		strncpy((char *)dest_ptr, (const char *)src_ptr+start, stop-start+1);
		dest_ptr[stop-start+1] = '\0';
}


 // physical address macros; Input address must be of type NAND_Addr 
 //#define ADDRESS_2_BLOCK(Address)    ((uint16_t) (Address >> 17))   // divide by 131072 (2^17 bytes per block)
 //#define ADDRESS_2_PAGE(Address)     ((uint16_t) ((Address >> 11) & 0x3F))
 //#define ADDRESS_2_COL(Address)      ((uint32_t) (Address & 0x07FF)) // take last 11 bits of address


template <uint8_t spiNo, uint8_t pmuxAlt, uint8_t serialNo, uint8_t portCS1, uint8_t bitCS1,uint8_t portCS2, uint8_t bitCS2>
class ram_t 
{
	    //#define URT serial##serialNo
		#define SPI (*(SPI_t*) (0x0940 + 0x20 * spiNo))
		//#define TB (*(TCB_t*) (0x0B00 + 0x10 * timerNo))
		#define PCS_1 (*(PORT_t*) (0x0400 + 0x20 * portCS1))
		//#define PCS_2 (*(PORT_t*) (0x0400 + 0x20 * portCS2))
public:
//uint8_t chip;

   // for writing   
	RowAddrs waddr;
	reis_t wreis;
	bool WCursorStatus;
	uint8_t page[PAGE_SIZE];
	uint16_t pageCnt;
	
	lastReisInfo_t LastReis;
	//длина запроса в б. п. кол.
	uint32_t rlen;
	//uint16_t LenCol;
	//uint8_t LenPages;
	//uint8_t  LenBlocks;
	
	//начало запроса а затем текущий адрес
	uint16_t rcol;
	RowAddrs raddr;
	uint32_t rlogadr;
	uint16_t rBlocksToEnd;
	reis_t  rreis;
	
	uint8_t lastStatus;
	INLN PORT_t* getSPIPort(void)
	{
		if (spiNo == SP1)
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return &PORTC;
				case ALT1:
				return &PORTC;
				case ALT2:
				return &PORTB;
			}
		} 
		else
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return &PORTA;
				case ALT1:
				return &PORTE;
			}
		}
	}				
	INLN uint8_t getSPIbit(void)
	{
		if (spiNo == SP1)
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return 0;
				case ALT1:
				return 4;
				case ALT2:
				return 4;
			}
		}
		else
		{
			switch (pmuxAlt)
			{
				case DEF0:
				return 4;
				case ALT1:
				return 0;
			}
		}
	}
    INLN void CS_on(void) 
	{
		//if(chip == 1) 
			PCS_1.OUTCLR = 1 << bitCS1; 
		//else 
		//	PCS_2.OUTCLR = 1 << bitCS2;
	}
    INLN void CS_off(void)
	{
		PCS_1.OUTSET = 1 << bitCS1; 
		//PCS_2.OUTSET = 1 << bitCS2;
	}		
    INLN void init(void)
	{
			SPI.CTRLA = 1 << SPI_CLK2X_bp |    // Enable Double Speed: enabled 
						0 << SPI_DORD_bp   // Data Order Setting: disabled 
			| 1 << SPI_ENABLE_bp // Enable Module: enabled 
			| 1 << SPI_MASTER_bp // SPI module in master mode 
			| SPI_PRESC_DIV4_gc; // System Clock / 4  // 2*2000000 = 4MHz
			
			SPI.INTFLAGS; 

	}
    INLN void deinit(void){SPI.CTRLA = 0; SPI.INTFLAGS;}
	INLN uint8_t spi(const uint8_t d)
	{
		SPI.DATA = d;
		while (!(SPI.INTFLAGS & SPI_IF_bm)); 
		return SPI.DATA;
	}	
	void SPI_read(uint8_t *buff, uint16_t cnt)
	{
		do
		{
			*buff = spi(*buff);
			buff++;
			
		} while (--cnt > 0);
	}
	void SPI_write(const uint8_t *buff, uint16_t cnt)
	{
		do
		{
			spi(*buff++);
			
		} while (--cnt > 0);
	}
	
	uint8_t GetFeature(uint8_t addr)
	{
		init();
		CS_on();
		spi(SPI_NAND_GET_FEATURES);
		spi(addr);
		uint8_t res = spi(0);
		CS_off();
		deinit();
		return res;
	}
	
	void SetFeature(uint8_t addr, uint8_t data)
	{
		CS_on();
		spi(SPI_NAND_SET_FEATURES);
		spi(addr);
		spi(data);
		CS_off();
	}
	INLN void writeEna(void)
	{
		CS_on();
		spi(SPI_NAND_WRITE_ENABLE);
		CS_off();
		
	}
	bool waitRDY(uint16_t tmout100us)
	{		
		_delay_us(100);
		
		CS_on();
		
		spi(SPI_NAND_GET_FEATURES);
		spi(SPI_NAND_STATUS_REG_ADDR);
		
		for(uint8_t i = 0; i<tmout100us; i++)
		{
			lastStatus = spi(0);
			if (!CHECK_OIP(lastStatus))
			{				
				CS_off();				
				return true;
			}
			_delay_us(100);
		}		
		CS_off();		
		return false;
	}	
	
	bool BeginPageRead(RowAddrs* adr)
	{
		uRowAddrs* a = (uRowAddrs*)adr;
		uint8_t cmdadr[4] = {SPI_NAND_PAGE_READ, a->B[2],a->B[1],a->B[0]};
		CS_on();
		SPI_write(cmdadr,4);
		CS_off();
		return waitRDY(10);
	}
	
	INLN void CacheRead(loadCol* col, uint8_t* buf, uint16_t len)
	{
		uColAddrs* c=(uColAddrs*) col;
		uint8_t command_cache_read[4] = {SPI_NAND_READ_CACHE_X1, c->B[1], c->B[0], 0};
		CS_on();
		SPI_write(command_cache_read,4);
		SPI_read(buf,len);
		CS_off();
	}
	
	bool PageRead(RowAddrs* adr, uint16_t col, uint8_t*ppage, uint16_t len)
	{
		bool res = BeginPageRead(adr);
		if (res) 
		{
			loadCol c;
			c.col = col;
			CacheRead(&c,ppage,len);
		}
		return res;
	}
	bool SkipBlock(RowAddrs*a, reis_t*z)
	{
		uint8_t bad=0xAA;
		bool res = PageRead(a,BAD_BLOCK_BYTE, &bad,1);
		if (!res||(bad != 0xFF))
		{
			return true;
		}
		loadCol c ={.col = NAND_REIS};
		CacheRead(&c, (uint8_t*)z, sizeof(reis_t));
		// стертый блок без форматирования появляется вместе с плохим блоком при сдвиге данных
		return z->reis == 0xFFFF;
	}	
	INLN bool NAND_ReadPage(RowAddrs* adr, uint16_t col, uint8_t*ppage, uint16_t len)
	{
		init();
		bool res = PageRead(adr, col, ppage,len);
		deinit();
		return res;
	}
	bool NAND_Read_Param_Page(param_page_t *ppage)
	{
		uint8_t rbuf[NUM_OF_PPAGE_BYTES];
		RowAddrs a = {0};
		a.page=1;
		bool ret;
		init();
		SetFeature(SPI_NAND_CFG_REG_ADDR, CFG1|ECC_EN);
		ret = PageRead(&a,0, rbuf,NUM_OF_PPAGE_BYTES);
		SetFeature(SPI_NAND_CFG_REG_ADDR, ECC_EN);
		deinit();
		
		if (ret)
		{
			__as_string(rbuf, ppage->signature, 0, 3);
			// Revision number
			ppage->rev_num = __as_uint16(rbuf[4], rbuf[5]);
			
			// Features supported
			ppage->feature = __as_uint16(rbuf[6], rbuf[7]);
			
			// Optional commands supported
			ppage->command = __as_uint16(rbuf[8], rbuf[9]);
			
			// Device manufacturer
			__as_string(rbuf, ppage->manufacturer, 32, 43);
			
			// Device part number
			__as_string(rbuf, ppage->model, 44, 63);
			
			// Manufacturer ID (Micron = 2Ch)
			ppage->jedec_id = rbuf[64];
			
			// Date code
			ppage->date_code = __as_uint16(rbuf[65], rbuf[66]);
			
			// Number of data bytes per page
			ppage->data_bytes_per_page = __as_uint32(rbuf[80], rbuf[81], rbuf[82], rbuf[83]);
			
			// Number of spare bytes per page
			ppage->spare_bytes_per_page = __as_uint16(rbuf[84], rbuf[85]);
			
			// Number of data bytes per partial page
			ppage->data_bytes_per_partial_page = __as_uint32(rbuf[86], rbuf[87], rbuf[88], rbuf[89]);
			
			// Number of spare bytes per partial page
			ppage->spare_bytes_per_partial_page = __as_uint16(rbuf[90], rbuf[91]);
			
			// Number of pages per block
			ppage->pages_per_block = __as_uint32(rbuf[92], rbuf[93], rbuf[94], rbuf[95]);
			
			// Number of blocks per unit
			ppage->blocks_per_lun = __as_uint32(rbuf[96], rbuf[97], rbuf[98], rbuf[99]);
			
			// Number of logical units (LUN) per chip enable
			ppage->luns_per_ce = rbuf[100];
			
			// Number of address cycles
			ppage->num_addr_cycles = rbuf[101];
			
			// Number of bits per cell (1 = SLC; >1= MLC)
			ppage->bit_per_cell = rbuf[102];
			
			// Bad blocks maximum per unit
			ppage->max_bad_blocks_per_lun = __as_uint16(rbuf[103], rbuf[104]);
			
			// Block endurance
			ppage->block_endurance = __as_uint16(rbuf[105], rbuf[106]);
			
			// Guaranteed valid blocks at beginning of target
			ppage->guarenteed_valid_blocks = rbuf[107];
			
			// Block endurance for guaranteed valid blocks
			ppage->guarenteed_valid_blocks = __as_uint16(rbuf[108], rbuf[109]);
			
			// Number of programs per page
			ppage->num_programs_per_page = rbuf[110];
			
			// Partial programming attributes
			ppage->partial_prog_attr = rbuf[111];
			
			// Number of bits ECC bits
			ppage->num_ECC_bits_correctable = rbuf[112];
			
			// Number of interleaved address bits
			ppage->num_interleaved_addr_bits = rbuf[113];
			
			// Interleaved operation attributes
			ppage->interleaved_op_attr = rbuf[114];
		}
		return ret;
		
	}	
	
	INLN void _assignRes(reis_t& cur , RowAddrs& a)	
	{
		LastReis.reis = cur.reis;
		LastReis.blocks = cur.block;
		LastReis.startBlock = (cur.block == 0) ? a.block : 0xFFFF;
		LastReis.endBlock = a.block;
	}	
    bool NAND_CheckFormat(void)
	{
		init();
		RowAddrs a ={.page =0, .block=NUM_BLOCKS-1};
		uint16_t cnt = 0;
		reis_t cur;
		reis_t old;
		uint16_t badzaezd = 0;
		uint16_t badblcnt = 0;
		bool first = true;
		
		while (cnt < NUM_BLOCKS)
		{
			if(SkipBlock(&a,&cur))
			{
				 a.block--;
				 cnt++;
				 continue;
			}
			if (first)
			{
				first =false;
				// поиск курсора 
				_assignRes(cur,a);
			}
			else if (old.reis != cur.reis)
			{
				if (!((old.reis-1 == cur.reis) && (old.block == 0)))
				{
					badzaezd++;
				}
			}
			else 
			{
				if(cur.block != old.block-1)
				{
					badblcnt++;
				}
			}
			// поиск курсора 
			if (LastReis.reis< cur.reis)
			{
				_assignRes(cur,a);
			}
			else if (LastReis.reis == cur.reis)
			{
				if (cur.block == 0) LastReis.startBlock = a.block;
				else if (cur.block > LastReis.blocks) 
				{
					LastReis.blocks = cur.block;
					LastReis.endBlock = a.block;
				}
			}
			
			old = cur;
			a.block--;
			cnt++;
		}
		deinit();	
		LastReis.blocks++;
		return badblcnt == 0 && badzaezd <= 1;	
	}
	
	void NAND_CheckBadBlocks(uint16_t* bb)
	{
		RowAddrs a ={0, 0};
		uint8_t badIdx =0;
		
		//Ram.chip = die+1;
		init();
		for (uint16_t i =0; i < 2048; i++)
		{
			a.block = i;
			uint8_t bad = 0;
			PageRead(&a, BAD_BLOCK_BYTE, (uint8_t*)&bad, 1);
			if (bad != 0xFF)
			{
				bb[badIdx++] = i;
				if (badIdx == 40)
				{
					deinit();
					return;
				}
			}
		}
		deinit();
	}
	
	
	INLN void unlockAllBlock(void)
	{
		SetFeature(SPI_NAND_BLKLOCK_REG_ADDR,0);
	}
	//int16_t dist(reis_t* l, reis_t*r)
	//{
		//if (l->reis != r->reis) return r->reis - l->reis;
		//else return r->block - l->block;
	//}
	
	INLN bool NAND_IsNotFormatted(void)
	{
		RowAddrs a;
		a.block=NUM_BLOCKS-40;
		a.page=0;
		reis_t z;
		uint8_t cnt=0;
		init();
		while (cnt++ < 40 && SkipBlock(&a,&z)) a.block++;
		deinit();
		return cnt >= 20;
	}
	INLN void NAND_Format(void)
	{
		init();
		unlockAllBlock();
		for(uint16_t b =0; b<NUM_BLOCKS; b++)
		{
			if (BlockEraseWithMarkBad(b))
			{
				uint16_t cb = (b+NUM_BLOCKS+0)%NUM_BLOCKS;
				reis_t z = {cb/256, cb%256};
				
				bool res = _writeReis(b,&z);
				
				if(!(res && ((lastStatus & SPI_NAND_PF) ==0))) markBadBlock(b);
			}
		}
		deinit();
	}
// низкоуровнивые функции записи страницы	
// _writeReis markBadBlock _pageWrite
	INLN bool _writeReis(uint16_t block, reis_t* za)
	{
		writeEna();
		CS_on();
		uColAddrs c;
		c.col.col = NAND_REIS;
		uint8_t command_load[3] = {SPI_NAND_PROGRAM_LOAD_X1, c.B[1], c.B[0]};
		SPI_write(command_load,3);
		SPI_write((uint8_t*)za, sizeof(reis_t));
		CS_off();
		CS_on();
		uRowAddrs a;
		a.row.page = 0;
		a.row.block=block;
		uint8_t cmdadr[4] = {SPI_NAND_PROGRAM_EXEC, a.B[2],a.B[1],a.B[0]};
		SPI_write(cmdadr,4);
		CS_off();
		return waitRDY(10);
	}	
	bool markBadBlock(uint16_t block)
	{
		writeEna();
		CS_on();
		uColAddrs c;
		c.col.col = PAGE_DATA_SIZE;
		uint8_t command_load[3] = {SPI_NAND_PROGRAM_LOAD_X1, c.B[1], c.B[0]};
		SPI_write(command_load,3);
		SPI_write((uint8_t*)"\0",1);
		CS_off();
		CS_on();
		uRowAddrs a;
		a.row.page = 0;
		a.row.block = block;
		uint8_t cmdadr[4] = {SPI_NAND_PROGRAM_EXEC, a.B[2],a.B[1],a.B[0]};
		SPI_write(cmdadr,4);
		CS_off();
		return waitRDY(10);
	}
		
	INLN bool _pageWrite(RowAddrs* adr)
	{
		writeEna();
		CS_on();
		
		uint8_t command_load[3] = {SPI_NAND_PROGRAM_LOAD_X1,0,0};
		SPI_write(command_load,3);
		SPI_write(page,PAGE_DATA_SIZE);
		CS_off();
		if(adr->page == 0)
		{
			CS_on();
			uColAddrs c;
			c.col.col = NAND_REIS;
			uint8_t command_random_load[3] = {SPI_NAND_PROGRAM_LOAD_RANDOM_X1, c.B[1], c.B[0]};
			SPI_write(command_random_load,3);
			SPI_write((uint8_t*)&wreis,sizeof(reis_t));
			CS_off();
		}
		CS_on();
		uRowAddrs* a = (uRowAddrs*)adr;
		uint8_t cmdadr[4] = {SPI_NAND_PROGRAM_EXEC, a->B[2],a->B[1],a->B[0]};
		SPI_write(cmdadr,4);
		CS_off();
		bool res = waitRDY(10);
#ifdef TEST_RAM_EMULATE_BAD
		if (Emulate_bad_program_page.block == adr->block && Emulate_bad_program_page.page == adr->page)
		{
			lastStatus |= SPI_NAND_PF;
		}
#endif
		return res;
	}
	
/////////////////////////////////////////	
#ifdef TEST_RAM_EMULATE_BAD
	RowAddrs Emulate_bad_move;
	RowAddrs Emulate_bad_program_page;
	uint16_t Emulate_bad_erase_block;
#endif
//////////////////////////////////////////	
		
///////////////////////////////////
// функции стирания блока		
// низкоуровнив _BlErase 
// высокоуровнив BlockEraseWithMarkBad EraseToGood NextEraseToGood
	INLN bool _BlErase(uint16_t block)
	{
		writeEna();
		CS_on();
		uRowAddrs a={0};
		a.row.block = block;
		uint8_t cmdadr[4] = {SPI_NAND_BLOCK_ERASE, a.B[2],a.B[1],a.B[0]};
		SPI_write(cmdadr,4);
		CS_off();
		bool res = waitRDY(110);
		#ifdef TEST_RAM_EMULATE_BAD
		if (Emulate_bad_erase_block == block)
		{
			lastStatus |= SPI_NAND_EF;
		}
		#endif
		return res;
	}
	bool BlockEraseWithMarkBad(uint16_t block)
	{
		if (!_BlErase(block) || (lastStatus & SPI_NAND_EF) != 0)
		{
			markBadBlock(block);
			return false;
		}
		else return true;
	}
	INLN bool _ChekEraseMarkBad(uint16_t block)
	{
		RowAddrs a ={0};
		a.block = block;
		uint8_t bad = 0xAA;
		if (!PageRead(&a,BAD_BLOCK_BYTE, &bad, 1) || (bad != 0xFF)) return false;
		
		return BlockEraseWithMarkBad(block);
	}
	bool EraseToGood(void)
	{
		for(uint8_t i=0; i<40; i++)
		{
			if(_ChekEraseMarkBad(waddr.block)) return true;			
			waddr.block++;
		}		
		return false;
	}
	INLN bool NextEraseToGood(void)
	{
 		waddr.block++;
		return EraseToGood();
	}
////////////////////////////////////////////////
	INLN bool _pageMove(uRowAddrs* from, uRowAddrs* to)
	{
		CS_on();
		uint8_t cmdadrfrom[4] = {SPI_NAND_PAGE_READ, from->B[2],from->B[1],from->B[0]};
		SPI_write(cmdadrfrom,4);
		CS_off();
		
		if (!waitRDY(10)) return false;
		
		writeEna();
		
		CS_on();
		uint8_t command_random_load[3] = {SPI_NAND_PROGRAM_LOAD_RANDOM_X1, 0,0};
		SPI_write(command_random_load,3);
		CS_off();
		
		CS_on();
		uint8_t cmdadrto[4] = {SPI_NAND_PROGRAM_EXEC, to->B[2],to->B[1],to->B[0]};
		SPI_write(cmdadrto,4);
		CS_off();
		bool res = waitRDY(10);
		#ifdef TEST_RAM_EMULATE_BAD
		if (Emulate_bad_move.block == to->row.block && Emulate_bad_move.page == to->row.page)
		{
			return false;
		}
		#endif
		return res && (lastStatus & SPI_NAND_PF) == 0;
	}
    INLN bool BlockMove(uint16_t from, uint16_t to, uint8_t pages)	
	{
		uRowAddrs f = {0};
		uRowAddrs t = {0};
		
		f.row.block = from;
		t.row.block = to;		
			
		for(uint8_t i=0; i<pages; i++)
		{
			f.row.page = i;
			t.row.page = i;
			if(!_pageMove(&f,&t))
			{
				 markBadBlock(to);
				 return false;
			}
		}
		return true;
	}
/////////////////////////////////////////////	
	bool _wPage(void)
	{
		uint16_t lastBadBlock = 0xFFFF;
		while (true)
		{
			if (_pageWrite(&waddr) && (lastStatus & SPI_NAND_PF) == 0)
			{
				if(++waddr.page == 0)
				{
					wreis.block++;
					waddr.block++;
				}
				if (lastBadBlock != 0xFFFF) 
				{
					markBadBlock(lastBadBlock);					
				}
				if (waddr.page == 0)
				{
					if (!EraseToGood()) return false;
				}
				return true;								
			}
			// блок который записывался но произошла ошибка
			lastBadBlock = waddr.block;
			
			if (!NextEraseToGood()) return false;
						
			if (waddr.page > 0)
			{
				while (!BlockMove(lastBadBlock, waddr.block, waddr.page))
				{
					if (!NextEraseToGood()) return false;			
				}
			}
		}				
	}		
	bool write(uint8_t *buf, buf_len_t cnt)
	{
		bool res = true;
		if (pageCnt + cnt >= PAGE_DATA_SIZE)
		{
			uint8_t pc = PAGE_DATA_SIZE - pageCnt;
			memcpy( &page[pageCnt], buf, pc);
			init();
			res = _wPage();
			deinit();
			pageCnt = 0;
			cnt -= pc;
			buf += pc;
		}
		memcpy( &page[pageCnt], buf,cnt);
		pageCnt += cnt;
		
		return res;		
	}	
	
////////////////////////////////////////////////////////////	
public:
	INLN ram_t(void)
	{
		rlogadr = 0xFFFFFFFF;
			//| 0 << SPI_BUFEN_bp   // Buffer Mode Enable: disabled
			//| 0 << SPI_BUFWR_bp // Buffer Write Mode: disabled
			//| SPI_MODE_3_gc     // SPI Mode 3
	    SPI.CTRLB = 1 << SPI_SSD_bp | SPI_MODE_3_gc;  // Slave Select Disable: disabled 		
		//CHIP SELECT
		//chip = 1;		
		PCS_1.DIRSET = 1 << bitCS1;
		//PCS_2.DIRSET = 1 << bitCS2;
		CS_off();		
		// spi pins
		// mux
		PORTMUX.SPIROUTEA &= ~(3 << (spiNo*2)); //clr mux
		PORTMUX.SPIROUTEA |= (pmuxAlt << (spiNo*2)); //set mux
		// MOSI MISO SCK
		uint8_t bit = getSPIbit();
		PORT_t* port = getSPIPort();
		volatile uint8_t *port_pin_ctrl = ((uint8_t *) port + 0x10 + bit+1);
		// MISO
		*port_pin_ctrl = PORT_ISC_INTDISABLE_gc | PORT_PULLUPEN_bm;
		// MOSI, SCK
		port->DIRSET = 5 << bit;			
	}
	
	bool NAND_Reset(void)
	{
		init();
		_delay_us(2000);
		CS_on();
		spi(SPI_NAND_RESET);
		CS_off();
		bool r = waitRDY(20);
		deinit();
		return r;
	}
	uint16_t NAND_ReadID(void)
	{
		init();
		//_delay_us(2000);
		CS_on();
		spi(SPI_NAND_READ_ID);
		spi(0);
		uint8_t manuf = spi(0);
		uint8_t dev = spi(0);
		CS_off();
		deinit();
		return manuf<<8 | dev;
	}
	
	INLN bool cleared(void)
	{
		return false;
	}
	
	bool initWCursor(void)
	{
		pageCnt = 0;
		init();
		rlogadr = 0xFFFFFFFF;
		waddr.block = LastReis.endBlock;
		waddr.page = 0;
		wreis.reis = LastReis.reis+1;
		wreis.block =0;
		unlockAllBlock();
		bool res = NextEraseToGood();
		deinit();
		return res;
	}
	INLN void ResetPageBase(void) 
	{
		if (!WCursorStatus) initWCursor();
		WCursorStatus = false;
	}		
		
	INLN bool startClear(void)
	{
		 WCursorStatus = initWCursor();
		return true;
	}
	INLN bool checkReadReady(void)
	{		
		return WCursorStatus;
	}
	/////////////////////////////////////////
	// чтение памяти
	bool NextPageBlock(void)
	{
		rcol = 0;
		if (++raddr.page == 0)
		{
			raddr.block++;
			reis_t z;
			while (SkipBlock(&raddr,&z)) raddr.block++;
			
			if (z.reis != rreis.reis || rBlocksToEnd == 0)
			{
				Indicator.SetCriticalError2();
				return false;
			}
			//next block
			rBlocksToEnd--;
		}
		return true;
	}	
	INLN bool initRCursorAndBeginPageRead(ram_read_t* rr)
	{
		rlen = rr->len;
		if (rlogadr == rr->addr)
		{
			if (rcol == PAGE_DATA_SIZE)
			{
				//next page block
				if (!NextPageBlock()) return false;
				BeginPageRead(&raddr);
			}
			return true;
		}
		rlogadr = rr->addr;
		// смещение в логических блоках 0..bloks-1
		uint16_t rLogBlock = (rlogadr>>(COL_DATA_BITS+ROW_ADDRESS_PAGE_BITS));
		if (rLogBlock >= LastReis.blocks) return false;
		// колонка (смещение в странице)0..2047
		rcol = rlogadr & (PAGE_DATA_SIZE-1);
		// страница 0..63
		uint8_t rpage = (rlogadr>>COL_DATA_BITS) & (NUM_PAGES_PER_BLOCK-1);
		
		rreis.reis = LastReis.reis;
		// last block 0..bloks-1;
		rreis.block = rLogBlock;
		// phy start block;
		raddr.block = LastReis.startBlock+rLogBlock;
		raddr.page = 0;
		rBlocksToEnd = LastReis.blocks-rLogBlock;
		
		for(uint8_t i=0; i<40; i++)
		{
			reis_t z;
			while (SkipBlock(&raddr,&z)) raddr.block++;
			if (z.reis != rreis.reis) return false;
			if (z.block > rLogBlock ) return false;
			if (z.block == rreis.block)
			{
				raddr.page = rpage;
				BeginPageRead(&raddr);
				return true;
			}
		}
		return false;
	}
	INLN void BeginCacheRead(loadCol* col)
	{
		uColAddrs* c=(uColAddrs*) col;
		uint8_t command_cache_read[4] = {SPI_NAND_READ_CACHE_X1, c->B[1], c->B[0], 0};
		CS_on();
		SPI_write(command_cache_read,4);
	}
	INLN void EndCacheRead(void)
	{
		CS_off();
	}
	INLN void InitFirstChunk(uint16_t* chunk)
	{
		uint16_t toendpageCount = PAGE_DATA_SIZE- rcol;
		*chunk = (rlen <= toendpageCount) ? rlen : toendpageCount;
		rlen -= *chunk;
		rlogadr += *chunk;
		rcol += *chunk;
		if (rcol > PAGE_DATA_SIZE)
		{
			Indicator.SetCriticalError2();
		}
		//else if (rcol == PAGE_DATA_SIZE) rcol = 0;
	}
	INLN bool NextChunk(uint16_t* chunk)
	{
		if (rlen == 0) return false;
		if (rcol != PAGE_DATA_SIZE)
		{
			Indicator.SetCriticalError2();
			return false;
		}
		else {
			//next page block
			if (!NextPageBlock()) return false; 
		}
		*chunk = (rlen < PAGE_DATA_SIZE) ? rlen : PAGE_DATA_SIZE;
		rlen -= *chunk;
		rlogadr += *chunk;
		rcol += *chunk;
		if (rcol > PAGE_DATA_SIZE)
		{
			Indicator.SetCriticalError2();
		}
		//else if (rcol == PAGE_DATA_SIZE) rcol = 0;
		BeginPageRead(&raddr);
		loadCol cl = {.col = 0};
		BeginCacheRead(&cl);
		SPI.DATA = 0;
		return true;
	}
	INLN void readAndSendUart(uint8_t* rd)
	{
		init();
		if (initRCursorAndBeginPageRead((ram_read_t*)rd))
		{
			loadCol kl = {.col = rcol % PAGE_DATA_SIZE};
			BeginCacheRead(&kl);
			
			uint16_t chunk;// = ((ram_read_t*)rd)->len;
			uint16_t crc = 0xFFFF;
			InitFirstChunk(&chunk);
			if (!Clock.HiSpeedReady)
			{
				Clock.MAXInternalClock();
				RUart.setBaud_MAXFSPU();				
			}
			RUart.noIntMode();
			RUart.disableRxD();
			RUart.enableTxD();			
			RUart.write(RUart.buf[0]);
			SPI.DATA = 0;
			crc = crc16next(crc, RUart.buf[0]);
			#ifdef BT2
			RUart.write(RUart.buf[1]);
			crc = crc16next(crc, RUart.buf[1]);
			#endif
			do
			{
				do{
					//SPI.INTFLAGS;
					while (!(SPI.INTFLAGS & SPI_IF_bm));
					uint8_t d = SPI.DATA;
					SPI.DATA = 0;
					RUart.writeFast(d);
					crc = crc16next(crc, d);
					RUart.writeWait();
				}while (--chunk > 0);	
				while (!(SPI.INTFLAGS & SPI_IF_bm));			
  				EndCacheRead();
			}while (NextChunk(&chunk));
			RUart.write(crc);
			RUart.writeTxC2(crc >> 8);
			RUart.disableTxD();
			if (!Clock.HiSpeedReady)
			{
				Clock.RestoreExternalClock();
				RUart.resetBaud_MAXFSPU();				
			}			
			RUart.intMode();
			RUart.enableRxD();
			Indicator.Not();			
		}
		//CS_off();
		deinit();
	}
};

extern ram_t<MT29F4G> Ram;

#endif // USARTS_H_ 