## 模组汉化迁移

### 步骤

因为原版 V2 的汉化是按 `GB2312` 编码的，所以在迁移时需要将文件编码转换为 `UTF-8`。使用任何文本编辑器都可以完成这个操作。

1. 进入模组目录 `mod\<模组名称>`。
2. 新建 `assets\localisation\zh-CN` 文件夹。
3. 将原模组 `localisation` 文件夹下的文件复制到 `zh-CN` 文件夹下。
4. 将 `zh-CN` 文件夹下的文件编码设置为 `UTF-8`。

### 注意事项

如果 `events`、`decisions`、`common` 等文件中含有汉化，需要将其迁移到 `.csv` 文件中。因为这些文件会被游戏以 `Windows-1252` 编码读取，无法正常显示。

爱丽丝计划给出了一种解决方案：

在 `decisions\RUS.txt` 文件中，有如下内容：

```
change_province_name = "符拉迪沃斯托克"
```

该文本会被游戏以 `Windows-1252` 编码读取，无法正常显示。但是如果将其改为：

```
change_province_name = "Vladivostok"
```

并在 `assets\localisation\zh-CN` 文件夹下新建一个编码为 `UTF-8` 的 `RUS.csv` 文件，内容如下：

```
Vladivostok;符拉迪沃斯托克
```

那么游戏就会读取 `RUS.csv` 文件中的内容，显示为 `符拉迪沃斯托克`。

可以参考爱丽丝计划的汉化，将其他文件夹中的汉化迁移到 `.csv` 文件中。
