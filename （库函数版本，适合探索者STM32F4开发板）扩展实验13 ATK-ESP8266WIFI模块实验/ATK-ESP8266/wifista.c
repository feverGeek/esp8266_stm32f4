#include "common.h"
#include "stdlib.h"


u8 netpro=0;	
u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	
	u8 *p;
	u16 t=999;		
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;
	p=mymalloc(SRAMIN,32);							
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		
	atk_8266_send_cmd("AT+RST","OK",20);		
	delay_ms(1000);        
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					
PRESTA:
	netpro|=atk_8266_netpro_sel(50,30,(u8*)ATK_ESP8266_CWMODE_TBL[0]);	
	if(netpro&0X02)   //UDP
	{
		LCD_Clear(WHITE);
		POINT_COLOR=RED;
		Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
		Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
		if(atk_8266_ip_set("WIFI-STA 远端UDP IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入
		sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipbuf,(u8*)portnum);   
		delay_ms(200);
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20); 
		delay_ms(200);
		LCD_Clear(WHITE);
		while(atk_8266_send_cmd(p,"OK",500));
	}
	else     //TCP
	{
		if(netpro&0X01)   
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
			Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
			if(atk_8266_ip_set("WIFI-STA 远端IP设置",(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],(u8*)portnum,ipbuf))goto PRESTA;	//IP输入
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20);  
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);  
			while(atk_8266_send_cmd(p,"OK",200))
			{
				LCD_Clear(WHITE);
				POINT_COLOR=RED;
				Show_Str_Mid(0,40,"WK_UP:返回重选",16,240);
				Show_Str(30,80,200,12,"ATK-ESP 连接TCP失败",12,0); 
				key=KEY_Scan(0);
				if(key==WKUP_PRES)goto PRESTA;
			}	
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      
		}
		else					//TCP Server
		{
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240); 
			Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);
			atk_8266_send_cmd("AT+CIPMUX=1","OK",20);  
			sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)portnum);  
			atk_8266_send_cmd(p,"OK",50);    
		}
	}
			LCD_Clear(WHITE);
			POINT_COLOR=RED;
			Show_Str_Mid(0,30,"ATK-ESP WIFI-STA 测试",16,240);
			Show_Str(30,50,200,16,"正在配置ATK-ESP模块,请稍等...",12,0);			
			LCD_Fill(30,50,239,50+12,WHITE);			
			Show_Str(30,50,200,16,"WK_UP:退出测试  KEY0:发送数据",12,0);
			LCD_Fill(30,80,239,80+12,WHITE);
			atk_8266_get_wanip(ipbuf);
			sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
			Show_Str(30,65,200,12,p,12,0);			
			Show_Str(30,80,200,12,"状态:",12,0); 		
			Show_Str(120,80,200,12,"模式:",12,0); 	
			Show_Str(30,100,200,12,"发送数据:",12,0); 	
			Show_Str(30,115,200,12,"接收数据:",12,0);	
			atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
			POINT_COLOR=BLUE;
			Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		
			USART3_RX_STA=0;
			while(1)
			{
				key=KEY_Scan(0);
				if(key==WKUP_PRES)		
				{ 
					res=0;					
					atk_8266_quit_trans();	
					atk_8266_send_cmd("AT+CIPMODE=0","OK",20); 
					break;												 
				}
				else if(key==KEY0_PRES)	//KEY0
				{
				
					if((netpro==3)||(netpro==2))   //UDP
					{
						sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);
						Show_Str(30+54,100,200,12,p,12,0);
						atk_8266_send_cmd("AT+CIPSEND=25","OK",200);  
						delay_ms(200);
						atk_8266_send_data(p,"OK",100); 
						timex=100;
					}
					else if((netpro==1))   //TCP Client
					{
						atk_8266_quit_trans();
						atk_8266_send_cmd("AT+CIPSEND","OK",20);        
						sprintf((char*)p,"ATK-8266%s测试%d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);
						Show_Str(30+54,100,200,12,p,12,0);
						u3_printf("%s",p);
						timex=100;
					}
					else    //TCP Server
					{
						sprintf((char*)p,"ATK-8266%s测试%02d\r\n",ATK_ESP8266_WORKMODE_TBL[netpro],t/10);
						Show_Str(30+54,100,200,12,p,12,0);
						atk_8266_send_cmd("AT+CIPSEND=0,25","OK",200);  
						delay_ms(200);
						atk_8266_send_data(p,"OK",100);  
						timex=100;
					}
				}else;
			
				if(timex)timex--;
				if(timex==1)LCD_Fill(30+54,100,239,112,WHITE);
				t++;
				delay_ms(10);
				if(USART3_RX_STA&0X8000)		
				{ 
					rlen=USART3_RX_STA&0X7FFF;	
					USART3_RX_BUF[rlen]=0;		
					printf("%s",USART3_RX_BUF);	
					sprintf((char*)p,"收到%d字节,内容如下",rlen);
					LCD_Fill(30+54,115,239,130,WHITE);
					POINT_COLOR=BRED;
					Show_Str(30+54,115,156,12,p,12,0); 			
					POINT_COLOR=BLUE;
					LCD_Fill(30,130,239,319,WHITE);
					Show_Str(30,130,180,190,USART3_RX_BUF,12,0);
					USART3_RX_STA=0;
					if(constate!='+')t=1000;		
					else t=0;                   
				}  
				if(t==1000)
				{
//			//		LCD_Fill(30+54,125,239,130,WHITE);
//					LCD_Fill(60,80,120,92,WHITE);
					constate=atk_8266_consta_check();
					if(constate=='+')Show_Str(30+30,80,200,12,"连接成功",12,0);  
					else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
					t=0;
				}
				if((t%20)==0)LED0=!LED0;
				atk_8266_at_response(1);
			}
	myfree(SRAMIN,p);		
	return res;		
} 


