日期：2019-4-1
	1). 取消了心跳判断
	2). 增加密文通信
	3). 增加注册数据写入文件的功能。
	
	
日期：2019-6-4
	1). 修改心跳功能,管理功能;修改给我基础定时器1s，增加时间判断功能。
	2). 增加上报历史log的打包子函数，增加文件夹搜索文件、链表功能。
	3). 
	
	
		
	
	
	
	
	
	
	
	
	
	
	
	
	
debug record：

    fix_head->State					= 0x8001;  //load  ,			okay				补充协议添加用户控制列表
    
    fix_head->State					= 0x8002;  //config,lora属性,	okay
    
    fix_head->State					= 0x8003;  //authorize   		okay			
    
    fix_head->State					= 0x8004;  //remote open		okay
    
    fix_head->State					= 0x8005;  //emergency_open		okay
    
    fix_head->State					= 0x8006;  //set lora   para  okay
    fix_head->State					= 0x8007;  //get lora   para	okay
    
    fix_head->State					= 0x8008;  //read locker  para 	读锁体属性
    
(1) report code = 8005,workType	=	0x10



(2)  heart beat
send data:
5a a5 12 34 56 78 00 00 00 00 00 00 00 01 20 19 03 13 12 35 02 00 00 3e 0a 00 10 0a 1a 08 55 54 43 2d 31 31 00 65 20 08 65 6e 00 63 68 2d 62 6a 28 01 32 06 67 45 8b 6b 00 00 3a 10 63 68 2d 62 6a 73 2d 32 30 31 39 2d 31 2d 33 31 42 08 67 45 8b 6b 00 00 00 00 00 00 0e e0 c0 5c 71 71 
../../socket/Packet.c,line:99 ,val=2,crc32=71715CC0,*r_len=94


 fc 5a a5 12 34 56 78 00 00 00 ff 00 00 00 01 20 19 03 13 12 35 03 03 
 35 00 = 53 byte
0a00100a1a0b3138343538353832363236220d3137322e31372e3134312e33302a093132372e302e302e313208bb563412785634aa00000000000ee06f2b4edb ,total=64 bytes
00 fc 
../../socket/VirtualPortMonitor.c,line:238, receive length=91

uart packet data out_len=89
07 f2 fc 01 50 01 01 3b 00 00 07 f2 02 01 20 19 01 01 00 00 20 19 02 29 00 00 20 19 01 01 00 00 20 19 02 29 00 00 20 19 01 01 00 00 20 19 02 29 00 00 20 19 01 01 00 00 20 19 02 29 00 00 20 19 01 01 00 00 20 19 02 29 00 00 20 19 01 01 00 00 20 19 02 29 00 00 08 20 c0 

 07 f2 fc 01 20 01 01 3c 00 00 07 f2 01 类型错误 20 00 00 00 03 19 19 00 07 04 e0 0a 16 03 00 50 01 01 40 07 f2 38 00 00 90 00 ac 7e
../../uart/UartUnpacket.c,line:101,ruart receive data ok

uart packet data out_len=33
07 f2 fc 01 16 01 01 3b 00 00 07 f2 03 01 07 04 e0 0a 16 03 00 50 01 01 40 07 f2 18 01 00 00 2a 3e 

 07 f2 fc 01 20 01 01 3c 00 00 07 f2 01类型错误  20 00 00 00 03 19 25 00 07 04 e0 0a 16 03 00 50 01 01 40 07 f2 18 00 00 90 00 52 aa
../../uart/UartUnpacket.c,line:101,ruart receive data ok

(3) authorize
finger van  add  error



