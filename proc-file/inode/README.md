# About this module
This is a simple proc module with handle writing and reading by users.

## Usage
After compile, the file **hello-world-inode** will be created on **/proc** directory. You should to write using `echo` and then to read using `cat`, similar to the following example.
```bash
$ echo "I will no longer mutilate and destroy myself." > /proc/hello-world-inode
$ cat /proc/hello-world-inode
I will no longer mutilate and destroy myself.

$ echo "Your soul is the world." > /proc/hello-world-inode
$ cat /proc/hello-world-inode
Your soul is the world.
```

## Debugging
If you want to read the generated logs for this module, try executing the following command
```bash
$ sudo dmesg | grep '\[hello-world-inode\]'
[37252.540578] [hello-world-inode] /proc/hello-world-inode created
[37535.512747] [hello-world-inode] my_write 90 bytes
[37541.893993] [hello-world-inode] my_read 90 bytes
[37541.894021] [hello-world-inode] my_read end
[37558.933258] [hello-world-inode] my_write 29 bytes
[37563.998585] [hello-world-inode] my_read 29 bytes
[37563.998619] [hello-world-inode] my_read end
``` 
