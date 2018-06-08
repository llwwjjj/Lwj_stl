//
//  Lwj_uninitialized.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_uninitialized_h
#define Lwj_uninitialized_h
#include "Lwj_traits.h"
#include <algorithm>
#include "Lwj_construct.h"
/*
 uninitialized_copy          //区间上构造元素
 uninitialized_fill_n        //区间上填充n个元素
 uninitialized_fill          //区间first--end上填充元素
 */



namespace Lwj_stl {
    
//-------------------------------------------------uninitialized_copy-----------------------------------------------------
  
    
    template<class InputIterator, class ForwardIterator,class T>
    inline ForwardIterator __uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result,T*) {
        typedef typename __type_traits<T>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());//让编译器做参数推导
    }
    
    template<class InputIterator,class ForwardIterator>
    inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
        return __uninitialized_copy(first, last, result, Lwj_stl::value_type(result));
    }
    
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
        return std::copy(first, last, result);//调用STL算法copy()
    }
    
    template<class InputIterator, class ForwardIterator>
    inline ForwardIterator
    __uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
        ForwardIterator cur = result;
        for (; first != last; ++first, ++cur)
            construct(&*cur, *first);//必须一个一个元素地构造，无法批量进行，安全但是慢
        return cur;
    }
    
    //针对char* 和 wchar_t* 两种类型，可以采用最具效率的做法memmove(直接移动内存内容)来执行复制行为
    //针对 const char* 特化版本
    inline char* uninitialized_copy(const char* first, const char* last, char* result) {
        memmove(result, first, last - first);
        return result + (last - first);
    }
    
    //针对 const wchar_t* 特化版本
    inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
        memmove(result, first, (last - first)*sizeof(wchar_t));
        return result + (last - first);
    }
//------------------------------------------------------------------------------------------------------------------------------
    

    
//-------------------------------------------------uninitialized_fill_n---------------------------------------------------------
    template <class ForwardIterator,class Size,class T>
    inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
        //首先萃取出迭代器first的value type
        return __uninitialized_fill_n(first, n, x, value_type(first));
    }
    
    template<class ForwardIterator,class Size,class T,class T1>
    inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
        //以下__type_traits<>技法，判断value type是否为POD型别
        //POD指PLain Old Data，必然拥有trival ctor/dtor/copy/assignment函数
        //因此，我们可以对POD型别采取最有效率的初值填写手法，对non-POD型别采取最保险安全的做法
        typedef typename __type_traits<T1>::is_POD_type  is_POD;
        return __uninitialized_fill_n_aux(first,n,x,is_POD());
    }
    
    //藉由function template 的参数推导机制而得
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator
    __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
        return fill_n(first, n, x);//交由高阶函数执行
    }

    
    
    template<class ForwardIterator, class Size, class T>
    inline ForwardIterator
    __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
        ForwardIterator cur = first;
        //    ...    (省略了异常处理)
        for (; n > 0; --n, ++cur)
            construct(&* cur, x);
        return cur;
    }
    
//------------------------------------------------------------------------------------------------------------------------------
    
    
    
//-------------------------------------------------uninitialized_fill------------------------------------------------------------
    

    template<class ForwardIterator,class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
        return __uninitialized_fill(first, last, x, value_type(first));
    }
    
    template<class ForwardIterator, class T,class T1>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
        typedef typename __type_traits<T>::is_POD_type is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }
    
    template<class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
        fill(first, last, x);
    }
    
    template<class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
        ForwardIterator cur = first;
        for (; cur != last; ++cur)
            construct(&*cur, x);
    }
    
    
}
#endif /* Lwj_uninitialized_h */
