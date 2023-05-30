## 数据在哪？

本文是一个快速指南，旨在帮助你从数据容器（ `sys::state` 对象的 `world` 成员）中获取信息。这并不意味着这是该对象中信息的完整文档，而更像是关于常见问题和任务的“快速”参考。从长远来看，你可能会需要阅读 [DataContainer 文档](https://github.com/schombert/DataContainer)，因为许多游戏状态都存储在其中。

### 让 Intellisense(TM) 或其他代码完成工具帮助你

`world` 是一个数据容器对象，其中定义了许多函数。如果你不熟悉这些函数的名称是如何从数据容器中产生的，那么直接从 `world` 中找到想要的函数可能并不容易。因此，我推荐使用以下技巧。任何由数据容器管理的对象或关系类型都可以变成一个“fat”句柄。特定类型的标识符组成的 fat 句柄只暴露与该对象或关系相关的函数，这使得通过代码补全找到需要的函数更加容易。只要你不存储制作的 fat 句柄（或将其传递给其他函数），就不会因为易用性而影响性能。以下是一个 fat 句柄的示例：`auto fat_handle_result = fatten(state.world, raw_handle_id);` 当你从 fat 句柄中调用函数时，大多数由其成员函数返回的句柄值本身会自动升级为 fat 句柄，因此通常只需要调用 `fatten` 一次或两次即可。


### 省份的属性

假设你有一个名为 `id` 的 `province_id`，则可以将其定义为 `auto fat_id = fatten(state.world, id);` 若要获取任何在省份中定义的属性（如 `dcon_generated.txt` 中所示），可以使用以下方法： `fat_id.get_life_rating()` ，这样你就可以获得所需的属性。

除了属性之外，省份还可以与其他对象相关，并且这些关系往往更加有趣。目前，省份可以通过控制权和所有权与国家建立关系，工厂可以位于省份内部，省份可以通过核心与国家（即 tag）建立关系。你可以像属性一样获取关系本身（每个关系都可以作为伪对象处理）。例如，你可以调用 `fat_id.get_province_ownership();` 以获取与该省份相关的拥有权句柄，然后你可以调用 `ownership_handle.get_nation();` 来查找其所属国家对象。然而，对于一些常见情况，你想要“跨越”关系来查找其他相关内容，数据容器提供了一个捷径。在这种情况下，你可以调用 `fat_id.get_nation_from_province_ownership();` 来直接获得拥有国家对象的句柄。

#### 省份是水域吗？

这是一个常见的问题，但却没有确切的答案，因为没有 `get_is_sea()` 成员函数。这是因为省份按它们是陆地还是海洋分组。因此，要获取省份是否为水域，你只需测试它是否属于水域组，通过比较 `province_id.index()` 和 `sys::state` 对象的 `province_definitions.first_sea_province` 成员的索引来实现。具有相同或更大索引值的省份属于水域。

#### 省份是否有所有者？

一般而言，当你请求一个不存在的对象时，数据容器会返回无效的索引。因此，如果省份没有所有者并且你请求所有者，将得到一个无效的 `nation_id` 句柄。可以通过将句柄强制转换为 bool 类型来判断其是否无效（无效句柄为 false，有效句柄为 true）。如 `if(bool(fat_id.get_nation_from_province_ownership())) { ... }` 将对有省份的所有者进行判断。

### 国家对象

TAG 的名称、旗帜、政党和地图颜色被认为是国家对象（national identity object）的一部分。这些对象通过 `identity_holder` 关系进行唯一关联（即每个国家都有一个国家对象，每个国家对象最多属于一个国家）。如果你有一个国家的 fat 句柄，可以直接通过 `get_identity_from_identity_holder()` 访问其国家对象。从国家对象中，你可以通过 `get_color()` 获取其颜色（转换为 uint32_t）。并且可以使用在 `container_types.hpp` 中定义的 `int_red_from_int` 和 `red_from_int` 函数族来提取此颜色的组件作为 `float` 或整数。（**重要提示**：目前，你必须从省份到拥有国家再到其国家对象才能获得颜色，比如 `prov.get_nation_from_province_ownership().get_identity_from_identity_holder().get_color()` 在将来，我们可能会在国家本身中缓存当前的颜色和名称等信息，因此请注意，以后可能需要删除 `get_identity_from_identity_holder` 步骤。）

请注意，国家对象和核心存在多对多的关系：每个国家对象可以在许多省份拥有核心，而每个省份也可以有许多国家对象核心。因此，当你从一个省份开始时，无法通过 `core` 关系获得唯一的国家对象。相反，函数 `fat_id.get_core()` 返回该省份的 `core` 关系的范围，可以使用 `for(auto relationship : fat_id.get_core())` 遍历，然后使用 `relationship.get_identity()` 获取国家对象。

### 列强关系

尽管大多数外交关系可以存在于任何两个国家之间，但有一些属性仅在列强与其他国家的关系中才有意义。这些属性存储在 `gp_relationship` 关系中。

有三种方法访问这些关系：

首先，你可以使用 `for(auto gp_rel : state.world.nation_get_gp_relationship_as_great_power(some_nation_id)) { .. }` 。这将遍历所有是列强的 `some_nation_id` 的列强关系。如果该国家不是列强，则此循环不会执行。

其次，你可以使用 `for(auto gp_rel : state.world.nation_get_gp_relationship_as_influence_target(some_nation_id)) { .. }` 。这基本上与之前的方式相同，只是此循环将遍历所有列强正在影响该国家的关系。这样的循环不会遍历超过8个关系，因为最多只有8个列强。但是，它可能少于8个关系，因为如果列强未与特定国家互动，则不会生成任何关系。

最后，你可以使用 `state.world.get_gp_relationship_by_gp_influence_pair(target_id, great_power_id)` 获取任何国家和任何列强之间的 `gp_relationship` 句柄。请注意，如果两个国家之间不存在关系，则此函数将返回无效句柄值（你可以通过将其强制转换为 bool 类型来判断它）。

关系的 `influence` 属性以 `float` 存储当前影响值，这应该是很好理解的。然而， `status` 属性更加复杂，因为它包含了一些打包在一起的值。你可以通过将 `uint8_t` 与 `nations::influence` 命名空间中的常量之一进行与运算，从中获取单个属性。使用 `(status & nations::influence::level_mask)` （不要忘记括号，否则 `&` 将在 `==` 之后计算）提取影响关系的级别，范围从 `nations::influence::level_hostile` 到 `nations::influence::level_in_sphere` 。使用 `(status & nations::influence::priority_mask)` 提取分配影响点时赋予国家的优先级,级别从 `nations::influence::priority_zero` 到 `nations::influence::priority_three` 。最后，可以使用 `nations::influence::is_expelled`、`nations::influence::is_discredited` 和 `nations::influence::is_banned` 判断，以查看是否激活了状态效果。

因为通过迭代大国关系来找出哪个势力范围内的国家将是相当低效的，所以每个国家还在其 `in_sphere_of` 属性中存储了一个它所属的势力范围内的国家的句柄。如果该国家不属于任何范围，则此属性将为无效句柄值。

### 对称外交关系

关于一般对称外交关系的信息存储在 `diplomatic_relation` 类型关系中。这些关系存储两个国家之间当前的关系值，它们是否结盟以及最新停战日期（如果有）的有效期。

与列强关系一样，可以通过直接使用两个国家的句柄查找外交关系，或者迭代特定国家所涉及的对称外交关系来找到外交关系。在第一种情况下，你可以使用以下函数调用：
`state.world.get_diplomatic_relation_by_diplomatic_pair(id_a, id_b);`
请注意，id 的顺序无关紧要，如果使用 `(id_b, id_a)` 调用该函数，则会返回相同的关系。还要注意，与列强关系一样，不能假定任何两个国家之间存在外交关系。如果没有外交关系，则应将它们视为未结盟、没有停战，并且关系值为0。

你还可以迭代涉及特定国家的所有外交关系。你可以编写以下循环：
`for(auto rel : state->world.nation_get_diplomatic_relation(id_a)) { ... }`
这将遍历涉及一个国家的所有外交关系。现在，了解另一个涉及到该外交关系的国家是什么非常重要。这里稍有一点复杂，因为内部关系不知道你从哪个 id 来查找它。因此，要找到其他国家，你必须编写类似以下的内容：
`auto other = rel.get_related_nations(0) == id_a ? rel.get_related_nations(1) : rel.get_related_nations(0);`
你可以将 `related_nations` 视为关系内部包含两个涉及方的标识符的数组。要找出其他国家是谁，你只需找出你来自的国家是否在数组的第一个槽中即可，这会立即告诉你哪个槽保存其他国家。

#### 停战

如果存储的停战日期是无效句柄（表示从未存储过停战）或停战的结束日期早于当前日期，则认为国家没有停战。

### 战争

每场目前进行中的战争都由 war 对象表示。每场战争都有一个主要攻击方、一个主要防御方和一个开始日期（以及最终其它属性，例如当前战争得分，但这仍然在待办事项清单上）。每场战争还与两个或更多国家作为参与者相关联（每个参与者都被标记为攻击方或防御方）。请注意，主要防御方和攻击方也将出现在此列表中，因此如果您正在迭代该列表，则不必单独检查它们。

当前以攻击方或防御方参与任何战争的国家将被设置其 `is_at_war` 属性。（这是一个缓存的值，因为也有可能根据需要重新计算它。）

如果您想知道给定的任何两个国家是否处于战争状态，有一个在 `military.hpp` 中声明的 `military` 命名空间中的便捷函数：`bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b);` ，如果有涉及双方的任何活动中的战争，则返回 true 。

如果您需要编写自己的此类函数，查看 `are_at_war` 的实现可能有所帮助，它是非常简单的函数：

```
bool are_at_war(sys::state const& state, dcon::nation_id a, dcon::nation_id b) {
	for(auto wa : state.world.nation_get_war_participant(a)) {
		bool is_attacker = wa.get_is_attacker();
		for(auto o : wa.get_war().get_war_participant()) {
			if(o.get_nation() == b && o.get_is_attacker() != is_attacker)
				return true;
		}
	}
	return false;
}
```

在该函数中，我们首先迭代该国参与的所有战争。然后，对于每一个战争，我们记录我们的国家是攻击方还是防御方。知道这一点之后，我们迭代所有参与者，查看其中是否有任何我们关注的、在对方阵营的国家。

### 人口

大多数省份有多个 pops （尽管某些省份可能根本没有 pops ）。要迭代省份中的 pops ，可以编写以下循环：
```
for(auto pops_by_location : state.world.province_get_pop_location(province_id)) {
	auto pop_id = pops_by_location.get_pop();
	...
}
```
此循环遍历涉及指定省份的所有 pop-to-province 位置关系。在循环的主体中， `pops_by_location` 将是该关系实例之一的 fat 句柄。然后，您可以使用 `get_pop()` 从关系中取出 pop ，这也将生成一个 pop 的 fat 句柄。

#### 存储 pop 的 id

**永远**不要不采取一些预防措施就存储 `pop_id` 。与大多数对象不同， pop 可以出现和消失，并且为了提高性能，数据容器将改变 pop，它们被删除以保持在一个紧凑的组中。这意味着，在不同天中的同一个 pop 的句柄，可能会指向完全不同的 pop 。然而，为了使用户界面响应，您可能希望存储窗口或元素正在显示的 pop 的 id 。为解决此问题，在存储 pop_id 时，您还应存储 pop 所在位置的省份 id 、pop 的文化 id 、pop 的宗教 id 和 pop 的类型 id 。这些属性的组合能确保其唯一。在存储 pop_id 时，不仅应存储这些其他属性，还应为 ui 元素编写一个 `update` 函数（`update` 在时间改变后发送到可见的 ui 元素），其中搜索您存储的省份以找到具有相同匹配文化、宗教和 pop 类型的 pop。然后使用此方式找到的 id 更新已存储的 pop id。（如果您无法通过此方式找到匹配的 pop，则表示该 pop 已经消失，您不应再尝试显示有关其信息。）

### 最后提醒

请记住，您提取句柄的大多数方式有时可能会产生“无效”句柄，表示不存在这样的对象。例如，如果尝试获取未拥有省份的所有者，则会获得此类句柄。将句柄（ fat 或其它）转换为 `bool` 可检查其有效性。虽然有一些情况下从无效句柄读取数据是安全的（请参阅数据容器文档），但经常检查句柄的有效性可能更安全。

[English](../accessing_data.md)（此版本由 Stevezxc 翻译于 2023/05/30 11:15 UTC+8）
