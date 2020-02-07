/*01 05 00 00 00 00 CD CA		关闸
  01 05 00 00 FF 00 8C 3A		合闸		
	先关闸再合闸，注意波特率较低 指令间时间*/		
	
strcpy(d_str,(char*)str1+10);
		ip_write();
		AT24Cxx_WriteBuf(0,150,IP_Set.Camera1,sizeof(IP_Set));
		printf("%d\r\n",IP_Set.Camera5[3]);
		delay_ms(2000);
		strcpy(d_str,(char*)str+10);
		memset(IP_Set.Camera1,0,sizeof(IP_Set));
		ip_write();
		AT24Cxx_WriteBuf(0,150,IP_Set.Camera1,sizeof(IP_Set));
		
		printf("读24c02\r\n");
		AT24Cxx_ReadBuf(0, 150, IP_Set.Camera1, sizeof(IP_Set));
		printf("%d\r\n",IP_Set.Camera5[3]);
		
		
		/*上位机发送格式，第一条:本机IP、处理网关
		
		第二条：IP1-IP7*/
		最后一条：NVR