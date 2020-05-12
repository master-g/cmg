Pseudo Number Generation
===

Flip the coin
---

>God doesn't play dice with the world -- Albert Einstein

### 随机数 Random Number

随机数是棋牌游戏中一个重要组成部分：生成发牌序列，制定AI的行为策略，生成游戏内容等都需要随机数  

### 伪随机数 Pseudo Number

遗憾的是，在标准计算机环境下，想要获得严格意义上的随机数比较困难，一般都需要使用专门的硬件设备，从诸如热噪声、放射源、大气物理现象等等中获取真随机数。处于简单原则，这里只介绍一种比较常用的伪随机数生成方法。

### 梅森旋转 Mersenne Twister

这个算法由Makoto Matsumoto(松本真)和Takuji Nishimura(西村拓士)在1997年提出的。通过在一个有限的二进制域上进行矩阵线性递归操作，可以快速地获得高质量的伪随机数，填补了其他旧算法的诸多缺陷。  

**Mersenne Twister算法的主要优势有:**

* 对于一个k比特宽的数据，MT算法可以在[0, 2^k - 1]的范围上生成几乎均匀分布的随机数
* 其最常用的变种MT19937，周期长达 2^19937 - 1
* 对于给定的k，1≤k≤623，其k分布精度高达32bit
* 通过了无数严格的统计学测试，包括Die-Hard，TestU01等等

当然，MT算法也有缺陷，但是这个缺陷主要体现在密码学应用上，故在此略去。

### 算法描述 Algorithm Description

严格的数学描述由于本人水平的缺失，在此先暂时不予描述，只给出wiki上的伪算法描述

<pre>
// Create a length 624 array to store the state of the generator
 int[0..623] MT
 int index = 0

 // Initialize the generator from a seed
 function initialize_generator(int seed) {
     index := 0
     MT[0] := seed
     for i from 1 to 623 { // loop over each other element
         MT[i] := last 32 bits of(1812433253 * (MT[i-1] xor (right shift by 30 bits(MT[i-1]))) + i) // 0x6c078965
     }
 }

 // Extract a tempered pseudorandom number based on the index-th value,
 // calling generate_numbers() every 624 numbers
 function extract_number() {
     if index == 0 {
         generate_numbers()
     }

     int y := MT[index]
     y := y xor (right shift by 11 bits(y))
     y := y xor (left shift by 7 bits(y) and (2636928640)) // 0x9d2c5680
     y := y xor (left shift by 15 bits(y) and (4022730752)) // 0xefc60000
     y := y xor (right shift by 18 bits(y))

     index := (index + 1) mod 624
     return y
 }

 // Generate an array of 624 untempered numbers
 function generate_numbers() {
     for i from 0 to 623 {
         int y := (MT[i] & 0x80000000)                       // bit 31 (32nd bit) of MT[i]
                        + (MT[(i+1) mod 624] & 0x7fffffff)   // bits 0-30 (first 31 bits) of MT[...]
         MT[i] := MT[(i + 397) mod 624] xor (right shift by 1 bit(y))
         if (y mod 2) != 0 { // y is odd
             MT[i] := MT[i] xor (2567483615) // 0x9908b0df
         }
     }
 }
</pre>

### 源代码
对该算法的C语言实现位于[kitsune](https://gitcafe.com/SNSTeam/kitsune/tree/master/kitsune/src/math)工程中

- - -
### 引用和链接
[MT官方网站](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html)  
[wiki百科](http://en.wikipedia.org/wiki/Mersenne_twister)
