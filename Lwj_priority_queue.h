//
//  Lwj_priority_queue.h
//  S
//
//  Created by 练文健 on 2018/5/31.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_priority_queue_h
#define Lwj_priority_queue_h
#include "Lwj_heap.h"
namespace Lwj_stl{
    template<class T,class Sequence,class Compare>
    class Lwj_priority_queue{
    public:
        typedef typename Sequence::value_type value_type;
        typedef typename Sequence::size_type size_type;
        typedef typename Sequence::reference reference;
        typedef typename Sequence::const_reference const_reference;
    public:
        Sequence v;
        Compare comp;//元素比较大小标准
        Lwj_priority_queue():v(){}
        explicit Lwj_priority_queue(const Compare& x):v(),comp(x){}
        
        template<class InputIterator>
        Lwj_priority_queue(InputIterator first,InputIterator last,const Compare& x):v(first,last),comp(x){Lwj_stl::make_heap(v.begin(),v.end(),comp);}
        
        template<class InputIterator>
        Lwj_priority_queue(InputIterator first,InputIterator last):v(first,last){Lwj_stl::make_heap(v.begin(),v.end(),comp);}
        
        bool empty() const {return v.empty();}
        size_type size() const {return v.size();}
        const_reference top() const {return v.front;}
        
        
        void push(const value_type& x){
            v.push_back(x);
            Lwj_stl::push_heap(v.begin(),v.end(),comp);
        }
        
        void pop(){
            Lwj_stl::pop_heap(v.begin(),v.end(),comp);
            v.pop_back();
        }
    };
}

#endif /* Mypriority_queue_h */




