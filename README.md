# diy_drone
基于STM32F103C8T6的DIY无人机

<img src="./Res/飞控板.jfif" width="450" height="auto" />
<img src="./Res/遥控器.jfif" width="450" height="auto" />

# 成品演示
<iframe src="[//player.bilibili.com/player.html?bvid=BV1EjWN6gEyi&page=1](https://www.bilibili.com/video/BV1EjWN6gEyi?t=5.7)" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"></iframe>

<div>
  <video controls src="./Res/演示.mp4" muted="false"></video>
</div>
<div>
  <video controls src="https://www.bilibili.com/video/BV1EjWN6gEyi?t=5.7" muted="false"></video>
</div>
[![视频封面图]("./Res/遥控器.jfif")](https://www.bilibili.com/video/BV1EjWN6gEyi)
[![视频封面图](./Res/遥控器.jfif)](https://www.bilibili.com/video/BV1EjWN6gEyi)

# 文件说明
此工程是跟着尚硅谷无人机教程制作的
原视频地址：https://www.bilibili.com/video/BV1cRso6FEuM

过程中遇到挺多问题，但好在最后完成了基本制作，能够实现基本的飞行控制，以及定高，悬停等功能。

此工程中包含了以下文件：
- diy_drone文件夹为飞控板的源码；
- diy_drone_ctrl文件夹为遥控板源码；
- PCB文件夹为嘉立创的eda设计文稿，建议用最新日期的版本；

# 硬件说明
- 飞控板：STM32F103C8T6；
- 遥控板：STM32F103C8T6；
- 激光测距：VL53L1X；
- 光流：PMW3901；
- 陀螺仪：MPU6050；
- 通信：SI24R1；
- 电机：8520空心杯电机；
- 电源管理：IP5305T；
- 电池：25C 1300mAh 3.7V XH2.54接口
PS：阻容可在立创商城购买，其他元件在淘宝购买即可。


# 遇到的问题
- SI24R1需添加电子开关，否则上电初始化时状态会乱；
- SI24R1地弹问题，需单独铺地；
- 网购的电池XH2.54公头接口和立创商城买来的XH2.54母座线序相反，需反接（烧了一个IP5305T芯片得来的教训，一定注意正负极连接正确）；
- 网购的8520空心杯机架套件，桨叶顺序不对，偏航角无法正确控制，买回来后需自行检查；
- 焊接完后一定用万用表测下引脚是否短路。

# 关于焊接
- 正面建议用恒温加热台，中温4号粉焊锡膏，100℃预热30秒，然后调到230℃保持30秒，然后关机自然冷却即可。
- 背面一颗激光测距，一颗光流芯片，焊接难度稍高，作者自己也尝试多次才成功，建议使用低温4号粉焊锡膏（锡量务必越少越好，非常容易连锡），在加热台上放个5mm*10mm左右大小的导热铜块，在铜块上焊接。芯片太小，热风枪容易吹飞芯片，焊接难度较大。如果有焊接大神路过，请不要笑话我😂


# 其他
1、因为本人之前是互联网背景，刚接触嵌入式不久，很多工具和方法都是初次接触，中间解决问题挺头大的。
2、费用上：加热台，热风枪，烙铁，锡膏，万用表，洗板水，吸锡带、飞线铜丝、高温胶带、杜邦线，stlink下载器 等等... 很多必要的工具和材料，都是头次购买。总共下来，大概花费了500~600RMB。


# TODO List（等有空了再搞吧...）
- SI24R1通信不是特别稳，待查；
- PMW3901只能捕获30cm内的像素偏移，待查；

有兴趣的小伙伴欢迎一起参与，一起学习，一起进步！
