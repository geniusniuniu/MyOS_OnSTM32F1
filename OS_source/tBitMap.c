#include "tLib.h"

tBitmap taskPrioBM;


void tBitmapInit (tBitmap * bitmap)
{
	bitmap->bitmap = 0x00000000;
}

uint32_t tBitmapPosCount (void)
{
	return (uint32_t)32;
}

void tBitmapSet (tBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap |= 1 << pos;
}

void tBitmapClear (tBitmap * bitmap, uint32_t pos)	
{
	bitmap->bitmap &= ~(1 << pos);
}

uint32_t tBitmapGetFirstSet (tBitmap * bitmap)
{
	if(bitmap->bitmap & 0xFF)
	{
		return quickFindTable[bitmap->bitmap & 0xFF];
	}
	else if(bitmap->bitmap & 0xFF00)
	{
		return quickFindTable[(bitmap->bitmap >> 8) & 0xFF] + 8;
	}
	else if(bitmap->bitmap & 0xFF0000)
	{
		return quickFindTable[(bitmap->bitmap >> 16) & 0xFF] + 16;
	}
	else if(bitmap->bitmap & 0xFF000000)
	{
		return quickFindTable[(bitmap->bitmap >> 24) & 0xFF] + 24;
	}
	else
		return tBitmapPosCount();
}

