#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>

#include "spidev.h"
#include "FM1702_drv.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	
	abort();
}

static const char *device = "/dev/spidev2.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 400 * 1000;//500000;
static uint16_t delay;

int g_SPI_Fd = 0;

unsigned char UID[5], Temp[4] ,gcBuffer[65] ,gcUID[5];

int WriteFIFO(int addr, int data)
{
	int ret;
	int fd = g_SPI_Fd;
	
	unsigned char  TxBuf[2];

	//bit7:MSB=0,bit6~1:addr,bit0:RFU=0
	TxBuf[0] = (unsigned char)addr;
	//TxBuf[0] &= 0x7E;

	TxBuf[1] = (unsigned char)data;

	
	ret = write(fd, TxBuf, 2);
	if (ret < 0)
		printf("spi:SPI Write error\n");

	//usleep(10);

	return ret;
}


int WriteRawRC(int addr, int data)
{
	int ret;
	int fd = g_SPI_Fd;
	
	unsigned char  TxBuf[2];

	//bit7:MSB=0,bit6~1:addr,bit0:RFU=0
	TxBuf[0] = ((unsigned char)addr << 1)&0x7E;
	//TxBuf[0] &= 0x7E;

	TxBuf[1] = (unsigned char)data;

	
	ret = write(fd, TxBuf, 2);
	if (ret < 0)
		printf("spi:SPI Write error\n");

//	usleep(10);

	return ret;
}

unsigned char ReadRawRC(int addr)
{
	int ret;
	int fd = g_SPI_Fd;
	unsigned char  ReData;
	unsigned char  Address;

	Address  = (unsigned char)addr << 1;
	Address |= (1 << 7);
	Address &= ~(1 << 0);


	ret = write(fd, &Address, 1);
	if (ret < 0)
		printf("spi:SPI Write error\n");

//	usleep(100);

	ret = read(fd, &ReData, 1);
	if (ret < 0)
		printf("spi:SPI Read error\n");

	return ReData;
}

void SetBitMask(unsigned char reg,unsigned char mask)  
{
  	char tmp = 0x0;
	
  	tmp = ReadRawRC(reg) | mask;
	
  	WriteRawRC(reg,tmp | mask);
}

void ClearBitMask(unsigned char reg, unsigned char mask)  
{
	char tmp = 0x0;
	
	tmp = ReadRawRC(reg)&(~mask);
	
	WriteRawRC(reg, tmp);  // clear bit mask
}

unsigned char FM1715_Bus_Sel(void)
{
    unsigned int   i;
    unsigned char  ver;

    WriteRawRC(Page_Sel,0x80);
    for(i=0; i<100; i++)
    {
    	ver = ReadRawRC(Command);
    	printf("Chip ver: 0x%x\n", ver);
        if(ver == 0)
        {
            WriteRawRC(Page_Sel,0);
            return 1;
        }
    }
    return 0;
}


int FM1702_init()
{
	int ret;
	char version = 0 , count = 0;;
#if 0
	//reset
	WriteRawRC(CommandReg, PCD_RESETPHASE);
	usleep(10);
	WriteRawRC(ModeReg, 0x3D);
	WriteRawRC(TReloadRegL, 30);
	WriteRawRC(TReloadRegH, 0);
	WriteRawRC(TModeReg, 0x8D);
	WriteRawRC(TPrescalerReg, 0x3E);

	version = ReadRawRC(VersionReg);
	printf("Chip Version: 0x%x\n", version);
	usleep(50000);
#endif	
	WriteRawRC(Page_Sel, 0x80);
	for(count=100;count > 0;count--)
    {
		version = ReadRawRC(Page_Sel);
		if(version == 0x80)	
			break;		
	}
	printf("Chip Version: 0x%x\n", version);
	if(count == 0)
	{
		printf("FM1702 Init Fail! \n");
	}
	else
	{
		printf("Check FM1702 OK!!! \n");
		
    //	version = FM1715_Bus_Sel();
	//	if(version == 1)
	//	{
	//		printf("FM1702 Sel OK!!! \n");
	//	}
	
		WriteRawRC(TimerClock, 0x0b);
		version = ReadRawRC(TimerClock);
		if(version != 0x0B)printf("REG TimerClock WR error\n");
		
		WriteRawRC(TimerControl, 0x02);
		version = ReadRawRC(TimerControl);
		if(version != 0x02)printf("REG TimerControl WR error\n");	
			
		WriteRawRC(TimerReload, 0x42);
		version = ReadRawRC(TimerReload);
		if(version != 0x42)printf("REG TimerReload WR error\n");
		
		WriteRawRC(InterruptEn, 0x7F);
		version = ReadRawRC(InterruptEn);
		if(version != 0x7F)printf("REG InterruptEn WR error\n");
		printf("InterruptEn : 0x%x\n", version);
				
		WriteRawRC(Int_Req, 0x7F);
		version = ReadRawRC(Int_Req);
		if(version != 0x7F)printf("REG Int_Req WR error\n");
		printf("Int_Req : 0x%x\n", version);		
		
		WriteRawRC(TxControl, 0x5B);
		version = ReadRawRC(TxControl);
		if(version != 0x5B)printf("REG TxControl WR error\n");
		
		WriteRawRC(TypeSH, 0x00);
		version = ReadRawRC(TypeSH);
		if(version != 0x00)printf("REG TypeSH WR error\n");		
	
	}


	return 0;
}

unsigned char Clear_FIFO(void)
{
    unsigned char temp;
    unsigned int  i;

    temp = ReadRawRC(Controls);
    temp |= 0x01;
    WriteRawRC(Controls,temp);

    for(i=0; i<RF_TimeOut; i++)
    {
        if(ReadRawRC(FIFO_Length) == 0)
            return 1;
    }
    return 0;
}

void RF_WriteFIFO(unsigned char Count,unsigned char *Data)
{
    unsigned char  i;
    unsigned char Reg;

    Reg = FIFO << 1;
    Reg = Reg & 0x7e;

//    printf("Check Reg 0x%x \n",Reg);
//    printf("Check Data 0x%x \n",Data[0]);
    
    for(i=0; i<Count; i++)
        WriteFIFO(Reg,Data[i]);

}

unsigned char Read_FIFO(unsigned char *buf)
{
	unsigned char temp,i,Reg;
	unsigned char Address;
	int fd = g_SPI_Fd;
	int ret;
	temp = ReadRawRC(FIFO_Length);
	if(temp  > 64)temp = 64;
	//printf("Check FIFO Length 0x%x \n",temp);
#if 0
	Address  = FIFO << 1;
	Address  &= 0x7E;
	Address  |= 0x80;	
	//printf("Check FIFO Length 0x%x \n",Address);
	ret = write(fd, &Address, 1);
	if (ret < 0)
		printf("spi:SPI Write error\n");

	usleep(100);	

	ret = read(fd, &buf, temp);
	if (ret < 0)
		printf("spi:SPI Read error\n");
	Clear_FIFO();
#endif
	for(i=0;i<temp;i++)
	{
		buf[i] = ReadRawRC(FIFO);
	}	
	
	return 0;
}

unsigned char Command_Send(unsigned char count,unsigned char *buffer,unsigned char cmd,unsigned char timeout)
{
    unsigned int  i;
    unsigned char temp,temp1;

    WriteRawRC(Command,0);
    temp = Clear_FIFO();

    if(temp != 1)
    {
    	return 0;
    	//printf("Clear FIFO Fail!!! \n");
    }

  
 //       temp = ReadRawRC(FIFO_Length);
//    printf("Check FIFO Length 0x%x \n",temp);
    
    RF_WriteFIFO(count,buffer);
 //       temp = ReadRawRC(FIFO_Length);
//    printf("Check FIFO Length 0x%x \n",temp);
    
    WriteRawRC(Command,cmd);
    
 //       temp = ReadRawRC(FIFO_Length);
//    printf("Check FIFO Length 0x%x \n",temp);
    for(i=0; i<0x07ff*timeout; i++)
    {

        temp   = ReadRawRC(Command);

        if(temp == 0)
            return 1;
    }
//    printf("COMMAND Send Timeout!!! \n");
    return 0;
}


char PcdRequest(unsigned char req_code, unsigned char *pTagType)
{
	char status;  
	unsigned int  unLen;
	unsigned char ucComMF522Buf[MAXRLEN] ,temp; 
	unsigned char ucbuf[2];
	
	WriteRawRC(CRCPresetLSB, 0x63);
	WriteRawRC(CWConductance, 0x3F);
	
  	ucComMF522Buf[0] = req_code;
  	
	WriteRawRC(Bit_Frame,0x07);
	WriteRawRC(ChannelRedundancy,0x03);
	temp = ReadRawRC(Controls) & 0xf7;
	WriteRawRC(Controls,temp);
	
//  	status = PcdComMF522(PCD_TRANSCEIVE, ucComMF522Buf,1, ucComMF522Buf, &unLen);
		status = Command_Send(1, ucComMF522Buf,Transceive,1);
	if(status == 0)return FM1715_NOTAGERR;
//	printf("COMMAND Send OK!!! \n");	
	status = Read_FIFO(ucbuf);
//	printf("Card Type 0x%x \n",ucbuf[0]);
//	printf("Card Type 0x%x \n",ucbuf[1]);
  	if ((status == MI_OK) )
  	{    
    		*pTagType     = ucbuf[0];
    		*(pTagType+1) = ucbuf[1];
    		return FM1715_OK;
  	}
  	else
	{
		printf("Return ERR!!! \n");
		status = MI_ERR;
		return FM1715_NOTAGERR;
  	}

  	
}

void Save_UID(unsigned char row,unsigned char col,unsigned char length)
{
    unsigned char  i;
    unsigned char  temp,temp1;

    if ((row == 0x00) && (col == 0x00))
    {
        for(i = 0; i < length; i++)
        {
            gcUID[i] = gcBuffer[i];
        }
    }
    else
    {
        temp = gcBuffer[0];
        temp1 = gcUID[row - 1];
        switch (col)
        {
        case 0:
            temp1 = 0x00;
            row = row + 1;
            break;
        case 1:
            temp = temp & 0xFE;
            temp1 = temp1 & 0x01;
            break;
        case 2:
            temp = temp & 0xFC;
            temp1 = temp1 & 0x03;
            break;
        case 3:
            temp = temp & 0xF8;
            temp1 = temp1 & 0x07;
            break;
        case 4:
            temp = temp & 0xF0;
            temp1 = temp1 & 0x0F;
            break;
        case 5:
            temp = temp & 0xE0;
            temp1 = temp1 & 0x1F;
            break;
        case 6:
            temp = temp & 0xC0;
            temp1 = temp1 & 0x3F;
            break;
        case 7:
            temp = temp & 0x80;
            temp1 = temp1 & 0x7F;
            break;
        default:
            break;
        }
        gcBuffer[0] = temp;
        gcUID[row - 1] = temp1 | temp;
        for(i = 1; i < length; i++)
        {
            gcUID[row - 1 + i] = gcBuffer[i];
        }
    }
}
unsigned char Check_UID(void)
{
    unsigned char  i,temp;

    temp = 0x00;
    for(i = 0; i < 5; i++)
    {
        temp = temp ^ gcUID[i];
    }
    if(temp == 0)
    {
        return TRUE;
    }
    return FALSE;
}
void Set_BitFraming(unsigned char row,unsigned char col)
{
    switch (row)
    {
    case 0:
        gcBuffer[1] = 0x20;
        break;
    case 1:
        gcBuffer[1] = 0x30;
        break;
    case 2:
        gcBuffer[1] = 0x40;
        break;
    case 3:
        gcBuffer[1] = 0x50;
        break;
    case 4:
        gcBuffer[1] = 0x60;
        break;
    default:
        break;
    }

    switch(col)
    {
    case 0:
        WriteRawRC(Bit_Frame,0x00);
        break;
    case 1:
        WriteRawRC(Bit_Frame,0x11);
        gcBuffer[1] = (gcBuffer[1] | 0x01);
        break;
    case 2:
        WriteRawRC(Bit_Frame,0x22);
        gcBuffer[1] = (gcBuffer[1] | 0x02);
        break;
    case 3:
        WriteRawRC(Bit_Frame,0x33);
        gcBuffer[1] = (gcBuffer[1] | 0x03);
        break;
    case 4:
        WriteRawRC(Bit_Frame,0x44);
        gcBuffer[1] = (gcBuffer[1] | 0x04);
        break;
    case 5:
        WriteRawRC(Bit_Frame,0x55);
        gcBuffer[1] = (gcBuffer[1] | 0x05);
        break;
    case 6:
        WriteRawRC(Bit_Frame,0x66);
        gcBuffer[1] = (gcBuffer[1] | 0x06);
        break;
    case 7:
        WriteRawRC(Bit_Frame,0x77);
        gcBuffer[1] = (gcBuffer[1] | 0x07);
        break;
    default:
        break;
    }
}

unsigned char Select_Card(void)
{
    unsigned char temp,i;

    WriteRawRC(CRCPresetLSB,0x63);
    WriteRawRC(CWConductance,0x3f);
    gcBuffer[0] = RF_CMD_SELECT;
    gcBuffer[1] = 0x70;
    for(i = 0; i < 5; i++)
    {
        gcBuffer[i+2]=gcUID[i];
    }
    WriteRawRC(ChannelRedundancy,0x0f);
    temp=Command_Send(7,gcBuffer,Transceive,2);

    if(temp==FALSE)
    {
        return(FM1715_NOTAGERR);
    }
    else
    {
        temp = ReadRawRC(ErrorFlag);
        if ((temp & 0x02)==0x02)
            return(FM1715_PARITYERR);
        if((temp & 0x04)==0x04)
            return(FM1715_FRAMINGERR);
        if ((temp & 0x08)==0x08)
            return(FM1715_CRCERR);

        temp = ReadRawRC(FIFO_Length);
        if (temp!=1)
            return(FM1715_BYTECOUNTERR);
        Read_FIFO(gcBuffer);
        temp = *gcBuffer;
//		if ((temp==0x18) ||(temp==0x08) || (temp==0x88) ||(temp==0x28) || (temp==0x53)) //ÅÐ¶ÏÓ¦´ðÐÅºÅÊÇ·ñÕýÈ·
        return(FM1715_OK);
//		else
//			return(FM1715_SELERR);
    }
}


unsigned char PcdAnticoll(unsigned char *uid)
{

    unsigned char temp;
    unsigned char  i;
    unsigned char row,col;
    unsigned char pre_row;
   

    row = 0;
    col = 0;
    pre_row = 0;

    WriteRawRC(CRCPresetLSB,0x63);
    WriteRawRC(CWConductance,0x3f);
    WriteRawRC(ModConductance,0x3f);
    gcBuffer[0] = RF_CMD_ANTICOL;
    gcBuffer[1] = 0x20;
    WriteRawRC(ChannelRedundancy,0x03);
    temp=Command_Send(2, gcBuffer, Transceive,1);

    while(1)
    {
        if(temp==FALSE)
        {
            return(FM1715_NOTAGERR);
        }
        temp = ReadRawRC(ErrorFlag);
        temp = ReadRawRC(FIFO_Length);
        if (temp==0)
        {
            return FM1715_BYTECOUNTERR;
        }

        Read_FIFO(gcBuffer);
        Save_UID(row, col, temp);					//½«ÊÕµ½µÄUID·ÅÈëUIDÊý×éÖÐ
        temp = ReadRawRC(ErrorFlag);				//ÅÐ¶Ï½Ó…§Êý¾ÝÊÇ·ñ³ö´í
        temp = temp & 0x01;
        if(temp == 0x00)
        {
            temp = Check_UID();					//Ð£ÑéÊÕµ½µÄUID
            if(temp == FALSE)
            {
                return(FM1715_SERNRERR);
            }
//            memcpy(UID,gcUID,5);
			UID[0] = gcUID[3];UID[1] = gcUID[2];UID[2] = gcUID[1];UID[3] = gcUID[0];
            return(FM1715_OK);
        }
        else
        {
            temp = ReadRawRC(CollPos);
            row = temp / 8;
            col = temp % 8;
            gcBuffer[0] = RF_CMD_ANTICOL;
            Set_BitFraming(row + pre_row, col);
            pre_row = pre_row + row;
            for(i = 0; i < pre_row + 1; i++)
            {
                gcBuffer[i + 2] = gcUID[i];
            }
            if(col != 0x00)
            {
                row = pre_row + 1;
            }
            else
            {
                row = pre_row;
            }
            temp = Command_Send(row + 2, gcBuffer, Transceive,1);
        }
    }

}

unsigned char MIF_Halt(void)
{
    unsigned char  temp,i;

    WriteRawRC(ChannelRedundancy,0x07);
    gcBuffer[0] = RF_CMD_HALT;					//cmd
    gcBuffer[1] = 0x00;

    temp = Command_Send(2, gcBuffer, Transmit,4);
    if (temp == 1)									//==true
    {
        for (i = 0; i < 0x50; i++)
        {
            //_nop_();
            
        }
        return FM1715_OK;
    }
    else
    {
        temp = ReadRawRC(ErrorFlag);
        if ((temp & 0x02)==0x02)
        {
            return(FM1715_PARITYERR);
        }
        if ((temp & 0x04)==0x04)
        {
            return(FM1715_FRAMINGERR);
        }
        return(FM1715_NOTAGERR);
    }
}

void Auto_Reader(void)
{
	int i = 0;
	unsigned char flag;
	unsigned long num = 0;
	
  	while(1)
	{
		flag = PcdRequest(0x26,Temp);
		if(flag != MI_OK)
		{
			flag = PcdRequest(0x26,Temp);
		//	if(flag != MI_OK)
		//		break;
		}
		printf("Reader flag 0x%x \n",flag);
		if(flag == MI_OK)
		{
			if(Temp[0]==0x04 && Temp[1]==0x00)  
				printf("MFOne-S50\n");
			else if(Temp[0]==0x02 && Temp[1]==0x00)
				printf("MFOne-S70\n");
			else if(Temp[0]==0x44 && Temp[1]==0x00)
				printf("MF-UltraLight\n");
			else if(Temp[0]==0x08 && Temp[1]==0x00)
				printf("MF-Pro\n");
			else if(Temp[0]==0x44 && Temp[1]==0x03)
				printf("MF Desire\n");
			else
				printf("Unknown\n");
			
			if(PcdAnticoll(UID) == MI_OK)
			{ 
				if(Select_Card() ==  MI_OK)
				{
					printf("Seletc Card OK !!!\n"); 
								
					printf("Card Id is(%d):", num++);
					for(i=0; i<4; i++)
						printf("%X", UID[i]);
					printf("\n");
					MIF_Halt();
				}
	//			PcdRequest(0x52,Temp);//clear

			}
			else
			{
				printf("no serial num read\n");
			}
	
		}
		else
		{
			printf("No Card!\n");
		}

	//	usleep(300000);
	} 
}


void PcdAntennaOn()
{
	
}

void HandleConfigMenu(unsigned char inputvalue)
{
#if 0
    switch(toupper(inputvalue)) 
    {
    case 'A':
              Auto_Reader();
              break;
    case 'F':
              Find_Card();
              break;
    default:
              DisplayConfigMenu();
    }
#endif

	//Find_Card();

	Auto_Reader();
}


static void print_usage(const char *prog)
{
	printf("Usage: %s [-DsbdlHOLC3]\n", prog);
	puts("  -D --device   device to use (default /dev/spidev1.1)\n"
	     "  -s --speed    max speed (Hz)\n"
	     "  -d --delay    delay (usec)\n"
	     "  -b --bpw      bits per word \n"
	     "  -l --loop     loopback\n"
	     "  -H --cpha     clock phase\n"
	     "  -O --cpol     clock polarity\n"
	     "  -L --lsb      least significant bit first\n"
	     "  -C --cs-high  chip select active high\n"
	     "  -3 --3wire    SI/SO signals shared\n");
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	while (1) {
		static const struct option lopts[] = {
			{ "device",  1, 0, 'D' },
			{ "speed",   1, 0, 's' },
			{ "delay",   1, 0, 'd' },
			{ "bpw",     1, 0, 'b' },
			{ "loop",    0, 0, 'l' },
			{ "cpha",    0, 0, 'H' },
			{ "cpol",    0, 0, 'O' },
			{ "lsb",     0, 0, 'L' },
			{ "cs-high", 0, 0, 'C' },
			{ "3wire",   0, 0, '3' },
			{ "no-cs",   0, 0, 'N' },
			{ "ready",   0, 0, 'R' },
			{ NULL, 0, 0, 0 },
		};
		int c;

		c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NR", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'b':
			bits = atoi(optarg);
			break;
		case 'l':
			mode |= SPI_LOOP;
			break;
		case 'H':
			mode |= SPI_CPHA;
			break;
		case 'O':
			mode |= SPI_CPOL;
			break;
		case 'L':
			mode |= SPI_LSB_FIRST;
			break;
		case 'C':
			mode |= SPI_CS_HIGH;
			break;
		case '3':
			mode |= SPI_3WIRE;
			break;
		case 'N':
			mode |= SPI_NO_CS;
			break;
		case 'R':
			mode |= SPI_READY;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	unsigned char i;
	
	int ret = 0;
	int fd;

	parse_opts(argc, argv);

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	g_SPI_Fd = fd;

#if 0
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");

	printf("spi mode: %d\n", mode);
	printf("bits per word: %d\n", bits);
	printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
#endif

	FM1702_init();

	PcdAntennaOn();

	HandleConfigMenu(i);
#if 0
	close(fd);
#endif
	return ret;
}


