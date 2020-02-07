#include "gps.h"
#include "string.h"


//nmea_msg gpsx;
unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx)
{	 		    
	unsigned char *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}

int NMEA_Pow(unsigned char m,unsigned char n)
{
	int result=1;	 
	while(n--)result*=m;    
	return result;
}

int NMEA_Str2num(unsigned char *buf,unsigned char*dx)
{
	unsigned char *p=buf;
	int ires=0,fres=0;
	unsigned char ilen=0,flen=0,i;
	unsigned char mask=0;
	int res;
	while(1) 
	{
		if(*p=='-'){mask|=0X02;p++;}
		if(*p==','||(*p=='*'))break;
		if(*p=='.'){mask|=0X01;p++;}
		else if(*p>'9'||(*p<'0'))	
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	
	for(i=0;i<ilen;i++)	
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	
	*dx=flen;	 		
	for(i=0;i<flen;i++)	
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 


void NMEA_GPRMC_Analysis(nmea_msg *gpsx,unsigned char *buf)
{
	unsigned char *p1,dx;			 
	unsigned char posx;     
	int temp;	   
	float rs;  
	p1=(unsigned char*)strstr((const char *)buf,"GPRMC");
	posx=NMEA_Comma_Pos(p1,1);								
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	
		rs=temp%NMEA_Pow(10,dx+2);						 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
	}
	posx=NMEA_Comma_Pos(p1,4);								
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	
		rs=temp%NMEA_Pow(10,dx+2);						 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;
	}
	posx=NMEA_Comma_Pos(p1,6);								
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
}



void GPS_Analysis(nmea_msg *gpsx,unsigned char *buf)
{
//	NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSVœâÎö
//	NMEA_GPGGA_Analysis(gpsx,buf);	//GPGGAœâÎö
//	NMEA_GPGSA_Analysis(gpsx,buf);	//GPGSAœâÎö
	NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMCœâÎö
//	NMEA_GPVTG_Analysis(gpsx,buf);	//GPVTGœâÎö
}
