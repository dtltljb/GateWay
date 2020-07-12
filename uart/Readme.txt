
(1) . 线程入口参数传递的时效

		如,线程 void *config_channel_WaitSend(void *pvoid)的现场,发送信号程序的场景内容，在发生信号时传递给该线程。
		eg:
			a> .  socket_protocol_thread()线程接收服务端数据内容并处理，是信号量的主要分发者,此上下文的VirtualPort内容，会随信号传递;
			b> .  uart_Change_Channel_pack()函数是其他端口线程的子函数,在处理完后会发送信号到该现场，该上下文内容也会随着信号传递.














































	
	
	