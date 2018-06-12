# Lwj_stl
运用所学知识和自己的理解编写一个STL，目的在于加深对STL的理解。
开发环境：Xcode

目录
-------------
* [项目组件]
    * 内存配置
      * Lwj_uninitialized.h
      * Lwj_alloc.h
      * Lwj_construct.h
    * traits技巧
      * Lwj_iterator.h
      * Lwj_traits.h
    * 顺序容器
      * Lwj_vector.h
      * Lwj_list.h
      * Lwj_deque.h
      * Lwj_slist.h
      * Lwj_stack.h
      * Lwj_queue.h
      * Lwj_heap.h
      * Lwj_priority_queue.h
    * 关联容器
      * Lwj_rbtree.h
      * Lwj_set.h
      * Lwj_map.h
      * Lwj_hashtable.h(未完成)
      * Lwj_hashset.h(未完成)
      * Lwj_hashmap.h(未完成)
    * STL算法(尚未完成)
    



1.1 Lwj_alloc.h
------------------
* [allocate] 用于空间的配置
   * 默认使用第二级配置器，维护16个自由链表，链表大小为8，16，24，32，40，48，56，64，72，80，88，96，104，112，120，128和一个内存池
   * 调用allocate时：
      * 先判断申请的空间是否大于128个字节，如果大于则调用第一级配置器，直接调用malloc，内存不足则调用oom处理函数。
      * 若小于等于128个字节，则向上向8取整，如41则向自由链表链表48区块申请内存，如果自由链表之内有可用的区块，就直接拿来用，如果没有可用区块就像内存池申请空间来填充链表再分配内存。
      * 默认向内存池申请20个区块数大小的内存，如果内存池剩余空间完全满足需求量，则直接分配；若内存池剩余空间不能完全满足需求量，但足够供应一个或以上的区块，则能分配多少个区块大小的内存就分配多少；若内存池剩余空间连一个区块的大小都无法提供，则先把内存池剩余所有空间分配给适当的自由链表区块，再向heap空间申请内存，用来补充内存池；如果heap空间不足，就搜寻自由链表，将尚未用的且足够大的区块用来分配内存池。
      * 如果灯枯油尽，就调用第一级配置器，使用out-of-memory机制。
* [deallocate] 用于空间的释放
   * 调用deallocate时：
      * 先判断释放的空间是否大于128个字节，如果大于则调用第一级配置器，直接调用free。
      * 若小于等于128个字节，则调整自由链表回收区块。
* [reallocate] 用于空间的重新配置
   * 调用reallocate时：大于128个字节则直接调用realloc。
* [simple_alloc] 接口，便于容器使用底层的空间配置函数



1.2 Lwj_construct.h
---------------------
* [construct] 负责对象的构造
   * 使用placement new
* [destory] 负责对象的析构
   * 使用type_traits判断元素的数值类型是否有trival_destructor，做不同处理。
   * 针对迭代器为char* 和 wchar_t* 实施特化版
   
   
   
1.3 Lwj_uninitialized.h 
------------------------
* [uninitialized_copy] 在区间上构造元素
   * 首先判断迭代器类型是否为char* 和 wchar_t* 是则用内存底层操作memove，速度极快。
   * 萃取出迭代器的value_type，使用type_traits判断是否为POD类型
      * 是则调用STL算法copy实现拷贝
      * 不是则一个一个元素地构造，无法批量进行
* [uninitialized_fill_n] 在区间上填充n个元素
   * 首先萃取出迭代器的value_type，使用type_traits判断是否为POD类型
      * 是则调用STL算法fill_n实现拷贝
      * 不是则一个一个元素地构造，无法批量进行
* [uninitialized_fill] 在区间first--end上填充元素
   * 首先萃取出迭代器的value_type，使用type_traits判断是否为POD类型
      * 是则调用STL算法fill实现拷贝
      * 不是则一个一个元素地构造，无法批量进行      
  
  
  
2.1 LWj_traits.h & Lwj_iterator
----------------------------------
* [traits]
   * 为了提供通用的操作而又不损失效率，用到的一种特殊的技巧；通过定义一些结构体或类，并利用模板类特化和偏特化的能力，给类型赋予一些特性，这些特性根据类型的不同而异。可以使用这些traits来判断一个类型的一些特性，引发c++函数的重载机制，实现同一种操作因类型不同而异的效果。traits极度弥补了c++语言的不足。
* [iterator]
   * iterator_traits
   
 

3.1 Lwj_vector.h
------------------
* [迭代器]
   * vector维护的是一个连续线性空间，所以不论其元素类型是什么，普通指针都可以作为它的迭代器，Random Access Iterators
* [容器类]
   * 成员变量：三个迭代器，其中start表示目前使用空间的头，finish表示目前使用空间的尾，end_of_storage表示目前可用空间的尾。
   * 构造函数，析构函数
   * 成员函数：begin,end,capacity,size,empty,operator[],back,front,push_back,pop_back,insert
   * push_back 如果finish=end_of_storage，则重新申请一个原空间而被大小的空间，复制过去并执行push过程。
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_vector.h"
#include <vector>
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_vector<int> v;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        v.push_back(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_vector.h"
#include <vector>
using namespace std;
int main(int argc, const char * argv[]) {
    vector<int> v;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        v.push_back(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
contianer            |quantity     |time(ms)    |
---------------------|-------------|------------|
Lwj_stl::Lwj_vector  | 100,000     |1.287       |
Lwj_stl::Lwj_vector  | 1,000,000   |9.766       |
Lwj_stl::Lwj_vector  | 10,000,000  |120.845     |
Lwj_stl::Lwj_vector  | 100,000,000 |1137.44     |
std::vector          | 100,000     |2.326       |
std::vector          | 1,000,000   |22.257      |
std::vector          | 10,000,000  |265.646     |
std::vector          | 100,000,000 |2408.51     |

3.2 Lwj_list.h
------------------
* [节点]
   * 包含一个指向前节点的指针，一个指向后节点的指针和数据
* [迭代器]
   * 包含一个指向节点的指针，实现== != * -> ++ -- 操作符,Bidirectional Iterators
* [容器类]
   * 成员变量：只包含一个指向节点的指针，因为list是环状双向链表。
   * 构造函数，析构函数
   * 成员函数：begin,end,size,empty,get_node,put_node,destroy_node,back,front,push_back,,push_front,pop_back,pop_front,insert,clear,remove,unique,transfer,splice,merge,reverse,sort
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_list.h"
#include <list>
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_list<int> l;
    Lwj_stl::Lwj_list<int>::iterator it=l.begin();
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        l.insert(it,i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_list.h"
#include <list>
using namespace std;
int main(int argc, const char * argv[]) {
    list<int> l;
    list<int>::iterator it=l.begin();
    clock_t start = clock();
    for(int i=0;i!=100000000;++i)
        l.insert(it,i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
contianer            |quantity     |time(ms)    |
---------------------|-------------|------------|
Lwj_stl::Lwj_list    | 100,000     |4.385       |
Lwj_stl::Lwj_list    | 1,000,000   |44.615      |
Lwj_stl::Lwj_list    | 10,000,000  |439.551     |
Lwj_stl::Lwj_list    | 100,000,000 |4612.19     |
std::list            | 100,000     |12.741      |
std::list            | 1,000,000   |130.051     |
std::list            | 10,000,000  |1270.58     |
std::list            | 100,000,000 |12868.2     |


3.3 Lwj_deque.h
------------------
* [迭代器]
   * 包含一个所指缓冲区现行元素的指针，一个所指缓冲区的头的指针，一个所指缓冲区的尾的指针，和一个指向管控中心的指针 Random Access Iterators
   * ++ -- += -= 操作要考虑跳跃缓冲区
* [容器类]
   * 成员变量：包含一个表现第一个节点的迭代器和一个表现最后一个节点的迭代器
   * 构造函数，析构函数
   * 成员函数：begin,end,size,empty,back,front,push_back,,push_front,pop_back,pop_front,insert,clear,erase，reallocate_map
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_deque.h"
#include <deque>
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_deque<int> d;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        d.push_front(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
   
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_deque.h"
#include <deque>
using namespace std;
int main(int argc, const char * argv[]) {
    deque<int> d;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        d.push_front(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
contianer            |quantity     |time(ms)    |
---------------------|-------------|------------|
Lwj_stl::Lwj_deque   | 100,000     |0.85        |
Lwj_stl::Lwj_deque   | 1,000,000   |8.506       |
Lwj_stl::Lwj_deque   | 10,000,000  |85.481      |
Lwj_stl::Lwj_deque   | 100,000,000 |836.061     |
std::deque           | 100,000     |3.105       |
std::deque           | 1,000,000   |29.833      |
std::deque           | 10,000,000  |299.025     |
std::deque           | 100,000,000 |2910.16     |

3.4 Lwj_slist.h
------------------
* [节点]
   * 包含一个指向后节点的指针和数据
* [迭代器]
   * 包含一个指向节点的指针，实现== != * -> ++ 操作符,没有实现-- ，因为这是一个 forward iterator
* [容器类]
   * 成员变量：包含一个节点
   * 构造函数，析构函数
   * 成员函数：begin,end,size,empty,back,front,push_front,pop_front,insert,clear，create_node,destroy_node,swap   
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_slist.h"
#include <deque>
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_slist<int> s;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        s.push_front(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}

```
contianer            |quantity     |time(ms)    |
---------------------|-------------|------------|
Lwj_stl::Lwj_deque   | 100,000     |2.931       |
Lwj_stl::Lwj_deque   | 1,000,000   |29.414      |
Lwj_stl::Lwj_deque   | 10,000,000  |292.182     |
Lwj_stl::Lwj_deque   | 100,000,000 |3049.57     |

3.5 Lwj_stack.h
------------------
* [后进先出]
* [迭代器]
   * 没有迭代器，因为stack允许新增元素，移除元素，但除了最顶端外，没有任何其他方法可以存取stack的其他元素，也就是说stack不允许有遍历行为。
* [容器类]
   * 以某种既有容器尾底部结构
   * 成员函数：empty，size,top,push,pop,operator=,operator<
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_stack.h"
#include <stack>
#include "Lwj_vector.h"
#include "Lwj_list.h"
#include "Lwj_deque.h"
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_vector<int>> s;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        s.push(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_stack.h"
#include <stack>
#include <vector>
#include <list>
#include <deque>
using namespace std;
int main(int argc, const char * argv[]) {
    stack<int,vector<int>> s;
    clock_t start = clock();
    for(int i=0;i!=100000000;++i)
        s.push(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
adapter                                          |quantity     |time(ms)    |
-------------------------------------------------|-------------|------------|
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_vector<int>> | 100,000     |1.195       |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_vector<int>> | 1,000,000   |11.505      |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_vector<int>> | 10,000,000  |128.252     |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_vector<int>> | 100,000,000 |1243.05     |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_list<int>>   | 100,000     |4.703       |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_list<int>>   | 1,000,000   |46.771      |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_list<int>>   | 10,000,000  |457.986     |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_list<int>>   | 100,000,000 |4917.9      | 
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_deque<int>>  | 100,000     |1.072       |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_deque<int>>  | 1,000,000   |9.614       |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_deque<int>>  | 10,000,000  |95.469      |
Lwj_stl::Lwj_stack<int,Lwj_stl::Lwj_deque<int>>  | 100,000,000 |933.977     |   
std::stack<int,std::vector<int>>                 | 100,000     |2.584       |
std::stack<int,std::vector<int>>                 | 1,000,000   |22.847      |
std::stack<int,std::vector<int>>                 | 10,000,000  |273.921     |
std::stack<int,std::vector<int>>                 | 100,000,000 |2457.53     |
std::stack<int,std::list<int>>                   | 100,000     |11.686      |
std::stack<int,std::list<int>>                   | 1,000,000   |114.34      |
std::stack<int,std::list<int>>                   | 10,000,000  |1157.76     |
std::stack<int,std::list<int>>                   | 100,000,000 |11482.5     |
std::stack<int,std::deque<int>>                  | 100,000     |3.705       |
std::stack<int,std::deque<int>>                  | 1,000,000   |36.748      |
std::stack<int,std::deque<int>>                  | 10,000,000  |367.485     |
std::stack<int,std::deque<int>>                  | 100,000,000 |3600.53     |

3.6 Lwj_queue.h
------------------
* [先进先出]
* [迭代器]
   * 没有迭代器，因为queue允许末端新增元素，前端移除元素、也就是说queue不允许有遍历行为。
* [容器类]
   * 以某种既有容器尾底部结构
   * 成员函数：empty，size,push,pop,front,back,operator=,operator<
* [性能测试] 测试环境:Xcode
   * 测试代码如下
```cpp
#include <time.h>
#include <iostream>
#include "Lwj_queue.h"
#include "Lwj_list.h"
#include "Lwj_deque.h"
using namespace std;
int main(int argc, const char * argv[]) {
    Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_list<int>> q;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        q.push(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
```cpp
#include <time.h>
#include <iostream>
#include <queue>
#include <list>
#include <deque>
using namespace std;
int main(int argc, const char * argv[]) {
    queue<int,list<int>> q;
    clock_t start = clock();
    for(int i=0;i!=100000;++i)
        q.push(i);
    clock_t finish = clock();
    cout<<double(finish-start)/ CLOCKS_PER_SEC*1000 <<endl;
    return 0;
}
```
adapter                                          |quantity     |time(ms)    |
-------------------------------------------------|-------------|------------|
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_list<int>>   | 100,000     |4.638       |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_list<int>>   | 1,000,000   |46.663      |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_list<int>>   | 10,000,000  |471.375     |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_list<int>>   | 100,000,000 |4644.18     | 
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_deque<int>>  | 100,000     |0.972       |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_deque<int>>  | 1,000,000   |9.725       |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_deque<int>>  | 10,000,000  |95.553      |
Lwj_stl::Lwj_queue<int,Lwj_stl::Lwj_deque<int>>  | 100,000,000 |924.236     |   
std::queue<int,std::list<int>>                   | 100,000     |11.701      |
std::queue<int,std::list<int>>                   | 1,000,000   |116.98      |
std::queue<int,std::list<int>>                   | 10,000,000  |1180.74     |
std::queue<int,std::list<int>>                   | 100,000,000 |11739.6     |
std::queue<int,std::deque<int>>                  | 100,000     |3.838       |
std::queue<int,std::deque<int>>                  | 1,000,000   |44.797      |
std::queue<int,std::deque<int>>                  | 10,000,000  |388.711     |
std::queue<int,std::deque<int>>                  | 100,000,000 |3671.16     |


3.7 Lwj_heap.h
------------------
* [不归属于STL容器组件]
* [迭代器]
   * 没有迭代器，因为heap的所有元素都必须遵循特别的排列规则，所以heap不提供遍历功能。
* [heap算法]
   * push_heap
   * pop_heap
   * sort_heap
   * make_heap
   
 
 3.8 Lwj_priority_queue.h
--------------------------
* [权值最高者，排在最前面]
* [迭代器]
   * 没有迭代器，因为进出都有一定的规则，只有queue顶端的元素才有机会被外界去用，不提供遍历功能。
* [容器类]
   * 缺省情况下是以vector为底部容器
   
   
4.1 Lwj_rbtree.h
------------------
* [规则]
   * 每个节点不是红色就是黑色
   * 根节点为黑色
   * 如果节点为红，其子节点必须为黑
   * 任一节点值NULL的任何路径，所含黑节点数必须相同
* [节点]
   * 包含一个bool量表示节点颜色
   * 包含一个指向父节点的指针，一个指向左子节点的指针和一个指向右子节点的指针
   * 包含一个数值
* [迭代器]
   * 包含一个指向节点的指针，实现++ -- * -> 其中前进和后退的实现过程需要特别注意，Bidirectional iterator
* [容器类]
   * 成员变量：包含一个header,root的父亲，它的父亲也是root，左子节点指向最小键值，右子节点指向最大键值；包含一个无符号整型变量表示节点数量，一个函数对象表示键值大小比较准则
   * 构造函数，析构函数
   * 成员函数：root,leftmost,rightmost
             left,right,parent,value,key,color
             minimum,maxmum,key_comp,begin,end,empty,size,max_size
             insert_equal(允许节点值重复),insert_unique(保持节点值独一无二)
   * insert_equal/insert_unique:先满足各自对个数的要求，同时找出插入的位置，再让新节点为红插入，然后调整红黑树结构(左旋转，右旋转)
   * 1.插入节点的父节点为黑色 2.插入节点父节点为红，伯父节点为红 3.插入节点为黑，伯父节点为黑，外侧插入 4.插入节点为黑，伯父节点为黑，内侧插入


4.2 Lwj_set.h
--------------------------
* [键值=实值]
* [不允许两个元素值相同]
* [迭代器]
   * 不能通过迭代器改变set元素值，因为set元素值就是键值，关系到set元素的排列规则，也就是说set的迭代器是constant iterators
* [容器类]
   * 包含一个rbtree
   * 构造函数，析构函数
   * 成员函数：key_comp,begin,end,empty,size,max_size
             insert(insert_unique)均以rbtree的成员函数来实现
   * 默认以less<>比较键值，所以元素必须满足可以用<来比较
   
   
   
4.3 Lwj_map.h
--------------------------
* [键值不等于实值]
* [不允许两个元素拥有相同的键值]
* [迭代器]
   * 不能通过迭代器改变map的键值，但可以改变map的实值。
* [容器类]
   * 包含一个rbtree
   * 构造函数，析构函数
   * 成员函数：key_comp,begin,end,empty,size,max_size，operator[]
             insert(insert_unique)均以rbtree的成员函数来实现
   * 默认以less<>比较键值，所以元素必须满足可以用<来比较
