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


## 效果展示 ##
VOFA+显示波形
![shot.png][1]
控制比较稳定，后面波动是加上负载测试稳定性

  [1]: http://101.33.205.183/usr/uploads/2022/04/2347352204.png