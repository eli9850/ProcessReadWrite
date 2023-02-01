### ProcessReadWrite

Windows kernel-mode that driver gives any process the ability to write and read data from another process.
This repo contains 3 projects:
•	The kernel-mode driver.
•	Commander for the driver.
•	Test exe to test the driver.


# Limitations
The driver support read and write only for x64 processes.

# Usage
sc create ProcessReadWrite binPath= {driver_path} type= kernel
sc start ProcessReadWrite
ProcessReadWriteCommander {read/write} pid address {size_to_read/data_to_write}
