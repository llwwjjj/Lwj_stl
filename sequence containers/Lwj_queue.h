//
//  Lwj_queue.h
//  S
//
//  Created by 练文健 on 2018/5/30.
//  Copyright © 2018年 练文健. All rights reserved.
//sgi以deque为底部结构，还可以list
//没有迭代器，因为不支持遍历
#ifndef Lwj_queue_h
#define Lwj_queue_h
#include "Lwj_deque.h"
namespace Lwj_stl{
    template<class T,class Sequence=Lwj_stl::Lwj_deque<T> >
    class Lwj_queue{
        //friend bool operator==和<
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence d;
    public:
        bool empty() const {return d.empty();}
        size_type size() const {return d.size();}
        reference front() {return d.front();}
        const_reference front() const {return d.front();}
        reference back() {return d.back();}
        const_reference back() const {return d.back();}
        void push (const value_type& x){ d.push_back(x);}
        void pop () { d.pop_front();}
        
        bool operator==(const Lwj_queue<T,Sequence>& other){return d==other.d;}
    };
}


#endif /* Myqueue_h */
