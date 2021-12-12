# About this module
This is a simple proc module that handle writing and reading operations, displaying the data such as a logs, including a timestamp for every writing action.

## Parameters
| Name | Type | Default value |
| --- | --- | --- |
| utc_offset | int | 0 |

## Compile
When you compile the module, pass can pass the parameter **utc_offset** specifying your UTC zone, else the UTC value will be 0.
```bash
$ make
...
$ sudo insmod hello-world-inode.ko utc_offset=-6
```
After compile, the file **hello-world-inode** will be created on **/proc** directory.

## Usage
Write to **/proc** file using ``echo`` and read using ``cat``
```console
$ echo "one" > /proc/hello-world-inode
$ echo "two" > /proc/hello-world-inode
$ echo "three" > /proc/hello-world-inode
$ cat /proc/hello-world-inode
[2021-11-4 22:14:51] one
[2021-11-4 22:14:54] two
[2021-11-4 22:14:59] three
```

## Debugging
If you want to read the generated logs for this module, try executing the following command
```console
$ sudo dmesg | grep '\[hello-world-inode\]'
[37252.540578] [hello-world-inode] /proc/hello-world-inode created
[37535.512747] [hello-world-inode] my_write 90 bytes
[37541.893993] [hello-world-inode] my_read 90 bytes
[37541.894021] [hello-world-inode] my_read end
[37558.933258] [hello-world-inode] my_write 29 bytes
[37563.998585] [hello-world-inode] my_read 29 bytes
[37563.998619] [hello-world-inode] my_read end
```
