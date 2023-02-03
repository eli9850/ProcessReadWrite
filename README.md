# ProcessReadWrite

Windows kernel-mode that driver gives any process the ability to write and read data from another process.
This repo contains 3 projects:
•	The kernel-mode driver.
•	Commander for the driver.
•	Test exe to test the driver.


### Limitations
The driver support read and write only for x64 processes.

### Usage
sc create ProcessReadWrite binPath= {driver_path} type= kernel
sc start ProcessReadWrite
ProcessReadWriteCommander {read/write} pid address {size_to_read/data_to_write}

### Test

In order to check the driver, you can use the commander on any process that you want.
Alternatively, you can just use the test project that was added to the solution.
The process creates 1 variable and initializes it to a random number. After the initialization, it checks if the number stays the same. If that is so it prints, it's address, sleeps for 1 second, and then returns to the beginning of the while loop. The result is that the process will never finish.
You can use the driver in order to read the value of the variable (use the address that the process prints) and after you read it, you can change it to something else in order to make the process finish its job.
