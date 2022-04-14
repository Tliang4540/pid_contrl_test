## 项目简介 ##
    1.单片机PWM1通道4输出20KHz频率控制电机，通道2采集霍尔信号。PWM2用来计时计算转速
    2.串口输出VOFA+的FireWater格式数据，用来显示转速波形。接收2字节相同数据来设置转速
    3.通过PID调整PWM的占空比来调整转速

## PID控制算法 ##
```C

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

```
## 任务划分 ##
    1.喂狗任务：延时1S后初始化PWM参数，定时喂狗和检测按键复位用来烧写程序
    2.PID运算任务：100ms运行一次pid算法调整控制比例
    3.转速打印输出认为：200ms打印一次当前转速
    4.PWM1中断：下降沿输入触发，触发后开启PWM2定时器计时，当第二次触发时根据时间计算转速
    5.PWM2中断：超时，认为转速为0
    6.串口中断：处理串口数据输出，以及数据接收。收到两字节相同数据则设置目标转速

## 效果展示 ##
VOFA+显示波形
![shot.png][1]
控制比较稳定，后面波动是加上负载测试稳定性

全部源码：[GitHub][2]


  [1]: https://github.com/Tliang4540/pid_contrl_test/blob/master/shot.png
  [2]: https://github.com/Tliang4540/pid_contrl_test