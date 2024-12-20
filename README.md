This is a C++ program that I made, using the Arduino Uno R4 Wi-Fi, that uses five individual servo motors to individually move finger depending on different hex values received via an external BLE program

For the updated code, I determined the amount each servo needed to rotate, as well as how long the arms needed to be by the equation Lf = √((Li + R) (1 - cos(θ))^2 + (R sin(θ))^2) where Lf = final length of the string ; Li = the initial length of the string ; R = length of the arm on the servo ; θ = the amount that the servo needs to rotate
The hex code to servo goes as follows

Servo 1: 0 (original position) and 1 (set position) = Pinky

Servo 2: 2 (original position) and 3 (set position) = Ring

Servo 3: 4 (original position) and 5 (set position) = Middle

Servo 4: 6 (original position) and 7 (set position) = Index

Servo 5: 8 (original position) and 9 (set position) = Thumb

The servo numbers correspond to the digital output they are assigned to, Servo # - 1 = pinout.
The BLE software that I used is BlueLight on the iPhone 11 IOS 18.1.
For the contractile motion, I used braided fishing line and for the extension, I used 1/16 in shock cord (might move to springs)
The joints of the fingers use a set of 3mm nuts, bolts, and washers to aid in smoother movement.

For the physical hand, I took heavy inspiration from Gaël Langevin on InMoov for most of the physical attributes of the hand and have linked his site below.
https://tinyurl.com/24frtkhj 

3D models: https://github.com/elandes06/Simple-Hand
