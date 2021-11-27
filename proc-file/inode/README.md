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
