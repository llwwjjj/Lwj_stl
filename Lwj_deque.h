//
//  Lwj_deque.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_deque_h
#define Lwj_deque_h
#include "Lwj_alloc.h"
#include "Lwj_construct.h"
#include "Lwj_uninitialized.h"
#include "Lwj_iterator.h"
#include <algorithm>
#include <math.h>
namespace Lwj_stl{
    //------------------------------------------------------全局函数-------------------------------------------------------------
    inline size_t Lwj_deque_BufSiz(size_t n,size_t sz){
        return n!=0 ? n : (sz<512 ? size_t(512/sz) : size_t(1));
    }
    
    
    //-------------------------------------------------------迭代器--------------------------------------------------------------
    template <class T,class Ref,class Ptr,size_t BufSiz>
    class Lwj_deque_iterator{
    public:
        typedef Lwj_deque_iterator<T,T&,T*,BufSiz> iterator;
        typedef Lwj_deque_iterator<T,const T&,const T*,BufSiz> const_iterator;
        static size_t buffer_size(){return Lwj_deque_BufSiz(BufSiz,sizeof(T));}//调用全局函数
        //未继承std::iterator,所以必须自行撰写五个必要的迭代器相应类型
        typedef random_access_iterator_tag iterator_category;//1
        typedef T value_type;//2
        typedef Ptr pointer;//3
        typedef Ref reference;//4
        typedef ptrdiff_t difference_type;//5
        typedef size_t size_type;
        typedef T** map_pointer;
        typedef Lwj_deque_iterator<T,Ref,Ptr,BufSiz> selfs;
        //保持与容器的联结
        T* cur;//此迭代器指向缓冲区中的现行元素
        T* first;//此迭代器指向缓冲区的头
        T* last;//此迭代器指向缓冲区的尾
        map_pointer node;//指向管控中心
        //deque::begin()传回迭代器start,deque::end()传回迭代器finish，这两个迭代器都有着四个指针
        
        //deque迭代器加减等操作都要小心遇到缓冲区边缘
        
        //跳到新的缓冲区
        void set_node(map_pointer new_node){
            node = new_node;
            first = *new_node;
            last = first + difference_type(buffer_size());
        }
        
    public:
        //重载运算符
        reference operator*() const {return *cur;}
        pointer operator->() const {return &(operator*());}
        
        difference_type operator- (const selfs& x) const{
            return difference_type(buffer_size())*(node-x.node-1)+(cur-first)+(x.last-x.cur);
        }
        
        
        selfs& operator++(){
            ++cur;
            if(cur==last){
                set_node(node+1);
                cur=first;
            }
            return *this;
        }
        selfs operator++(int){
            selfs tmp=*this;
            ++*this;
            return tmp;
        }
        
        selfs& operator--(){
            if(cur==first){
                set_node(node-1);
                cur=last;
            }
            --cur;
            return *this;
        }
        selfs operator--(int){
            selfs tmp=*this;
            --*this;
            return tmp;
        }
        
        
        //以下实现随机存取，迭代器可以直接跳跃n个距离
        selfs& operator+=(difference_type n){
            difference_type offset=n+cur-first;
            if(offset>=0 && offset<difference_type(buffer_size()))//在同一缓冲区内
                cur+=n;
            else{//不在同一缓冲区内
                difference_type node_offset=offset>0? offset/difference_type(buffer_size()) : -difference_type((-offset-1)/buffer_size())-1;
                //切换至正确的节点
                set_node(node+node_offset);
                //切换至正确的元素
                cur=first+(offset-node_offset*difference_type(buffer_size()));
            }
        }
        selfs operator+(difference_type n) const {
            selfs tmp=*this;
            return tmp+=n;
        }
        
        selfs& operator-=(difference_type n){return *this+= -n;}
        selfs operator-(difference_type n) const {
            selfs tmp=*this;
            return tmp-=n;
        }
        
        //实现随机存取，迭代器可以直接跳跃n个距离,即迭代器可以it[2]这样
        reference operator[](difference_type n) const{ return *(*this+n);}
        
        bool operator==(const selfs& x) const {return cur==x.cur;}
        bool operator!=(const selfs& x) const {return !(*this==x);}//用==实现
        bool operator<(const selfs& x) const {return (node==x.node) ? (cur<x.cur) : (node<x.node);}
    };
    
    
    //--------------------------------------------------deque类--------------------------------------------------------------
    template<class T,class Alloc=Lwj_alloc,size_t BufSiz=0>
    class Lwj_deque{
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type& reference;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef Lwj_deque_iterator<T, T&, T*, BufSiz> iterator;
        static size_t buffer_size(){return Lwj_deque_BufSiz(BufSiz,sizeof(T));}
        
        typedef simple_alloc<value_type, Alloc> data_allocator;
        typedef simple_alloc<pointer,Alloc> map_allocator;
    protected:
        //指向元素指针的指针
        typedef pointer* map_pointer;
    protected:
        map_pointer map;//指向map,map是块连续空间,其内的每个元素都是一个指针，称为节点，指向一块缓冲区,类型是T**
        size_type map_size;//map内可容纳多个指针
        iterator start;//第一个节点
        iterator finish;//最后一个节点
    public:
        iterator begin(){return start;}
        iterator end(){return finish;}
        
        reference operator[](size_type n){return start[difference_type(n)];}
        reference front(){return *start;}
        reference back(){
            iterator tmp=finish;
            --tmp;
            return tmp;
        }
        size_type size() const {return finish-start;;}//?
        size_type max_size() const {return size_type(-1);}//因为size_t是unsigned的，所以将-1强制转换为unsigned会得到这个类型的最大值
        bool empty() const {return finish == start;}
        
        
        void create_map_and_nodes(size_type num_elements) {
            // 需要節點數=(元素個數/每個緩衝區可容納的元素個數)+1
            // 如果剛好整除，會多配一個節點。
            size_type num_nodes = num_elements / buffer_size() + 1;
            
            // 一個 map 要管理幾個節點。最少8個，最多是 “所需節點數加2”
            // （前後各預留一個，擴充時可用）。
            map_size = (8>num_nodes + 2)?8:num_nodes+2;
            map = map_allocator::allocate(map_size);
            // 以上配置出一個 “具有 map_size個節點” 的map。
            
            // 以下令nstart和nfinish指向map所擁有之全部節點的最中央區段。
            // 保持在最中央，可使頭尾兩端的擴充能量一樣大。每個節點可對應一個緩衝區。
            map_pointer nstart = map + (map_size - num_nodes) / 2;
            map_pointer nfinish = nstart + num_nodes - 1;
            
            map_pointer cur;
                // 為map內的每個現用節點配置緩衝區。所有緩衝區加起來就是deque的空間
                // （最後一個緩衝區可能留有一些餘裕）。
                for (cur = nstart; cur <= nfinish; ++cur)
                    *cur = allocate_node();
            start.set_node(nstart);
            finish.set_node(nfinish);
            start.cur=start.first;
            finish.cur=finish.first;
            }
    
        Lwj_deque(): start(), finish(), map(0), map_size(0)
        // 以上 start() 和 finish() 喚起 iterator（亦即 __deque_iterator）
        // 的 default ctor，於是令其 cur, first, last, node 皆為0。
        {
            create_map_and_nodes(0);
        }
        
        Lwj_deque(int n,const value_type& value):start(),finish(),map(0),map_size(0){
            fill_initialized(n,value);
        }
    private:
        pointer allocate_node() { return data_allocator::allocate(buffer_size()); }
        void deallocate_node(pointer n) {data_allocator::deallocate(n, buffer_size());}
    public:
        void fill_initialized(size_type n,const value_type& value){
            create_map_nodes(n);//把deque的结构都产生并安排好
            map_pointer cur;
            for(cur=start.node;cur!=finish.node;++cur)
                Lwj_stl::uninitialized_fill(*cur,*cur+buffer_size(),value);
            //最后一个节点的设定稍有不同
            Lwj_stl::uninitialized_fill(finish.first,finish.cur,value);
        }
        
        void create_map_nodes(size_type num_elements){
            size_type num_nodes=num_elements/buffer_size()+1;
            if(num_nodes+2>8)//最少八个，最多是所需节点数+2
                map_size=num_nodes+2;
            else
                map_size=8;
            map=map_allocator::allocate(map_size);
            //保持在最中央
            map_pointer nstart=map+(map_size-num_nodes)/2;
            map_pointer nfinish=nstart+num_nodes-1;
            map_pointer cur;
            for(cur=nstart;cur<=nfinish;++cur)
              *cur=allocate_node();
            
            start.set_node(nstart);
            finish.set_node(nfinish);
            start.cur=start.first;
            finish.cur=finish.first+num_elements%buffer_size();
        }
        
        //元素操作
        void push_back(const value_type& t){
            if(finish.cur!=finish.last-1){
                //最后缓冲区尚有两个以上的元素备用空间
                Lwj_stl::construct(finish.cur,t);
                ++finish.cur;
            }
            else{
                //若符合某种条件则必须重换一个map
                if(map_size-(finish.node-map)>2)
                    reallocate_map(false);
                *(finish.node+1)=allocate_node();
                Lwj_stl::construct(finish.cur,t);
                finish.set_node(finish.node+1);
                finish.cur=finish.first;
                
                deallocate_node(*(finish.node+1));
            }
        }
        
        void push_front(const value_type& t){
            if(start.cur!=start.first){
                Lwj_stl::construct(start.cur-1,t);
                --start.cur;
            }
            else{
                //若符合某种条件则必须重换一个map
                if(start.node-map<1)
                    reallocate_map(true);
                *(start.node-1)=allocate_node();
                start.set_node(start.node-1);
                start.cur=start.last-1;
                Lwj_stl::construct(start.cur,t);
            }
        }
        
        void reallocate_map(bool add_at_front){
            size_type old_num_nodes=finish.node-start.node+1;
            size_type new_num_nodes=old_num_nodes+1;
            map_pointer new_nstart;
            //在旧map上做文章,一头不够空间另一头空间还很多
            if(map_size>2*new_num_nodes){
                new_nstart=map+(map_size-new_num_nodes)/2+(add_at_front?1:0);
                if(new_nstart<start.node)
                    std::copy(start.node,finish.node+1,new_nstart);
                else
                    std::copy_backward(start.node, finish.node+1, new_nstart+old_num_nodes);
            }
            //新map
            else{
                size_type new_map_size=2*map_size+2;
                //配置一块空间，准备给新map使用
                map_pointer new_map=map_allocator::allocate(new_map_size);
                new_nstart=new_map+(new_map_size-new_num_nodes)/2+(add_at_front?1:0);
                //把原map内容拷过来
                std::copy(start.node,finish.node+1,new_nstart);
                //释放原map
                map_allocator::deallocate(map,map_size);
                //设定新map的起始地址和大小
                map=new_map;
                map_size=new_map_size;
                //不需要配置节点，因为是map中的指针移动
                //重新设定迭代器start和finish
                start.set_node(new_nstart);
                finish.set_node(new_nstart+old_num_nodes-1);
                //不需要更新cur，因为这是在push_back和push_front中调用的，push_back/push_front后续会更新
                
            }
        }
        
        
        void pop_back(){
            if(finish.cur!=finish.first){//最后的缓冲区有一个以上元素
                --finish.cur;
                Lwj_stl::destroy(finish.cur);
            }
            else{//最后的缓冲区没有元素
                deallocate_node(finish.first);//释放最后一个缓冲区
                finish.set_node(finish.node-1);
                finish.cur=finish.last-1;
                Lwj_stl::destroy(finish.cur);//将该元素析构
            }
        }
        void pop_front(){
            if(start.cur!=start.last-1){
                Lwj_stl::destroy(start.cur);
                ++start.cur;
            }
            else{
                Lwj_stl::destroy(start.cur);
                deallocate_node(start.first);
                start.set_node(start.node+1);
                start.cur=start.first;
            }
        }
        
        //清除整个deque，deque最初状态保有一个缓冲区，因此要恢复原始状态，也要保留一个缓冲区
        void clear(){
            //以下针对头尾以外的每一个缓冲区，他们一定是饱满的
            for(map_pointer node=start.node+1;node<finish.node;++node){
                //将缓冲区内的所有元素析构，调用destory第二版本
                Lwj_stl::destroy(*node,*node+buffer_size());
                //释放缓冲区内存
                data_allocator::deallocate(*node,buffer_size());
            }
            if(start.node!=finish.node){//至少有头尾两个缓冲区
                Lwj_stl::destroy(start.cur,start.last);
                Lwj_stl::destroy(finish.start,finish.cur);
                //以下释放尾缓冲区，保留头缓冲区
                data_allocator::deallocate(finish.first,buffer_size());
            }
            else//只有一个缓冲区
                Lwj_stl::destroy(start.cur,finish.cur);
            
            finish=start;//调整状态
        }
        
        
        //清除pos所指元素
        iterator erase(iterator pos){
            iterator next=pos;
            ++next;
            difference_type index=pos-start;//清除那个点之前的元素个数
            if(index<(size()>>1)){//如果清除点之前的元素比较少，就移动清除点之前的元素
                copy_backward(start, pos, next);
                pop_front();//去掉最前面的元素
            }
            else{
                copy(next,finish,pos);
                pop_back();//去掉最后面的元素
            }
            return start+index;
        }
        
        //清除[first,last)区间内的所有元素
        iterator erase(iterator first,iterator last){
            if(first==start && last==finish){
                clear();
                return finish;
            }
            else{
                difference_type n=last-first;
                difference_type elems_before=first-start;
                if(elems_before<(size()-n)/2){//如果前方元素比较少
                    copy_backward(start, first, last);
                    iterator new_start=start+n;//标记新起点
                    Lwj_stl::destroy(start,new_start);
                    //将冗余的缓冲区释放
                    for(map_pointer cur =start.node;cur<new_start.node;++cur)
                    data_allocator::deallocate(*cur,buffer_size());
                    start=new_start;
                }
                else{
                    copy(last,finish,first);
                    iterator new_finish=finish-n;
                    Lwj_stl::destroy(new_finish,finish);
                    for(map_pointer cur =finish.node+1;cur<=new_finish.node;++cur)
                    data_allocator::deallocate(*cur,buffer_size());
                    finish=new_finish;
                }
                return start+elems_before;
            }
        }
        
        //在pos处插入一个元素
        void insert(iterator pos,const T& x){
            if(pos.cur==start.cur){
                push_front(x);
                return start;
            }
            else if(pos.cur==finish.cur){
                push_back(x);
                iterator tmp=finish;
                --tmp;
                return tmp;
            }
            else{
                difference_type index=pos-start;
                if(index<size()>>1){//前面元素较少
                    push_front(front());//前端加入与第一元素同值的
                    iterator front1=start;
                    ++front1;
                    iterator front2=front1;
                    ++front2;
                    pos=start+index;//迭代器会失效，要重置
                    iterator pos1=pos;
                    ++pos1;
                    copy(front2,pos1,front1);
                }
                else{
                    push_back(back());
                    iterator back1=finish;
                    --back1;
                    iterator back2=back1;
                    --back2;
                    pos=start+index;
                    copy_backward(pos,back2,back1);
                }
                *pos=x;
                return pos;
            }
        }
    };
}
#endif /* Lwj_deque_h */
