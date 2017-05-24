/***********************************************************************/
/*  FILE        :Uart.H                                             */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :PIC32MX2     Crystal: 10/40M                          */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdlib.h>
#include <plib.h>		// 常用C定义

extern void Uart1_Init(void);
extern void UART_Decode(void);
extern void HA_uart_email(UINT8 EMIAL_id_PCS_x);
extern void HA_uart_send_APP(void);
extern void HA_uart_email_Repeat(void);

#if defined(__Product_PIC32MX2_Receiver__) 

#define U1Busy_IO  TRISBbits.TRISB14
#define U1Busy_OUT  LATBbits.LATB14
#define FrameHeadSataus 0x00  //帧头
#define DataStatus 0x01       //数据位置 
#define FrameEndStatus 0x02   //桢结束 
#define FrameHead 0x02      //数据开始 
#define FrameSingnalID 0x11 //信号ID 

extern UINT8 UartStatus;
extern UINT8 UartLen;
extern UINT8 UartCount;
extern UINT8 UART_DATA_buffer[493];
void ReceiveFrame(UINT8 Cache);
void OprationFrame(void);
void TranmissionACK(void);
typedef union {
    unsigned char Data[4];
    struct {
        unsigned ID_No:8;
        unsigned Statues:4;
        unsigned Mode:4;
        unsigned Abnormal:8;
        unsigned Standby:8;
    };
    struct {
        unsigned :8;
        unsigned SW_Info:8;
        unsigned AbnormalOut1:8;
        unsigned AbnormalOut2:8;
    };
    struct {
        unsigned w:32;
    };
} __Databits_t;
typedef enum {
    IdelStatues = 0,
    ReceivingStatues,
    ReceiveDoneStatues,
    ACKingStatues,
    ACKDoneStatues,
}__U1Statues; 
extern __U1Statues U1Statues;
extern unsigned int U1AckTimer;
#define U1AckDelayTime  2

#endif
void WriteString(const char *string);
void PutCharacter(const char character);
void SendOut(const char *Cache,unsigned int lenth);
#if defined(__Product_PIC32MX2_WIFI__)
extern void uart_send_APP_allID(void);
extern void uart_send_APP_answer(UINT8 answer_data);
#endif

#if defined(__32MX250F128D__)
extern void APP_OUT_TEST1(unsigned char *time_TEST1);
extern void APP_OUT_TEST2(unsigned char *time_TEST2);
#endif