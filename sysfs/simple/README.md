# About this module
This a a simple device driver using sysfs parameters.

## Usage
After compiling and mounting, the module will create the path **/sys/kernel/hello-world-sysfs/** which contains the file **var**.

You can read and write the value of **var** using ``cat`` and ``echo`` respectively.
Keep in mind that the **writing** must be executed from **root** user.
```console
$ cat /sys/kernel/hello-world-sysfs/var
$ 0
$ echo 100 > /sys/kernel/hello-world-sysfs/var
bash: /sys/kernel/hello-world-sysfs/var: Permission denied
$ sudo su
root@carlos:~# echo 100 > /sys/kernel/hello-world-sysfs/var
root@carlos:~# cat /sys/kernel/hello-world-sysfs/var
root@carlos:~# exit
$ cat /sys/kernel/hello-world-sysfs/var
100
```

## Remove module
```bash
$ sudo rmmod hello-world-sysfs
```
