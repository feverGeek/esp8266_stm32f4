#include "common.h"

//ATK-ESP8266 WIFI AP测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 atk_8266_wifiap_test(void)
{
	u8 netpro=0;	 
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	 
	u8 *p;
	u16 t=999;		 
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	 
	p=mymalloc(SRAMIN,32);		 
	
PRESTA:
	netpro=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[1]);	 
	if(netpro&0X02)   //UDP
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		Show_Str_Mid(0,30,"ATK-ESP WIFI-AP test",16,240); 
		Show_Str(30,50,200,16,"config ATK-ESP module waiting...",12,0);
		if(atk_8266_ip_set("WIFI-AP remote UDP IP :",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入
		sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);     
		atk_8266_send_cmd("AT+CIPMUX=0","OK",100);   
		LCD_Clear(WHITE);
		while(atk_8266_send_cmd(p,"OK",500));
			
	}
	else     //TCP
	{
		if(netpro&0X01)      
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-AP test",16,240); 
			Show_Str(30,50,200,16,"config ATK-ESP module waiting...",12,0);
			if(atk_8266_ip_set("WIFI-AP remote IP:",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA; 
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);    
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);     
			while(atk_8266_send_cmd(p,"OK",200))
			{
				LCD_Clear(WHITE);
				POINT_COLOR=RED;
				Show_Str_Mid(0,40,"WK_UP:exit",16,240);
				Show_Str(30,80,200,12,"ATK-ESP TCP connect failed",12,0);  
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);       	
		}
		else					//TCP Server
		{
				LCD_Clear(WHITE);
				POINT_COLOR=RED;
				Show_Str_Mid(0,30,"ATK-ESP WIFI-AP test",16,240); 
				Show_Str(30,50,200,16,"config ATK-ESP module waiting...",12,0);
				atk_8266_send_cmd("AT+CIPMUX=1","OK",20);    
				sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);
				atk_8266_send_cmd(p,"OK",20);      
		}
	}
	LCD_Clear(WHITE);
	POINT_COLOR=RED;
	Show_Str_Mid(0,30,"ATK-ESP WIFI-AP test",16,240);
	Show_Str(30,50,200,16,"config ATK-ESP module waiting...",12,0);			
	LCD_Fill(30,50,239,50+12,WHITE);			 
	Show_Str(30,50,200,16,"config ATK-ESP successful",12,0);
	delay_ms(200);
	Show_Str(30,50,200,16,"WK_UP:exit  KEY0:send",12,0);
	LCD_Fill(30,80,239,80+12,WHITE);
	atk_8266_get_wanip(ipbuf); 
	sprintf((char*)p,"IP addr:%s port:%s",ipbuf,(u8*)portnum);
	Show_Str(30,65,200,12,p,12,0);				 	
	Show_Str(30,80,200,12,"state:",12,0); 		 
	Show_Str(120,80,200,12,"mode:",12,0); 		 
	Show_Str(30,100,200,12,"send:",12,0); 	 
	Show_Str(30,115,200,12,"recv:",12,0);	 
	atk_8266_wificonf_show(30,180,"please connect WIFI:",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);
	POINT_COLOR=BLUE;
	Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
	USART3_RX_STA=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)			//WK_UP 退出测试		 
		{  
			res=0;
			atk_8266_quit_trans();	//退出透传
			atk_8266_send_cmd("AT+CIPMODE=0","OK",20);   //关闭透传模式
			break;												 
		}
		else if(key==KEY0_PRES)	//KEY0 发送数据 
		{
		
			if((netpro==3)||(netpro==2))   //UDP
			{
				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
			}
			else if((netpro==1))   //TCP Client
			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);       //开始透传
				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				u3_printf("%s",p);
				timex=100;
			}
			else    //TCP Server
			{
				sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);//测试数据
				Show_Str(30+54,100,200,12,p,12,0);
				atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  //发送指定长度的数据
				delay_ms(200);
				atk_8266_send_data(p,"OK",100);  //发送指定长度的数据
				timex=100;
			}
		}
			
		if(timex)timex--;
		if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
		t++;
		delay_ms(5);
		if(USART3_RX_STA&0X8000)		//接收到一次数据了
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符 
			printf("%s",USART3_RX_BUF);	//发送到串口   
			sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
			LCD_Fill(30+54,115,239,130,WHITE);
			POINT_COLOR=BRED;
			Show_Str(30+54,115,156,12,p,12,0); 			//显示接收到的数据长度
			POINT_COLOR=BLUE;
			LCD_Fill(30,130,239,319,WHITE);
			Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
			USART3_RX_STA=0;
			if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
			else t=0;                   //状态为已经连接了,10秒后再检查
		}  
		if(t==1000)//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		{
			constate=atk_8266_consta_check();//得到连接状态
			if(constate=='+')Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
			else Show_Str(30+30,80,200,12,"连接失败",12,0); 	  	 
			t=0;
		}
		if((t%20)==0)LED0=!LED0;
		atk_8266_at_response(1);
	}
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 

/* tcp */
void atk_8266_ap_tcp(void)
{
	u8 ipbuf[16];
	u8 p[70];
	u8* p_recv;

	/* 设置本地ip和端口 */
	atk_8266_ap_tcp_bind("192.168.4.3", "8086");
	/* 设置接收显示对方ip和端口 */
	atk_8266_ap_tcp_recv_disp_info();
	/* 获取本地ip */
	atk_8266_get_wanip(ipbuf); 
	sprintf((char*)p,"localhost:%s port:%s",ipbuf,(u8*)"8086");
	Show_Str(30,65,380,24,p,24,0);				 	
	/* 显示wifi配置 */
	atk_8266_wificonf_show(30,180,"please connect WIFI:",(u8*)wifiap_ssid,(u8*)wifiap_encryption,(u8*)wifiap_password);

	POINT_COLOR=BLUE;
	USART3_RX_STA = 0;
	while(1)
	{
		/* tcp接收, 阻塞函数 */
		p_recv = atk_8266_ap_tcp_recv();
		Show_Str(30,330,200,319,p_recv,24,0);//显示接收到的数据  
	}
}

u8* atk_8266_ap_tcp_recv()
{
	u16 recvLen = 0;
	u8* p_recv;
	while(!(USART3_RX_STA&0x8000));
	recvLen = USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
	USART3_RX_BUF[recvLen]=0;		//添加结束符 
	p_recv = USART3_RX_BUF;
	USART3_RX_STA=0;
	return p_recv;
}

void atk_8266_ap_tcp_set_ip(char* ip)
{
	u8 temp[40];
	sprintf((char*)temp, "AT+CIPAP=\"%s\"", ip);
	atk_8266_send_cmd(temp, "OK", 20);
}

void atk_8266_ap_tcp_bind(u8* ip, u8* portnum)
{
	u8 temp[40];
	sprintf((char*)temp, "AT+CIPAP=\"%s\"", ip);
	atk_8266_send_cmd(temp, "OK", 20);
	memset(temp, 0, sizeof(u8)*40);

	atk_8266_send_cmd("AT+CIPMUX=1","OK",20);    
	sprintf((char*)temp,"AT+CIPSERVER=1,%s",(u8*)portnum);
	atk_8266_send_cmd(temp,"OK",20);  
	memset(temp, 0, sizeof(u8)*40);
}

void atk_8266_ap_tcp_recv_disp_info()
{
	atk_8266_send_cmd("AT+CIPDINFO=1", "OK", 20);
}
