/*==============================================================================+
|  Function : flash                                                             |
|  Task     : flash Source File                                                 |
|-------------------------------------------------------------------------------|
|  Compile  :                                                  					|
|  Link     :                                                   				|
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   :                                                               	|
|  Version  : V1.00                                                             |
|  Creation : 2016/09/07                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include    "driver/include/_timer.h"
#include    "driver/include/_flash.h"
#include    "M4_lib/driverlib/flash.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
static  SIZE_T          _Flash_ReadByte     (   BYTE_T *, SIZE_T, OFFSET_T );
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Global Variables                                                    |
+==============================================================================*/
BYTE_T cBuf[1024*16];

/*==============================================================================+
|           Class declaration -                                     			|
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Inner Global Variable                                               |
+------------------------------------------------------------------------------*/
DEV_ENGINE_T            g_Flash_Engine  =
{
	NULL,
    NULL,
	NULL,
	NULL,
    NULL,
    NULL
};
/*------------------------------------------------------------------------------+
|                               Flash_Open                            	        |
+------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|  Function :        _Flash_ReadByte                                         |
+----------------------------------------------------------------------------+
|  Description :     浠嶧lash鎸囧畾鍦板潃澶勮鍙�涓瓧鑺�                               	 					 |
|                                                                            |
|  Parameter:    	@pbyBuf鈥斺�鏁版嵁缂撳啿鍖烘寚閽�                        		                 		 |
|                   @Block鈥斺�鍧楀彿锛屽彇鍊�-31								 		 |
|                   @Offset鈥斺�鎵囧尯鍋忕Щ閲忥紝鍙栧�0-(1024*16-1)                      	 |
|  Return   :       *pbyBuf鈥斺�璇诲彇鐨勬暟鎹�                    	                        		 |
+---------------------------------------------------------------------------*/
SIZE_T 		   _Flash_ReadByte	   (   BYTE_T      *pbyBuf,
                                       SIZE_T      Block,
                                       OFFSET_T    Offset  )
{
	BYTE_T *pcData;

	if (Block > MAX_Block )
		return	0;

	if (Offset > MAX_OFFSET)
		return  0;

	pcData = (BYTE_T *)(Block * 16 * 1024 + (Offset & MAX_OFFSET));

	*pbyBuf = *pcData;

	return	(*pbyBuf);
}
/*---------------------------------------------------------------------------+
|  Function :        _Flash_Read		                                     |
+----------------------------------------------------------------------------+
|  Description :     浠嶧lash鎸囧畾鍦板潃澶勮鍙栧涓瓧鑺�                                 						 |
|                                                                            |
|  Parameter:    	@pbyBuf鈥斺�鏁版嵁缂撳啿鍖烘寚閽�                        		                 		 |
|					@Size鈥斺�璇绘暟鎹暱搴︼紝Block*16*1024 + Offset +Size涓嶈兘瓒呭嚭512K	 |
|                   @Block鈥斺�鍧楀彿锛屽彇鍊�-31							 			 |
|                   @Offset鈥斺�鎵囧尯鍋忕Щ閲忥紝鍙栧�0-(1024*16-1)						 |
|  Return   :       		                     	                         |
+---------------------------------------------------------------------------*/
SIZE_T          Flash_Read         (   BYTE_T      *pbyBuf,
                                       SIZE_T      Size,
									   WORD_T	   Block,
                                       OFFSET_T    Offset  )
{
	SIZE_T i;
	SIZE_T Num_Block;
	SIZE_T SizeLoop = 0;

	if (Block > MAX_Block )
		return	0;

	if (Offset > MAX_OFFSET)
		return  0;

	if (Block * 16 * 1024 + Offset + Size > MAX_Flash )
        return  0;

	Num_Block = (Offset + Size - 1) / (1024 * 16) + 1;

	while( Num_Block > 0 )
	{
		if(Num_Block == 1)
			SizeLoop = Size + Offset;
		if(Num_Block > 1)
			SizeLoop = 1024 * 16;

		for (i = Offset; i < SizeLoop; i++)
		{
			_Flash_ReadByte(pbyBuf++, Block, i);
		}
		Num_Block--;
		Block++;
		Size = Size - (SizeLoop - Offset);
		Offset = 0;
	}

	return	1;
}
/*---------------------------------------------------------------------------+
|  Function :        _Flash_Write		                                     |
+----------------------------------------------------------------------------+
|  Description :     鍚慒lash鎸囧畾鍦板潃澶勫啓鍏ュ涓瓧鑺�                                 						 |
|                                                                            |
|  Parameter:    	@pbyBuf鈥斺�鏁版嵁缂撳啿鍖烘寚閽�                        		                 		 |
|					@Size鈥斺�璇绘暟鎹暱搴︼紝Block*16*1024 + Offset +Size涓嶈兘瓒呭嚭512K	 |
|                   @Block鈥斺�鎵囧尯鍙凤紝鍙栧�0-31								 	 |
|                   @Offset鈥斺�鎵囧尯鍋忕Щ閲忥紝鍙栧�0-(1024*16-1)						 |
|  Return   :       		                     	                         |
+---------------------------------------------------------------------------*/
SIZE_T          Flash_Write        (   BYTE_T      *pbyBuf,
                                       SIZE_T      Size,
									   WORD_T	   Block,
                                       OFFSET_T    Offset  )
{
	SIZE_T i;
	SIZE_T Num_Block;
	SIZE_T SizeLoop = 0;
	uint32_t *pulData = (uint32_t *)(cBuf);

	if (Block > MAX_Block )
		return	0;

	if (Offset > MAX_OFFSET)
		return  0;

	if (Block * 1024 * 16 + Offset + Size > MAX_Flash )
        return  0;

	memset(cBuf, NULL, 1024 * 16);
	Num_Block = (Offset + Size - 1) / (1024 * 16) + 1;

	while( Num_Block > 0 )
	{
		if(Num_Block == 1)
			SizeLoop = Size + Offset;
		if(Num_Block > 1)
			SizeLoop = 1024 * 16;

		Flash_Read(cBuf, 1024 * 16, Block, 0);						//  璇诲彇16KB鎵囧尯鍐呭
		for (i = Offset; i < SizeLoop; i++)             			//  淇敼16KB缂撳啿鍖哄唴瀹�
		{
			cBuf[i] = *pbyBuf++;
		}
		FlashErase(Block * 1024 * 16);      						//  鎿﹂櫎16KB鎵囧尯
		FlashProgram(pulData, Block * 1024 * 16, 1024 * 16);    	//  缂栫▼16KB鎵囧尯
		Num_Block--;
		Block++;
		Size = Size - (SizeLoop - Offset);
		Offset = 0;
	}

	return 1;
}
/*------------------------------------------------------------------------------+
|           end                                                        			|
+------------------------------------------------------------------------------*/
