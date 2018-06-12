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

3.1 Lwj_list.h
------------------
* [节点]
   * 包含一个指向前节点的指针，一个指向后节点的指针和数据
* [迭代器]
   * 包含一个指向节点的指针，实现== != * -> ++ -- 操作符
* [容器类]
   * 成员变量：只包含一个指向节点的指针，因为list是环状双向链表。
   * 构造函数，析构函数
   * 成员函数：begin,end,size,empty,get_node,put_node,destory_node,back,front,push_back,,push_front,pop_back,pop_front,insert,clear,remove,unique,transfer,splice,merge,reverse,sort
   * push_back 如果finish=end_of_storage，则重新申请一个原空间而被大小的空间，复制过去并执行push过程。
* [性能测试] 测试环境:Xcode
   * 测试代码如下
