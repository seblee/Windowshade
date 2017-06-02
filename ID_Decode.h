/***********************************************************************/
/*  FILE        :ID_Decode.h                                           */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :PIC32MX2     Crystal: 10/40M                          */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdlib.h>
#include <plib.h> // ����C����

extern void ID_Decode_Initial_CNx(void);
extern void ID_Decode_Initial_INT(void);
extern void ID_Decode_function(void);
extern void ID_Decode_IDCheck(void);
extern void ID_Decode_OUT(void);
extern void ID_code_IDCheck(void);
extern void Freq_Scanning(void);
extern void SetTxData(void);
extern void SendTxData(void);
extern void BEEP_and_LED(void);

#if defined(__Product_PIC32MX2_WIFI__)
extern void Confirm_BEEP_and_LED(void);
#endif
