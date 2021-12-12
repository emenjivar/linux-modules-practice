# About this module
This is a driver module using sysfs to handle multiple parameters.

## Available parameters
| Name | Type | Description |
| --- | --- | --- |
| background | string | path of image wallpaper |
| style | string | stretched, centered or none |
| icon_size | int | default value 48 |

## How to use
After compilation, the files **background**, **style** and **icon_size** will be created under **/sys/kernel/custom-screen/**

You can set the values of those files using ``echo`` as **root** user

The file **/proc/custom-screen** display the configuration on terminal.

```bash
# Display default values
carlos:~$ cat /proc/custom-screen
background :
style : none
icon_size : 48

# Setting new values
root:~# echo "/home/carlos/Pictures/wallpapers/my_image.png" > /sys/kernel/custom-screen/background
root:~# echo "centered" > /sys/kernel/custom-screen/style
root:~# echo 30 > /sys/kernel/custom-screen/icon_size

# Display new values
carlos:~$ cat /proc/custom-screen
background : /home/carlos/Pictures/wallpapers/my_image.png
style : centered
icon_size : 30
```
