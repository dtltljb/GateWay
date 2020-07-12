/*=========================================================================
o¡¥¨ºy??3?¡êoEncrptData
?¡Â¨°a1|?¨¹¡êo??¨ºy?Y¨¢¡Â??DD3DES?¨®?¨¹
¨¨??¨²2?¨ºy¡êo
          PlainTextBuf:?¡Â???o3???????
          CipherTextBuf:?¨¹???o3???????
					EnLen:D¨¨¨°a?¨®?¨¹¦Ì?¨ºy?Y3¡è?¨¨
3??¨²2?¨ºy¡êoNO
¨¦¨´?¡Â¡êo¨¨¡¤¡À¡êPlainTextBuf?T?¡Â??¨ºy?Y¦Ì?2?¡¤?¨¨?2??a?¡ã0?¡À¡ê??¨®?¨¹¨®?021??¡Á????¡ê???o3???¨º1¨®??¡ã¡À?D?????0';
=================================================================================*/
void EncrptData(bjs_uchar *PlainTextBuf,bjs_uchar *CipherTextBuf,bjs_uint EnLen)
{
	bjs_uint i;
	bjs_uint j=0;
	bjs_uchar data1[8];
	bjs_uchar TCipherBuf[8];
	bjs_uchar EnPacket = 0;
	for(i=0;i<8;i++)TCipherBuf[i] = 0x00;

	EnPacket = (EnLen/8);                            //¨¨???¡ã¨¹¨ºy?Y 
  if((EnLen%8)!=0)EnPacket++;
  for(i=0;i<EnPacket;i++)
  {
		 bjs_memcpy(TCipherBuf,PlainTextBuf+j,8);    //¨¨?8¡Á??¨²?¡Â??
		 deskey(key, EN0);
     des(TCipherBuf, data1);                    //key1 ?¨®?¨¹;  
  	 deskey(key+8, DE1);
     des(data1,TCipherBuf);                     //key2 ?a?¨¹
		 deskey(key, EN0);
     des(TCipherBuf, data1);                    //key1 ?¨®?¨¹;
		 bjs_memcpy(CipherTextBuf+j,data1,8);   // ¡À¡ê¡ä??¨¹??
		 j=j+8;
	}	
}
/*=========================================================================
o¡¥¨ºy??3?¡êoDescrptData
?¡Â¨°a1|?¨¹¡êo??¨ºy?Y¨¢¡Â??DD3DES?a?¨¹
¨¨??¨²2?¨ºy¡êo
          PlainTextBuf:?¡Â???o3???????
          CipherTextBuf:?¨¹???o3???????
					EnLen:D¨¨¨°a?¨®?¨¹¦Ì?¨ºy?Y3¡è?¨¨
3??¨²2?¨ºy¡êoNO
¨¦¨´?¡Â¡êo¨¨¡¤¡À¡êPlainTextBuf?T?¡Â??¨ºy?Y¦Ì?2?¡¤?¨¨?2??a?¡ã0?¡À¡ê??¨®?¨¹¨®?021??¡Á????¡ê???o3???¨º1¨®??¡ã¡À?D?????0';
=================================================================================*/
void DescrptData(bjs_uchar *PlainTextBuf,bjs_uchar *CipherTextBuf,bjs_uint EnLen)
{
	bjs_uint i;
	bjs_uint j=0;
	bjs_uchar data1[8];
	bjs_uchar TCipherBuf[8];
	bjs_uchar EnPacket = 0;
	for(i=0;i<8;i++)
		TCipherBuf[i] = 0x00;
	
	EnPacket = (EnLen/8); 
   if((EnLen%8)!=0)
   	EnPacket++;	
  for(i=0;i<EnPacket;i++)
  {
		 bjs_memcpy(TCipherBuf,CipherTextBuf+j,8);    //¨¨?8¡Á??¨²?¡Â??
		 
		 deskey(key, DE1);
     des(TCipherBuf, data1);                    //key1 ?a?¨¹  
     
  	 deskey(key+8, EN0);
     des(data1,TCipherBuf);                     //key2 ?¨®?¨¹
     
		 deskey(key, DE1);
     des(TCipherBuf, data1);                    //key1 ?a?¨¹;
     
		 bjs_memcpy(PlainTextBuf+j,data1,8);   			// ¡À¡ê¡ä??¡Â??
		 j=j+8;
	}
}




exsample


//¨ºy?Y3¡è?¨¨?a9??¡Á??¨²¡ê?21??7??¡Á??¨²0
	bjs_memset(g_Send_Buf,100);
	g_Receive_Buf[0] = 0x11;
	g_Receive_Buf[1] = 0x22;
	g_Receive_Buf[2] = 0x33;
	g_Receive_Buf[3] = 0x44;
	g_Receive_Buf[4] = 0x55;
	g_Receive_Buf[5] = 0x66;
	g_Receive_Buf[6] = 0x77;
	g_Receive_Buf[7] = 0x88;
	g_Receive_Buf[8] = 0x99;
	g_Receive_Buf[9] = 0x00;
	g_Receive_Buf[10] = 0x00;
	g_Receive_Buf[11] = 0x00;
	g_Receive_Buf[12] = 0x00;
	g_Receive_Buf[13] = 0x00;
	g_Receive_Buf[14] = 0x00;
	g_Receive_Buf[15] = 0x00;

	EncrptData(g_Receive_Buf,g_Send_Buf,16);   //ÃÜÎÄ 5A 1A 68 88 BB 02 1B D4 36 EC A4 67 1E B8 3A BE 
	
	bjs_memset(g_Receive_Buf,16);
	DescrptData(g_Receive_Buf,g_Send_Buf,16); //½âÃÜºó£º11 22 33 44 55 66 77 88 99 00 00 00 00 00 00 00
	