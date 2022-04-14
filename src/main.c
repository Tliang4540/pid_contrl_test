#include "SDCC_STC8H.h"
#include "MiniSch.h"
#include "interrupt_handled.h"
#include "uart.h"

bit MINI_SCH_RUN = 1;
volatile uint8_t timers[MAXTASKS];

extern uint8_t out_speed;
volatile uint8_t set_speed = 0;

void MiniSch_Init(void)
{
  AUXR = 0x00;	
  
  TMOD = 0x00;	//time0 sch, time1 uart
  IE   = 0x92;  	//ea = 1, et0 = 1
  TL0  = 0xCD;
  TH0  = 0xF8;
  TR0  = 1;

  SCON = 0x50;	
  TL1 = 0xFC;		//115200
  TH1 = 0xFF;
  TR1 = 1;
}

void CPU_Init(void)
{
  WDT_CONTR = 0x26;	//wdt 2s.
  WDT_CONTR = 0x36;	//clear wdt.

  P3 = 0xC3;
  P3M0 = 0x3E;
  P3M1 = 0x00;
  
  P1 = 0x7F;
  P1M0 = 0x82;
  P1M1 = 0x00;
  
  P5 = 0x00;
  P5M0 = 0x10;
  P5M1 = 0x00;
}

void pid(void)
{
  int16_t p_error = 0;  //偏差
  static int16_t ki = 0;//积分
  static int16_t last_error = 0;//上次偏差
  if(set_speed < 15)    //设置速度小于 15转/秒 无法稳定,设置无效输出0
  {
    PWM1_CCR4H = 0x00;
    PWM1_CCR4L = 0x00;
    ki = 0;
    last_error = 0;
  }
  else 
  {
    p_error = (int16_t)set_speed - (int16_t)out_speed;  //当前偏差量
    ki = ki + p_error;  //积分,积分比例=1
    if(ki > 2500)       //积分限幅, 限制过小时高转速无法消除静差
    {
      ki = 2500;
    }
    else if(ki < -2500)
    {
      ki = -2500;
    }
    last_error = (p_error - last_error);  //微分比例=1
    last_error = (ki << 1) + (p_error << 1) + last_error;//控制输出，控制比例2，积分控制*2
    if(last_error > 1106)last_error=1106; //输出限幅
    else if(last_error < 0)last_error=0;
    PWM1_CCR4H = last_error / 256;
    PWM1_CCR4L = last_error % 256;
    last_error = p_error;
  }
}

void pwm_init(void)
{
  P_SW2 = 0x80;
  PWM2_PSCRH = 0x01;
  PWM2_PSCRL = 0x61;

  PWM2_ARRH = 0x18;
  PWM2_ARRL = 0x68;

  PWM1_CCER1 = 0x00;
  PWM1_CCER2 = 0x00;

  PWM1_CCMR2 = 0x01;
  PWM1_CCMR4 = 0x60;
  
  PWM1_CCER1 = 0x30;
  PWM1_CCER2 = 0xc0;

  PWM1_CCR4H = 0x00;
  PWM1_CCR4L = 0x00;
  PWM1_ARRH  = 0x04;
  PWM1_ARRL  = 0x52;

  PWM1_ENO   = 0x80;
  PWM1_BKR   = 0x80;
  PWM1_PS    = 0xC0;

  PWM1_IER   = 0x04;
  PWM1_CR1   = 0x01;
  P32 = 1;
}

uint8_t speed_printf_task(void)
{
  _SS 
  while(MINI_SCH_RUN)
  {
    if(set_speed < 15)
    {
      WaitX(1);
    }
    else
    {
      uint8_t tmp[5];
      WaitX(200);
      tmp[3] = out_speed;
      tmp[2] = (tmp[3] % 10) + '0';
      tmp[3] /= 10;
      tmp[1] = (tmp[3] % 10) + '0';
      tmp[3] /= 10;
      tmp[0] = (tmp[3] % 10) + '0';
      tmp[3] = '\n';
      tmp[4] = 0;
      printf(tmp);
    }
  }
  _EE
}

uint8_t clr_wdt_task(void)
{
  _SS 
  WaitX(250);
  WaitX(250);
  WaitX(250);
  WaitX(250);
  WaitX(250);
  pwm_init();
  while(MINI_SCH_RUN)
  {
    WaitX(100);
    if(P15 == 0)IAP_CONTR |= 0x60;
    WDT_CONTR = 0x36;
  }
  _EE
}

uint8_t pid_run_task(void)
{
  _SS 
  while(MINI_SCH_RUN)
  {
    WaitX(100);
    pid();
  }
  _EE
}

void main(void)
{
  InitTasks();
  CPU_Init();
  MiniSch_Init();
  while(1)
  {
    RunTaskA(clr_wdt_task, 0);
    RunTaskA(pid_run_task, 1);
    RunTaskA(speed_printf_task, 3);
  }
}