# Release Notes

## Version Mapping

### Product Version

<table><tbody><tr id="row41561572"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.1.1"><p id="p11044137">Product Name</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.1.1 "><p id="p1597721693713">Kunpeng BoostKit</p>
</td>
</tr>
<tr id="row24726251"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.2.1"><p id="p56669300">Product Version</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.2.1 "><p id="p11923034"><span id="text152431189308">26.0.RC1</span></p>
</td>
</tr>
<tr id="row1930811171892"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.3.1"><p id="p2030912172097">Software Name</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.3.1 "><p id="p1730912179911">Kunpeng GNU C Library (kpglibc)</p>
</td>
</tr>
<tr id="row5497143514612"><th class="firstcol" valign="top" width="42.17%" id="mcps1.1.3.4.1"><p id="p162251517551">Software Version</p>
</th>
<td class="cellrowborder" valign="top" width="57.830000000000005%" headers="mcps1.1.3.4.1 "><p id="p6225131165519">1.0.3</p>
</td>
</tr>
</tbody>
</table>

### OS, Compiler, and CPU

|OS|CPU|Compiler|
|--|--|--|
|openEuler 22.03 LTS SP3|Kunpeng 920 series|GCC 10.3.1|
|openEuler 24.03 LTS SP2|Kunpeng 950|GCC 12.3.1|
|Debian 12|Kunpeng 950|GCC 12.2.0|

## Version Updates

### V1.0.3

**New Features**

The memcpy, memmove, memcmp, and memset functions optimized for the Kunpeng 950 processor are added, providing acceleration in most scenarios.
Adaptation to Debian 12 is verified.

### V1.0.2

**Modified Features**

The memcpy and memset functions are continuously optimized for the new Kunpeng 920 processor model, providing acceleration in small-byte scenarios.

### V1.0.1

**New Features**

Seven common glibc functions (strcpy, strcmp, memchr, memcmp, gettimeofday, memcpy, and memset) are optimized based on the new Kunpeng 920 processor model.
