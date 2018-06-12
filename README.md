# Lwj_stl
运用所学知识和自己的理解编写一个STL，目的在于加深对STL的理解。
开发环境：Xcode

目录
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
    

Lwj_alloc.h空间配置器
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

Lwj_construct.h
* [construct] 负责对象的构造
   * 使用placement new
* [destory] 负责对象的析构
   * 使用type_traits判断元素的数值类型是否有trival_destructor，做不同处理。
   * 针对迭代器为char* 和 wchar_t* 实施特化版
   
   ```cpp
      string &operator+(const string& A,const string& B) //cpp
   ```
   
