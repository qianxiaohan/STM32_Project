# STM32F4系列的工程文件

## STM32F407 Template

STM32F4系列标准库的工程模板



## STM32F407-LVGL-Template

STM32F4系列标准库和LVGL的模板，LVGL版本`8.3.10`，其中LVGL的界面设计使用`GUI GUIDER v1.9.1`，整体界面如下所示：

![i.png](./img/1.png)

整体目录结构：

```
├─CORE								//cotex m4所需要的文件
├─FWLIB								//标准库外设文件     
├─HARDWARE							//自定义的外设文件
├─LVGL								//LVGL库
│      LVGL_SRC						//LVGL移植的目录
│      UI							//GUI GUIDER生成UI文件存放的目录
├─OBJ								//Keil构建项目输出目录
├─SYSTEM							//包括串口、延时函数以及IO定义，直接复制的正点原子的案例
└─User								//主函数所在目录
```

