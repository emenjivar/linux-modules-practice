# How to use this module?
This **device driver module** makes musical recommendations on the output according to your mood, when you read the **device driver** with **cat** or another command.

## Mounting the driver
When you mount the module, you can pass the parameter ``mood`` with any of the following values: **sad**, **lonely**, **happy** and **weird**. Where **sad** is the default value when you do not pass it.

```bash
$ make
make -C /lib/modules/5.11.0-40-generic/build M=/home/carlos/Documents/repositories/linux-modules-practice/device-files modules
make[1]: Entering directory '/usr/src/linux-headers-5.11.0-40-generic'
  CC [M]  /home/carlos/Documents/repositories/linux-modules-practice/device-files/main.o
  LD [M]  /home/carlos/Documents/repositories/linux-modules-practice/device-files/my-device-module.o
  MODPOST /home/carlos/Documents/repositories/linux-modules-practice/device-files/Module.symvers
  CC [M]  /home/carlos/Documents/repositories/linux-modules-practice/device-files/my-device-module.mod.o
  LD [M]  /home/carlos/Documents/repositories/linux-modules-practice/device-files/my-device-module.ko
  BTF [M] /home/carlos/Documents/repositories/linux-modules-practice/device-files/my-device-module.ko
Skipping BTF generation for /home/carlos/Documents/repositories/linux-modules-practice/device-files/my-device-module.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.11.0-40-generic'

$ sudo insmod my-device-module.ko mood=weird
```

After mounting the module, this will create a **device driver file** on **/dev/** directory, called **my-device-driver**.

## Usage
You must ``cat`` the file (or read using another command) to execute it, and every time the module will recommend you a random track according to the value of **mood** parameter.

```bash
$ sudo cat /dev/my-device-module
Track: 13 angels standing guard round the side of your bed - Silver Mt Zion

$ sudo cat /dev/my-device-module
Track: Mercy street - Peter Gabriel

$ sudo cat /dev/my-device-module
Track: Pneumonia - Bjork
```

You cannot write on **my-device-module** file, the operation is not allowed, even if you try using sudo.
```bash
$ sudo echo "hello" > /dev/my-device-module
bash: /dev/my-device-module: Permission denied
```

## Remove from Linux
After tested the module, you can remove using the following command:
```bash
$ sudo rmmod my-device-driver
```

This command will remove the **device driver file** mounted on **/dev/** directory.
