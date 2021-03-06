#define MAXRLEN        18
#define MIN_STRENGTH  228

//命令模式
#define  	KEYA_MODE				0			//keyaÈÏÖ¤
#define  	KEYB_MODE				1			//keybÈÏÖ¤


#define 	Transceive      			0x1E   		
#define 	Transmit					0x1a	    
#define 	ReadE2					0x03			
#define 	WriteE2					0x01			
#define 	Authent1				0x0c			
#define 	Authent2				0x14			//ÑéÖ¤ÃüÁîÈÏÖ¤¹ý³ÌµÚ2²½
#define 	LoadKeyE2				0x0b			//½«ÃÜÔ¿´ÓEEPROM¸´ÖÆµ½KEY»º´æ
#define 	LoadKey					0x19			//½«ÃÜÔ¿´ÓFIFO»º´æ¸´ÖÆµ½KEY»º´æ

#define 	RF_TimeOut				0x10ffL	//0xffff	//·¢ËÍÃüÁîÑÓÊ±Ê±¼ä	

//ÉäÆµ¿¨Í¨ÐÅÃüÁîÂë¶¨Òå
#define	RF_CMD_REQUEST_STD 			0x26
#define	RF_CMD_REQUEST_ALL 			0x52
#define	RF_CMD_ANTICOL 				0x93
#define	RF_CMD_SELECT 				0x93
#define	RF_CMD_AUTH_LA 				0x60
#define	RF_CMD_AUTH_LB 				0x61
#define	RF_CMD_READ					0x30
#define	RF_CMD_WRITE				0xa0
#define	RF_CMD_INC					0xc1
#define	RF_CMD_DEC					0xc0
#define	RF_CMD_RESTORE				0xc2
#define	RF_CMD_TRANSFER				0xb0
#define	RF_CMD_HALT					0x50

#define RF_CMD_CPURATS      0xe0//T=CL µÄ¸´Î»ÃüÁî
#define RF_CMD_CPUDESELECT  0xC4//T=CL µÄÊÍ·ÅÑ¡ÔñÃüÁî
#define RF_CMD_CPUCMD       0xe1//T=CL µÄÆäËûÃüÁî

//Status Values
#define	ALL							0x01
#define	KEYB						0x04
#define	KEYA						0x00
#define	_AB							0x40
#define	CRC_A						1
#define 	CRC_B					2

#define	CRC_OK						0
#define 	CRC_ERR					1

#define	BCC_OK						0
#define 	BCC_ERR					1


#define MI_OK                          0 
#define MI_ERR                      0xFE 

#define	 FALSE                    0
#define		TRUE				1
//******************* º¯Êý´íÎó´úÂë¶¨Òå********************
#define	FM1715_OK					0	    		//ÕýÈ·
#define	FM1715_NOTAGERR				1			//ÎÞ¿¨
#define	FM1715_CRCERR				2		   	//¿¨Æ¬CRCÐ£Ñé´íÎó
#define	FM1715_EMPTY				3	    		//ÊýÖµÒç³ö´íÎó
#define	FM1715_AUTHERR				4			//ÑéÖ¤²»³É¹¦
#define	FM1715_PARITYERR			5		    	//¿¨Æ¬ÆæÅ¼Ð£Ñé´íÎó
#define	FM1715_CODEERR				6			//Í¨Ñ¶´íÎó(BCCÐ£Ñé´í)
#define	FM1715_SERNRERR				8			//¿¨Æ¬ÐòÁÐºÅ´íÎó(anti-collision ´íÎó)
#define	FM1715_SELECTERR			9		   	//¿¨Æ¬Êý¾Ý³¤¶È×Ö½Ú´íÎó(SELECT´íÎó)
#define	FM1715_NOTAUTHERR			10			//¿¨Æ¬Ã»ÓÐÍ¨¹ýÑéÖ¤
#define	FM1715_BITCOUNTERR			11			//´Ó¿¨Æ¬½ÓÊÕµ½µÄÎ»Êý´íÎó
#define	FM1715_BYTECOUNTERR			12			//´Ó¿¨Æ¬½ÓÊÕµ½µÄ×Ö½ÚÊý´íÎó£¨½ö¶Áº¯ÊýÓÐÐ§£©
#define	FM1715_RESTERR				13			//µ÷ÓÃrestoreº¯Êý³ö´í
#define	FM1715_TRANSERR				14			//µ÷ÓÃtransferº¯Êý³ö´í
#define	FM1715_WRITEERR				15			//µ÷ÓÃwriteº¯Êý³ö´í
#define	FM1715_INCRERR				16			//µ÷ÓÃincrementº¯Êý³ö´í
#define	FM1715_DECRERR				17			//µ÷ÓÃdecrementº¯Êý³ö´í
#define	FM1715_READERR				18			//µ÷ÓÃreadº¯Êý³ö´í
#define FM1715_LOADKEYERR			19			//µ÷ÓÃLOADKEYº¯Êý³ö´í
#define FM1715_FRAMINGERR			20      		//FM1715Ö¡´íÎó
#define FM1715_REQERR				21      		//µ÷ÓÃreqº¯Êý³ö´í
#define	FM1715_SELERR				22			//µ÷ÓÃselº¯Êý³ö´í
#define	FM1715_ANTICOLLERR			23      		//µ÷ÓÃanticollº¯Êý³ö´í
#define	FM1715_INTIVALERR			24			//µ÷ÓÃ³õÊ¼»¯º¯Êý³ö´í
#define	FM1715_READVALERR			25			//µ÷ÓÃ¸ß¼¶¶Á¿éÖµº¯Êý³ö´í
#define	FM1715_DESELECTERR			26
#define	FM1715_CMD_ERR				42			//ÃüÁî´íÎó

//******************* FM1715¼Ä´æÆ÷µØÖ·¶¨Òå********************
#define	Page_Sel				0x00			//Ò³Ð´¼Ä´æÆ÷
#define	Command				0x01			//ÃüÁî¼Ä´æÆ÷
#define	FIFO					0x02			//64×Ö½ÚFIFO»º³åµÄÊäÈëÊä³ö¼Ä´æÆ÷
#define	PrimaryStatus			0x03			//·¢ÉäÆ÷£¬½ÓÊÕÆ÷¼°FIFOµÄ×´Ì¬¼Ä´æÆ÷1
#define	FIFO_Length				0x04			//µ±Ç°FIFOÄÚ×Ö½ÚÊý¼Ä´æÆ÷
#define	SecondaryStatus 			0x05			//¸÷ÖÖ×´Ì¬¼Ä´æÆ÷2
#define 	InterruptEn				0x06			//ÖÐ¶ÏÊ¹ÄÜ/½ûÖ¹¼Ä´æÆ÷
#define	Int_Req					0x07			//ÖÐ¶ÏÇëÇó±êÊ¶¼Ä´æÆ÷

#define	Controls					0x09			//¿ØÖÆ¼Ä´æÆ÷
#define	ErrorFlag				0x0A			//´íÎó×´Ì¬¼Ä´æÆ÷
#define 	CollPos					0x0B			//³åÍ»¼ì²â¼Ä´æÆ÷
#define 	TimerValue				0x0C			//¶¨Ê±Æ÷µ±Ç°Öµ
#define	Bit_Frame				0x0F			//Î»Ö¡µ÷Õû¼Ä´æÆ÷

#define	TxControl				0x11			//·¢ËÍ¿ØÖÆ¼Ä´æÆ÷
#define	CWConductance			0x12			//Ñ¡Ôñ·¢Éä½ÅTX1ºÍTX2·¢ÉäÌìÏßµÄ×è¿¹
#define	ModConductance			0x13			//¶¨ÒåÊä³öÇý¶¯×è¿¹
#define	CoderControl				0x14			//¶¨Òå±àÂëÄ£Ê½ºÍÊ±ÖÓÆµÂÊ
#define	TypeBFraming			0x17			//¶¨ÒåISO14443BÖ¡¸ñÊ½

#define	DecoderControl			0x1A			//½âÂë¿ØÖÆ¼Ä´æÆ÷
#define	Rxcontrol2				0x1E			//½âÂë¿ØÖÆ¼°Ñ¡Ôñ½ÓÊÕÔ´

#define 	RxWait					0x21			//Ñ¡Ôñ·¢ÉäºÍ½ÓÊÕÖ®¼äµÄÊ±¼ä¼ä¸ô
#define	ChannelRedundancy		0x22			//RFÍ¨µÀ¼ìÑéÄ£Ê½ÉèÖÃ¼Ä´æÆ÷
#define 	CRCPresetLSB			0x23			//CRCÔ¤ÖÃ¼Ä´æÆ÷µÍ8Î»
#define	CRCPresetMSB			0x24          		//CRCÔ¤ÖÃ¼Ä´æÆ÷¸ß8Î»
#define 	MFOUTSelect				0x26			//mf OUT Ñ¡ÔñÅäÖÃ¼Ä´æÆ÷	

#define 	TimerClock				0x2A			//¶¨Ê±Æ÷ÖÜÆÚÉèÖÃ¼Ä´æÆ÷
#define 	TimerControl				0x2B			//¶¨Ê±Æ÷¿ØÖÆ¼Ä´æÆ÷
#define 	TimerReload				0x2C			//¶¨Ê±Æ÷³õÖµ¼Ä´æÆ÷

#define 	TypeSH					0x31			//ÉÏº£±ê×¼Ñ¡Ôñ¼Ä´æÆ÷
#define 	TestDigiSelect  			0x3D			//²âÊÔ¹Ü½ÅÅäÖÃ¼Ä´æÆ÷
