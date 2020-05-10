# Controlling a Quadcopter using FRDM-K64F and MPU6050 Accel/Gyro (Work in progress!!!)

This project is to control a quadcopter using a FRDM-K64F board and the MPU6050 Accel/Gyro.

> **Licensing:** 
> <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br /><span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">Quadcopter</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="https://github.com/leandroGHsoft/Quadcopter" property="cc:attributionName" rel="cc:attributionURL">Leandro Fabian Rocco</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.



## Getting Started 🚀

At the first, you need to install MCUXpresso from NXP company and the SDK for the FRDM-K64F board with all peripheral drivers. Also, you need the MPU6050 accelerometer and gyroscope sensor to implement the PID (Proportional, Integral, Derivative) controller.


## Hardware 🔧

The following images show the hardware used for this project, from the left to the right:

* EMAX CF2822 BLDC motor.
* Skywalker 20-A ESC for each motor (refresh rate of the throttle signal: 50Hz to 432Hz).
* NXP FRDM-K64F development board with 3-axis accelerometer and magnetometer sensor.
* MPU6050 accelerometer/gyroscope sensor.
* HC-06 Bluetooth module to controlling the quadcopter througth an cellphone Android app.


![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/motor.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/esc.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/frdm-k64f.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/mpu6050.jpg)

Below are some images of the quadcopter being assembled.

![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/quad1.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/quad2.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/quad3.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/quad4.jpg)![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/quad5.jpg)



## Software 🛠️

The project was developed and compilled using the following software:

* MCUXpresso IDE v11.1.0 (Build 3209) (2019-12-12).
* SDK_2.7.0_FRDM-K64F (Mcuxpresso 303 2019-12-19) created with NXP SDK Builder.


## Android app :iphone:

An Android app to controlling the Quadcopter using Bluetooth link was developed. The main activity app is show in the next image:

![NO IMAGE](https://github.com/leandroGHsoft/Quadcopter/blob/master/images/app.png)

The app have the following controls:

* Circular seekbar to control throttle of quadcopter.
* Joystick to controlling pitch, roll and yaw of quadcopter.
* Switch button to turn on/off the bluetooth module.
* Text box that shows the status of bluetooth module and some information for the user.


```
The Android application code is in a private repository, which I will make public in the future. In the meantime, if you are interested in obtaining it, send me a private message.
```



## Author ✒️

* **Lendro Fabian Rocco** - [GitHub](https://github.com/leandroGHsoft) - e-mail: <leandro.f.rocco@gmail.com>




