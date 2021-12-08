# About this module
This is proc_module created using **seq_file** interface, which display the total writes and reads on the file.

## Compile and mount
```bash
$ make
...
$ sudo insmod hello-world-seq-file.ko
```

## Usage
After compile, you have to **write** the file using ``echo`` and **read** using ``cat``, following the next example:
```bash
$ echo 0 > /proc/hello-world-seq-file
$ echo 0 > /proc/hello-world-seq-file
$ echo 0 > /proc/hello-world-seq-file
$ cat /proc/hello-world-seq-file
reading 1 times
writting 3 times
```

## Remove
```bash
$ sudo rmmod hello-world-seq-file
```
