# Steps after insert the module
After executing ``insmod my-device-module.ko`` command, you must create the **device file** and assign the **major version** number. In order to do that, execute the next command:
```bash
$ sudo dmesg | grep -e 'my-device-module'
[24428.870720] my-device-module I was assigned major number 234.
[24428.870728] my-device-module Create the device using 'mknod /dev/my-device-module c 234 0'
```

Run the show **mknod** command on the terminal, this will create a device file and assign a valid **major version**.

```bash
$ sudo mknod /dev/my-device-module c 234 0
```

## Check if device was created
```bash
$ ls /dev
... my-device-module

$ more /proc/devices | grep 'my-device-module'
234 my-device-module
```

# Testing the module
To execute a module, you must ``cat`` the file, and every time the internal counter of module will increase +1. Write operations are not a allowed on the **device file**, so even as root, they will be blocked.
```bash
$ cat /dev/my-device-module
I already told you 0 times Hello world!

$ cat /dev/my-device-module
I already told you 1 times Hello world!

$ cat /dev/my-device-module
I already told you 2 times Hello world!

$ echo "Hello" > /dev/my-device-module
bash: /dev/my-device-module: Permission denied

$ sudo dmesg | grep -e 'Sorry'
[16401.765620] Sorry, this operation is not supported.
```
# Remove a module
Before to remove the module, execute the following command to make sure the file is not being used.

```bash
# The name of the module out the output uses "_" instead of "-"
$ more /proc/modules | grep 'my_device_module'
my_device_module 16384 0 - Live 0x0000000000000000 (OE)
```

Finally, if the third column of the output is 0 then you can remove the module safely.
```bash
$ sudo rmmod my-device-module
```
