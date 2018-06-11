//
//  Lwj_stack.h
//  S
//
//  Created by 练文健 on 2018/5/29.
//  Copyright © 2018年 练文健. All rights reserved.
//
//sgi以deque实现的,这里选择vector，还可以；list
//不归类为container,而被归类为container adapter
//没有迭代器，因为不支持遍历
#ifndef Lwj_stack_h
#define Lwj_stack_h

#include "Lwj_vector.h"
#include "Lwj_deque.h"
namespace Lwj_stl{
    template<class T, class Sequence = Lwj_stl::Lwj_vector<T> >
    class Lwj_stack{
        //friend bool operator== (const Mystack<T,Sequence>&,const Mystack&);
        //friend bool operator<(const Mystack<T,Sequence>&,const Mystack&);
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence v;//底层容器
    public:
        //以下完全利用Sequence v的操作，完成stack操作
        bool empty() const{return v.empty();}
        size_type size() const{return v.size();}
        reference top(){return v.back();}//vector既有back()也有front()
        const_reference top() const {return v.back();}
        void push(const value_type& x) {v.push_back(x);}
        void pop() {v.pop_back();}
        
        bool operator==(const Lwj_stack<T,Sequence> other){return v==other.v;}
    };
    
    
    template<class T,class Sequence>
    bool operator==(const Lwj_stack<T,Sequence>& x,const Lwj_stack<T,Sequence>& y){
        return x.v==y.v;
    }
    template<class T,class Sequence>
    bool operator<(const Lwj_stack<T,Sequence>& x,const Lwj_stack<T,Sequence>& y){
        return x.v<y.v; //vector定义了operator <
    }
}

#endif /* Mystack_h */




















