#include "uart.h"

#define BUF_MAX 256

volatile bit send_flag = 0;                     //��ʼ���ͱ�־
volatile uint8_t UartSend_Count = 0;            //������λ��
volatile uint8_t UartSend_Length = 0;           //д����λ��
volatile uint8_t xdata UartSend_Buff[BUF_MAX];  //����
volatile uint8_t receive[2];                     //�յ�6���ֽ����ݽ��и�λ,�������ϵ����ճ���
volatile uint8_t receive_count = 0;

extern volatile uint8_t set_speed;
#ifdef __SDCC
void UART1_Interrupt(void) interrupt(4)
#else
void UART1_Interrupt(void) interrupt 4
#endif
{
  if(TI)
  {
    TI = 0;
    if(UartSend_Count != UartSend_Length)
    {
      SBUF = UartSend_Buff[UartSend_Count++];
      //if(UartSend_Count > (BUF_MAX-1))UartSend_Count = 0;
    }
    else 
    {
      send_flag = 0;
      UartSend_Count = 0;
      UartSend_Length = 0;
    }
  }
  if(RI)
  {
    RI = 0;
    receive[receive_count++] = SBUF;
    if(receive_count > 1)
    {
      receive_count = 0;
      if(receive[0] == receive[1])
      {
        set_speed = receive[0];
      }
    }
  }
}

void printf(uint8_t *s)
{
  while(*s != 0)
  {
    UartSend_Buff[UartSend_Length++] = *s++;    //��仺������
    //if(UartSend_Length > (BUF_MAX-1))UartSend_Length = 0;
  }
  if(send_flag == 0)
  {
    send_flag = 1;
    SBUF = UartSend_Buff[UartSend_Count++];
  }
}

void printf_byte(uint8_t byte)
{
  UartSend_Buff[UartSend_Length++] = byte;
  //if(UartSend_Length > (BUF_MAX-1))UartSend_Length = 0;
  if(send_flag == 0)
  {
    send_flag = 1;
    SBUF = UartSend_Buff[UartSend_Count++];
  }
}
