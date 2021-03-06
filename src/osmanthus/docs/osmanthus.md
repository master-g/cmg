Osmanthus Poker Evaluator
===

>全是二进制数据，万一有可执行代码怎么办？你怎么知道没有后门？ —— 摘自 CCTV《走近科学——中二病也要玩扑克》

Accurate, Fast, Safe
---

### 关于牌型计算

在扑克游戏中，如何确定同样张牌的情况下，不同组合的大小，从而决定谁胜谁负，构成了整个扑克游戏的核心玩法。  
在这里，我想简单介绍一种快速、准确，而且安全的牌型评估算法。
这个算法最初是由`Kevin L. Suffecool`发明的，由MG进行移植。

### 牌的阶和熵

**阶（rank）**，即扑克牌上的数字：2，3，4，5，6，7，8，9，10，J，Q，K，A  
**熵（entropy）**，即扑克牌组合后，用于和其他组合比较大小的一个度量  
例如，我们规定豹子AAA的熵为1，金花542的熵为298，那么熵值越小的牌型越大。

**请注意**，熵这个词是笔者从物理学里借来的一个概念，由于熵可以用于衡量一个系统的混乱程度，那么在本文当中，**`其值越小，价值就越高`**。如果读者有更规范的表述，请告知，不胜感激。

### Osmanthus Poker Hand Evaluator

任何学过组合数学的人都知道，在3张牌的情况下，一共有C(52, 3)，即22,100种可能的组合，只有两万多种，而且其中许多组合都有相同的熵值。例如，红桃顺子JQK和黑桃顺子JQK的熵值是一样的。尽管它们的组合都是特别的，它们拥有同样的值，在扑克中，这种熵值相同的情况就称为平局(tie)  

让我们换一种方式来看这个问题。假设你能够在足球场上聚集22,100个朋友，然后你给每个人3张牌，每个人都拿到那22,100种组合里唯一的一种。随后你大吼一声，让每个人都和其他人比较牌的大小，这个过程估计会花一些时间，随后，你让牌型最大的人站出来，显然，会有4个人站出来，他们手上拿着的是各种花色的豹子AAA，我们将他们编号为1，然后让他们离开足球场。然后我们再让场上的人相互比较手牌，这一次是另外4个人站出来，他们手上的是K阶豹子，我们将他们编号为2，让他们离开场地。我们不断重复这个过程，可以得到下面的数据。  

|比较的次数|人数|情况|概率|
|-------:|--:|--:|--:|
|-|52|-- **豹子** --|0.24%|
|1|4|AAA|…|  
|2|4|KKK|…|
|…|…|…|…|
|12|4|333|…|
|13|4|222|…|
|-|48|-- **金花顺** --|0.22%|
|14|4|AKQ|…|
|15|4|KQJ|…|
|…|…|…|…|
|24|4|432|…|
|25|4|32A|…|
|-|1096|-- **金花** --|4.96%|
|26|4|AKJ|…|
|27|4|AK10|…|
|…|…|…|…|
|298|4|542|…|
|299|4|532|…|
|-|720|-- **顺子** --|3.26%|
|300|60|AKQ|…|
|301|60|KQJ|…|
|…|…|…|…|
|310|60|432|…|
|311|60|32A|…|
|-|3744|-- **对子** --|16.94|
|312|24|AAK|…|
|313|24|AAQ|…|
|…|…|…|…|
|466|24|224|…|
|467|24|223|…|
|-|16440|-- **散牌** --|74.39%|
|-|3840|-- **A阶散牌** --|17.38%|
|468|60|AKJ|…|
|469|60|AK10|…|
|…|…|…|…|
|530|60|A43|…|
|531|60|A42|…|
|-|3240|-- **K阶散牌** --|14.66%|
|532|60|KQ10|…|
|533|60|KQ9|…|
|…|…|…|…|
|584|60|K42|…|
|585|60|K32|…|
|-|2640|-- **Q阶散牌** --|11.95%|
|586|60|QJ9|…|
|587|60|QJ8|…|
|…|…|…|…|
|628|60|Q42|…|
|629|60|Q32|…|
|-|2100|-- **J阶散牌** --|9.50%|
|630|60|J108|…|
|631|60|J107|…|
|…|…|…|…|
|663|60|J42|...|
|664|60|J32|…|
|-|1620|-- **10阶散牌** --|7.33%|
|665|60|1097|…|
|666|60|1096|…|
|…|…|…|…|
|690|60|1042|…|
|691|60|1032|…|
|-|1200|-- **9阶散牌** --|5.43%|
|692|60|986|…|
|693|60|985|…|
|…|…|…|…|
|710|60|942|…|
|711|60|932|…|
|-|840|-- **8阶散牌** --|3.80%|
|712|60|875|…|
|713|60|874|…|
|…|…|…|…|
|724|60|842|…|
|725|60|832|…|
|-|540|-- **7阶散牌** --|2.44%|
|726|60|764|…|
|727|60|763|…|
|…|…|…|…|
|733|60|742|…|
|734|60|732|…|
|-|300|-- **6阶散牌** --|1.36%|
|735|60|653|…|
|736|60|652|…|
|…|…|…|…|
|738|60|642|…|
|739|60|632|…|
|-|120|-- **5阶散牌** --|0.54%|
|740|60|542|…|
|741|60|532|…|

终于，在对两万种牌型进行比较之后，我门得到下面的表

|牌型|牌型数|熵相同的情况|
|--:|------:|-----------:|
|豹子|52|13|
|金花顺|48|12|
|金花|1096|274|
|顺子|720|12|
|对子|3744|156|
|散牌|16440|274|
|**`总数`**|**`22100`**|**`741`**|

**741**，这个数字就是3张牌的情况下，具有不同熵值的组合数量，请牢记这个数字：**741**.

#### 数据结构

当我们知道熵只有741种值的时候，我们需要找到将22,100种组合转换到对应的熵的方法。  
从复杂度上考虑，这个算法必须是顺序无关的，换句话说，如果将♦A♣K♠J传递进评估函数，那么无论按什么顺序传入，都必须返回468这个值。

我们可以使用质数(**`prime`**)来表示牌的阶(rank)  


|Rank|Deuce|Trey|Four|Five|Six|Seven|Eight|Nine|Ten|Jack|Queen|King|Ace|
|:--:|:---:|:--:|:--:|:--:|:-:|:---:|:---:|:--:|:-:|:--:|:---:|:--:|:-:|
|Prime|2|3|5|7|11|13|17|19|23|29|31|37|41|

这个系统美妙之处就在于，如果你将手上的牌的阶相乘，你都能够得到一个唯一的值，从而巧妙的避免了排序。上面的顺子♦A♣K♠J的值总是47027  

不过在相乘之前，必须检查牌的花色是否构成了金花顺。这对于整个评估函数而言至关重要，因为金花顺比顺子的熵要低很多（熵越小，牌的组合就越大）。  

Suffecool给出了用于表示每张牌的数据结构 —— 一个32位整数，或者说，4个字节

<pre>
+--------+--------+--------+--------+
|xxxbbbbb|bbbbbbbb|cdhsrrrr|xxpppppp|
+--------+--------+--------+--------+
</pre>

其中  
p 是 代表牌阶的质数(duce = 2, trey = 3, four = 5, …, ace = 41)  
r 是 牌阶的值(duce = 0, trey = 1, four = 2, five = 3, …, ace = 12)  
cdhs 是 牌的花色(c = club梅花, d = diamond方块, h = heart红桃, s = spade黑桃)  
b 是 按照不同牌阶的标志位

其中prime的表格之前已经给出了, 下面给出不同位域的表格  

|r域|牌阶|值|
|--:|--:|--:|
|0000|2|0|
|0001|3|1|
|0010|4|2|
|0011|5|3|
|0100|6|4|
|0101|7|5|
|0110|8|6|
|0111|9|7|
|1000|10|8|
|1001|J|9|
|1010|K|10|
|1011|Q|11|
|1100|A|12|

|cdhs域|花色|值|
|--:|--:|--:|
|0001|♠|1|
|0010|♥|2|
|0100|♦|4|
|1000|♣|8|

|b域|牌阶|值|
|--:|--:|--:|
|0000000000001|2|1|
|0000000000010|3|2|
|0000000000100|4|4|
|0000000001000|5|8|
|0000000010000|6|16|
|0000000100000|7|32|
|0000001000000|8|64|
|0000010000000|9|128|
|0000100000000|10|256|
|0001000000000|J|512|
|0010000000000|Q|1024|
|0100000000000|K|2048|
|1000000000000|A|4096|

用该系统表述牌的例子:  

<pre>
xxxAKQJT 98765432 CDHSrrrr xxPPPPPP
00001000 00000000 01001011 00100101 ♦K
00000000 00001000 00010011 00000111 ♠5
00000010 00000000 10001001 00011101 ♣J
</pre>

#### 算法

现在，我们可以将代表3张牌的整数传递到评估函数中去了，将3张牌分别标记为c1, c2, c3  

##### 首先检查花色，看看是不是同花:  

<pre>
c1 & c2 & c3 & 0xF000		/* '&' 是位的"与"运算 */
</pre>

如果上面的表达式不为0，那么我们就得到了一个同花  

下面的图应该非常非常清晰了

<pre>
xxxAKQJT 98765432 CDHSrrrr xxPPPPPP    card/mask
-------- -------- -------- --------
xxxxxxxx xxxxxxxx ????xxxx xxxxxxxx    c1
xxxxxxxx xxxxxxxx ????xxxx xxxxxxxx    c2
xxxxxxxx xxxxxxxx ????xxxx xxxxxxxx    c3
00000000 00000000 11110000 00000000    0xF000
-------- -------- -------- --------    result
                                       00000000 00000000 ????0000 00000000
</pre>

如果我们得到的是同花，我们还需要计算出组合的熵值（即将其转换为一个1到741之间的值）。查(水)表法（lookup table）是比较快的一种方法。  
下面的代码用于生成这个表的查询索引（下标）  

<pre>
q = (c1 | c2 | c3) >> 16
</pre>

图解

<pre>
xxxAKQJT 98765432 CDHSrrrr xxPPPPPP    card/mask
-------- -------- -------- --------
xxx????? ???????? xxxxxxxx xxxxxxxx    c1
xxx????? ???????? xxxxxxxx xxxxxxxx    c2
xxx????? ???????? xxxxxxxx xxxxxxxx    c3
-------- -------- -------- --------    result
                                       00000000 00000000 xxx????? ????????
</pre>

注意到我们现在得到的是金花，那么3张牌的阶必定是互异的。  
我们必定能在结果的?位域上有3个置1的位。  
最小的模式应该是0x0007(7)，二进制0 0000 0000 0111， 即432；  
最大的模式应该是0x1C00(7168)，二进制1 1100 0000 0000，即AKQ。  

自然的，我们需要一个大小为7169的数组，不妨称其为flushes[]。  
对于金花顺AKQ，索引值为7168，flushes[7168]的值为14 —— 因为14是金花顺AKQ的熵  
对于金花AKJ，模式为0x1A00(6656)，flushes[6656]的值应该是26 —— 因为26是金花AKJ的熵。  
显然，这种方式会导致数组中存在许多的冗余空间，这就是使用查找表所要付出的代价 —— 使用空间换取时间。  
不过，既然同花情况下最大的熵为7168，我们可以使用*short*类型来存放熵的值，这样，我们只需要14336bytes。并不是很大。

#####不是金花，也不是金花顺，就检查是否是顺子或是散牌:
同样的，我们还是使用查表法，建立一个名为unique3[]的数组，这个数组的大小与flushes一致。依然使用q作索引，如果查到的值为0，那么该组合就不是顺子或是散牌；如果查到的值不为0，那么这个值就是这3张牌的熵。例如，顺子KQJ的模式是0x0E00(3584)，unique3[3584]的值为301，而最小的散牌532，unique3[0x000B] = unique3[11] = 741。

####剩下的情况
使用两个简单的算法和两个查寻表，我们已经从741个熵值中确定了其中的286 * 2个。接下来就是质数（prime）登场的时候了  

剩下的情况包括豹子（三张同阶牌），和对子（有且仅有两张牌的阶相同），这时候就不能通过使用位运算来获得一个唯一的索引.  

将牌的质数提取出来并相乘:  
<pre>
q = (c1 & 0xFF) * (c2 & 0xFF) * (c3 & 0xFF)
</pre>

图解  

<pre>
xxxAKQJT 98765432 CDHSrrrr xxPPPPPP    card/mask
-------- -------- -------- --------
xxxxxxxx xxxxxxxx ???????? xx??????    c1
xxxxxxxx xxxxxxxx ???????? xx??????    c2
xxxxxxxx xxxxxxxx ???????? xx??????    c3
00000000 00000000 00000000 11111111    mask
-------- -------- -------- --------    result
                                       p[c1]*p[c2]*p[c3]
</pre>

质数乘积的值就是最后这个others数组的下标  

这个乘积的值跨度会还是比较可观的  
最小值是当组合为222时，他们的质数乘积为8;  
最大值是组合AAA，质数乘积为68,921。  

显然，这个时候查找表的长度是68,922, 将近134KB  

####哈希
如果觉得剩下情况的查找表太大，我们还可以通过完美最小哈希函数(MPH)的方式将查找的键值范围缩小  

基本思路是  

~~~
质数积hash = Hash(质数乘积)  
hash_others[质数积hash] = others[质数乘积]
~~~

使用的MPH算法能够将68,922范围内的稀疏键映射到256的范围内  

这样只需要不到1KB就能够存放所有others的熵值  

- - -

### 相关链接

[Kevin L. Suffecool的网站](http://www.suffecool.net/poker/evaluator.html)  
[Minimum Perfect Hashing相关网站](http://burtleburtle.net/bob/hash/perfect.html)
