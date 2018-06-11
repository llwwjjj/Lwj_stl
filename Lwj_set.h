//
//  Lwj_set.h
//  S
//
//  Created by 练文健 on 2018/6/11.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_set_h
#define Lwj_set_h
#include "Lwj_rbtree.h"
namespace Lwj_stl{
    template<class Key,class Compare=std::less<Key>,class Alloc=Lwj_stl::Lwj_alloc>
    class Lwj_set{
    public:
        typedef Key key_type;
        typedef Key value_type;//实值就是键值
        typedef Compare key_compare;
        typedef Compare value_compare;//同一个比较函数
    private:
        template<class T>
        struct identity:public std::unary_function<T,T>{
            const T& operator()(const T& x)const{return x;}
        };
        typedef Lwj_rbtree<key_type, value_type, identity<value_type>, key_compare,Alloc> rb_type;
        
        rb_type t;
    public:
        typedef typename rb_type::const_pointer pointer;
        typedef typename rb_type::const_pointer const_pointer;
        typedef typename rb_type::const_reference reference;
        typedef typename rb_type::const_reference const_reference;
        typedef typename rb_type::const_iterator iterator;//set迭代器无法执行写入操作，因为set的元素有一定的次序安排
        typedef typename rb_type::const_iterator const_iterator;
        //reverse_iterator
        typedef typename rb_type::size_type size_type;
        typedef typename rb_type::difference_type difference_type;
        //构造，set不允许相同键值存在，所以使用insert_unique()
        Lwj_set():t(Compare()){}
        //explicit 构造函数不能在隐式转换时使用
        explicit Lwj_set(const Compare& comp):t(comp){}
        template<class InputIterator>
        Lwj_set(InputIterator first,InputIterator last):t(Compare()){t.insert_unique(first,last);}
        template<class InputIterator>
        Lwj_set(InputIterator first,InputIterator last,const Compare& comp):t(comp){t.insert_unique(first,last);}
        Lwj_set(const Lwj_set<Key,Compare,Alloc>& x):t(x.t){}
        Lwj_set<Key,Compare,Alloc>& operator=(const Lwj_set<Key,Compare,Alloc>& x){
            t=x.t;
            return *this;
        }
        //所有的set操作红黑树均已提供，所以set只要传递调用即可
        key_compare key_comp() const{return t.key_comp();}
        value_compare value_comp() const{return t.key_comp();}
        iterator begin() {return t.begin();}
        iterator end() {return t.end();}
        bool empty() const{return t.empty();}
        size_type size() const {return t.size();}
        size_type max_size() const{return t.max_size;}
        //void swap(Lwj_set<Key,Compare,Alloc>& x){t.Lwj_stl::swap(x.t);}
        void clear(){t.clear();}
       //----------------------------------------------------插入-----------------------------------------------------------
        
        Lwj_stl::pair<iterator,bool> insert(const value_type& x){
            return t.insert_unique(x);//这里存在pointer和reference转换为const_pointer和const_reference
        }
        template<class InputIterator>
        void insert(InputIterator first,InputIterator last){t.insert_unique(first,last);}
    };
}

#endif /* Lwj_set_h */
