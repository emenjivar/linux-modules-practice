obj-m += hello-world-inode.o
hello-world-inode-objs := main.o current-time.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
