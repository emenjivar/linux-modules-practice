# Passing arguments on the module
During the mounting, you should (optionally) define the values of the following arguments

| Name | Type | Default value |
| --- | --- | --- |
| port | int | 81 |
| ip | string      | 192.168.0.1 |
| excluded_ports | int array(5) |  |

```console
$ insmod parameters.ko port=8080 ip=127.0.01 excluded_ports=3306,5432,1433,1350
$ dmesg | grep 'hello'
[12451.439897] hello... port=8080 ip=127.0.01
[12451.439906] hello.. excluded_port: 3306
[12451.439908] hello.. excluded_port: 5432
[12451.439910] hello.. excluded_port: 1433
[12451.439917] hello.. excluded_port: 1350
```

## Invalid input
Array index out bound exception, the param **excluded_ports** supports only 5 elements
```console
$ insmod parameters.ko excluded_ports=3306,5432,1433,1350,3309,3312
insmod: ERROR: could not insert module parameters.ko: Invalid parameters
```

Invalid type of data
```console
$ insmod parameters.ko port=hello
insmod: ERROR: could not insert module parameters.ko: Invalid parameters
```
