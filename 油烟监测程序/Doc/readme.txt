/*01 05 00 00 00 00 CD CA		��բ
  01 05 00 00 FF 00 8C 3A		��բ		
	�ȹ�բ�ٺ�բ��ע�Ⲩ���ʽϵ� ָ���ʱ��*/		
	
strcpy(d_str,(char*)str1+10);
		ip_write();
		AT24Cxx_WriteBuf(0,150,IP_Set.Camera1,sizeof(IP_Set));
		printf("%d\r\n",IP_Set.Camera5[3]);
		delay_ms(2000);
		strcpy(d_str,(char*)str+10);
		memset(IP_Set.Camera1,0,sizeof(IP_Set));
		ip_write();
		AT24Cxx_WriteBuf(0,150,IP_Set.Camera1,sizeof(IP_Set));
		
		printf("��24c02\r\n");
		AT24Cxx_ReadBuf(0, 150, IP_Set.Camera1, sizeof(IP_Set));
		printf("%d\r\n",IP_Set.Camera5[3]);
		
		
		/*��λ�����͸�ʽ����һ��:����IP����������
		
		�ڶ�����IP1-IP7*/
		���һ����NVR