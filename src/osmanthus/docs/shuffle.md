Shuffle
===

Fair play
---

>I must complain the cards are ill shuffled till I have a good hand -- Jonathan Swift  

### 随机混选算法

洗牌在扑克游戏里可以说是非常非常核心的部分，虽然有故意调整洗牌算法来提高整个游戏的刺激度的手段，但是这里我们只考虑公平公正的洗牌方式。

### Fisher-Yates Shuffle

这个算法应该是比较没有争议的选择，先看代码

<pre>
typedef uint32_t (*randfunc)();

void shuffle(int arr[], int len, randfunc func) {
  int i = len, j;

  while (--i) {
    j = func() % (i + 1);
    swap(&arr[i], &arr[j]);
  }
}
</pre>

其正确性在计算机科学大神Knuth的The Art of Computer Programming第二卷中的半数值算法里已经证明了。  
这里只做简单介绍  

上面代码等价于  
<pre>
for i = 1 to n do swap(a[i], a[random(i, n)]);
</pre>

其中random是一个随机函数  

### 测试

我们已经选择了Mersenne Twister作为我们的随机函数，接下来就可以测试一下这个洗牌函数了。

<pre lang="C"><code>
#include stdio.h
#include string.h
#include stdlib.h
#include "random.h"
#include "shuffle.h"

int main(int argc, char *argv[]) {
  int CARD_TYPES = 52;
  int TEST_COUNT = 1000;

  int i = 0;
  int j = 0;

  int cards[CARD_TYPES];
  int stat[CARD_TYPES][CARD_TYPES]; /* [card][position] */

  int maximum = 0;
  int minimum = 0;

  double standard_rrate = 0.0;
  double error_max = 0.0;
  double error_min = 0.0;

  random_init(1234);

  /* clear statics result */
  memset(stat, 0, sizeof(int) * CARD_TYPES * CARD_TYPES);

  /* read cmd */
  if (argc >= 3) {
    CARD_TYPES = atoi(argv[1]);
    TEST_COUNT = atoi(argv[2]);
  }

  minimum = TEST_COUNT;

  printf("\n Test on %d Cards, %d times\n", CARD_TYPES, TEST_COUNT);

  /* make cards */
  for (i = 0; i < CARD_TYPES; i++)
    cards[i] = i;

  /* do shuffle */
  for (i = 0; i < TEST_COUNT; i++) {
    shuffle(cards, CARD_TYPES, random_uint32);
    for (j = 0; j < CARD_TYPES; j++) {
      stat[cards[j]][j]++;
    }
  }

  for (i = 0; i < CARD_TYPES; i++) {
    printf("%2d ", i);
    for (j = 0; j < CARD_TYPES; j++) {
      if (stat[i][j] > maximum)
        maximum = stat[i][j];

      if (stat[i][j] < minimum)
        minimum = stat[i][j];

      printf("%8d ", stat[i][j]);
    }
    printf("\n");
  }

  standard_rrate = (double)(TEST_COUNT / (CARD_TYPES * 1.0));
  error_max = ((double)maximum - standard_rrate) / standard_rrate * 100.0;
  error_min = ((double)minimum - standard_rrate) / standard_rrate * 100.0;

  error_max = error_max > 0 ? error_max : -error_max;
  error_min = error_min > 0 ? error_min : -error_min;

  printf("\n max: %d min: %d standard:%d \n", maximum, minimum,
         (int)standard_rrate);
  printf("error max: %.3f min: %.3f\n", error_max, error_min);

  return 0;
}
</code></pre>

测试结果如下  

Test on 10 Cards, 1000 times  

|牌型|在每个位置出现的次数|  0|  1|  2|  3|  4|  5|  6|  7|  8|  9|
|:--|:----------------|--:|--:|--:|--:|--:|--:|--:|--:|--:|--:|
|0  |                 | 88|114|107| 93| 94|113| 97| 89| 98|107|
|1  |                 | 93|110|115| 99|110| 92| 96|104| 93| 88|
|2  |                 |103| 79| 89|116|107| 86|115|109|110| 86|
|3  |                 |106| 89|104|105|116| 97| 95|102| 97| 89|
|4  |                 |108|112| 84| 94|109| 85|102| 97|112| 97|
|5  |                 | 92|119| 98|104| 90|112| 95| 91| 98|101|
|6  |                 |104|102|100|105| 77| 84|112| 94| 99|123|
|7  |                 |107| 81|102| 79|103|108| 88|123| 97|112|
|8  |                 |104| 87|107|110| 89|119|101|101| 86| 96|
|9  |                 | 95|107| 94| 95|105|104| 99| 90|110|101|

误差为: 23%  

Test on 10 Cards, 10000 times

|牌型|在每个位置出现的次数|   0|   1|   2|   3|   4|   5|   6|   7|   8|   9|
|:--|:----------------|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
|0  |                 |1042|1047|1031| 968| 982| 992|1012| 950|1005| 971|
|1  |                 | 983|1003|1037| 965|1037| 973| 974| 996| 990|1042|
|2  |                 | 977| 970| 996|1051|1006| 997|1014|1006| 992| 991|
|3  |                 | 941| 994|1000|1006| 987|1012|1026|1044|1011| 979|
|4  |                 |1023| 987| 951| 984|1054| 999|1017|1035| 995| 955|
|5  |                 |1001|1048|1000| 999| 999| 963| 970| 982|1018|1020|
|6  |                 |1033|1037| 974| 973| 975|1033|1033| 985| 959| 998|
|7  |                 |1001| 950|1018| 974|1017|1003| 958|1006|1014|1059|
|8  |                 | 994| 997|1003|1005| 946|1022|1018| 967|1010|1038|
|9  |                 |1005| 967| 990|1075| 997|1006| 978|1029|1006| 947|

误差为: 5.9%

- - -
**52张牌的情况**  

Test on 52 Cards, 10000 times  
误差:24%

Test on 52 Cards, 100000 times  
误差:8%

Test on 52 Cards, 1000000 times  
误差:2.5%

可以得出结论，误差随着测试次数上升是呈收敛趋势的，证明Fisher-Yates算法是正确的，而且随机函数的质量也非常高，符合预期。
