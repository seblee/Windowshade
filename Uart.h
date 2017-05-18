/***********************************************************************/
/*  FILE        :Uart.H                                             */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :PIC32MX2     Crystal: 10/40M                          */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdlib.h>
#include <plib.h>		// ����C����

extern void Uart1_Init(void);
extern void UART_Decode(void);
extern void HA_uart_email(UINT8 EMIAL_id_PCS_x);
extern void HA_uart_send_APP(void);
extern void HA_uart_email_Repeat(void);


#if defined(__Product_PIC32MX2_WIFI__)
extern void uart_send_APP_allID(void);
extern void uart_send_APP_answer(UINT8 answer_data);
#endif

 #if defined(__32MX250F128D__)
extern void APP_OUT_TEST1(unsigned char *time_TEST1);
extern void APP_OUT_TEST2(unsigned char *time_TEST2);
 #endif