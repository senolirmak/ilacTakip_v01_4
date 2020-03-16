# ilacTakip_v02_4
Günlük düzenli hasta ilaç takip sistemi

TaskScheduler library
Cooperative multitasking for Arduino

Version 2.1.0: 2016-02-01

A lightweight implementation of cooperative multitasking (task scheduling) supporting:

Periodic task execution (with dynamic execution period in milliseconds or microseconds) Number of iterations (n times) Execution of tasks in predefined sequence Dynamic change of task execution parameters (frequency, number of iterations, callback function) Power saving via entering IDLE sleep mode between tasks are scheduled to run Task invocation via Status Request object Task IDs and Control Points for error handling and watchdog timer Local Task Storage pointer (allowing use of same callback code for multiple tasks) Layered task prioritization

TaskScheduler has been migrated to Github.com:

https://github.com/arkhipenko/TaskScheduler.git

