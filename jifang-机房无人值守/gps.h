#ifndef GPS_H_
#define GPS_H_

 typedef struct  
{										    
 	unsigned char num;		
	unsigned char eledeg;	
	short azideg;	
	unsigned char sn;			   
}nmea_slmsg; 
typedef struct  
{										    
 	short year;	
	unsigned char month;	
	unsigned char date;	
	unsigned char hour; 	
	unsigned char min; 	
	unsigned char sec; 	
}nmea_utc_time;   

 typedef struct  
{										    
 	unsigned char svnum;					
	nmea_slmsg slmsg[12];		
	nmea_utc_time utc;			
	int latitude;				
	unsigned char nshemi;							  
	int longitude;			   
	unsigned char ewhemi;					
	unsigned char gpssta;								  
 	unsigned char posslnum;				
 	unsigned char possl[12];				
	unsigned char fixmode;					
	short pdop;					
	short hdop;					
	short vdop;					
	int altitude;			 		 
	short speed;					
}nmea_msg; 
unsigned char NMEA_Comma_Pos(unsigned char *buf,unsigned char cx);
int NMEA_Pow(unsigned char m,unsigned char n);
int NMEA_Str2num(unsigned char *buf,unsigned char*dx);
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,unsigned char *buf);
//void Gps_Msg_Show(void);
//void NMEA_GPVTG_Analysis(nmea_msg *gpsx,unsigned char *buf);
//void NMEA_GPGSA_Analysis(nmea_msg *gpsx,unsigned char *buf);
//void NMEA_GPGGA_Analysis(nmea_msg *gpsx,unsigned char *buf);
//void NMEA_GPGSV_Analysis(nmea_msg *gpsx,unsigned char *buf);
void GPS_Analysis(nmea_msg *gpsx,unsigned char *buf);
#endif
