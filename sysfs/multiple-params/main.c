// https://github.com/spotify/linux/blob/master/samples/kobject/kobject-example.c
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

static int foo;
static int bar;
static int
