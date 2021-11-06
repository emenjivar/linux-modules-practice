# Naming a module
My compilated module must names as ``my-complete-module.ko``, then you must define a ``my-complete-module-objs`` and include here the ``*.0`` files on the **Makefile**.

```bash
obj-m += my-complete-module.o
my-complete-module=objs := file_one.o file_two.o file_three.o
...
```
