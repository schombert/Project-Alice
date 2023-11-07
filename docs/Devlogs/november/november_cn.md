# 2023 11 报告
## Public alpha
[0.8.7 alpha](https://github.com/schombert/Project-Alice/releases/download/v0.8.7%CE%B1/0.8.7-ALPHA.zip)可用了。感谢大家反馈的bug使我们推向1.0。乐观地说，本月可能发布beta版。

## 新地图
本月修改了地图，即使无法加到现有地图，但[first of them](https://github.com/schombert/Project-Alice/blob/main/openv2%20map%20mod/OpenV2Map.zip)在测试，适于原版游戏，仍有零星问题，如像素点和小问题等。非洲一些地名要改、新历史、省区。

![地图](globe.png)

计划写个升级地图指南。

## 图形
Leaf仍努力完善多人游戏（还有不同步问题）和跟踪修复错误，才能加到正式版。但她一直改进。

### 战争迷雾
图形设置新选项：fog of war，隐藏无视野的省份情况。

![战争迷雾](fow.png)

### 抗锯齿
图形设置新选项：anti-aliasing，级别0-16(不推荐，很卡，我的是4)。这改善了边界和河流的外观，尤其缩小地图时。

![抗锯齿](aa.png)

(向译者道歉)

### 国名
为之使三位程序员工作三周，这不简单。这些在你缩小地图一定时会出现，flat平坦、curvy二次曲线、very wiggly曲线。

![国名](text.png)

## 质量
### 科技队列
在添加事件自动选择之后，提前研究科技排队（shift+点击以添加，鼠标右键以移除）。当当前科技已研发时，自动开始目前有能力且最早的科技，意味着将未来科技加到队列，直到可研究（换言之，开局将提高研究速度的科技加到队列，之后不管）。

![科技队列](queue.png)

### 图例
目前不完整，它会解释各种地图模式颜色含义。最终，它允许向地图模式添加更多信息，不用看手册。

![图例](label.png)

### 交易窗口
红绿色以显示产品盈亏情况（添加了提示信息）。

![交易窗口](trade.png)

## 脚本扩展
依mod作者们的要求而开始实现脚本扩展功能。没重构，但类似添加新效果的工作，好做。除非mod作者要用的时候，否则不会添加。我不会添加那些`可能用得到的东西`，以后再说。目前，加了其他pdx游戏脚本的新版本。

[脚本扩展文档](https://github.com/schombert/Project-Alice/blob/main/docs/extensions.md)同步更新。

## 结尾
下月再见(或等不及就加入[discord](https://discord.gg/QUJExr4mRn))！