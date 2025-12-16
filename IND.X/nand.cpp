
#include <avr/io.h>
#include <mt29f4g.hpp>
#include <Data.h>

using namespace induc;

extern ram_t<MT29F4G> Ram;

void NAND_ReadBadBlocks(void* n, uint8_t cnt)
{
    memset(n, 0xFF, cnt*2);
    Ram.NAND_CheckBadBlocks((uint16_t*)n, cnt);
}

bool NAND_ReadModel(void* ptr)
{
    param_page_t pp;
    if (Ram.NAND_Read_Param_Page(&pp))
    {   flash_nand_t* n = (flash_nand_t*) ptr;
        memset(n,0xff,sizeof(flash_nand_t));
        memcpy(n->manufacturer, &pp.manufacturer, sizeof(flash_nand_t::manufacturer));
        memcpy(n->model, &pp.model, sizeof(flash_nand_t::model));
        n->data_bytes_per_page = pp.data_bytes_per_page;
        n->spare_bytes_per_page = pp.spare_bytes_per_page;
        n->pages_per_block = pp.pages_per_block;
        n->blocks_per_lun = pp.blocks_per_lun;
        n->max_bad_blocks_per_lun = pp.max_bad_blocks_per_lun;
        n->guarenteed_valid_blocks = pp.guarenteed_valid_blocks;
        return true;
    }
    else return false;    
}
  
bool NAND_Check(void)
{
	return Ram.NAND_Reset() && !Ram.NAND_IsNotFormatted() && Ram.NAND_CheckFormat();// && Ram.initWCursor();
}
