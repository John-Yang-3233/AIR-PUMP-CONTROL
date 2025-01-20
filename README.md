## Introduction
tb6612fng is controled by the PWM & GPIO in this project.  
The motor's RPM can be modified by `PWM_DUTY_CYCLE` = `PWM_PLUSE/PWM_PERIOD`.  


### control logic

controled by single button, when pressed `phase` changes between 0~2

`phase0` : channelA & channelB off

`phase1` : channelA on & channelB off

`phase2` : channelA off & channelB on

### pin assignments

|PWMA|AIN2|AIN1|STBY|BIN1|BIN2|PWMB|
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
|11|10|12|14|16|18|13|

|AOUT|BOUT|
|:---:|:---:|
|two inflow motor|valve & outflow motor|

|Vm|Vcc|
|:---:|:---:|
|12V supply|3.3V supply|


## Changelog

