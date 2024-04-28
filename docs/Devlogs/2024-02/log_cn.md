# 2024 02 月报

今天是圣情人节，我们把爱与和平带到需要它的维多利亚时代。

## 1.0.9

[1.0.9版本](https://github.com/schombert/Project-Alice/releases/download/v1.0.9/1.0.9.7z)可用！

## mod扩展和经济

这月工作主要是再设计经济模式。在此特别感谢开发员Peter，在原有经济模型迭代了两次，虽不清楚它的运作方式，但它可能有更多的动态行为，取消了价格波动限制，且整体上贴近了现实。

另外，支持非AVX2 CPU，帮助了有些人在旧电脑上能玩PA。但`our vectorization engine, Data Container, did, in fact, support SSE4.2, SSE3 and AVX2`，仍需些调整才能使电脑正常运行。

## 一图胜万言

**地图黑色字体**：多次收到此投诉，接受了。`ESC`-`Graphics Options`-`Black Map Font`.

![Black map fonts](./images/map.png)

**新事件占位符**：为你的事件和决议添加更多风味。如`$party$`, `$government$`, `$culture_last_name$`等。

![Party](./images/party.png)

**新地图模式**：宗教、要塞、议题、收入、好战度等。

![Religion](./images/religion.png)

**新经济**：动态价格、供需平衡、弹性等。

![Trade](./images/trade.png)

**UI改进**：扩展了工厂控制功能。

![Expand](./images/expand.png)

**多人游戏改进**：修复各种不同步问题（大多是多线程）。聊天始终可见！

![Chat](./images/chat.png)

## AHMP

我们开发员`Naveah`移植并改造HPM使其适应爱丽丝引擎，游戏玩法随之改变，比如开局研究任一科技（先研究前提科技）。新的子状态机制、迎合多人游戏的总方向。在此认识到PA与V2的不同处。

获取[AHMP](https://github.com/nivaturimika/ahmp)！
