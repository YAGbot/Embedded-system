#include "types.h"
#include "ntp.h"
#include "config.h"


ntpformat NTPformat;
uint8 ntp_msg[48];

void ntpclient_init(void)
{
	u8 flag;

   NTPformat.leap 		= 0;/* leap indicator */ 
   NTPformat.version 	= 4;/* version number */
   NTPformat.mode 		= 3;/* mode */
   NTPformat.stratum 	= 0;/* stratum */
   NTPformat.poll 		= 0;/* poll interval */
  // NTPformat.precision 	= 0; /* precision */
//   NTPformat.rootdelay 	= 0; /* root delay */
//   NTPformat.rootdisp 	= 0; /* root dispersion */
   NTPformat.refid 		= 0; /* reference ID */
//   NTPformat.reftime 	= 0; /* reference time */
//   NTPformat.org 		= 0; /* origin timestamp */
//   NTPformat.rec 		= 0; /* receive timestamp */
  // NTPformat.xmt 		= 1; /* transmit timestamp */
   
   flag = (NTPformat.leap<<6)+(NTPformat.version<<3)+NTPformat.mode; //one byte Flag
   
   memcpy(ntp_msg,(void const*)(&flag),1);
}









