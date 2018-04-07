
## Robot

This robot is constructed of off the shelf dev boards, materials from the local hardware store, and a lot of zip ties. 

### Construction

1. The frame is constructed of L shaped aluminum stock and sheet aluminum. 
2. Electronics are mounted on a corrugated plastic sheet. 
3. Power comes from a sealed 12V lead acid battery. 
4. 12V power for the motors is supplied through a fused panel. 
5. 5V power for electronics is supplied through a 12V to 5V adapter.
6. The drivetrain consists of two small geared motors mounted to rubberized 4 inch wheels. 
7. Trailing the robot are two casters. 
8. The main controller is a Raspberry Pi 2. 
9. The Rpi connects to an Arduino Nano which in turn drives the motor controller and reads input from the ultrasonic sensor. 
10. The motor controller drives the two motors. 
11. The ultrasonic sensor provides distance measure for the front of the robot and hopefully prevents us from crashing the robot.

![Robot](/iotweb/static/images/robot.png)

### How it works

1. The Raspberry Pi acts as a WiFi access point and as the robot controller.
2. There's a UDP server on the RPi that listens for robot controller packets. Each packet is a number that corresponds to a robot  command.
3. ServerUDP translates this command into percentage directions for each motor. For example a full rotate right command would translate to 100 left and -100 right.
4. These percentages are passed to a sketch on the Arduino over serial using the format RLLLLRRRR! For rotate right this looks like R0100-100!.
5. The Arduino sketch does two things, accepts and processes serial commands from the controller and processing distance using the ultrasonic sensor.
6. In translating RPi commands, the sketch translates the percentages sent by the RPi to a PWM duty cycles that the controller can understand. Using the analogWrite() function a cycle can range from 0 to 255, with 255 at 100%. 
7. The controller the PWM signal into a voltage range to send to each motor. A mid-level PWM duty cycle of  127 tells the motor to stop, 0 is full reverse and 255 is full forward.
8. The Ultrasonic sensor is a cheap HC-SR04 unit, providing a range from 0 to 150cm.  The sensor works by sending an ultrasonic ping on a trigger pin, then it measures the amount of time it takes for the ping to return on the echo pin. 
9. This Cytron motor controller supports 2 brushed DC motors each drawing 13 Amps. The motors can operate in locked antiphase as described above where the PWM signal controls both speed and direction. Or it can operate in Sign Magnitude mode where PWM controls speed and a separate pin controls direction.
10. The motor controller runs off the 12 Volts used to supply power to the motors. The controller has a set of buttons that can be used to test the controller and motors.
11. The motors used are planetary geared motors each spin at a maximum of 98RPM drawing a maximum of 3.8Amps. The motors are mounted to 4 inch rubberized wheels.  
12. Sitting on top of the Raspberry Pi is a Sense Hat. The Sense Hat is not used for this project, but does include the LED matrix and sensors such as an acceleromter, gyroscope, magnetometer, temperature, humidity, and barometer.
13. The LED strip runs on its own controller pulling power from the 5V converter.

#### Raspberry Pi

![Raspberry Pi](/iotweb/static/images/pi.png)

#### Arduino Nano

![Arduino Nano](/iotweb/static/images/nano.png)

#### PWM Signal

![PWM](/iotweb/static/images/pwm.gif)

#### Cytron Motor Controller

![Cytron Controller](/iotweb/static/images/controller.png)

#### Geared Motor

![Geared Motor](/iotweb/static/images/motor.jpg)

#### Sense Hat

![Sense Hat](/iotweb/static/images/astro.png)
