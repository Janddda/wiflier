#include "mystuff.h"
#include "mfs.h"
#include "spi_flash.h"
#include "ets_sys.h"

extern SpiFlashChip * flashchip;

//Returns 0 on succses.
//Returns size of file if non-empty
//If positive, populates mfi.
//Returns -1 if can't find file or reached end of file list.
int8_t MFSOpenFile( const char * fname, struct MFSFileInfo * mfi )
{
	flashchip->chip_size = 0x01000000;
	uint32 ptr = MFS_START;
	struct MFSFileEntry e;
	while(1)
	{
		spi_flash_read( ptr, (uint32*)&e, sizeof( e ) );		
		ptr += sizeof(e);
		if( e.name[0] == 0xff || ets_strlen( e.name ) == 0 ) break;

		if( ets_strcmp( e.name, fname ) == 0 )
		{
			mfi->offset = e.start;
			mfi->filelen = e.len;
			flashchip->chip_size = 0x00080000;
			return 0;
		}
	}
	flashchip->chip_size = 0x00080000;
	return -1;
}

int32_t MFSReadSector( uint8_t* data, struct MFSFileInfo * mfi )
{
	 //returns # of bytes left tin file.
	if( !mfi->filelen )
	{
		return 0;
	}

	int toread = mfi->filelen;
	if( toread > MFS_SECTOR ) toread = MFS_SECTOR;

	flashchip->chip_size = 0x01000000;
	spi_flash_read( MFS_START+mfi->offset, (uint32*)data, MFS_SECTOR );
	flashchip->chip_size = 0x00080000;

	mfi->offset += toread;
	mfi->filelen -= toread;
	return mfi->filelen;
}



