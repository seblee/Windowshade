
/***********************************************************************/
/*  FILE        :uart.c                                               */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :PIC32MX2     Crystal: 10/40M                          */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdlib.h>
#include <plib.h>    // 常用C定义
#include "initial.h" // 初始化
#include "pcf8563.h"

#if defined(__Product_PIC32MX2_Receiver__)
#include "Uart.h"
UINT8 UartStatus = 0;
UINT8 UartLen = 0;
UINT8 UartCount = 0;
UINT8 UART_DATA_buffer[493] = {0};
__Databits_t Databits_t;
__U1Statues U1Statues;
UINT8 ACKBack[3] = {0x02, 0x03, 0x00};
unsigned int U1AckTimer = 0;

#endif

void HA_uart_send_APP(void);

#define BaudRate 128
#if defined(__Product_PIC32MX2_WIFI__)
const UINT8 wifi_uart[6] = {0xBB, 0x00, 0x06, 0x80, 0x00, 0x00};
const UINT8 Emial_uart[10] = {0xBB, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const UINT8 HA_uart_open[5] = {79, 80, 69, 78, 32};
const UINT8 HA_uart_close[5] = {67, 76, 79, 83, 69};
const UINT8 HA_uart_err[5] = {69, 82, 82, 32, 32};

void UART_DATA_cope(void);
void uart_send_APP_allID(void);
void uart_send_APP_Head(void);
void uart_send_APP_allalarm(void);
void uart_send_APP_SUN(void);
void uart_send_APP_Emial_time(void);
void uart_send_APP_HA_Change(void);
void uart_send_APP_Public(UINT8 Public_X, UINT8 Public_Y);
void uart_send_APP_To_and_Tc(UINT8 Public_X, UINT8 Public_Y, UINT8 Public_Z);
#endif

void Uart1_Init(void)
{
#if defined(__Product_PIC32MX2_Receiver__)
    U1Busy_IO = Output;
    U1Busy_OUT = 1;
    RPA0R = 1; //Set RPA0-->U1TX
    CFGCONbits.IOLOCK = 0;
    U1RXR = 5; //Set U1RX-->RPC6
    CFGCONbits.IOLOCK = 1;
    U1BRG = BaudRate; //Set Baud rate
    U1STA = 0;
    U1MODEbits.ON = 1;  //Enable UART for 8-bit data
    U1MODEbits.UEN = 0; //
    U1MODEbits.RXINV = 0;
    U1MODEbits.LPBACK = 0;
    U1MODEbits.PDSEL = 2;
    //no parity, 1 Stop bit
    U1STAbits.URXEN = 1; //Enable Transmit and Receive
    U1STAbits.UTXEN = 1;

    IPC8bits.U1IP = 3; // Set priority level=1
    IPC8bits.U1IS = 3; // Set Subpriority level=1
    // Can be done in a single operation by assigning PC2SET = 0x0000000D
    IFS1bits.U1RXIF = 0; // Clear the timer interrupt status flag
    IEC1bits.U1RXIE = 1; // Enable timer interrupts
#endif

#if defined(__Product_PIC32MX2_WIFI__)
    RPA0R = 1; //Set RPA0-->U1TX
    U1RXR = 4; //Set U1RX-->RPB2

    U1BRG = BaudRate; //Set Baud rate
    U1STA = 0;
    U1MODEbits.ON = 1; //Enable UART for 8-bit data
    //no parity, 1 Stop bit
    U1STAbits.URXEN = 1; //Enable Transmit and Receive
    U1STAbits.UTXEN = 1;

    IPC8bits.U1IP = 3; // Set priority level=1
    IPC8bits.U1IS = 3; // Set Subpriority level=1
    // Can be done in a single operation by assigning PC2SET = 0x0000000D
    IFS1bits.U1RXIF = 0; // Clear the timer interrupt status flag
    IEC1bits.U1RXIE = 1; // Enable timer interrupts
#endif
}

#if defined(__Product_PIC32MX2_Receiver__)

void __ISR(_UART_1_VECTOR, ipl3) Uart1Handler(void)
{
    UINT8 Cache;
    Cache = U1RXREG;
    ReceiveFrame(Cache);
    IFS1bits.U1RXIF = 0;
}

void ReceiveFrame(UINT8 Cache)
{
    switch (UartStatus)
    {
    case FrameHeadSataus:
    {
        UART_DATA_buffer[0] = UART_DATA_buffer[1];
        UART_DATA_buffer[1] = UART_DATA_buffer[2];
        UART_DATA_buffer[2] = Cache;
        if ((UART_DATA_buffer[0] == FrameHead) &&
            (UART_DATA_buffer[2] == FrameSingnalID))
        {
            U1Statues = ReceivingStatues;
            UartStatus++;
            UartLen = UART_DATA_buffer[1];
        }
    }
    break;
    case DataStatus:
    {
        UART_DATA_buffer[UartCount + 3] = Cache;
        UartCount++;
        if (UartCount >= (UartLen - 3))
            UartStatus++;
    }
    break;
    default:
        UartStatus = 0;
        U1Statues = IdelStatues;
        break;
    }
    if (UartStatus == 0x02 /*FrameEndStatus*/) //接收完一帧处理数据
    {
        //add Opration function
        OprationFrame();
        UartStatus = 0;
        UartCount = 0;
        //        Receiver_LED_OUT_INV = !Receiver_LED_OUT_INV;
        U1Statues = ReceiveDoneStatues;
        U1AckTimer = U1AckDelayTime;
        U1Busy_OUT = 1;
    }
}

void OprationFrame(void)
{
    unsigned char i;
    for (i = 0; i < 4; i++)
        Databits_t.Data[i] = UART_DATA_buffer[3 + i];
    if (Databits_t.ID_No == 0x92)
    {
        ACKBack[2] = 0;
        switch (Databits_t.Mode)
        {
        case 0:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        default:
            ACKBack[2] = 1;
            return;
            break;
        }
        switch (Databits_t.Statues)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        default:
            ACKBack[2] = 1;
            return;
            break;
        }
        switch (Databits_t.Abnormal)
        {
        case 0x00:
            break;
        case 0x04:
            break;
        case 0x06:
            break;
        case 0x45:
            break;
        case 0x46:
            break;
        case 0x47:
            break;
        case 0x48:
            break;
        default:
            ACKBack[2] = 1;
            return;
            break;
        }
    }
    else
    {
        ACKBack[2] = 1;
        return;
    }
}
void TranmissionACK(void)
{
    unsigned char i = 0;
    if ((U1Statues == ReceiveDoneStatues) && (U1AckTimer == 0))
    {
        U1Busy_OUT = 1;
        if (UARTTransmitterIsReady(UART1))
        {
            do
            {
                while (!UARTTransmitterIsReady(UART1))
                    ;
                UARTSendDataByte(UART1, ACKBack[i]);
                i++;
            } while (i < 3);
            U1Statues = ACKingStatues;
        }
    }
    if (U1Statues == ACKingStatues)
    {
        if (UARTTransmissionHasCompleted(UART1))
        {
            U1Statues = IdelStatues;
            U1Busy_OUT = 1;
        }
    }
}

#endif
void SendOut(const char *Cache, unsigned int lenth)
{
    unsigned int i;
    for (i = 0; i < lenth; i++)
        PutCharacter(*(Cache + i));
}
void WriteString(const char *string)
{
    while (*string != '\0')
    {
        while (!UARTTransmitterIsReady(UART1))
            ;
        UARTSendDataByte(UART1, *string);
        string++;
    }
    while (!UARTTransmissionHasCompleted(UART1))
        ;
}
void PutCharacter(const char character)
{
    while (!UARTTransmitterIsReady(UART1))
        ;
    UARTSendDataByte(UART1, character);
    while (!UARTTransmissionHasCompleted(UART1))
        ;
}

#if defined(__Product_PIC32MX2_WIFI__)

void __ISR(_UART_1_VECTOR, ipl3) Uart1Handler(void)
{
    uni_i uart_x;
    UART_DATA_buffer[UART_DATA_cnt] = U1RXREG;
    if ((FLAG_UART_0xBB == 0) && (UART_DATA_cnt >= 1))
    {
        if (UART_DATA_buffer[UART_DATA_cnt] == 0xBB)
            TIME_UART = 2; //解决开机UART收到乱码，APP正常控制时不能正确收到。
        else if ((UART_DATA_buffer[UART_DATA_cnt - 1] == 0xBB) && (UART_DATA_buffer[UART_DATA_cnt] == 0x00))
        {
            TIME_UART = 13;
            UART_DATA_cnt = 1;
            UART_DATA_buffer[0] = 0xBB;
            UART_DATA_buffer[1] = 0x00;
            FLAG_UART_0xBB = 1;
        }
    }
    UART_DATA_cnt++;
    //            if(UART_DATA_buffer[6]==0x05){
    //                if(UART_DATA_cnt>=15){
    //                    UART_DATA_cnt=0;
    //                    FLAG_UART_0xBB=0;
    //                    for(UART_DATA_i=0;UART_DATA_i<15;UART_DATA_i++)UART1_DATA[UART_DATA_i]=UART_DATA_buffer[UART_DATA_i];
    //                    FLAG_UART_R=1;
    //                }
    //            }
    //            else if(UART_DATA_buffer[6]==0x08){
    //                if(UART_DATA_cnt>=18){
    //                    UART_DATA_cnt=0;
    //                    FLAG_UART_0xBB=0;
    //                    for(UART_DATA_i=0;UART_DATA_i<18;UART_DATA_i++)UART1_DATA[UART_DATA_i]=UART_DATA_buffer[UART_DATA_i];
    //                    FLAG_UART_R=1;
    //                }
    //            }
    //            else if(UART_DATA_buffer[6]==0x00){
    //                if(UART_DATA_cnt>=10){
    //                    UART_DATA_cnt=0;
    //                    FLAG_UART_0xBB=0;
    //                    /*****2013年11月22日修改  提高Emial稳定性****/
    //                    for(UART_DATA_i=0;UART_DATA_i<10;UART_DATA_i++)UART1_DATA[UART_DATA_i]=UART_DATA_buffer[UART_DATA_i];
    //                    FLAG_UART_R=1;
    //                    /******************************************/
    //                }
    //            }
    if ((UART_DATA_buffer[2] == 0x06) && (UART_DATA_buffer[3] == 0x80))
    {
        uart_x.uc[0] = UART_DATA_buffer[8];
        uart_x.uc[1] = UART_DATA_buffer[9];
        if (uart_x.ui == 0x0101)
            FG_HA_Inquiry_NO_again_send = 1; //2015.4.1修正3 由于APP查询受信器HA状态需要很长的时间，所以追加指令查询缓存在通信机里面的HA状态
        else
            FG_HA_Inquiry_NO_again_send = 0;
        switch (uart_x.ui)
        {
        case 0x0101: //卷帘门依次单个操作，HA状态取得
        case 0x0102:
        //case 0x0110:                //以下2015.08.21追加    取消该命令
        case 0x0111:
            UART_DATA_i = 18;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        case 0x0105: //RTC_write
            UART_DATA_i = 20;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            time_APP_Start_up = 500; //2015.04.27修正
            break;
        case 0x0106: //控制定时器设定要求
            if (UART_DATA_cnt >= 18)
            {
                if (UART_DATA_cnt == 18)
                    UART_DATA_i = 20 + UART_DATA_buffer[17] * 3;
                if (UART_DATA_cnt >= UART_DATA_i)
                    UART_DATA_cope();
            }
            break;
        case 0x0107: //控制定时器取得要求
        case 0x010A: //邮件定时器取得要求
            UART_DATA_i = 14;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        case 0x0108: //一齐操作
            if (UART_DATA_cnt >= 13)
            {
                if (UART_DATA_cnt == 13)
                    UART_DATA_i = 15 + UART_DATA_buffer[12] * 3;
                if (UART_DATA_cnt >= UART_DATA_i)
                    UART_DATA_cope();
            }
            break;
        case 0x0109: //邮件定时器设定要求
            UART_DATA_i = 20;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        case 0x010B: //日出日落设定要求
            if (UART_DATA_cnt >= 16)
            {
                if (UART_DATA_cnt == 16)
                    UART_DATA_i = 18 + UART_DATA_buffer[15] * 3;
                if (UART_DATA_cnt >= UART_DATA_i)
                    UART_DATA_cope();
            }
            break;
        case 0x0103: //APP获取卷帘门ID全部
        case 0x0104: //RTC_read
        case 0x010C: //日出日落取得要求
        case 0x010E: //卷帘门状态变化是否邮件送信取得
        case 0x010F: //集中通讯机版本取得
        case 0x01F1: //日出日落表格数据DATA取得
            UART_DATA_i = 13;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        case 0x010D: //卷帘门状态变化是否邮件送信设定
            UART_DATA_i = 16;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        case 0x01F0: //日出日落表格数据DATA设置
            UART_DATA_i = 493;
            if (UART_DATA_cnt >= UART_DATA_i)
                UART_DATA_cope();
            break;
        default:
            break;
        }
    }
    else if ((UART_DATA_buffer[2] == 0x20) && (UART_DATA_buffer[3] == 0x80))
    {
        UART_DATA_i = 10;
        if (UART_DATA_cnt >= UART_DATA_i)
            UART_DATA_cope();
    }
    if ((UART_DATA_cnt >= 493) || ((UART_DATA_cnt > 2) && (TIME_UART == 0)))
    {
        UART_DATA_cnt = 1;
        FLAG_UART_0xBB = 0;
    } //UART_DATA_cnt=0-->1  解决UART收到乱码后，每第一次接收数据失败，后面就好了。
    IFS1bits.U1RXIF = 0;
}

void UART_DATA_cope(void)
{
    UINT16 UART_DATA_j;
    UART_DATA_cnt = 0;
    FLAG_UART_0xBB = 0;
    for (UART_DATA_j = 0; UART_DATA_j < UART_DATA_i; UART_DATA_j++)
        UART1_DATA[UART_DATA_j] = UART_DATA_buffer[UART_DATA_j];
    FLAG_UART_R = 1;
}
#endif

void UART_Decode(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i, j;
    UINT16 m = 0;
    UINT16 n = 0;
    uni_rom_id y;
    uni_i uart_y;
    UINT8 xm00[10] = {0};
    UINT16 RTC_Minutes00;

    if (UART1_DATA[2] == 0x06)
    { /*****2013年11月22日修改  提高Emial稳定性****/
        for (i = 0; i < 6; i++)
        {
            if (UART1_DATA[i] == wifi_uart[i])
                ;
            else
            {
                FLAG_UART_ok = 0;
                return;
            }
        }
        uart_y.uc[0] = UART1_DATA[8];
        uart_y.uc[1] = UART1_DATA[9];
        switch (uart_y.ui)
        {
        case 0x0101: //卷帘门依次单个HA状态取得
            //case 0x0110:         //以下2015.08.21追加    取消该命令
            APP_check_char = 0; //2014.10.11修改
        case 0x0102:            //卷帘门依次单个操作
            for (i = 8; i < 16; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[16] + UART1_DATA[17] * 256;
            if (m == n)
            {
                //20150501 JAPAN追加  解决是反复启动APP时，在查询通信机缓沉内部HA状态时，将Control_code=1控制出去了（在之前有一齐操作的情况，正在实施当中）
                ID_data_uart_CMD0101_01.IDB[0] = UART1_DATA[11];
                ID_data_uart_CMD0101_01.IDB[1] = UART1_DATA[12];
                ID_data_uart_CMD0101_01.IDB[2] = UART1_DATA[13];
                ID_data_uart_CMD0101_01.IDB[3] = 0x00;
                if ((uart_y.ui == 0x0101) && (UART1_DATA[14] == 0x01))
                    ;
                else
                    Control_code = UART1_DATA[14];
                //eeprom_IDcheck_UART();
                eeprom_IDcheck_CMD0101_01_UART();
                if (FLAG_IDCheck_OK == 1)
                {
                    //if(Control_code==0x00){FLAG_HA_Inquiry=1;DATA_Packet_Control_0=0x00;}    //表示APP查询
                    if ((uart_y.ui == 0x0101) && (UART1_DATA[14] == 0x01))
                    { //2015.4.1修正3 由于APP查询受信器HA状态需要很长的时间，所以追加指令查询缓存在通信机里面的HA状态
                        Emial_Cache_HA = 0;
                        Emial_Cache_SWITCH = 0;
                        if (Email_check_TO_APP())
                        {
                            Control_code = 0x00;
                            goto CMD0101_01_to_00;
                        }
                        uart_send_APP_Head();
                        U1TXREG = 0x08;
                        U1TXREG = 0x00;
                        Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                        U1TXREG = 0x01;
                        U1TXREG = 0x01;
                        U1TXREG = 0x00;
                        m = 2;
                        U1TXREG = UART1_DATA[11];
                        m = m + UART1_DATA[11];
                        U1TXREG = UART1_DATA[12];
                        m = m + UART1_DATA[12];
                        U1TXREG = UART1_DATA[13];
                        m = m + UART1_DATA[13];
                        Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                        U1TXREG = Emial_Cache_HA;
                        m = m + Emial_Cache_HA;
                        U1TXREG = Emial_Cache_SWITCH;
                        m = m + Emial_Cache_SWITCH;
                        U1TXREG = m % 256;
                        U1TXREG = m / 256;

                        FLAG_IDCheck_OK = 0;
                        time_APP_Start_up = 5;
                    }
                    else
                    {
                        if ((uart_y.ui == 0x0102) && (UART1_DATA[14] >= 0x80))
                        { //以下2015.08.21追加
                            IDcheck_CMD0102_HA_Cache();
                            if (FLAG_IDcheck_CMD0102_HA == 1)
                            {
                                FLAG_IDcheck_CMD0102_HA = 0;
                                if ((HA_Cache_ha[CMD0102_To_or_Tc_HA] == 2) && (UART1_DATA[14] == 0x80))
                                    Control_code = 0x80 + ID_DATA_To[CMD0102_To_or_Tc_place];
                                else if ((HA_Cache_ha[CMD0102_To_or_Tc_HA] == 1) && (UART1_DATA[14] == 0xC0))
                                    Control_code = 0xC0 + ID_DATA_Tc[CMD0102_To_or_Tc_place];
                                else
                                    goto CMD0102_NG;
                            }
                            else
                                goto CMD0102_NG;
                        }
                    CMD0101_01_to_00:
                        ID_data.IDB[0] = ID_data_uart_CMD0101_01.IDB[0];
                        ID_data.IDB[1] = ID_data_uart_CMD0101_01.IDB[1];
                        ID_data.IDB[2] = ID_data_uart_CMD0101_01.IDB[2];
                        ID_data.IDB[3] = 0x00;
                        if ((Control_code == 0x00) || (Control_code == 0x02) || (Control_code == 0x08))
                        {
                            FLAG_HA_Inquiry = 1;
                            DATA_Packet_Control_0 = 0x00;
                        } //表示APP查询
                        FLAG_IDCheck_OK = 0;
                        FLAG_UART_ok = 1;
                        time_APP_Start_up = 0; //2015.04.27修正
                    }
                }
                else
                {
                CMD0102_NG:
                    HA_uart_app[8] = UART1_DATA[8];
                    HA_uart_app[9] = UART1_DATA[9];
                    HA_uart_app[10] = 0x01;
                    HA_uart_app[11] = UART1_DATA[11];
                    HA_uart_app[12] = UART1_DATA[12];
                    HA_uart_app[13] = UART1_DATA[13];
                    HA_uart_app[14] = 0x00;
                    HA_uart_app[15] = 0x00;
                    n = n - UART1_DATA[14] + 1;
                    HA_uart_app[16] = n % 256;
                    HA_uart_app[17] = n / 256;

                    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                    for (i = 0; i < 18; i++)
                    {
                        U1TXREG = HA_uart_app[i];
                        if (i % 6 == 0)
                            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                    }
                }
            }
            else
            {
                uart_send_APP_Public(UART1_DATA[8], 1);
                FLAG_UART_ok = 0;
            }
            break;
        case 0x0104: //RTC_read
        case 0x0105: //RTC_write
            if (UART1_DATA[8] == 0x04)
            {
                for (i = 8; i < 11; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[11] + UART1_DATA[12] * 256;
            }
            else
            {
                for (i = 8; i < 18; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[18] + UART1_DATA[19] * 256;
            }
            if (m == n)
            {
                if (UART1_DATA[8] == 0x05)
                {
                    Set_Time(&UART1_DATA[11]); //==0x00  写时钟
                    SUN_time_get(SUN_ON_OFF_seat[2]);
                    Read_Time(&xm00[0]);
                    //RTC_Minutes00=xm00[2]*60+xm00[1];
                    RTC_Minutes00 = Hex_Decimal(xm00[2], xm00[1]); //2014.10.11修改   解决TIMER有时不动作
                    NEW_set_alarm_pcf8563(RTC_Minutes00);
                    uart_send_APP_Public(0x05, 0);
                }
                else if (UART1_DATA[8] == 0x04)
                { //==0x01  读时钟
                    Read_Time(number_time);
                    uart_send_APP_Head();
                    U1TXREG = 10;
                    U1TXREG = 0x00;
                    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                    U1TXREG = 0x04;
                    U1TXREG = 0x01;
                    U1TXREG = 0x00;
                    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                    m = 0;
                    for (i = 0; i < 7; i++)
                    {
                        U1TXREG = number_time[i];
                        m = m + number_time[i];
                    }
                    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                    m = m + 0x05;
                    U1TXREG = m % 256;
                    U1TXREG = m / 256;
                }
            }
            else
                uart_send_APP_Public(UART1_DATA[8], 1);
            break;
        case 0x0106: //控制定时器设定要求
        case 0x0107: //控制定时器取得要求
            if (UART1_DATA[8] == 0x06)
            {
                for (i = 8; i < 18 + UART1_DATA[17] * 3; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[18 + UART1_DATA[17] * 3] + UART1_DATA[19 + UART1_DATA[17] * 3] * 256;
            }
            else
            {
                for (i = 8; i < 12; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[12] + UART1_DATA[13] * 256;
            }
            if (m == n)
            {
                if (UART1_DATA[8] == 0x06)
                {
                    //if((UART1_DATA[11]==0)||(UART1_DATA[11]>0x0A)||(UART1_DATA[12]>1)||(UART1_DATA[13]>9)||(UART1_DATA[14]>25)||(UART1_DATA[15]>60)||(UART1_DATA[17]==0))
                    if ((UART1_DATA[11] == 0) || (UART1_DATA[11] > 0x0A) || (UART1_DATA[14] > 0x25) || (UART1_DATA[15] > 0x60))
                        uart_send_APP_Public(0x06, 1);
                    else
                    {
                        eeprom_IDcheck_Multiple(17);
                        if (FLAG_IDCheck_OK == 1)
                        {
                            FLAG_IDCheck_OK = 0;
                            alarm_EEPROM_write();
                            if (FLAG_Write_Read_compare == 1)
                                uart_send_APP_Public(0x06, 0);
                            else
                                uart_send_APP_Public(0x06, 1);
                        }
                        else
                            uart_send_APP_Public(0x06, 1);
                    }
                }
                else
                    uart_send_APP_allalarm();
            }
            else
                uart_send_APP_Public(UART1_DATA[8], 1);
            break;
        case 0x0109: //邮件定时器设定要求
        case 0x010A: //邮件定时器取得要求
            if (UART1_DATA[8] == 0x09)
            {
                for (i = 8; i < 18; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[18] + UART1_DATA[19] * 256;
            }
            else
            {
                for (i = 8; i < 12; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[12] + UART1_DATA[13] * 256;
            }
            if (m == n)
            {
                if (UART1_DATA[8] == 0x09)
                {
                    if ((UART1_DATA[11] == 0) || (UART1_DATA[11] > 0x0A) || (UART1_DATA[13] > 0x25) || (UART1_DATA[14] > 0x60))
                        uart_send_APP_Public(0x09, 1);
                    else
                    {
                        Emial_time_EEPROM_write();
                        if (FLAG_Write_Read_compare == 1)
                            uart_send_APP_Public(0x09, 0);
                        else
                            uart_send_APP_Public(0x09, 1);
                    }
                }
                else
                    uart_send_APP_Emial_time();
            }
            else
                uart_send_APP_Public(UART1_DATA[8], 1);
            break;
        case 0x0111: //以下2015.08.21追加        //半开To 半闭Tc设定和读取
            for (i = 8; i < 16; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[16] + UART1_DATA[17] * 256;
            if (m == n)
            {
                ID_data_uart_CMD0111.IDB[0] = UART1_DATA[11];
                ID_data_uart_CMD0111.IDB[1] = UART1_DATA[12];
                ID_data_uart_CMD0111.IDB[2] = UART1_DATA[13];
                ID_data_uart_CMD0111.IDB[3] = 0x00;
                eeprom_IDcheck_CMD0111_UART();
                if (FLAG_IDCheck_OK == 1)
                {
                    if (((UART1_DATA[14] == 1) || (UART1_DATA[14] == 3)) && (UART1_DATA[15] != 0))
                    {
                        EEPROM_write_To_or_Tc(ID_DATA_To_or_Tc_place, UART1_DATA[14], UART1_DATA[15]);
                        uart_send_APP_To_and_Tc(0, UART1_DATA[14], 0);
                    }
                    else if (UART1_DATA[14] == 2)
                        uart_send_APP_To_and_Tc(0, UART1_DATA[14], ID_DATA_To[ID_DATA_To_or_Tc_place]);
                    else if (UART1_DATA[14] == 4)
                        uart_send_APP_To_and_Tc(0, UART1_DATA[14], ID_DATA_Tc[ID_DATA_To_or_Tc_place]);
                    else
                        uart_send_APP_To_and_Tc(1, UART1_DATA[14], 0);
                }
                else
                    uart_send_APP_To_and_Tc(1, UART1_DATA[14], 0);
            }
            else
                uart_send_APP_To_and_Tc(1, UART1_DATA[14], 0);
            break;
        case 0x010B: //日出日落设定要求
        case 0x010C: //日出日落取得要求
            if (UART1_DATA[8] == 0x0B)
            {
                for (i = 8; i < 16 + UART1_DATA[15] * 3; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[16 + UART1_DATA[15] * 3] + UART1_DATA[17 + UART1_DATA[15] * 3] * 256;
            }
            else
            {
                for (i = 8; i < 11; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[11] + UART1_DATA[12] * 256;
            }
            if (m == n)
            {
                if (UART1_DATA[8] == 0x0B)
                {
                    if ((UART1_DATA[13] > 10) || (UART1_DATA[14] > 0x80) || (UART1_DATA[15] > 32))
                        uart_send_APP_Public(0x0B, 1);
                    else
                    {
                        eeprom_IDcheck_Multiple(15);
                        if (FLAG_IDCheck_OK == 1)
                        {
                            FLAG_IDCheck_OK = 0;
                            SUN_EEPROM_write();
                            if (FLAG_Write_Read_compare == 1)
                                uart_send_APP_Public(0x0B, 0);
                            else
                                uart_send_APP_Public(0x0B, 1);
                        }
                        else
                            uart_send_APP_Public(0x0B, 1);
                    }
                }
                else
                    uart_send_APP_SUN();
            }
            else
                uart_send_APP_Public(UART1_DATA[8], 1);
            break;
        case 0x010D: //卷帘门状态变化是否邮件送信设定
        case 0x010E: //卷帘门状态变化是否邮件送信取得
            if (UART1_DATA[8] == 0x0D)
            {
                for (i = 8; i < 14; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[14] + UART1_DATA[15] * 256;
            }
            else
            {
                for (i = 8; i < 11; i++)
                    m += UART1_DATA[i];
                n = UART1_DATA[11] + UART1_DATA[12] * 256;
            }
            if (m == n)
            {
                if (UART1_DATA[8] == 0x0D)
                {
                    HA_Change_EEPROM_write();
                    if (FLAG_Write_Read_compare == 1)
                        uart_send_APP_Public(0x0D, 0);
                    else
                        uart_send_APP_Public(0x0D, 1);
                }
                else
                    uart_send_APP_HA_Change();
            }
            else
                uart_send_APP_Public(UART1_DATA[8], 1);
            break;
        case 0x0103: //APP获取卷帘门ID全部
            for (i = 8; i < 11; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[11] + UART1_DATA[12] * 256;
            if (m == n)
            {
                uart_send_APP_allID();
            }
            else
                uart_send_APP_Public(0x03, 1);
            break;
        case 0x010F: //APP获取软件版本
            for (i = 8; i < 11; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[11] + UART1_DATA[12] * 256;
            if (m == n)
            {
                uart_send_APP_Head();
                U1TXREG = 0x09;
                U1TXREG = 0x00;
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                U1TXREG = 0x0F;
                U1TXREG = 0x01;
                U1TXREG = 0x00;
                U1TXREG = 0x56; //V
                U1TXREG = 0x65; //e
                U1TXREG = 0x72; //r
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                U1TXREG = 0x36; //6              //2014.10.11修改
                U1TXREG = 0x2E; //.
                U1TXREG = 0x37; //7
                U1TXREG = 0xD8; //0x16B+0x33+0x39
                U1TXREG = 0x01;
            }
            else
                uart_send_APP_Public(0x0F, 1);
            break;
        case 0x0108: //一齐操作
            for (i = 8; i < 13 + UART1_DATA[12] * 3; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[13 + UART1_DATA[12] * 3] + UART1_DATA[14 + UART1_DATA[12] * 3] * 256;
            if (m == n)
            {
                eeprom_IDcheck_Multiple(12);
                if (FLAG_IDCheck_OK == 1)
                {
                    FLAG_IDCheck_OK = 0;
                    uart_send_APP_Public(0x08, 0);
                    uart_Control_code = UART1_DATA[11];
                    for (i = 0; i < UART1_DATA[12]; i++)
                        APP_UART_OUT(i); //2015.4.11追加修正3
                    //for(i=UART1_DATA[12];i>0;i--) APP_UART_OUT(i-1);
                }
                else
                    uart_send_APP_Public(0x08, 1);
            }
            else
                uart_send_APP_Public(0x08, 1);
            break;
        case 0x01F0: //日出日落表格数据DATA设置
            for (i = 8; i < 491; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[491] + UART1_DATA[492] * 256;
            if (m == n)
            {
                Sunrise_sunset_EEPROM_write();
                uart_send_APP_Public(0xF0, 0);
            }
            else
                uart_send_APP_Public(0xF0, 1);
            break;
        case 0x01F1: //日出日落表格数据DATA取得
            for (i = 8; i < 11; i++)
                m += UART1_DATA[i];
            n = UART1_DATA[11] + UART1_DATA[12] * 256;
            if (m == n)
            {
                uart_send_APP_Head();
                U1TXREG = 0xE3;
                U1TXREG = 0x01;
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                U1TXREG = 0xF1;
                U1TXREG = 0x01;
                U1TXREG = 0x00;
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                m = 0;
                for (i = 0; i < 480; i++)
                {
                    U1TXREG = Sunrise_sunset_DATA[i];
                    m = m + Sunrise_sunset_DATA[i];
                    if (i % 6 == 5)
                        Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                }
                m = m + 0xF2;
                U1TXREG = m % 256;
                U1TXREG = m / 256;
            }
            else
                uart_send_APP_Public(0xF1, 1);
            break;
        default:
            break;
        }
    } /*****2013年11月22日修改  提高Emial稳定性****/
    else if (UART1_DATA[2] == 0x20)
    {
        for (i = 0; i < 10; i++)
        {
            if (UART1_DATA[i] == Emial_uart[i])
                ;
            else
                return;
        }
        FLAG_email_Repeat = 0;
    }
/******************************************/
#endif
}

void HA_uart_email(UINT8 EMIAL_id_PCS_x)
{
#if defined(__Product_PIC32MX2_WIFI__)
    /*
     UINT8 h,l;
     UINT8 h1,bc[8];
     UINT16 m,i,j;
     UINT32 h0;

     //uart_send_APP_Public(0xFF,0);               //测试是否发送了邮件
     Delay100us(30);

 //    HA_uart[8]=HA_Change_send_email[1];       //调用该该函数之前  对接收邮件地址编号进行设置
 //    HA_uart[9]=HA_Change_send_email[2];

     Read_Time(number_time);           //计算邮件标题
     for(i=1;i<7;i++){
         if(i!=4){
             h=number_time[i];
             if(i<=3)j=24-(i-1)*3;
             else j=24-(i-2)*3;
             l=h&0x0F;
             HA_uart[j+1]=l+0x30;
             h=h&0xF0;
             h=h>>4;
             HA_uart[j]=h+0x30;
         }
     }
     HA_uart[26]=0x00;   //邮件标题结束符

     //HA_uart_Length=63+8;
     HA_uart_Length=63+15;
     //HA_uart_Length=63+8+3;
     for(j=0;j<EMIAL_id_PCS_x;j++){                      //计算邮件内容的atatus=...部分
         if((EMIAL_id_HA[j]==0x81)||(EMIAL_id_HA[j]==0x85)){
             HA_uart[HA_uart_Length]=111;      //open
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=112;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=101;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=110;
             HA_uart_Length++;
         }
         else if((EMIAL_id_HA[j]==0x82)||(EMIAL_id_HA[j]==0x86)){
             HA_uart[HA_uart_Length]=99;      //close
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=108;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=111;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=115;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=101;
             HA_uart_Length++;
         }
    #if defined(__32MX230F064D__)
         else if((EMIAL_id_HA[j]==0x83)||(EMIAL_id_HA[j]==0x87)||(EMIAL_id_HA[j]==0x84)||(EMIAL_id_HA[j]==0x88)){
             HA_uart[HA_uart_Length]=101;      //error
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=111;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
         }
     #endif
     #if defined(__32MX250F128D__)
         else if((EMIAL_id_HA[j]==0x83)||(EMIAL_id_HA[j]==0x87)){
             HA_uart[HA_uart_Length]=101;      //error1
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=111;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=49;
             HA_uart_Length++;
         }
         else if((EMIAL_id_HA[j]==0x84)||(EMIAL_id_HA[j]==0x88)){
             HA_uart[HA_uart_Length]=101;      //error2
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=111;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=114;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=50;
             HA_uart_Length++;
         }
         else if((EMIAL_id_HA[j]==0xFF)||(EMIAL_id_HA[j]==0x00)){
             HA_uart[HA_uart_Length]=102;      //fail
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=97;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=105;
             HA_uart_Length++;
             HA_uart[HA_uart_Length]=108;
             HA_uart_Length++;
         }
     #endif


         if(j!=(EMIAL_id_PCS_x-1)){
             HA_uart[HA_uart_Length]=44;   //,
             HA_uart_Length++;
         }
     }

     HA_uart[HA_uart_Length]=38;   //&id=
     HA_uart_Length++;
     HA_uart[HA_uart_Length]=105;
     HA_uart_Length++;
     HA_uart[HA_uart_Length]=100;
     HA_uart_Length++;
     HA_uart[HA_uart_Length]=61;
     HA_uart_Length++;      

     for(j=0;j<EMIAL_id_PCS_x;j++){      //计算邮件内容的&id=...部分
         h0=EMIAL_id_data[j];
         h1=0;
         for(i=8;i>0;i--){
             h=h0%10;
             bc[h1]=h+0x30;
             h1++;
             h0=h0/10;
             if(h0==0)i=1;
         }
         for(i=h1;i>0;i--){
             HA_uart[HA_uart_Length]=bc[i-1];
             HA_uart_Length++;
         }
         if(j!=(EMIAL_id_PCS_x-1)){
             HA_uart[HA_uart_Length]=44;   //,
             HA_uart_Length++;
         }
     }
     HA_uart[HA_uart_Length]=0x00;   //邮件内容结束符
     HA_uart_Length++;

     m=HA_uart_Length-8;            //计算数据长度
     HA_uart[6]=m%256;
     HA_uart[7]=m/256;

     m=0;                       //计算CRC16
     for(i=8;i<HA_uart_Length;i++)m=m+HA_uart[i];
     HA_uart[HA_uart_Length]=m%256;
     HA_uart[HA_uart_Length+1]=m/256;

     j=HA_uart_Length+2;
     if(ID_DATA_PCS!=0){
         for(i=0;i<j;i++){
             U1TXREG=HA_uart[i];
             if(i%6==0)Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
         }
     }
        Delay100us(300);
        TIME_email_Repeat=9000;
        FLAG_email_Repeat=1;
        UART_send_count=0;

        for(i=0;i<35;i++){
            Email_check_ID[i]=EMIAL_id_data[i];
            //EMIAL_id_data[i]=0;    //20150430 japan修改2
            Emial_check_Control[i]=EMIAL_id_HA[i];
            EMIAL_id_HA[i]=0;
        }
        EMIAL_id_PCS=0;
   // HA_uart_send_APP();
     */

    UINT8 h, l;
    UINT8 h1, bc[8];
    UINT16 m, i, j;
    UINT32 h0;

    //uart_send_APP_Public(0xFF,0);               //测试是否发送了邮件
    Delay100us(30);

    //    HA_uart[8]=HA_Change_send_email[1];       //调用该该函数之前  对接收邮件地址编号进行设置
    //    HA_uart[9]=HA_Change_send_email[2];

    Read_Time(number_time); //计算邮件标题
    for (i = 1; i < 7; i++)
    {
        if (i != 4)
        {
            h = number_time[i];
            if (i <= 3)
                j = 24 - (i - 1) * 3;
            else
                j = 24 - (i - 2) * 3;
            l = h & 0x0F;
            HA_uart[j + 1] = l + 0x30;
            h = h & 0xF0;
            h = h >> 4;
            HA_uart[j] = h + 0x30;
        }
    }
    HA_uart[26] = 0x00; //邮件标题结束符

    //HA_uart_Length=27+23+19;
    HA_uart_Length = 27 + 27 + 19;
    for (j = 0; j < EMIAL_id_PCS_x; j++)
    { //计算邮件内容的atatus=...部分
        if ((EMIAL_id_HA[j] == 0x81) || (EMIAL_id_HA[j] == 0x85))
        {
            HA_uart[HA_uart_Length] = 111; //open
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 112;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 101;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 110;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0x82) || (EMIAL_id_HA[j] == 0x86))
        {
            HA_uart[HA_uart_Length] = 99; //close
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 108;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 115;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 101;
            HA_uart_Length++;
        }
#if defined(__32MX230F064D__) else if ((EMIAL_id_HA[j] == 0x83) || (EMIAL_id_HA[j] == 0x87) || (EMIAL_id_HA[j] == 0x84) || (EMIAL_id_HA[j] == 0x88))
        {
            HA_uart[HA_uart_Length] = 101; //error
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
        }
#endif
#if defined(__32MX250F128D__)
        else if ((EMIAL_id_HA[j] == 0x83) || (EMIAL_id_HA[j] == 0x87))
        {
            HA_uart[HA_uart_Length] = 101; //error1
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 49;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0x84) || (EMIAL_id_HA[j] == 0x88))
        {
            HA_uart[HA_uart_Length] = 101; //error2
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 50;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0xFF) || (EMIAL_id_HA[j] == 0x00))
        {
            HA_uart[HA_uart_Length] = 102; //fail
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 97;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 105;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 108;
            HA_uart_Length++;
        }
#endif

        if (j != (EMIAL_id_PCS_x - 1))
        {
            HA_uart[HA_uart_Length] = 44; //,
            HA_uart_Length++;
        }
    }

    HA_uart[HA_uart_Length] = 38; //&id=
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 105;
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 100;
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 61;
    HA_uart_Length++;

    for (j = 0; j < EMIAL_id_PCS_x; j++)
    { //计算邮件内容的&id=...部分
        h0 = EMIAL_id_data[j];
        h1 = 0;
        for (i = 8; i > 0; i--)
        {
            h = h0 % 10;
            bc[h1] = h + 0x30;
            h1++;
            h0 = h0 / 10;
            if (h0 == 0)
                i = 1;
        }
        for (i = h1; i > 0; i--)
        {
            HA_uart[HA_uart_Length] = bc[i - 1];
            HA_uart_Length++;
        }
        if (j != (EMIAL_id_PCS_x - 1))
        {
            HA_uart[HA_uart_Length] = 44; //,
            HA_uart_Length++;
        }
    }

    HA_uart[HA_uart_Length] = 13; //回车
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 10; //换行
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 13; //回车
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 10; //换行
    HA_uart_Length++;
    //for(j=0;j<45;j++)
    for (j = 0; j < 49; j++)
    {
        HA_uart[HA_uart_Length] = HA_uart_ios[j];
        HA_uart_Length++;
    }
    for (j = 0; j < EMIAL_id_PCS_x; j++)
    { //计算邮件内容的atatus=...部分
        if ((EMIAL_id_HA[j] == 0x81) || (EMIAL_id_HA[j] == 0x85))
        {
            HA_uart[HA_uart_Length] = 111; //open
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 112;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 101;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 110;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0x82) || (EMIAL_id_HA[j] == 0x86))
        {
            HA_uart[HA_uart_Length] = 99; //close
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 108;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 115;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 101;
            HA_uart_Length++;
        }
#if defined(__32MX230F064D__) else if ((EMIAL_id_HA[j] == 0x83) || (EMIAL_id_HA[j] == 0x87) || (EMIAL_id_HA[j] == 0x84) || (EMIAL_id_HA[j] == 0x88))
        {
            HA_uart[HA_uart_Length] = 101; //error
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
        }
#endif
#if defined(__32MX250F128D__)
        else if ((EMIAL_id_HA[j] == 0x83) || (EMIAL_id_HA[j] == 0x87))
        {
            HA_uart[HA_uart_Length] = 101; //error1
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 49;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0x84) || (EMIAL_id_HA[j] == 0x88))
        {
            HA_uart[HA_uart_Length] = 101; //error2
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 111;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 114;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 50;
            HA_uart_Length++;
        }
        else if ((EMIAL_id_HA[j] == 0xFF) || (EMIAL_id_HA[j] == 0x00))
        {
            HA_uart[HA_uart_Length] = 102; //fail
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 97;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 105;
            HA_uart_Length++;
            HA_uart[HA_uart_Length] = 108;
            HA_uart_Length++;
        }
#endif
        if (j != (EMIAL_id_PCS_x - 1))
        {
            HA_uart[HA_uart_Length] = 44; //,
            HA_uart_Length++;
        }
    }

    HA_uart[HA_uart_Length] = 38; //&id=
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 105;
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 100;
    HA_uart_Length++;
    HA_uart[HA_uart_Length] = 61;
    HA_uart_Length++;

    for (j = 0; j < EMIAL_id_PCS_x; j++)
    { //计算邮件内容的&id=...部分
        h0 = EMIAL_id_data[j];
        h1 = 0;
        for (i = 8; i > 0; i--)
        {
            h = h0 % 10;
            bc[h1] = h + 0x30;
            h1++;
            h0 = h0 / 10;
            if (h0 == 0)
                i = 1;
        }
        for (i = h1; i > 0; i--)
        {
            HA_uart[HA_uart_Length] = bc[i - 1];
            HA_uart_Length++;
        }
        if (j != (EMIAL_id_PCS_x - 1))
        {
            HA_uart[HA_uart_Length] = 44; //,
            HA_uart_Length++;
        }
    }

    HA_uart[HA_uart_Length] = 0x00; //邮件内容结束符
    HA_uart_Length++;

    m = HA_uart_Length - 8; //计算数据长度
    HA_uart[6] = m % 256;
    HA_uart[7] = m / 256;

    m = 0; //计算CRC16
    for (i = 8; i < HA_uart_Length; i++)
        m = m + HA_uart[i];
    HA_uart[HA_uart_Length] = m % 256;
    HA_uart[HA_uart_Length + 1] = m / 256;

    j = HA_uart_Length + 2;
    if (ID_DATA_PCS != 0)
    {
        Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
        for (i = 0; i < j; i++)
        {
            U1TXREG = HA_uart[i];
            if (i % 6 == 0)
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
        }
    }
    Delay100us(300);
    TIME_email_Repeat = 9000;
    FLAG_email_Repeat = 1;
    UART_send_count = 0;

    for (i = 0; i < 35; i++)
    {
        Email_check_ID[i] = EMIAL_id_data[i];
        //EMIAL_id_data[i]=0;    //20150430 japan修改2
        Emial_check_Control[i] = EMIAL_id_HA[i];
        EMIAL_id_HA[i] = 0;
    }
    EMIAL_id_PCS = 0;
// HA_uart_send_APP();
#endif
}

void HA_uart_email_Repeat(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT8 i;
    //uart_send_APP_Public(0xFF,0);               //测试是否发送了邮件
    for (i = 0; i < HA_uart_Length + 2; i++)
    {
        U1TXREG = HA_uart[i];
        if (i % 6 == 0)
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    }
    Delay100us(300);
#endif
}

void HA_uart_send_APP(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    uni_rom_id b0;
    UINT8 i;
    UINT16 m;

    //    HA_uart_app[9]=0x01;
    //    if(UART_DATA_buffer[8]==0x01)HA_uart_app[8]=0x01;
    //    else if(UART_DATA_buffer[8]==0x02){HA_uart_app[8]=0x02;UART_DATA_buffer[8]=0x00;}
    //    else if(UART_DATA_buffer[8]==0x10)HA_uart_app[8]=0x10;
    //    else HA_uart_app[8]=0x01;

    HA_uart_app[9] = 0x01;
    if (UART1_DATA[8] == 0x02)
        HA_uart_app[8] = 0x02;
    else
        HA_uart_app[8] = 0x01;

    HA_uart_app[10] = 0x00;
    b0.IDL = DATA_Packet_ID;
    HA_uart_app[11] = b0.IDB[0];
    HA_uart_app[12] = b0.IDB[1];
    HA_uart_app[13] = b0.IDB[2];
    if ((FLAG_TIME_No_response == 1) && (TIME_No_response == 0))
    {
        HA_uart_app[14] = 05;
        b0.IDL = ID_data.IDL;
        DATA_Packet_ID = ID_data.IDL;
        HA_uart_app[11] = b0.IDB[0];
        HA_uart_app[12] = b0.IDB[1];
        HA_uart_app[13] = b0.IDB[2];
        HA_uart_app[15] = 0xFF;
        if (FG_send_Faile_again == 0)
        {
            FG_send_Faile_again = 1;
            FG_Second = 0;
            TIME_alarm_AUTO = 350;
            FLAG_HA_Inquiry = 1;
            DATA_Packet_Control_0 = 0x00;
            FLAG_AUTO_SEND_ok = 1;
        } //2015.1.30追加修改自动某ID发送一次失败，追加再发送一次
        else if (FG_send_Faile_again == 1)
        {
            FG_send_Faile_again = 2;
            FG_Second = 0;
            TIME_alarm_AUTO = 350;
            FLAG_HA_Inquiry = 1;
            DATA_Packet_Control_0 = 0x00;
            FLAG_AUTO_SEND_ok = 1;
        } //2015.4.2追加修改自动某ID发送一次失败，追加再发送两次
        else if (FG_send_Faile_again == 2)
        {
            time_APP_Start_up = 0;
            FG_Second = 1;
            APP_check_char = 0;
            FG_send_Faile_notice = 1;
        } //2015.3.31追加修改 2次发送都失败，SIG绿色LED 1Hz通知
        if (UART_DATA_buffer[8] == 0x10)
        {
            HA_uart_app[14] = 0xFF;
            HA_uart_app[15] = 0x00;
        }
    }
    else if ((DATA_Packet_Control_0 == 0x81) || (DATA_Packet_Control_0 == 0x85))
    {
        HA_uart_app[14] = 01;
        HA_uart_app[15] = SWITCH_DIP;
    }
    else if ((DATA_Packet_Control_0 == 0x82) || (DATA_Packet_Control_0 == 0x86))
    {
        HA_uart_app[14] = 02;
        HA_uart_app[15] = SWITCH_DIP;
    }
    else if ((DATA_Packet_Control_0 == 0x83) || (DATA_Packet_Control_0 == 0x87))
    {
        HA_uart_app[14] = 03;
        HA_uart_app[15] = SWITCH_DIP;
    }
    else if ((DATA_Packet_Control_0 == 0x84) || (DATA_Packet_Control_0 == 0x88))
    {
        HA_uart_app[14] = 04;
        HA_uart_app[15] = SWITCH_DIP;
    }

    //HA_uart_app[15]=0x00;
    HA_Cache_ha_1Hz_bak = HA_uart_app[14];
    //if(HA_uart_app[14]!=5)
    HA_Cache_ha_bak = HA_uart_app[14];
    //if(HA_uart_app[15]!=0xFF)
    HA_Cache_SWITCH_DIP_bak = HA_uart_app[15];
    SWITCH_DIP_bak = SWITCH_DIP;
    SWITCH_DIP_id_data_bak = DATA_Packet_ID;
    //if(DATA_Packet_soft_ver==1){HA_uart_app[15]=SWITCH_DIP|0x10;HA_Cache_SWITCH_DIP_bak=HA_uart_app[15]|0x10;}
    m = 0;
    for (i = 8; i < 16; i++)
        m = m + HA_uart_app[i];
    HA_uart_app[16] = m % 256;
    HA_uart_app[17] = m / 256;

    //if((APP_check_ID!=b0.IDL)||(APP_check_Control!=HA_uart_app[14])||(HA_uart_app[14]==5)||(APP_check_char==0)||(FG_WIFI_SWITCH_DIP==1))    //2014.10.11修改
    if ((APP_check_ID != b0.IDL) || (APP_check_Control != HA_uart_app[14]) || (APP_check_char == 0) || (FG_WIFI_SWITCH_DIP == 1))
    {
        if ((HA_uart_app[14] == 5) && (FG_Second == 0))
            ;
        else if (time_APP_Start_up == 0)
        {                   //2015.04.27修正
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
            for (i = 0; i < 18; i++)
            {
                U1TXREG = HA_uart_app[i];
                if (i % 6 == 0)
                    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
            }

            if (HA_uart_app[8] == 0x02)
            {
                HA_uart_app[8] = 0x01;
                m = 0;
                for (i = 8; i < 16; i++)
                    m = m + HA_uart_app[i];
                HA_uart_app[16] = m % 256;
                HA_uart_app[17] = m / 256;
                //Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
                Delay100us(30); //>=750,WIFI buffer ok
                for (i = 0; i < 18; i++)
                {
                    U1TXREG = HA_uart_app[i];
                    if (i % 6 == 0)
                        Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
                }
            }

            APP_check_ID = b0.IDL;
            APP_check_Control = HA_uart_app[14];
            APP_check_char = 1;

            FG_WIFI_SWITCH_DIP = 0;
        }
    }

    HA_Cache_ha_switch(b0.IDL); //2015.4.1修正3 由于APP查询受信器HA状态需要很长的时间，所以追加指令查询缓存在通信机里面的HA状态
#endif
}

void uart_send_APP_allID(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i;
    UINT16 d0, d1 = 0;
    UINT8 xp[3] = {0};
    UINT8 xc[4] = {0};
    uni_rom_id xk;
    uart_send_APP_Head();
    for (i = 0; i < ID_DATA_PCS; i++)
    {
        if (ID_Receiver_DATA[i] != 0)
            d1++;
    }
    d0 = d1 * 3 + 3;
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x03;
    U1TXREG = 0x01;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    d0 = 0x04;
    for (i = 0; i < ID_DATA_PCS; i++)
    {
        if (ID_Receiver_DATA[i] != 0)
        {
            xk.IDL = ID_Receiver_DATA[i];
            xc[0] = xk.IDB[0];
            d0 = d0 + xc[0];
            U1TXREG = xc[0];
            xc[1] = xk.IDB[1];
            d0 = d0 + xc[1];
            U1TXREG = xc[1];
            xc[2] = xk.IDB[2];
            d0 = d0 + xc[2];
            U1TXREG = xc[2];
            xk.IDB[3] = 0;
            if (i % 2 == 1)
                Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
        }
    }
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
#endif
}

void uart_send_APP_allalarm(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i, j;
    UINT16 d0, d1;
    UINT16 d2 = 0;
    uni_rom_id num100;
    uart_send_APP_Head();
    d1 = UART1_DATA[11] - 1;
    d0 = 10 + WIFI_alarm_data[d1][6] * 3;
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x07;
    U1TXREG = 0x01;
    U1TXREG = 0x00;
    d0 = 0x08;
    for (i = 0; i < 7; i++)
    {
        U1TXREG = WIFI_alarm_data[d1][i];
        d0 = d0 + WIFI_alarm_data[d1][i];
        if (i == 2)
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    }
    d2 = WIFI_alarm_data[d1][6] * 3;
    for (i = 0; i < d2; i++)
    {
        d0 = d0 + WIFI_alarm_data[d1][i + 7];
        U1TXREG = WIFI_alarm_data[d1][i + 7];
        if (i % 6 == 0)
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    }
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;

//                 d1=UART1_DATA[11]-1;
//                 for(i=0;i<103;i++){
//                     WIFI_alarm_data_planning[i]=WIFI_alarm_data[d1][i];
//                 }
//                 for(i=0;i<WIFI_alarm_data[d1][6];i++){
//                     d0=i*3;
//                     num100.IDB[0]=WIFI_alarm_data[d1][d0+7];
//                     num100.IDB[1]=WIFI_alarm_data[d1][d0+8];
//                     num100.IDB[2]=WIFI_alarm_data[d1][d0+9];
//                     num100.IDB[3]=0;
//                     for(j=0;j<ID_DATA_PCS;j++){
//                        if(ID_Receiver_DATA[j]==num100.IDL)j=100;
//                     }
//                     if(j!=101){
//                         for(j=d0+10;j<103;j++)WIFI_alarm_data_planning[j-3]=WIFI_alarm_data_planning[j];
//                         WIFI_alarm_data_planning[6]--;
//                     }
//                     ClearWDT(); // Service the WDT
//                 }
//                 uart_send_APP_Head();
//                 d0=10+WIFI_alarm_data_planning[6]*3;
//                 U1TXREG=d0%256;
//                 U1TXREG=d0/256;
//                 Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                 U1TXREG=0x07;
//                 U1TXREG=0x01;
//                 U1TXREG=0x00;
//                 d0=0x08;
//                 for(i=0;i<7;i++){
//                     U1TXREG=WIFI_alarm_data_planning[i];
//                     d0=d0+WIFI_alarm_data_planning[i];
//                     if(i==2)Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                 }
//                 d2=WIFI_alarm_data_planning[6]*3;
//                 for(i=0;i<d2;i++){
//                        d0=d0+WIFI_alarm_data_planning[i+7];
//                        U1TXREG=WIFI_alarm_data_planning[i+7];
//                        if(i%6==0)Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                }
//                U1TXREG=d0%256;
//                U1TXREG=d0/256;

#endif
}

void uart_send_APP_SUN(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i, j;
    UINT16 d0, d1;
    UINT16 d2 = 0;
    uni_rom_id num100;
    uart_send_APP_Head();
    d0 = 8 + WIFI_alarm_data[10][6] * 3;
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x0C;
    U1TXREG = 0x01;
    U1TXREG = 0x00;
    d0 = 0x0C + 0x01;
    for (i = 0; i < 3; i++)
    {
        U1TXREG = SUN_ON_OFF_seat[i];
        d0 = d0 + SUN_ON_OFF_seat[i];
    }
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    for (i = 0; i < 2; i++)
    {
        U1TXREG = WIFI_alarm_data[10][i + 5];
        d0 = d0 + WIFI_alarm_data[10][i + 5];
    }
    d2 = WIFI_alarm_data[10][6] * 3;
    for (i = 0; i < d2; i++)
    {
        d0 = d0 + WIFI_alarm_data[10][i + 7];
        U1TXREG = WIFI_alarm_data[10][i + 7];
        if (i % 6 == 0)
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    }
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;

//                 for(i=0;i<103;i++){
//                     WIFI_alarm_data_planning[i]=WIFI_alarm_data[10][i];
//                 }
//                 for(i=0;i<WIFI_alarm_data[10][6];i++){
//                     d0=i*3;
//                     num100.IDB[0]=WIFI_alarm_data[d1][d0+7];
//                     num100.IDB[1]=WIFI_alarm_data[d1][d0+8];
//                     num100.IDB[2]=WIFI_alarm_data[d1][d0+9];
//                     num100.IDB[3]=0;
//                     for(j=0;j<ID_DATA_PCS;j++){
//                        if(ID_Receiver_DATA[j]==num100.IDL)j=100;
//                     }
//                     if(j!=101){
//                         for(j=d0+10;j<103;j++)WIFI_alarm_data_planning[j-3]=WIFI_alarm_data_planning[j];
//                         WIFI_alarm_data_planning[6]--;
//                     }
//                     ClearWDT(); // Service the WDT
//                 }
//                 uart_send_APP_Head();
//                 d0=8+WIFI_alarm_data_planning[6]*3;
//                 U1TXREG=d0%256;
//                 U1TXREG=d0/256;
//                 Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                 U1TXREG=0x0C;
//                 U1TXREG=0x01;
//                 U1TXREG=0x00;
//                 d0=0x0C+0x01;
//                 for(i=0;i<3;i++){
//                     U1TXREG=SUN_ON_OFF_seat[i];
//                     d0=d0+SUN_ON_OFF_seat[i];
//                 }
//                 Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                 for(i=0;i<2;i++){
//                     U1TXREG=WIFI_alarm_data_planning[i+5];
//                     d0=d0+WIFI_alarm_data_planning[i+5];
//                 }
//                 d2=WIFI_alarm_data_planning[6]*3;
//                 for(i=0;i<d2;i++){
//                        d0=d0+WIFI_alarm_data_planning[i+7];
//                        U1TXREG=WIFI_alarm_data_planning[i+7];
//                        if(i%6==0)Delay100us(30);//延时2.1mS以上，缓冲区是8级FIFO
//                }
//                U1TXREG=d0%256;
//                U1TXREG=d0/256;
#endif
}

void uart_send_APP_Emial_time(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i, j;
    UINT16 d0, d1;
    UINT16 d2 = 0;
    uart_send_APP_Head();
    d1 = UART1_DATA[11] - 1;
    U1TXREG = 0x0A;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x0A;
    U1TXREG = 0x01;
    U1TXREG = 0x00;
    d0 = 0x0A + 0x01;
    for (i = 0; i < 7; i++)
    {
        U1TXREG = Emial_time_data[d1][i];
        d0 = d0 + Emial_time_data[d1][i];
        if (i == 2)
            Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    }
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
#endif
}

void uart_send_APP_HA_Change(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT16 i;
    UINT16 d0;
    uart_send_APP_Head();
    U1TXREG = 0x06;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x0E;
    U1TXREG = 0x01;
    U1TXREG = 0x00;
    d0 = 0x0F;
    for (i = 0; i < 3; i++)
    {
        U1TXREG = HA_Change_send_email[i];
        d0 = d0 + HA_Change_send_email[i];
    }
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = d0 % 256;
    U1TXREG = d0 / 256;
#endif
}

void uart_send_APP_Head(void)
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT8 i;
    Delay100us(50); //延时2.1mS以上，缓冲区是8级FIFO
    for (i = 0; i < 6; i++)
        U1TXREG = HA_uart_app[i];
#endif
}

void uart_send_APP_Public(UINT8 Public_X, UINT8 Public_Y) //Public_X ->指令类别低字节  Public_Y ->返回结果  0（OK） 1（NG）
{
#if defined(__Product_PIC32MX2_WIFI__)
    UINT8 i_x;
    uart_send_APP_Head();
    U1TXREG = 0x03;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = Public_X;
    U1TXREG = 0x01;
    U1TXREG = Public_Y;
    i_x = 0x01 + Public_X + Public_Y;
    U1TXREG = i_x;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
#endif
}
#if defined(__Product_PIC32MX2_WIFI__)

void uart_send_APP_To_and_Tc(UINT8 Public_X, UINT8 Public_Y, UINT8 Public_Z) //Public_X ->返回结果  0（OK） 1（NG）  Public_Y ->To/Tc SET/read指令   Public_Z ->To/Tc数据
{
    UINT16 i_x;
    uart_send_APP_Head();
    U1TXREG = 0x08;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0x11;
    U1TXREG = 0x01;
    U1TXREG = Public_X;
    U1TXREG = UART1_DATA[11];
    U1TXREG = UART1_DATA[12];
    U1TXREG = UART1_DATA[13];
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = Public_Y;
    U1TXREG = Public_Z;
    i_x = 0x12 + Public_X + UART1_DATA[11] + UART1_DATA[12] + UART1_DATA[13] + Public_Y + Public_Z;
    U1TXREG = i_x % 256;
    U1TXREG = i_x / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
}
#endif

#if defined(__32MX250F128D__)

void APP_OUT_TEST1(unsigned char *time_TEST1)
{
    UINT16 i_x0;
    uart_send_APP_Head();
    U1TXREG = 0x06;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0xE0;
    U1TXREG = 0x01;
    U1TXREG = time_TEST1[1];
    U1TXREG = time_TEST1[2];
    U1TXREG = WIFI_alarm_Hours_Minutes[1];
    U1TXREG = WIFI_alarm_Hours_Minutes[0];
    i_x0 = 0xE1 + time_TEST1[1] + time_TEST1[2] + WIFI_alarm_Hours_Minutes[1] + WIFI_alarm_Hours_Minutes[0];
    U1TXREG = i_x0 % 256;
    U1TXREG = i_x0 / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
}

void APP_OUT_TEST2(unsigned char *time_TEST2)
{
    UINT16 i_x0;
    uart_send_APP_Head();
    U1TXREG = 0x04;
    U1TXREG = 0x00;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
    U1TXREG = 0xE1;
    U1TXREG = 0x01;
    U1TXREG = time_TEST2[1];
    U1TXREG = time_TEST2[0];
    i_x0 = 0xE2 + time_TEST2[0] + time_TEST2[1];
    U1TXREG = i_x0 % 256;
    U1TXREG = i_x0 / 256;
    Delay100us(30); //延时2.1mS以上，缓冲区是8级FIFO
}
#endif
