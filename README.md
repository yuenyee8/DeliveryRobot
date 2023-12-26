# DeliveryRobot
在robot启动前是这样的，本机器人有3个灯(红黄绿)，一个2004的l2c lcd和一个push botton。在机器人通电时，红灯会亮着，而且可以使用第二功能(3x4 keypad)和第三功能(遥控器)来控制机器人，lcd也会显示 Delivery Robot: Switch once for line follow, twice for control mode。然后用户现在按一次push button，机器人将会是第一功能（沿线移动）并且黄灯会亮起来。然后当用户再按一次push button的时候，绿灯会亮起并且将由第二功能(3x4 keypad)和第三功能(遥控器)来控制机器人。当用户再按一次的时候，机器人将会回去第一功能（沿线移动）。最后，如果机器人遇到问题的时候，buzzer和红灯将会闪烁。

第一个功能是绕线避障，机器人会用(maker line)沿着线移动，但是当传感器检测到前方有物体时，机器人就会避开。 此机器人中将会使用到2个传感器（ultrasonic sensor & limit switch)。当左侧超声波传感器检测到时，机器人将向右避开（退后并向右场地直走，然后向左返回寻线并沿线返回）； 当右侧超声波传感器检测到时，机器人将向左避开（退后并向左场地直行，向右返回搜索线路并沿线路返回）。 当左侧的limit switch被按到的时候，机器人将向右避开（退后并向右场地直走，然后向左返回寻线并沿线返回）； 当右侧的limit switch被按时，机器人将向左避开（退后并向左场地直行，向右返回搜索线路并沿线路返回）。 只有机器人避开障碍物期间，buzzer才会发出声音。

第二个功能是使用3x4 keypad来控制机器人的沿线避障功能。键盘上有11键。其中的1-9号的功能是当按1号的时候，机器人会跟着沿线功能的第一个路线行走。当按2号的时候，机器人会跟着沿线功能的第二个路线行走。以此类推。

第三个功能是使用红外控制器来控制机器人的运动。遥控器上有21键。控制器需要随时可以控制机器人。虽然我们在使用遥控器在控制机器人，但是为了防止遥控器失控，它必须含有第一功能的避障能力。当它在绕线避障功能（第一功能）的时候，遥控器也是能控制它。遥控器上的按钮：
CH- 是让机器人使用绕线避障功能（第一功能）来控制机器人。
CH 是让机器人使用3x4键盘（第二功能）来控制机器人。
CH- 是让机器人回归遥控器功能来控制机器人。
1-9 号是和第二功能一样，3x4键盘来控制机器人。
10-19 号：10号是转左， 11号是前进， 12号是转右。


This is what it looks like before the robot starts. The robot has 3 lights (red, yellow and green), a 2004 L2C LCD and a push botton. When the robot is powered on, the red light will be on, and the second function (3x4 keypad) and third function (remote control) can be used to control the robot. The LCD will also display Delivery Robot: Switch once for line follow, twice for control mode . Then the user now presses the push button once, the robot will be the first function (moving along the line) and the yellow light will light up. Then when the user presses the push button again, the green light will light up and the robot will be controlled by the second function (3x4 keypad) and the third function (remote control). When the user presses it again, the robot will return to the first function (moving along the line). Finally, if the robot encounters a problem, the buzzer and red light will flash.

The first function is to avoid obstacles by circling the line. The robot will use the (maker line) to move along the line, but when the sensor detects an object in front, the robot will avoid it. This robot will use 2 sensors (ultrasonic sensor & limit switch). When the left ultrasonic sensor detects, the robot will avoid to the right (back up and walk straight to the right field, then return to the left to find the line and return along the line); when the right ultrasonic sensor detects, the robot will avoid to the left On (back up and go straight left field, go back right to search the line and back along the line). When the limit switch on the left is pressed, the robot will avoid to the right (back up and walk straight to the right field, then return to the left to find the line and return along the line); when the limit switch on the right is pressed, the robot will Avoid to the left (back up and go straight left field, return to the right to search the line and follow the line back). The buzzer will only make sounds when the robot is avoiding obstacles.

The second function is to use the 3x4 keypad to control the robot's obstacle avoidance function along the line. There are 11 keys on the keyboard. Among them, the function of No. 1-9 is that when No. 1 is pressed, the robot will follow the first route along the function. When pressing number 2, the robot will follow the second route along the line function. And so on.

The third function is to use an infrared controller to control the movement of the robot. There are 21 keys on the remote control. The controller needs to be able to control the robot at all times. Although we are using a remote controller to control the robot, in order to prevent the remote controller from losing control, it must have the first function of obstacle avoidance capability. When it is in the winding and obstacle avoidance function (the first function), the remote control can also control it. Buttons on the remote control:
CH- is to let the robot use the winding obstacle avoidance function (the first function) to control the robot.
CH is to let the robot use the 3x4 keyboard (second function) to control the robot.
CH- is to let the robot return to the remote control function to control the robot.
Numbers 1-9 are the same as the second function, 3x4 keyboard to control the robot.
No. 10-19: No. 10 is to turn left, No. 11 is to go forward, and No. 12 is to turn right.
