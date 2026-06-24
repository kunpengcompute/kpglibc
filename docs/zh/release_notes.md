# 版本说明书

## 版本配套说明

### 产品版本信息

<table><tbody><tr id="row41561572"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.1.1"><p id="p11044137">产品名称</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.1.1 "><p id="p1597721693713">Kunpeng BoostKit</p>
</td>
</tr>
<tr id="row24726251"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.2.1"><p id="p56669300">产品版本</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.2.1 "><p id="p11923034"><span id="text152431189308">26.1.RC1</span></p>
</td>
</tr>
<tr id="row1930811171892"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.3.1"><p id="p2030912172097">软件名称</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.3.1 "><p id="p1730912179911">kpglibc（Kunpeng Glibc）</p>
</td>
</tr>
<tr id="row5497143514612"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.4.1"><p id="p162251517551">软件包版本</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.4.1 "><p id="p6225131165519">1.0.4</p>
</td>
</tr>
</tbody>
</table>

### 与操作系统、编译器和CPU配套说明

|操作系统|CPU类型|编译器|
|--|--|--|
|openEuler 22.03 LTS SP3|鲲鹏920系列处理器|GCC 10.3.1|
|openEuler 24.03 LTS SP2|鲲鹏950处理器|GCC 12.3.1|
|Debian 12|鲲鹏950处理器|GCC 12.2.0|

## 版本更新说明

### V1.0.4

**修改特性**

针对鲲鹏950处理器持续优化memcmp函数。

### V1.0.3

**新增特性**

新增针对鲲鹏950处理器优化的memcpy/memmove/memcmp/memset函数，在大部分场景有加速效果。
验证适配Debian 12系统。

### V1.0.2

**修改特性**

针对鲲鹏920新型号处理器持续优化memcpy/memset函数，在小字节场景有加速效果。

### V1.0.1

**新增特性**

基于鲲鹏920新型号处理器优化Glibc常用的7个函数（strcpy、strcmp、memchr、memcmp、gettimeofday、memcpy、memset）。

## 版本配套文档

### V1.0.4版本配套文档

| 文档名称 | 内容简介 | 交付形式 |
| --- | --- | --- |
| 《版本说明书》 | 提供kpglibc每个发布版本的基础信息和特性更新信息。 | 开源仓 |
| 《API参考》 | 提供接口说明、接口调用示例等。 | 开源仓 |
| 《快速入门》 | 提供kpglibc使能并验证加速能力的快速入门指导。 | 开源仓 |
| 《安装指南》 | 提供kpglibc编译安装的详细指导。 | 开源仓 |

### 获取文档的方法

您可以通过[开源仓](https://gitcode.com/boostkit/kpglibc)浏览和获取相关文档。
