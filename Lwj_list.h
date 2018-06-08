//
//  Lwj_list.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_list_h
#define Lwj_list_h
#include "Lwj_alloc.h"
#include "Lwj_construct.h"
#include "Lwj_uninitialized.h"
#include "Lwj_iterator.h"
#include <algorithm>
namespace Lwj_stl{
    //-----------------------------------------------------------节点------------------------------------------------------------
    template<class T>
    struct ListNode{
        typedef void* void_pointer;
        void_pointer prev;
        void_pointer next;
        T data;
    };
    
    //-----------------------------------------------------------迭代器-----------------------------------------------------------
    template<class T,class Ref,class Ptr>
    struct ListIterator{
        //未继承std::iterator,所以必须自行撰写五个必要的迭代器相应类型
        typedef ListIterator<T,T&,T*> iterator;
        typedef ListIterator<T,Ref,Ptr> selfs;
        typedef bidirectional_iterator_tag iterator_catagory;//1
        typedef T value_type;//2
        typedef Ptr pointer;//3
        typedef Ref reference;//4
        typedef ListNode<T>* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;//5
        
        link_type node;//普通指针，指向list的节点
        
        ListIterator(link_type x):node(x){}//用普通指针初始化
        ListIterator() {}
        ListIterator(const iterator& x):node(x.node){}//用迭代器初始化
        
        bool operator==(const selfs x) const {return node=x.node;}
        bool operator!=(const selfs x) const {return node!=x.node;}
        
        reference operator*() const {return (*node).data;}
        pointer operator->() const {return &(operator*());}//标准做法
        
        selfs& operator++(){
            node=(link_type)((*node).next);
            return *this;
        }
        selfs operator++(int){
            selfs tmp=*this;
            ++ *this;
            return tmp;
        }
        
        selfs& operator--(){
            node=(link_type)((*node).prev);
            return *this;
        }
        selfs operator--(int){
            selfs tmp=*this;
            -- *this;
            return tmp;
        }
    };
    
    //---------------------------------------------------------List结构-----------------------------------------------------------
    template <class T,class Alloc = Lwj_alloc>
    class Lwj_list{
    protected:
        typedef ListNode<T> list_node;
    public:
        typedef ListNode<T>* link_type;
    protected:
        link_type node;//SGI list是环状双向链表，只需要一个指针便可以表现整个链表
    protected:
        typedef void* void_pointer;
        typedef simple_alloc<list_node, Alloc> list_node_allocator; //另外定义的，更方便地以节点大小为配置单位
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
    public:
        typedef ListIterator<T,T&,T*> iterator;
        typedef ListIterator<T,const T&,const T*> const_iterator;
        
        iterator begin(){return (link_type)node->next;}
        iterator end(){return node;}//node指向刻意置于尾端的一个空白节点
        
        bool empty() const {return node->next == node;}
        size_type size() const {
            size_type result=0;
            distance(begin(),end(),result);//全局函数，判断iterator_catagory
            return result;
        }
        
        reference front() { return *begin();}
        reference back() {return *(--end());}
        
        //配置一个节点并传回
        link_type get_node(){ return list_node_allocator::allocate();}
        //释放一个节点
        void put_node(link_type p){ list_node_allocator::deallocate(p);}
        //配置并构造一个节点，带有元素值
        link_type create_node(const T& x){
            link_type p=get_node();
            construct(&p->data,x);
            return p;
        }
        //析构并释放一个节点
        void destroy_node(link_type p){
            destroy(&p->data);
            put_node(p);
        }
        
        
        //构造函数之一，产生一个空链表
        Lwj_list(){empty_initialize();}
        
    protected:
        void empty_initialize(){
            node=get_node();
            node->next=node;
            node->prev=node;//令头尾都指向自己，不设元素值
        }
    public:
        void push_back(const T& x) { insert(end(),x);}
        void push_front(const T& x){insert(begin(),x);}
        
        iterator insert(iterator position,const T& x){
            link_type tmp = create_node(x);
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            (link_type(position.node->prev))->next = tmp;
            position.node->prev = tmp;
            return tmp;
        }
        
        
        //移除迭代器position所指节点,指向删掉节点后的节点
        iterator erase(iterator position){
            link_type next_node=link_type(position.node->next);
            link_type prev_node=link_type(position.node->prev);
            prev_node->next=next_node;
            next_node->prev=prev_node;
            destroy_node(position.node);
            return iterator(next_node);
        }
        
        //移除头节点
        void pop_front(){ erase(begin()); }
        //移除尾节点
        void pop_back(){
            iterator tmp=end();
            erase(--tmp);
        }
        
        //清除所有节点 O(n)
        void clear(){
            link_type current=begin();
            while(current != node){//遍历每一个节点
                link_type tmp=current;
                current=(link_type)current->next;
                destroy(tmp);
            }
            //恢复node原始状态
            node->next=node;
            node->prev=node;
        }
        
        //将数值为value的所有元素移除 O(n)
        void removve(const T& value){
            iterator first= begin();
            iterator last = end();
            while(first!=last){
                iterator tmp=first;
                ++first;
                if(*tmp==value)
                    erase(tmp);
            }
        }
        
        //去重,需要重复元素连续
        void unique(){
            iterator first= begin();
            iterator last = end();
            if(first==last) return;
            iterator tmp=first;
            while(first!=last){
                ++tmp;
                if(*first==*tmp)
                    erase(tmp);
                else
                    first=tmp;
                tmp=first;
            }
        }
        
        //将[first,last)内的所有元素移动到position之前,first和last和position可以在同一个list中,并非公开借口
        void transfer(iterator position,iterator first,iterator last){
            if(position!=last){
                link_type(last.node->prev)->next=position.node;
                link_type(first.node->prev)->next=last.ndoe;
                link_type(position.node->prev)->next=first.ndoe;
                link_type tmp=link_type(position.node->prev);
                position.node->prev=last.node->prev;
                last.node->prev=first.node->prev;
                first.node->prev=tmp;
            }
        }
        
        //接合操作
        void splice(iterator position,Lwj_list& x){
            if(x!=empty())
                transfer(position,x.begin(),x.end());
        }
        
        void splice(iterator position,iterator i){
            iterator j=i;
            ++j;
            if(position==i||position==j)
                return;
            transfer(position,i,j);
        }
        
        void splice(iterator position,iterator first,iterator last){
            if(first!=last)
                transfer(position,first,last);
        }
        
        //将x合并到*this身上，两个lists的内容都必须先经过递增排序
        void merge(Lwj_list<T/*,Alloc*/>& x){
            iterator first1=begin();
            iterator last1=end();
            iterator first2=x.begin();
            iterator last2=x.end();
            
            while(first1!=last1 && first2!=last2)
                if(*first2 < *first1){
                    iterator next=first2;
                    transfer(first1,first2,++next);
                    first2=next;
                }
                else
                    ++first1;
            if(first2!=last2)
                transfer(last1,first2,last2);
        }
        
        void reverse(){
            //空链表或只有一个元素则不进行操作，不用size（）==0和size（）==1，因为慢很多O（n）
            if(node->next==node || link_type(node->next)->next==node)
                return;
            iterator first =begin();
            ++first;
            while(first!=end()){
                iterator old=first;
                ++first;
                transfer(begin(),old,first);
            }
        }
        
        
        //list不能使用STL算法sort()，必须使用自己的sort()成员函数
        //因为STL算法sort()只接受RamdonAccessIterator
        //采用快速排序
        void sort(){
            //空链表或只有一个元素则不进行操作，不用size（）==0和size（）==1，因为慢很多O（n）
            if(node->next==node || link_type(node->next)->next==node)
                return;
            //一些新的lists,作为中介数据区
            Lwj_list<T,Alloc> carry;
            Lwj_list<T,Alloc> counter[64];
            int fill = 0;
            while(!empty()){
                carry.splice(carry.begin(),*this,begin());
                int i=0;
                while(i<fill && !counter[i].empty()){
                    counter[i].merge(carry);
                    carry.swap(counter[i++]);
                }
                carry.swap(counter[i]);
                if(i==fill)
                    ++fill;
            }
            for(int i=1; i<fill;++i)
                counter[i].merge(counter[i-1]);
            swap(counter[fill-1]);
        }
    };
}

#endif /* Lwj_list_h */
