
/***********************************************************************/
/*  FILE        :EEPROM.c                                              */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :PIC32MX2     Crystal: 10/40M                          */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdlib.h>
#include <plib.h>		// ����C����
#include "initial.h"		// ��ʼ��

/******************EEPROM ����˵��******************************
24LC32 PAGE=32Bits
addr:0x000��0x01D      ID1��ID10    һ��ID����3���ֽ� ���ֽ���ǰ
     0x01E 0x01F       δ��
     0x020��0x03D      ID11��ID20
     0x03E 0x03F       δ��
     ...
     ...  ��0x33F      ���256PCS
addr:0x3A0��0x3BF      ׷��To�뿪���洢��ͨ�Ż�   2015.08.21׷��
addr:0x3C0��0x3DF      ׷��Tc��գ��洢��ͨ�Ż�   2015.08.21׷��
addr:0x5E0��0x7BF      ����initial.C�ļ���Sunrise_sunset_DATA��ֵ��˳�����δ洢
addr:0x7DC             �洢�ճ�����������DATA��Դ   =0 RAM������    =1 EEPROM������
     0x7DD             �ճ�ON/OFF  ON=1  OFF=0
     0x7DE             ����ON/OFF  ON=1  OFF=0
     0x7DF             �ձ��ص�  1=����, 2=�ض�,3=����,4=����
addr:0x7E5             �뿪�źŵ�T������
addr:0x7ED             ������״̬�仯�Ƿ��ʼ�������   0x00->OFF    0x01->ON
     0x7EE             ������״̬�仯�ʼ�֪ͨ��ַ���ֽ�
     0x7EF             ������״̬�仯�ʼ�֪ͨ��ַ���ֽ�
addr:0x7FE 0x7FF       ѧϰ����ID����  ���ֽ���ǰ   ID_DATA_PCS
addr:0x800��0x866      ���ƶ�ʱ1����   �������ݲ��ա�����ͨѶ��APPЭ������9�桷���Ժ�汾
     0x867��0x87F      δ��
     0x880��0x8E6      ���ƶ�ʱ2����   
     0x8E7��0x8FF      δ��
     ...
     ...  ��0xCFF      �����ƶ�ʱ��10PCS
addr:0xD80��0xDE6      �ճ���ʱ������   �������ݲ��ա�����ͨѶ��APPЭ������9�桷���Ժ�汾
     0xDE7��0xDFF      δ��
     0xE00��0xE66      ���䶨ʱ������
     0xE67��0xE7F      δ��
addr:0xF00��0xF1B      �ʼ���ʱ��1��2��3��4����  һ���ʼ���ʱ����7���ֽ� �������ݲ��ա�����ͨѶ��APPЭ������9�桷���Ժ�汾
     0xF1C��0xF1F      δ��
     0xF20��0xF3B      �ʼ���ʱ��5��6��7��8����
     0xF3C��0xF3F      δ��
     0xF40��0xF4D      �ʼ���ʱ��9��10����
     0xF4E��0xF5F      δ��
***************************************************************/
UINT8 ack = 0;//Ӧ���ź�

void start_i2c(void);
void stop_i2c(void);
void Mack(void);
void N0ack(char a);
void send_byte(UINT8 c);
void Write(UINT8 *s,UINT16 suba,UINT8 n);
UINT8 receive_byte(void);
void Read(UINT8 *s,UINT16 suba,UINT8 n);
void eeprom_IDcheck(void);
void eeprom_IDcheck_UART(void);
void ID_EEPROM_write(void);
void ID_EEPROM_write_pcs(void);
void ID_Login_EXIT_Initial(void);
void ID_EEPROM_Initial(void);

#if defined(__Product_PIC32MX2_WIFI__)
void alarm_EEPROM_write(void);
void all_Erase_EEPROM_next(void);
void eeprom_IDcheck_Multiple(UINT8 value_m);
void Sunrise_sunset_EEPROM_write(void);
void SUN_time_get(UINT8 value);
void SUN_EEPROM_write(void);
void Emial_time_EEPROM_write(void);
void HA_Change_EEPROM_write(void);
void EEPROM_write_0x00toID(void);
void Erase_page(UINT8 Erase_data,UINT8 page);
#endif

void ID_learn(void)
{
    UINT16 i;
 #if defined(__Product_PIC32MX2_Receiver__)
 if(FG_10ms){
     FG_10ms = 0;

//     if(time_3sec)--time_3sec;
     if(TIME_EMC)--TIME_EMC;

     if(rssi_TIME)--rssi_TIME;
     if(TIMER60s)--TIMER60s;
     if(TIME_auto_out)--TIME_auto_out;
     if(TIME_auto_close)--TIME_auto_close;
     if(TIMER_err_1s)--TIMER_err_1s;
     if(TIMER_Sensor_open_1s)--TIMER_Sensor_open_1s;
     if(TIMER_Sensor_close_1s)--TIMER_Sensor_close_1s;
     if(TIME_DIP_switch)--TIME_DIP_switch;
     if(TIME_OUT_OPEN_CLOSE)--TIME_OUT_OPEN_CLOSE;          //2015.3.23�޸�
     if(TIME_Login_EXIT_Button)--TIME_Login_EXIT_Button;   //2015.3.23�޸�
     if(TIME_Receiver_LED_OUT)--TIME_Receiver_LED_OUT;   //2015.3.23�޸�
     if(time_Login_exit_256)--time_Login_exit_256;    //2015.3.23�޸�
     if(Manual_override_TIMER)--Manual_override_TIMER;  //2015.3.23�޸�
     if(TIME_Fine_Calibration)--TIME_Fine_Calibration;
     if(TIME_Receiver_Login_restrict)--TIME_Receiver_Login_restrict;
       else if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1));
          else {TIME_Receiver_Login=0;COUNT_Receiver_Login=0;}

         if(Receiver_Login==0){
             TIME_Receiver_Login++;
             TIME_Receiver_Login_restrict=350;
             if((COUNT_Receiver_Login>=2)&&(FLAG_ID_Erase_Login==0)&&(FLAG_ID_Login==0)&&(ID_DATA_PCS<256)){FLAG_ID_Login=1;TIME_Login_EXIT_rest=5380;TIME_Login_EXIT_Button=500;}   //2015.3.23�޸�
             if(((FLAG_ID_Erase_Login==1)&&(COUNT_Receiver_Login>=1))||
                ((FLAG_ID_Login==1)&&(COUNT_Receiver_Login>=3))){
		     if(TIME_Login_EXIT_Button==0)
                         ID_Login_EXIT_Initial();   //2015.3.23�޸�
		    }
         }
         if(Receiver_Login==1){
             if(TIME_Receiver_Login>3){if(COUNT_Receiver_Login<10)COUNT_Receiver_Login++;}   //2015.3.23�޸�
             if(FLAG_ID_Login_EXIT==1){FLAG_ID_Login_EXIT=0;COUNT_Receiver_Login=0;}
             TIME_Receiver_Login=0;
         }
         if(TIME_Receiver_Login>=260){
             TIME_Receiver_Login=0;
             FLAG_ID_Erase_Login=1;
             FLAG_ID_Erase_Login_PCS=1;    //׷�Ӷ��ID��¼
             TIME_Login_EXIT_rest=5380;    //2015.3.23�޸�
             TIME_Login_EXIT_Button=500;   //2015.3.23�޸�
         }
         if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1)){          
            TIME_Receiver_Login_led++;
            if(TIME_Receiver_Login_led >= 45){              //2015.3.23�޸�
                TIME_Receiver_Login_led=0;
                if(TIME_Receiver_LED_OUT>0)
                    Receiver_LED_OUT=FLAG_PORT_LEDoutput_allow;   //2015.3.23�޸�
                else Receiver_LED_OUT=!Receiver_LED_OUT;
            }
             if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0)){
                 //FLAG_ID_Login_OK_bank=1;             //׷�Ӷ��ID��¼
                 FLAG_ID_Login_OK=0;                   //׷�Ӷ��ID��¼
                 if(FLAG_IDCheck_OK==1)FLAG_IDCheck_OK=0;
                 else{
                     BEEP_and_LED();
                     TIME_Login_EXIT_rest=5380;       //׷�Ӷ��ID��¼  //2015.3.23�޸�
                     if((FLAG_ID_Login==1)&&(ID_Receiver_Login!=0xFFFFFE))ID_EEPROM_write();
                     else if(FLAG_ID_Erase_Login==1){
                         if(FLAG_ID_Erase_Login_PCS==1){FLAG_ID_Erase_Login_PCS=0;ID_EEPROM_write_pcs();}      //׷�Ӷ��ID��¼
                         if(ID_Receiver_Login!=0xFFFFFE)ID_EEPROM_write();
                     }
                 }//end else
             }//  end  if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0))
             if(TIME_Login_EXIT_rest)--TIME_Login_EXIT_rest;
              else ID_Login_EXIT_Initial();
         } //end if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1))

    DIP_switch_Get();
 }
#endif
#if defined(__Product_PIC32MX2_WIFI__)
 if(FG_10ms){
     FG_10ms = 0;
     if(TIME_EMC)--TIME_EMC;
     
     if(rssi_TIME)--rssi_TIME;
     if(TIME_APP_Inquiry_HA)--TIME_APP_Inquiry_HA;
     if(TIME_UART)--TIME_UART;
     if(TIME_No_response)--TIME_No_response;
     if(TIME_alarm_AUTO)--TIME_alarm_AUTO;
     if(TIME_email_Repeat)--TIME_email_Repeat;
     if((TIME_email_send)&&(HA_Change_email_time==0))--TIME_email_send;
     if(FLAG_all_Erase_time)--FLAG_all_Erase_time;
     if(HA_Change_email_time)--HA_Change_email_time;
     if(TIME_Fine_Calibration)--TIME_Fine_Calibration;
     if(TIME_send_Faile_notice)--TIME_send_Faile_notice;   //2015.3.31׷���޸� 2�η��Ͷ�ʧ�ܣ�SIG��ɫLED 1Hz֪ͨ
     if(TIME_WIFI_LAN_SELECT)--TIME_WIFI_LAN_SELECT;
     if(TIME_one_hour)--TIME_one_hour;         //2015.1.30׷���޸�1Сʱ��ѯһ��HA״̬
     if(time_APP_Start_up)--time_APP_Start_up;     //2015.04.27����
     if(TIME_ID_Login_delay)--TIME_ID_Login_delay;  //20150430 japan�޸�1  ���л�����¼ģʽ�����У����������429MHz����ʱҲ���ID��¼��ȥ��

     if(FLAG_all_Erase==0)          //EEPROM�������ݲ���ʱ������CODE��ִ��
     {
             if(TIME_Receiver_Login_restrict)--TIME_Receiver_Login_restrict;
               else if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1));
                  else {TIME_Receiver_Login=0;COUNT_Receiver_Login=0;}

                 if(WIFI_L_Login==0){
                     TIME_Receiver_Login++;
                     TIME_Receiver_Login_restrict=350;
                     if(COUNT_Receiver_Login>=2){FLAG_ID_Login=1;TIME_Login_EXIT_rest=6000;FLAG_ID_Login_OK=0;TIME_ID_Login_delay=50;}    //20150430 japan�޸�1
                     if(((FLAG_ID_Erase_Login==1)&&(COUNT_Receiver_Login>=1))||
                        ((FLAG_ID_Login==1)&&(COUNT_Receiver_Login>=3)))ID_Login_EXIT_Initial();
                 }
                 if(WIFI_L_Login==1){
                     if(TIME_Receiver_Login>3)COUNT_Receiver_Login++;
                     if(FLAG_ID_Login_EXIT==1){FLAG_ID_Login_EXIT=0;COUNT_Receiver_Login=0;}
                     TIME_Receiver_Login=0;
                 }
        //         if(TIME_Receiver_Login>=300){                  //ȡ��������¼����
        //             TIME_Receiver_Login=0;
        //             FLAG_ID_Erase_Login=1;
        //             FLAG_ID_Erase_Login_PCS=1;    //׷�Ӷ��ID��¼
        //             TIME_Login_EXIT_rest=6000;
        //         }
//                 if(TIME_Receiver_Login>=300){                  //ȡ��������¼����      2014��4��24���Ļ���� ȡ��ȫ��ID���
//                     TIME_Receiver_Login=0;
//                     COUNT_Receiver_Login=0;
//                     WIFI_LED_TX=1;
//                     while(WIFI_L_Login==0)ClearWDT(); // Service the WDT
//                     uart_send_APP_allID();
//                    WIFI_LED_TX=0;
//                 }
                 if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1)){
                     TIME_Receiver_Login_led++;
                     if(TIME_Receiver_Login_led>=30){
                         TIME_Receiver_Login_led=0;
                         WIFI_LED_RX=!WIFI_LED_RX;
                     }
                     if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0)){
                         //FLAG_ID_Login_OK_bank=1;     //׷�Ӷ��ID��¼
                         FLAG_ID_Login_OK=0;                   //׷�Ӷ��ID��¼
                         if(FLAG_IDCheck_OK==1){FLAG_IDCheck_OK=0;Confirm_BEEP_and_LED();}
                         else{
                             BEEP_and_LED();
                             FLAG_Receiver_IDCheck=0;
                             TIME_Login_EXIT_rest=6000;       //׷�Ӷ��ID��¼
                             if(FLAG_IDCheck_OK_0x00==1){
                                 FLAG_IDCheck_OK_0x00=0;
                                 EEPROM_write_0x00toID();
                             }
                             else if((FLAG_ID_Login==1)&&(DATA_Packet_ID!=0xFFFFFE))ID_EEPROM_write();
                             else if(FLAG_ID_Erase_Login==1){
                                 if(FLAG_ID_Erase_Login_PCS==1){FLAG_ID_Erase_Login_PCS=0;ID_EEPROM_write_pcs();}      //׷�Ӷ��ID��¼
                                 if(DATA_Packet_ID!=0xFFFFFE)ID_EEPROM_write();
                             }
                         }//end else
                     }//  end  if((FLAG_ID_Login_OK==1)&&(FLAG_ID_Login_OK_bank==0))
                     if(TIME_Login_EXIT_rest)--TIME_Login_EXIT_rest;
                      else ID_Login_EXIT_Initial();
                 } //end if((FLAG_ID_Erase_Login==1)||(FLAG_ID_Login==1))
     }
 }
#endif
}

void ID_Login_EXIT_Initial(void)
{
 #if defined(__Product_PIC32MX2_Receiver__)
     FLAG_ID_Login_EXIT=1;
     FLAG_ID_Login_OK=0;
     FLAG_ID_Login_OK_bank=0;
     FLAG_ID_Login=0;
     FLAG_ID_Erase_Login=0;
     Receiver_LED_OUT=FLAG_PORT_LEDoutput_NOallow;
     COUNT_Receiver_Login=0;     //2015.3.23�޸�
#endif
#if defined(__Product_PIC32MX2_WIFI__)
     FLAG_ID_Login_EXIT=1;
     FLAG_ID_Login_OK=0;
     FLAG_ID_Login_OK_bank=0;
     FLAG_ID_Login=0;
     FLAG_ID_Erase_Login=0;
     WIFI_LED_RX=0;
#endif
}
void all_Erase_EEPROM_next(void)
{
    UINT16 i,m1,m2,m3;
    UINT8 xm[32]={0};
 #if defined(__Product_PIC32MX2_WIFI__)
    if(FLAG_all_Erase==1){
        if((FLAG_all_Erase_time>0)&&((WIFI_L_Login!=0)||(WIFI_USBLogin!=0)))FLAG_all_Erase=0;
        else if((FLAG_all_Erase_time==0)&&(FLAG_all_Erase_loop==0)){FLAG_all_Erase_loop=1;TIMER1s=5000;TIMER300ms=300;WIFI_LED_TX=1;WIFI_LED_RX=0;}
        if(FLAG_all_Erase_loop==1){
            if(TIMER300ms==0){TIMER300ms=300;WIFI_LED_TX=!WIFI_LED_TX;WIFI_LED_RX=!WIFI_LED_RX;}
            if(FLAG_all_Erase_OK==0){
                FLAG_all_Erase_OK=1;
//                xm[0]=0xFF;
//                xm[1]=0xFF;
//                Write(&xm[0],0x7FE,2);
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                xm[0]=0xFF;
//                Write(&xm[0],0xFDE,1);

//                for(i=0;i<32;i++)xm[i]=0xFF;
//                for(i=0;i<128;i++){
//                    Write(&xm[0],i*32,32);
//                    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                    ClearWDT(); // Service the WDT
//                    if(TIMER300ms==0){TIMER300ms=300;WIFI_LED_TX=!WIFI_LED_TX;WIFI_LED_RX=!WIFI_LED_RX;}
//                }

//                 xm[0]=0;
//                 xm[1]=0;
//                 Write(&xm[0],0x7FE,2);
//                 Delay100us(100);
//                 xm[0]=0;
//                 xm[1]=0;
//                 xm[2]=0;
//                 for(i=0;i<32;i++){
//                    m2=i/10;
//                    m3=i%10;
//                    Write(&xm[0],32*m2+m3*3,3);//д�����ݵ�24LC16
//                    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                    ClearWDT(); // Service the WDT
//                    if(TIMER300ms==0){TIMER300ms=300;WIFI_LED_TX=!WIFI_LED_TX;WIFI_LED_RX=!WIFI_LED_RX;}
//                 }


                  for(i=0;i<128;i++){
                       Erase_page(0x00,i);
                       Delay100us(100);
                       ClearWDT(); // Service the WDT
                       if(TIMER300ms==0){TIMER300ms=300;WIFI_LED_TX=!WIFI_LED_TX;WIFI_LED_RX=!WIFI_LED_RX;}
                   }
                ID_EEPROM_Initial();

            }
            if(TIMER1s==0);//{FLAG_all_Erase_OK==0;FLAG_all_Erase=0;TIMER300ms=0;TIMER1s=0;SoftReset();}
            else ClearWDT(); // Service the WDT
        }
    }
    else ClearWDT(); // Service the WDT
 #endif
 #if defined(__Product_PIC32MX2_Receiver__)
    ClearWDT();
#endif
}
void all_Erase_EEPROM(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i;
    UINT8 xm[32]={0};

    FLAG_APP_BYTE=0;
    for(i=0;i<35;i++){EMIAL_id_data[i]=0;EMIAL_id_HA[i]=0;EMIAL_id_PCS=0;Email_check_ID[i]=0x00;Emial_check_Control[i]=0x00;HA_Cache_SWITCH_DIP[i]=0;HA_Cache_ha[i]=0;HA_Cache_IDdata[i]=0;}

    if((WIFI_L_Login==0)&&(WIFI_USBLogin==0)){FLAG_all_Erase=1;FLAG_all_Erase_time=300;}
    else {FLAG_all_Erase=0;TIMER300ms=0;TIMER1s=0;}
#endif
}
void ID_EEPROM_Initial(void)
{
    UINT16 i,j,RTC_Minutes,HA_Change_i;
    UINT8 xm[103]={0};
    UINT16 m1,m2,m3;
    uni_rom_id xn;
    for(i=0;i<256;i++)ID_Receiver_DATA[i]=0;//ID_Receiver_DATA[ID_DATA_PCS]=0;
    Read(&xm[0],0x7FE,2);
    ID_DATA_PCS=xm[1]*256+xm[0];
    if(ID_DATA_PCS==0xFFFF)ID_DATA_PCS=0;
    for(i=0;i<ID_DATA_PCS;i++){
        m2=i/10;
        m3=i%10;
        Read(&xm[0],32*m2+m3*3,3);
        xn.IDB[0]=xm[0];
        xn.IDB[1]=xm[1];
        xn.IDB[2]=xm[2];
        xn.IDB[3]=0;
        ID_Receiver_DATA[i]=xn.IDL;
#if defined(__Product_PIC32MX2_WIFI__)      //����2015.08.21׷��
        ClearWDT();
        Read(&xm[0],0x3A0+i,1);
        if((xm[0]==0)||(xm[0]==0xFF))ID_DATA_To[i]=16;  //Ĭ��Ϊ20��   20-4
        else ID_DATA_To[i]=xm[0];
        Read(&xm[0],0x3C0+i,1);
        if((xm[0]==0)||(xm[0]==0xFF))ID_DATA_Tc[i]=16;  //Ĭ��Ϊ20��   20-4
        else ID_DATA_Tc[i]=xm[0];
#endif
#if defined(__Product_PIC32MX2_Receiver__)
        ClearWDT();
        if((FLAG_POER_on==0)&&(ID_Receiver_DATA[i]!=0)){FLAG_POER_on=1;DATA_Packet_ID=ID_Receiver_DATA[i];}
#endif
    }
//#if defined(__Product_PIC32MX2_Receiver__)
//    Read(&xm[0],0x7E5,1);
//    if((xm[0]>0xBD)||(xm[0]<0x81))TIMER_Semi_open=20;      //30
//    else TIMER_Semi_open=(xm[0]-1)&0x7F;
//#endif
#if defined(__Product_PIC32MX2_WIFI__)
//    Write(&Sunrise_sunset_DATA[0],0xD00,32);
//    Delay100us(100);

    Read(&xm[0],0x7DC,4);       //0x7DC�洢�ճ�����������DATA��Դ   =0 RAM������    =1 EEPROM������
    if(xm[1]==0xFF)xm[1]=0;
    if(xm[2]==0xFF)xm[2]=0;
    if(xm[3]>10)xm[3]=0;
    SUN_ON_OFF_seat[0]=xm[1];   //0x7DD �ճ�ON/OFF  ON=1  OFF=0
    SUN_ON_OFF_seat[1]=xm[2];  //0x7DE ����ON/OFF  ON=1  OFF=0
    SUN_ON_OFF_seat[2]=xm[3];  //0x7DF �ձ��ص�  1=����, 2=�ض�,3=����,4=����

    Read(&xm[0],0xD80,32);
    Delay100us(100);

    for(i=0;i<13;i++){
       ClearWDT(); // Service the WDT
       if(i!=10){
               if(i==11){
                   Read(&xm[0],0x800+12*128,103);
                   xm[0]=11;
                   if(SUN_ON_OFF_seat[0]==0x00)xm[1]=0;
                   else xm[1]=1;
                   if(SUN_ON_OFF_seat[0]==0x01) xm[2]=0x08;
                   else if(SUN_ON_OFF_seat[0]==0x11) xm[2]=0x80;
                   else if(SUN_ON_OFF_seat[0]==0x10) xm[2]=0x01;
               }
               else if(i==12){
                   Read(&xm[0],0x800+12*128,103);
                   xm[0]=12;
                   if(SUN_ON_OFF_seat[1]==0x00)xm[1]=0;
                   else xm[1]=1;
                   if(SUN_ON_OFF_seat[1]==0x01) xm[2]=0x02;
                   else if(SUN_ON_OFF_seat[1]==0x11) xm[2]=0xC0;
                   else if(SUN_ON_OFF_seat[1]==0x10) xm[2]=0x01;
               }
               else Read(&xm[0],0x800+i*128,103);
               if(i>10)m1=i-1;
               else m1=i;
               if((xm[0]==0x00)||(xm[0]>12)||(xm[1]>1)||(xm[3]>0x24)||(xm[4]>0x60)||(xm[5]>0x80)||(xm[6]>0x20)){
                   WIFI_alarm_data[m1][0]=m1+1;
                   for(j=1;j<103;j++)WIFI_alarm_data[m1][j]=0x00;
               }
               else {
                   WIFI_alarm_data[m1][0]=m1+1;
                   for(j=1;j<103;j++)WIFI_alarm_data[m1][j]=xm[j];
               }
       }
    }

    Read(&xm[0],0x7DC,4);       //0x7DC�洢�ճ�����������DATA��Դ   =0 RAM������    =1 EEPROM������
    if(xm[0]==1){
             //Read(&Sunrise_sunset_DATA[0],0x5E0,480);
             for(i=0;i<15;i++){
                Read(&Sunrise_sunset_DATA[i*32],0x5E0+i*32,32);    //д�����ݵ�24LC16
                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
            }
    }
    SUN_time_get(SUN_ON_OFF_seat[2]);

    for(i=0;i<10;i++){
       ClearWDT(); // Service the WDT
       m2=i/4;
       m3=i%4;
       Read(&xm[0],0xF00+32*m2+m3*7,7);
       if((xm[0]==0x00)||(xm[0]>10)||(xm[1]>1)||(xm[2]>0x24)||(xm[3]>0x60)||(xm[4]>0x80)){
           Emial_time_data[i][0]=i+1;
           for(j=1;j<7;j++)Emial_time_data[i][j]=0x00;
       }
       else {
           Emial_time_data[i][0]=i+1;
           for(j=1;j<7;j++)Emial_time_data[i][j]=xm[j];
       }
    }

    Read_Time(&xm[0]);
    //RTC_Minutes=xm[2]*60+xm[1];
    RTC_Minutes=Hex_Decimal(xm[2],xm[1]);             //2014.10.11�޸�   ���TIMER��ʱ������
    NEW_set_alarm_pcf8563(RTC_Minutes);

    Read(&xm[0],0x7ED,3);       //0x7ED      ������״̬�仯�Ƿ��ʼ�������   0x00->OFF    0x01->ON
    if(xm[0]>1)xm[0]=0;
    HA_Change_i=xm[2]*256+xm[1];
    if(HA_Change_i>0x7FF){xm[1]=0;xm[2]=0;}
    HA_Change_send_email[0]=xm[0];   //
    HA_Change_send_email[1]=xm[1];  //0x7EE  ������״̬�仯�ʼ�֪ͨ��ַ���ֽ�
    HA_Change_send_email[2]=xm[2];  //0x7EF  ������״̬�仯�ʼ�֪ͨ��ַ���ֽ�


//    for(i=0;i<100;i++)
//        for(j=0;j<10;j++)
//        {
//            WIFI_alarm_data[i][j]=0;
//        }
//    Read(&xm[0],0xFDE,1);
//    WIFI_alarm_data_PCS=xm[0];
//    if(WIFI_alarm_data_PCS==0xFF){WIFI_alarm_data_PCS=0;xm[0]=0;Write_pcf8563(&xm[0],0x01,1);}
//    Read_Time(&xm[0]);
//    RTC_Minutes=xm[2]*60+xm[1];
//    for(i=0;i<WIFI_alarm_data_PCS;i++){
//        m2=i/3;
//        m3=i%3;
//        Read(&xm[0],0x800+m2*32+m3*10,10);
//        for(j=0;j<10;j++)WIFI_alarm_data[i][j]=xm[j];
//    }
//    NEW_set_alarm_pcf8563(RTC_Minutes);
#endif
}

void eeprom_IDcheck(void)
{
    UINT16 i;
    for(i = 0;i < ID_DATA_PCS;i++){
        if(ID_Receiver_DATA[i] == DATA_Packet_ID_buf){
            INquiry = i;
            i = ID_DATA_PCS;
            FLAG_IDCheck_OK = 1;
            DATA_Packet_ID = DATA_Packet_ID_buf;
            DATA_Packet_Control = DATA_Packet_Control_buf;
        }
        if((FLAG_ID_Erase_Login == 1) && (FLAG_ID_Erase_Login_PCS == 1)){
            i = ID_DATA_PCS;
            FLAG_IDCheck_OK = 0;
        }         //׷�Ӷ��ID��¼
    }
}

#if defined(__Product_PIC32MX2_WIFI__)
void eeprom_IDcheck_0x00(void)
{
    UINT16 i;
   for(i=0;i<ID_DATA_PCS;i++){
       if(ID_Receiver_DATA[i]==0x00){INquiry_0x00=i;i=ID_DATA_PCS;FLAG_IDCheck_OK_0x00=1;}
   }
}

UINT8 Email_check_TO_APP(void)     //2015.4.1����3 ����APP��ѯ������HA״̬��Ҫ�ܳ���ʱ�䣬����׷��ָ���ѯ������ͨ�Ż������HA״̬
{
    UINT16 i;
   for(i=0;i<35;i++){
       if(HA_Cache_IDdata[i]==ID_data_uart_CMD0101_01.IDL){Emial_Cache_HA=HA_Cache_ha[i]&0xFF;Emial_Cache_SWITCH=HA_Cache_SWITCH_DIP[i];i=64;}     //
   }
   if(i==35) return 1;
   else return 0;
}
#endif

void eeprom_IDcheck_UART(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i;
   for(i=0;i<ID_DATA_PCS;i++){
       if(ID_Receiver_DATA[i]==ID_data.IDL){i=ID_DATA_PCS;FLAG_IDCheck_OK=1;}
       if(FLAG_ID_Erase_Login==1){i=ID_DATA_PCS;FLAG_IDCheck_OK=0;}
   }
#endif
}

#if defined(__Product_PIC32MX2_WIFI__)
void eeprom_IDcheck_CMD0101_01_UART(void)
{
                //20150501 JAPAN׷��  ����Ƿ�������APPʱ���ڲ�ѯͨ�Ż������ڲ�HA״̬ʱ����Control_code=1���Ƴ�ȥ�ˣ���֮ǰ��һ����������������ʵʩ���У�
    UINT16 i;
   for(i=0;i<ID_DATA_PCS;i++){
       if(ID_Receiver_DATA[i]==ID_data_uart_CMD0101_01.IDL){CMD0102_To_or_Tc_place=i;i=ID_DATA_PCS;FLAG_IDCheck_OK=1;}
       if(FLAG_ID_Erase_Login==1){i=ID_DATA_PCS;FLAG_IDCheck_OK=0;}
   }
}
void IDcheck_CMD0102_HA_Cache(void)        //����2015.08.21׷��
{
    UINT16 i;
   for(i=0;i<ID_DATA_PCS;i++){
       if(HA_Cache_IDdata[i]==ID_data.IDL){CMD0102_To_or_Tc_HA=i;i=ID_DATA_PCS;FLAG_IDcheck_CMD0102_HA=1;}
   }
}
void eeprom_IDcheck_CMD0111_UART(void)
{
    UINT16 i;
   for(i=0;i<ID_DATA_PCS;i++){
       if(ID_Receiver_DATA[i]==ID_data_uart_CMD0111.IDL){ID_DATA_To_or_Tc_place=i;i=ID_DATA_PCS;FLAG_IDCheck_OK=1;}
       if(FLAG_ID_Erase_Login==1){i=ID_DATA_PCS;FLAG_IDCheck_OK=0;}
   }
}
#endif

#if defined(__Product_PIC32MX2_WIFI__)
void eeprom_IDcheck_Multiple(UINT8 value_m)
{
    UINT16 m_i,m_j;
    uni_rom_id data_m;
    for(m_i=0;m_i<UART1_DATA[value_m];m_i++){
            FLAG_IDCheck_OK=0;
            data_m.IDB[0]=UART1_DATA[value_m+1+m_i*3];
            data_m.IDB[1]=UART1_DATA[value_m+2+m_i*3];
            data_m.IDB[2]=UART1_DATA[value_m+3+m_i*3];
            data_m.IDB[3]=0x00;
            for(m_j=0;m_j<ID_DATA_PCS;m_j++){
                if(ID_Receiver_DATA[m_j]==data_m.IDL){FLAG_IDCheck_OK=1;m_j=ID_DATA_PCS;}
            }
            if(FLAG_IDCheck_OK==0)return;
    }
}
void Sunrise_sunset_EEPROM_write(void){         //�ճ�����������DATA���ñ���
    UINT16 i,j,RTC_Minutes;
    UINT8 xm[2]={0x01,0x00};
    UINT8 xn[10]={0x00};

    Write(&xm[0],0x7DC,1);     //0x7DC�洢�ճ�����������DATA��Դ   =0 RAM������    =1 EEPROM������
    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
    for(i=11;i<491;i++)Sunrise_sunset_DATA[i-11]=UART1_DATA[i];
    for(i=0;i<15;i++){
        Write(&Sunrise_sunset_DATA[i*32],0x5E0+i*32,32);    //д�����ݵ�24LC16
        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
    }
    SUN_time_get(SUN_ON_OFF_seat[2]);
    Read_Time(&xn[0]);
    //RTC_Minutes=xn[2]*60+xn[1];
    RTC_Minutes=Hex_Decimal(xn[2],xn[1]);             //2014.10.11�޸�   ���TIMER��ʱ������
    NEW_set_alarm_pcf8563(RTC_Minutes);
}
void SUN_time_get(UINT8 value){         //��ѯĳ��ĳ�����ճ�����������DATA�е�OPEN CLOSEʱ��
    UINT8 xm[10]={0};
    UINT8 x_h0,x_h1;
    //UINT16 i_m7;                        //UINT8 i; -----> UINT16 i;     2014��7��23������

    if((value>0)&&(value<=10)){
        Read_Time(&xm[0]);
        x_h0=xm[5]>>4;
        x_h0=x_h0*10;
        x_h1=xm[5]&0x0F;
        x_h0=x_h0+x_h1;
        DATA_SUN_time_get=(x_h0-1)*40+(value-1)*4;
        WIFI_alarm_data[10][3]=Sunrise_sunset_DATA[DATA_SUN_time_get];
        WIFI_alarm_data[10][4]=Sunrise_sunset_DATA[DATA_SUN_time_get+1];
        WIFI_alarm_data[11][3]=Sunrise_sunset_DATA[DATA_SUN_time_get+2];
        WIFI_alarm_data[11][4]=Sunrise_sunset_DATA[DATA_SUN_time_get+3];
        SUN_Weekdays_alarm=xm[5];
    }
}
void alarm_EEPROM_write(void)
{
    uni_rom_id xmm;
    UINT16 i,j;
    UINT8 FLAG_i,FLAG_j=0;
    UINT8 xm[10]={0};
    UINT16 m1,m2,m3;
    UINT16 RTC_Minutes;
    UINT8 Write_Read_compare[103];
    UINT8 WIFI_alarm_data_Cache[103];
    UINT8 Write_Read_compare_count=0;

    for(i=0;i<103;i++){Write_Read_compare[i]=0;WIFI_alarm_data_Cache[i]=0;}
    FLAG_Write_Read_compare=0;

    m1=UART1_DATA[11];
    m2=18+UART1_DATA[17]*3;
    for(i=11;i<m2;i++)WIFI_alarm_data_Cache[i-11]=UART1_DATA[i];  //WIFI_alarm_data[m1-1][i-11]=UART1_DATA[i];
    for(j=i;j<103;j++)WIFI_alarm_data_Cache[j]=0x00;  //WIFI_alarm_data[m1-1][j]=0x00;

    while((FLAG_Write_Read_compare==0)&&(Write_Read_compare_count<3))
    {
//                Write(WIFI_alarm_data[m1-1],0x800+(m1-1)*128,32);    //д�����ݵ�24LC16
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                Write(&WIFI_alarm_data[m1-1][32],0x800+(m1-1)*128+32,32);
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                Write(&WIFI_alarm_data[m1-1][64],0x800+(m1-1)*128+64,32);
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                Write(&WIFI_alarm_data[m1-1][96],0x800+(m1-1)*128+96,7);
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
                Write(&WIFI_alarm_data_Cache[0],0x800+(m1-1)*128,32);    //д�����ݵ�24LC16
                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
                Write(&WIFI_alarm_data_Cache[32],0x800+(m1-1)*128+32,32);
                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
                Write(&WIFI_alarm_data_Cache[64],0x800+(m1-1)*128+64,32);
                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
                Write(&WIFI_alarm_data_Cache[96],0x800+(m1-1)*128+96,7);
                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Read(&Write_Read_compare[0],0x800+(m1-1)*128,103);
        for(i=0;i<103;i++){
            if(Write_Read_compare[i]==WIFI_alarm_data_Cache[i])FLAG_Write_Read_compare=1;
            else {FLAG_Write_Read_compare=0;i=103;}
        }
        Write_Read_compare_count++;
    }

    if(FLAG_Write_Read_compare==1){
        for(i=0;i<103;i++)WIFI_alarm_data[m1-1][i]=WIFI_alarm_data_Cache[i];
        Read_Time(&xm[0]);
        //RTC_Minutes=xm[2]*60+xm[1];
        RTC_Minutes=Hex_Decimal(xm[2],xm[1]);             //2014.10.11�޸�   ���TIMER��ʱ������
        NEW_set_alarm_pcf8563(RTC_Minutes);
    }
//    xmm.IDB[0]=UART1_DATA[8];
//    xmm.IDB[1]=UART1_DATA[9];
//    xmm.IDB[2]=UART1_DATA[10];
//    xmm.IDB[3]=0x00;
//    if(xmm.IDL==0)return;
//    for(i=0;i<ID_DATA_PCS;i++){
//       if(ID_Receiver_DATA[i]==xmm.IDL){i=ID_DATA_PCS;FLAG_i=1;}
//    }
//    if(FLAG_i==1){
//        FLAG_i=0;
//        for(i=0;i<WIFI_alarm_data_PCS;i++){
//            if((WIFI_alarm_data[i][0]==UART1_DATA[8])&&(WIFI_alarm_data[i][1]==UART1_DATA[9])&&(WIFI_alarm_data[i][2]==UART1_DATA[10])){
//                for(j=8;j<18;j++){WIFI_alarm_data[i][j-8]=UART1_DATA[j];WIFI_alarm_data[i+1][j-8]=UART1_DATA[j];}
//                for(j=18;j<25;j++)WIFI_alarm_data[i+1][j-15]=UART1_DATA[j];
//                m2=i/3;
//                m3=i%3;
//                Write(WIFI_alarm_data[i],0x800+m2*32+m3*10,10);//д�����ݵ�24LC16
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                m2=(i+1)/3;
//                m3=(i+1)%3;
//                Write(WIFI_alarm_data[i+1],0x800+m2*32+m3*10,10);//д�����ݵ�24LC16
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                i=WIFI_alarm_data_PCS;
//                FLAG_j=1;
//            }
//        }
//     }
//        if(FLAG_j==0){
//             WIFI_alarm_data_PCS++;
//             WIFI_alarm_data_PCS++;
//             xm[0]=WIFI_alarm_data_PCS;
//             Write(&xm[0],0xFDE,1);
//             Delay100us(100);
//            for(j=8;j<18;j++){WIFI_alarm_data[WIFI_alarm_data_PCS-2][j-8]=UART1_DATA[j];WIFI_alarm_data[WIFI_alarm_data_PCS-1][j-8]=UART1_DATA[j];}
//            for(j=18;j<25;j++)WIFI_alarm_data[WIFI_alarm_data_PCS-1][j-15]=UART1_DATA[j];
//            m2=(WIFI_alarm_data_PCS-2)/3;
//            m3=(WIFI_alarm_data_PCS-2)%3;
//            Write(WIFI_alarm_data[WIFI_alarm_data_PCS-2],0x800+m2*32+m3*10,10);//д�����ݵ�24LC16
//            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//            m2=(WIFI_alarm_data_PCS-1)/3;
//            m3=(WIFI_alarm_data_PCS-1)%3;
//            Write(WIFI_alarm_data[WIFI_alarm_data_PCS-1],0x800+m2*32+m3*10,10);//д�����ݵ�24LC16
//            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//        }
//        Read_Time(&xm[0]);
//        RTC_Minutes=xm[2]*60+xm[1];
//        NEW_set_alarm_pcf8563(RTC_Minutes);
}
void SUN_EEPROM_write(void)
{
    UINT8 i,j;
    UINT8 xm[10]={0};
    UINT8 xmmmmmm[103]={0};
    UINT16 m1,m2,m3;
    UINT16 RTC_Minutes;
    UINT8 Write_Read_compare[2][103];
    UINT8 Write_Read_compare_SUN[3];
    UINT8 WIFI_alarm_data_Cache[2][103];
    UINT8 SUN_ON_OFF_seat_Cache[3];
    UINT8 Write_Read_compare_count=0;

    for(i=0;i<103;i++){Write_Read_compare[0][i]=0;Write_Read_compare[1][i]=0;WIFI_alarm_data_Cache[0][i]=0;WIFI_alarm_data_Cache[1][i]=0;}
    for(i=0;i<3;i++){Write_Read_compare_SUN[i]=0;SUN_ON_OFF_seat_Cache[i]=0;}
    FLAG_Write_Read_compare=0;

//    SUN_ON_OFF_seat[0]=UART1_DATA[11];   //0x7DD �ճ�ON/OFF  ON=1  OFF=0
//    SUN_ON_OFF_seat[1]=UART1_DATA[12];  //0x7DE ����ON/OFF  ON=1  OFF=0
//    SUN_ON_OFF_seat[2]=UART1_DATA[13];  //0x7DF �ձ��ص�  1=����, 2=�ض�,3=����,4=����
//    Write(&SUN_ON_OFF_seat[0],0x7DD,3);
    SUN_ON_OFF_seat_Cache[0]=UART1_DATA[11];   //0x7DD �ճ�ON/OFF  ON=1  OFF=0
    SUN_ON_OFF_seat_Cache[1]=UART1_DATA[12];  //0x7DE ����ON/OFF  ON=1  OFF=0
    SUN_ON_OFF_seat_Cache[2]=UART1_DATA[13];  //0x7DF �ձ��ص�  1=����, 2=�ض�,3=����,4=����

    WIFI_alarm_data_Cache[0][0]=11;
    //WIFI_alarm_data_Cache[0][1]=UART1_DATA[11];     //����2015.08.21׷��
    //WIFI_alarm_data_Cache[0][2]=0x08;
    if(SUN_ON_OFF_seat_Cache[0]==0x00)WIFI_alarm_data_Cache[0][1]=0;
    else WIFI_alarm_data_Cache[0][1]=1;
    if(SUN_ON_OFF_seat_Cache[0]==0x01)WIFI_alarm_data_Cache[0][2]=0x08;
    else if(SUN_ON_OFF_seat_Cache[0]==0x11)WIFI_alarm_data_Cache[0][2]=0x80;
    else if(SUN_ON_OFF_seat_Cache[0]==0x10)WIFI_alarm_data_Cache[0][2]=0x01;

    WIFI_alarm_data_Cache[0][3]=0;         //��EEPROM�е��ճ�����ʱ������Ϊ0���ǿ�SUN_time_get(SUN_ON_OFF_seat[2])�����ڱ��в�ѯ��ȡ
    WIFI_alarm_data_Cache[0][4]=0;
    WIFI_alarm_data_Cache[1][0]=12;
    //WIFI_alarm_data_Cache[1][1]=UART1_DATA[12];   //����2015.08.21׷��
    //WIFI_alarm_data_Cache[1][2]=0x02;
    if(SUN_ON_OFF_seat_Cache[1]==0x00)WIFI_alarm_data_Cache[1][1]=0;
    else WIFI_alarm_data_Cache[1][1]=1;
    if(SUN_ON_OFF_seat_Cache[1]==0x01)WIFI_alarm_data_Cache[1][2]=0x02;
    else if(SUN_ON_OFF_seat_Cache[1]==0x11)WIFI_alarm_data_Cache[1][2]=0xC0;
    else if(SUN_ON_OFF_seat_Cache[1]==0x10)WIFI_alarm_data_Cache[1][2]=0x01;
    
    WIFI_alarm_data_Cache[1][3]=0;       //��EEPROM�е��ճ�����ʱ������Ϊ0���ǿ�SUN_time_get(SUN_ON_OFF_seat[2])�����ڱ��в�ѯ��ȡ
    WIFI_alarm_data_Cache[1][4]=0;
    m2=16+UART1_DATA[15]*3;
    for(i=14;i<m2;i++){WIFI_alarm_data_Cache[0][i-9]=UART1_DATA[i];WIFI_alarm_data_Cache[1][i-9]=UART1_DATA[i];}
    for(i=m2-9;i<103;i++){WIFI_alarm_data_Cache[0][i]=0x00;WIFI_alarm_data_Cache[1][i]=0x00;}
//    Write(&WIFI_alarm_data[10][0],0x800+11*128,32);    //д�����ݵ�24LC16
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[10][32],0x800+11*128+32,32);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[10][64],0x800+11*128+64,32);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[10][96],0x800+11*128+96,7);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[11][0],0x800+12*128,32);    //д�����ݵ�24LC16
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[11][32],0x800+12*128+32,32);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[11][64],0x800+12*128+64,32);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//    Write(&WIFI_alarm_data[11][96],0x800+12*128+96,7);
//    Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms

    while((FLAG_Write_Read_compare==0)&&(Write_Read_compare_count<5))
    {
//                    for(i=11;i<13;i++){
//                        Write(&WIFI_alarm_data[i-1][0],0x800+i*128,32);    //д�����ݵ�24LC16
//                        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                        Write(&WIFI_alarm_data[i-1][32],0x800+i*128+32,32);
//                        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                        Write(&WIFI_alarm_data[i-1][64],0x800+i*128+64,32);
//                        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                        Write(&WIFI_alarm_data[i-1][96],0x800+i*128+96,7);
//                        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
//                    }
//                    Write(&SUN_ON_OFF_seat[0],0x7DD,3);
        for(i=11;i<13;i++){
            Write(&WIFI_alarm_data_Cache[i-11][0],0x800+i*128,32);    //д�����ݵ�24LC16
            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
            Write(&WIFI_alarm_data_Cache[i-11][32],0x800+i*128+32,32);
            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
            Write(&WIFI_alarm_data_Cache[i-11][64],0x800+i*128+64,32);
            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
            Write(&WIFI_alarm_data_Cache[i-11][96],0x800+i*128+96,7);
            Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        }
        Write(&SUN_ON_OFF_seat_Cache[0],0x7DD,3);
        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Read(&Write_Read_compare_SUN[0],0x7DD,3);
        for(i=0;i<3;i++){
            if(Write_Read_compare_SUN[i]==SUN_ON_OFF_seat_Cache[i])FLAG_Write_Read_compare=1;
            else {FLAG_Write_Read_compare=0;i=3;}
        }
        if(FLAG_Write_Read_compare==1){
            Read(&Write_Read_compare[0][0],0x800+11*128,103);
            Read(&Write_Read_compare[1][0],0x800+12*128,103);
            for(i=0;i<103;i++){
                if((Write_Read_compare[0][i]==WIFI_alarm_data_Cache[0][i])&&(Write_Read_compare[1][i]==WIFI_alarm_data_Cache[1][i]))FLAG_Write_Read_compare=1;
                else {FLAG_Write_Read_compare=0;i=103;}
            }
        }
        Write_Read_compare_count++;
    }

    if(FLAG_Write_Read_compare==1){
        for(i=0;i<3;i++)SUN_ON_OFF_seat[i]=SUN_ON_OFF_seat_Cache[i];
        for(i=0;i<103;i++){WIFI_alarm_data[10][i]=WIFI_alarm_data_Cache[0][i];WIFI_alarm_data[11][i]=WIFI_alarm_data_Cache[1][i];}
        SUN_time_get(SUN_ON_OFF_seat[2]);
        Read_Time(&xm[0]);
        //RTC_Minutes=xm[2]*60+xm[1];
        RTC_Minutes=Hex_Decimal(xm[2],xm[1]);             //2014.10.11�޸�   ���TIMER��ʱ������
        NEW_set_alarm_pcf8563(RTC_Minutes);
    }
}
void Emial_time_EEPROM_write(void)
{
    UINT16 i,j;
    UINT8 xm[10]={0};
    UINT16 m1,m2,m3;
    UINT16 RTC_Minutes;
    UINT8 Write_Read_compare[7];
    UINT8 Emial_time_data_Cache[7];
    UINT8 Write_Read_compare_count=0;

    for(i=0;i<7;i++){Write_Read_compare[i]=0;Emial_time_data_Cache[i]=0;}
    FLAG_Write_Read_compare=0;

    m1=UART1_DATA[11];
    m2=18;
    for(i=11;i<m2;i++)Emial_time_data_Cache[i-11]=UART1_DATA[i];   //Emial_time_data[m1-1][i-11]=UART1_DATA[i];
    m1=UART1_DATA[11]-1;
    m2=m1/4;
    m3=m1%4;
    while((FLAG_Write_Read_compare==0)&&(Write_Read_compare_count<3))
    {
    //Write(Emial_time_data[m1],0xF00+32*m2+m3*7,7);//д�����ݵ�24LC16
    //Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Write(&Emial_time_data_Cache[0],0xF00+32*m2+m3*7,7);//д�����ݵ�24LC16
        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Read(&Write_Read_compare[0],0xF00+32*m2+m3*7,7);
        for(i=0;i<7;i++){
            if(Write_Read_compare[i]==Emial_time_data_Cache[i])FLAG_Write_Read_compare=1;
            else {FLAG_Write_Read_compare=0;i=7;}
        }
        Write_Read_compare_count++;
    }

    if(FLAG_Write_Read_compare==1){
        for(i=0;i<7;i++)Emial_time_data[m1][i]=Emial_time_data_Cache[i];
        Read_Time(&xm[0]);
        //RTC_Minutes=xm[2]*60+xm[1];
        RTC_Minutes=Hex_Decimal(xm[2],xm[1]);             //2014.10.11�޸�   ���TIMER��ʱ������
        NEW_set_alarm_pcf8563(RTC_Minutes);
    }
}
void HA_Change_EEPROM_write(void)
{
    UINT16 i;
    UINT8 Write_Read_compare[3];
    UINT8 HA_Change_send_email_Cache[3];
    UINT8 Write_Read_compare_count=0;

    for(i=0;i<3;i++){Write_Read_compare[i]=0;HA_Change_send_email_Cache[i]=0;}
    FLAG_Write_Read_compare=0;

//                HA_Change_send_email[0]=UART1_DATA[11];
//                HA_Change_send_email[1]=UART1_DATA[12];
//                HA_Change_send_email[2]=UART1_DATA[13];
    HA_Change_send_email_Cache[0]=UART1_DATA[11];
    HA_Change_send_email_Cache[1]=UART1_DATA[12];
    HA_Change_send_email_Cache[2]=UART1_DATA[13];
    while((FLAG_Write_Read_compare==0)&&(Write_Read_compare_count<3))
    {
//                Write(&HA_Change_send_email[0],0x7ED,3);    //д�����ݵ�24LC16
//                Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Write(&HA_Change_send_email_Cache[0],0x7ED,3);    //д�����ݵ�24LC16
        Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
        Read(&Write_Read_compare[0],0x7ED,3);
        for(i=0;i<3;i++){
            if(Write_Read_compare[i]==HA_Change_send_email_Cache[i])FLAG_Write_Read_compare=1;
            else {FLAG_Write_Read_compare=0;i=3;}
        }
        Write_Read_compare_count++;
    }    
    if(FLAG_Write_Read_compare==1){
        for(i=0;i<3;i++)HA_Change_send_email[i]=HA_Change_send_email_Cache[i];
    }
}

void EEPROM_write_To_or_Tc(UINT8 vlue_x,UINT8 vlue_y,UINT8 vlue_z)      //˵��  vlue_x-��ʲôλ��    vlue_y-����To/Tc   vlue_z-��To/Tcֵ
{
    UINT8 xm[2]={0};
    xm[0]=vlue_z;
    if(vlue_y==1){
        Write(&xm[0],0x3A0+vlue_x,1);
        ID_DATA_To[vlue_x]=xm[0];
    }
    else{
        Write(&xm[0],0x3C0+vlue_x,1);
        ID_DATA_Tc[vlue_x]=xm[0];
    }
    Delay100us(100);
}
#endif



void ID_EEPROM_write_pcs(void)
{
    UINT8 xm[3]={0};
    UINT16 i,m1,m2,m3;
    uni_rom_id xn;

     ID_DATA_PCS=0;
     xm[0]=ID_DATA_PCS%256;
     xm[1]=ID_DATA_PCS/256;
     Write(&xm[0],0x7FE,2);
     Delay100us(100);

     for(i=0;i<256;i++)ID_Receiver_DATA[i]=0;
}
void ID_EEPROM_write(void)
{
    UINT8 xm[3]={0};
    UINT16 m1,m2,m3;
    uni_rom_id xn;

     ID_DATA_PCS++;
     xm[0]=ID_DATA_PCS%256;
     xm[1]=ID_DATA_PCS/256;
     Write(&xm[0],0x7FE,2);
     Delay100us(100);
     ID_Receiver_DATA[ID_DATA_PCS-1]=ID_Receiver_Login;
#if defined(__Product_PIC32MX2_WIFI__)    //����2015.08.21׷��
     ID_DATA_To[ID_DATA_PCS-1]=16;
     ID_DATA_Tc[ID_DATA_PCS-1]=16;
#endif
     xn.IDL=ID_Receiver_Login;
     xm[0]=xn.IDB[0];
     xm[1]=xn.IDB[1];
     xm[2]=xn.IDB[2];
     m1=ID_DATA_PCS-1;
     m2=m1/10;
     m3=m1%10;
     Write(&xm[0],32*m2+m3*3,3);//д�����ݵ�24LC16
     Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
#if defined(__Product_PIC32MX2_Receiver__)
     if(ID_DATA_PCS>=256){ID_Login_EXIT_Initial();DATA_Packet_Control=0;time_Login_exit_256=110;}
#endif
}
void ID_EEPROM_write_0x00(void)
{
   UINT8 xm[3]={0};
   UINT16 n2,n3;
     xm[0]=0;
     xm[1]=0;
     xm[2]=0;
     ID_Receiver_DATA[INquiry]=0x00;
     n2=INquiry/10;
     n3=INquiry%10;
     Write(&xm[0],32*n2+n3*3,3);//д�����ݵ�24LC16
     Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms
}

#if defined(__Product_PIC32MX2_WIFI__)
void EEPROM_write_0x00toID(void)
{
    UINT8 xm[3]={0};
    UINT16 m1,m2,m3;
    uni_rom_id xn;

     ID_Receiver_DATA[INquiry_0x00]=ID_Receiver_Login;
     xn.IDL=ID_Receiver_Login;
     xm[0]=xn.IDB[0];
     xm[1]=xn.IDB[1];
     xm[2]=xn.IDB[2];
     m1=INquiry_0x00;
     m2=m1/10;
     m3=m1%10;
     Write(&xm[0],32*m2+m3*3,3);//д�����ݵ�24LC16
     Delay100us(100);            //д����ʱ��  24LCΪ5ms,24c��24wcΪ10ms

}
#endif

//===================start_i2c()�����ߺ���=======//
void start_i2c(void)
{
 SDA=1; //������ʼ�����������ź�
 NOP();
 NOP();
 SCL=1;
 Delayus(5);  //>4.7us
 SDA=0; //������ʼ�ź�
 Delayus(4);  //>4us
 SCL=0; //ǯסI2C����,׼���������ݻ��������
 NOP();
 NOP();
 }
//==================stop_i2c()ֹͣ���ߺ���=========//
void stop_i2c(void)
{
 SDA = 0; //���ͽ��������������ź�
 Delayus(5);  //>4.7us
 SCL = 1;
 Delayus(4);//>4us
 SDA = 1;
 Delayus(5);  //>4.7us
}
//===============Mack()����Ӧ���ź�===============//
void Mack(void) //����Ӧ���ź�
{
 //SDA = 1;
 NOP();
 NOP();
 SDAIO = 1;
 NOP();
 NOP();
 Delayus(5); //>4.7us
 SCL = 1;
 Delayus(4); //>4us
 //SDAIO = 1;
 if(SDA_I == 1)
     ack = 0;
 else
     ack = 1; //��ASK=1Ϊ��Ӧ���ź�
 SCL = 0;
 NOP();
 NOP();
 SDAIO = 0;
}
//=============send_byte()�ֽ�д����===============//
void send_byte(UINT8 c)
{
 UINT8 bit_count;
 for(bit_count = 0;bit_count < 8;bit_count++)
     {
      if((c << bit_count) & 0x80) SDA = 1;
      else  SDA = 0;
      NOP();
      Delayus(5); //>4.7us
      SCL = 1;
      Delayus(4); //>4us
      SCL = 0;
     }
 Mack();
}


#if defined(__Product_PIC32MX2_WIFI__)
//==============Erase_page()  24lc32��32���ֽ�Ϊһ��page=================//
void Erase_page(UINT8 Erase_data,UINT8 page)
{
 UINT16 i;
 start_i2c(); //��������
 /*************������24LC32��Ӧ��SOFT*************/
 send_byte(0xa0); //����������ַ
 i=page*32;
 send_byte(i/256); //�����ָߵ�ַ
 send_byte(i%256); //�����ֵ͵�ַ
 /************************************************/
 for (i = 0;i < 32;i++) send_byte(Erase_data); //�����ֽ�����
 stop_i2c(); //����ֹͣλ
}
#endif


//==============Write()дN�ֽ�����=================//
void Write(UINT8 *s,UINT16 suba,UINT8 n)                //24LC16B
{
 UINT16 i;
 start_i2c(); //��������
 /*************������24LC16��Ӧ��SOFT*************/
// i=suba/256;
// i=i*2+0xa0;
// send_byte(i); //����������ַ
// i=suba%256;
// send_byte(i); //�����ֵ�ַ
 /************************************************/
 /*************������24LC32��Ӧ��SOFT*************/
 send_byte(0xa0); //����������ַ
 i=suba;
 send_byte(i/256); //�����ָߵ�ַ
 send_byte(i%256); //�����ֵ͵�ַ
 /************************************************/
 for (i = 0;i < n;i++)
     {
      send_byte(*s); //�����ֽ�����
      s++;
     }
 stop_i2c(); //����ֹͣλ
}
//===============receive_byte()�ֽڶ�����=================//
UINT8 receive_byte(void)
{
 UINT8 retc, bit_count;
 retc = 0;
 for(bit_count = 0;bit_count < 8;bit_count++)
     {
      NOP();
      SCL = 0;
      Delayus(5);  //>4.7us
      //SDA =1;
      SCL = 1;
      Delayus(5); //>4.7us
      retc = retc << 1;
      if(SDA_I == 1)
          retc = retc + 1;
     }
 SCL = 0;
 NOP();
 NOP();
 return(retc);
}
//===============NOack()=======================//
void N0ack(char a) //����Ӧ��λ
{
 SDAIO = 0;
 NOP();
 if(a==0)SDA = 0;
 else  SDA = 1;
 Delayus(5);  //>4.7us
 SCL = 1;
 Delayus(4);//>4us
 SCL = 0;
}
//===============//
void Read(UINT8 *s,UINT16 suba,UINT8 n)
{
 UINT16 i ,j;
 start_i2c(); //��������
 /*************������24LC16��Ӧ��SOFT*************/
// i=suba/256;
// i=i*2+0xa0;
// send_byte(i);//����������ַ
// j=suba%256;
// send_byte(j); //�����ֵ�ַ
// start_i2c(); //������������
// i=i+1;
// send_byte(i); //���Ͷ������������ַ
 /************************************************/
 /*************������24LC32��Ӧ��SOFT*************/
 send_byte(0xa0); //����������ַ
 i=suba;
 send_byte(i/256); //�����ָߵ�ַ
 send_byte(i%256); //�����ֵ͵�ַ
 start_i2c(); //������������
 send_byte(0xa1); //���Ͷ�����
 /************************************************/
 for(i = 0;i < n - 1;i++)
     {
      SDAIO = 1;
      *s = receive_byte(); //�������
      s++;
      N0ack(0); //����Ӧ���ź�
     }
 SDAIO = 1;
 *s = receive_byte(); //�������
 N0ack(1); //���ͷ�Ӧ��λ
 stop_i2c(); //����ֹͣλ
}
