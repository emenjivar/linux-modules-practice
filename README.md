# Writting linux modules
This repository follow this [book](https://tldp.org/LDP/lkmpg/2.6/html/index.html) and [blog](https://www.thegeekstuff.com/2013/07/write-linux-kernel-module/) about writting linux kernel modules from scratch.

## Requeriments
```bash
apt-get install build-essential linux-headers-$(uname -r)
```

## Compile a module
It is as simple as moving to the module folder and executing the ``make`` command

```bash
$ make
make -C /lib/modules/5.11.0-38-generic/build M=/home/carlos/Documents/repositories/linux-modules-practice/hello-world modules
make[1]: Entering directory '/usr/src/linux-headers-5.11.0-38-generic'
  MODPOST /home/carlos/Documents/repositories/linux-modules-practice/hello-world/Module.symvers
scripts/Makefile.modfinal:57: target '/home/carlos/Documents/repositories/linux-modules-practice/hello-world/hello.c' does not match the target pattern
make[1]: Leaving directory '/usr/src/linux-headers-5.11.0-38-generic'
```

This command will generate some files, the important here is **.ko**
```bash
$ ls
hello_module.c  hello_module.ko  hello_module.mod  hello_module.mod.c  hello_module.mod.o  hello_module.o  Makefile  modules.order  Module.symvers

```

## Useful commands
```bash
# Display information about a kernel module
# The module does not need to be mounted
$ modinfo hello_module.ko
filename:       /home/carlos/Documents/repositories/linux-modules-practice/hello-world/hello.ko
description:    Simple hello world module
author:         carlos menjivar
license:        GPL
srcversion:     C5B4EAE80577F2992F94F93
depends:        
retpoline:      Y
name:           hello_module
vermagic:       5.11.0-38-generic SMP mod_unload modversions
```

```bash
# Insert a new module in the kernel (require root privileges)
$ insmod hello_module.ko

# Check kernel logs
$ dmesg | grep -e 'Hello world'
[ 4875.502744] Hello world
```
```bash
# List modules that are already loaded in the kernel
# Optionally, filter my loaded module
$ lsmod | grep 'hello_module'
```

```bash
# Remove a module from kernel (require root privileges)
$ rmmod hello_module

# Check kernel logs
$ dmesg | grep -e 'Bye Hello world'
[ 5202.288643] Bye Hello world
```
