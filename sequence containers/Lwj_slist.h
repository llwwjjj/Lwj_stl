//
//  Lwj_slist.h
//  S
//
//  Created by 练文健 on 2018/6/9.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_slist_h
#define Lwj_slist_h
#include "Lwj_alloc.h"
#include "Lwj_construct.h"
#include "Lwj_uninitialized.h"
#include "Lwj_iterator.h"
#include <algorithm>
namespace Lwj_stl{
    //Myslist单链表 节点和迭代器的设计在架构上比list复杂，运用了继承关系
    
    //节点基类
    struct __slist_node_base{
        __slist_node_base* next;
    };
    //节点派生类
    template<class T>
    struct __slist_node:public __slist_node_base{
        T data;
    };
    
    //全局函数 已知某一节点，插入新节点于其后,返回新节点
    inline __slist_node_base* __slist_make_link(__slist_node_base* pos,__slist_node_base* new_node){
        new_node->next=pos->next;
        pos->next=new_node;
        return new_node;
    }
    
    //全局函数 求链表大小
    inline size_t __slist_size(__slist_node_base* node){
        size_t result=0;
        for(;node!=NULL;node=node->next)
            ++result;
        return result;
    }
    
    
    //迭代器基类
    struct __slist_iterator_base{
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef forward_iterator_tag iterator_catagory;//单向
        
        __slist_node_base* node;
        __slist_iterator_base(__slist_node_base* x): node(x){}
        
        void incr(){node=node->next;}
        
        bool operator==(const __slist_iterator_base& x) const{
            return node==x.node;
        }
        
        bool operator!=(const __slist_iterator_base& x) const{
            return node!=x.node;
        }
    };
    
    //迭代器派生类
    template<class T,class Ref,class Ptr>
    struct __slist_iterator:public __slist_iterator_base{
        typedef __slist_iterator<T,T&,T*> iterator;
        typedef __slist_iterator<T,const T&,const T*> const_iterator;
        typedef __slist_iterator<T,Ref,Ptr> selfs;
        
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __slist_node<T> slist_node;
        
        __slist_iterator(slist_node* x):__slist_iterator_base(x){}
        __slist_iterator():__slist_iterator_base(0){}
        __slist_iterator(const iterator& x):__slist_iterator_base(x.node){}
        
        
        reference operator*() const {return ((slist_node*)node)->data;}
        pointer operator->() const {return &(operator*());}
        
        selfs& operator++(){
            incr();
            return *this;
        }
        selfs operator++(int){
            selfs tmp=*this;
            incr();
            return tmp;
        }
    };
    
    
    
    
    template<class T,class Alloc=Lwj_stl::Lwj_alloc>
    class Lwj_slist{
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        
        typedef __slist_iterator<T,T&,T*> iterator;
        typedef __slist_iterator<T,const T&,const T*> const_iterator;
    private:
        typedef __slist_node<T> slist_node;
        typedef __slist_iterator_base slist_iterator_base;
        typedef __slist_node_base slist_node_base;
        typedef simple_alloc<slist_node,Alloc> list_node_allocator;
        
        static slist_node* create_node(const value_type& x){
            slist_node* node = list_node_allocator::allocate();
            Lwj_stl::construct(&node->data,x);
            node->next=NULL;
            return node;
        }
        
        static void destroy_node(slist_node* node){
            Lwj_stl::destroy(&node->data);
            list_node_allocator::deallocate(node);
        }
    private:
        slist_node_base head;//头部 不是指针
    public:
        Lwj_slist() {head.next=NULL;}
        ~Lwj_slist(){clear();}
        
        iterator begin() {return iterator((slist_node*)head.next);}
        iterator end() {return NULL;}
        size_type size() const {return __slist_size(head.next);}
        bool empty() const{return head.next==NULL;}
        void clear() { erase_after(&head, 0); }
        
        //两个slist互换，只要将head交换互指即可
        void swap(Lwj_slist& sl){
            slist_node_base* tmp=head.next;
            head.next=sl.head.next;
            sl.head.next=tmp;
        }
    public:
        reference front() {return ((slist_node*)head.next)->data;}
        //创建一个节点，然后插到头部,没有push_back()
        void push_front(const value_type& x){__slist_make_link(&head, create_node(x));}
        void pop_front(){
            slist_node* node=(slist_node*)head.next;
            head.next=node->next;
            destroy_node(node);
        }
        
        slist_node_base* erase_after(slist_node_base* before_first,
                                    slist_node_base* last_node) {
            slist_node* cur = (slist_node*) (before_first->next);
            while (cur != last_node) {
                slist_node* tmp = cur;
                cur = (slist_node*) cur->next;
                destroy_node(tmp);
            }
            before_first->next = last_node;
            return last_node;
        }
    };
}

#endif /* Lwj_slist_h */
