//
//  Lwj_construct.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_construct_h
#define Lwj_construct_h
#include <new>
#include "Lwj_traits.h"
#include "Lwj_iterator.h"
namespace Lwj_stl{
    template<class T1,class T2>
    inline void construct(T1* p, const T2& value) {
        new(p) T1(value);
    }
    template <class T>
    inline void destory(T* pointer) {
        pointer->~T();
    }
    
    //不知道为什么这两个函数一定要调换位置
    template <class ForwardIterator,class T>
    inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
        typedef typename __type_traits<T>::has_trivial_destructor trival_destructor;
        __destroy_aux(first, last, trival_destructor());
    }
    
    //第二版本，接受两个迭代器。此函数设法找出元素的数值型别
    //进而利用__type_traits<>求取最适当措施
    template <class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last) {
        __destroy(first, last, value_type(first));
    }
 
    
    
    
    //如果value_type有non-trival destructor
    template<class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
        destroy(&*first);
    }
    
    //如果value_type有trival destructor
    template<class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, __true_type) {}//什么也不做
    
    /*template<class Iterator>
    inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator){
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }*/
}
#endif /* Lwj_construct_h */
