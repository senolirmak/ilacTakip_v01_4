# ilactakip
Günlük düzenli hasta ilaç takip sistemi
TaskScheduler library
Cooperative multitasking for Arduino

Version 2.1.0: 2016-02-01

A lightweight implementation of cooperative multitasking (task scheduling) supporting:

Periodic task execution (with dynamic execution period in milliseconds or microseconds) Number of iterations (n times) Execution of tasks in predefined sequence Dynamic change of task execution parameters (frequency, number of iterations, callback function) Power saving via entering IDLE sleep mode between tasks are scheduled to run Task invocation via Status Request object Task IDs and Control Points for error handling and watchdog timer Local Task Storage pointer (allowing use of same callback code for multiple tasks) Layered task prioritization

TaskScheduler has been migrated to Github.com:

https://github.com/arkhipenko/TaskScheduler.git

wire
This library allows you to communicate with I2C / TWI devices. On the Arduino boards with the R3 layout (1.0 pinout), the SDA (data line) and SCL (clock line) are on the pin headers close to the AREF pin. The Arduino Due has two I2C / TWI interfaces SDA1 and SCL1 are near to the AREF pin and the additional one is on pins 20 and 21. As a reference the table below shows where TWI pins are located on various Arduino boards. Board I2C / TWI pins Uno, Ethernet A4 (SDA), A5 (SCL) Mega2560 20 (SDA), 21 (SCL) Leonardo 2 (SDA) , 3 (SCL) Due 20 (SDA), 21 (SCL), SDA1, SCL1 As of Arduino 1.0, the library inherits from the Stream functions, making it consistent with other read/write libraries. Because of this, send() and receive() have been replaced with read() and write().
# ilacTakip_v02_4
