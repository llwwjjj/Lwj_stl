//
//  Lwj_map.h
//  S
//
//  Created by 练文健 on 2018/6/11.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_map_h
#define Lwj_map_h
#include "Lwj_rbtree.h"
namespace Lwj_stl{
    template <class Key,class T,class Compare=std::less<Key>,class Alloc=Lwj_stl::Lwj_alloc>
    class Lwj_map{
    public:
        typedef Key key_type;//键值类型
        typedef T data_type;//数据类型
        typedef T mapped_type;
        typedef Lwj_stl::pair<const Key,T> value_type;//元素类型
        typedef Compare key_compare;
        class value_compare:public std::binary_function<value_type,value_type,bool>{
        friend class Lwj_map<Key,T,Compare,Alloc>;
        protected:
            Compare comp;
            value_compare(Compare c):comp(c){}
        public:
            //也就是比较两键值的大小
            bool operator()(const value_type& x,const value_type& y)const{return comp(x.first,y.first);}
        };
        
    private:
        //接受一个pair，返回其第一元素
        template <class Pair>
        struct select1st : public std::unary_function<Pair, typename Pair::first_type> {
            const typename Pair::first_type& operator()(const Pair& x) const
            {
                return x.first;
            }
        };
        
        typedef Lwj_rbtree<key_type, value_type,select1st<value_type>, key_compare,Alloc> rb_type;
        rb_type t;
    public:
        typedef typename rb_type::pointer pointer;
        typedef typename rb_type::const_pointer const_pointer;
        typedef typename rb_type::reference reference;
        typedef typename rb_type::const_reference const_reference;
        typedef typename rb_type::iterator iterator;
        typedef typename rb_type::const_iterator const_iterator;
        //reverse_iterator
        typedef typename rb_type::size_type size_type;
        typedef typename rb_type::difference_type difference_type;
        
        //构造函数
        Lwj_map():t(Compare()){}
        explicit Lwj_map(const Compare& comp):t(comp){}
        template<class InputIterator>
        Lwj_map(InputIterator first,InputIterator last):t(Compare()){t.insert_unique(first,last);}
        template<class InputIterator>
        Lwj_map(InputIterator first,InputIterator last,const Compare& comp):t(comp){t.insert_unique(first,last);}
        Lwj_map(const Lwj_map<Key,T,Compare,Alloc>& x):t(x.t){}
        Lwj_map<Key,T,Compare,Alloc>& operator=(const Lwj_map<Key,T,Compare,Alloc>& x){
            t=x.t;
            return *this;
        }
        
        //一下所有的map操作行为，红黑树都已提供，map只要转调用即可
        key_compare key_comp() const{return t.key_comp();}
        value_compare value_comp() const{return value_compare(t.key_comp());}
        iterator begin()  {return t.begin();}
        iterator end() {return t.end();}
        bool empty() const{return t.empty();}
        size_type size() const {return t.size();}
        size_type max_size() const{return t.max_size;}
        void clear(){t.clear();}
        
        T& operator[](const key_type&k){
            return (*((insert(value_type(k,T()))).first)).second;
        }//用键值生成一个元素，数据值设为临时变量T(),然后将生成的元素插入，插入函数返回pair<iterator,bool>,取first，得到迭代器，*iterator.second得到所需数据的值
        
        Lwj_stl::pair<iterator,bool> insert(const value_type& x){//输入参数为元素类型，即pair
            return t.insert_unique(x);
        }
        
        template<class InputIterator>
        void insert(InputIterator first,InputIterator last){t.insert_unique(first,last);}
    };
}
#endif /* Lwj_map_h */
