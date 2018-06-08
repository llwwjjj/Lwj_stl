//
//  Lwj_trait.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_trait_h
#define Lwj_trait_h


namespace Lwj_stl{
    struct __true_type {};
    struct __false_type {};
    
    template <class type>
    struct __type_traits {
        typedef __true_type this_dummy_member_must_be_first;
        
        //做保守估计，默认型别都没有
        typedef __false_type has_trivial_default_constuctor;
        typedef __false_type has_trivial_copy_constructor;
        typedef __false_type has_trivial_assignment_operator;
        typedef __false_type has_trivial_destructor;
        typedef __false_type is_POD_type;
    };
    
    //以下针对C++内置类型做特化
    template <>
    struct __type_traits<bool> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<char> {
        typedef __true_type has_trivial_default_constructsor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<unsigned char> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<unsigned short> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<unsigned int> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<unsigned long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<long long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<unsigned long long> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<float> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template <>
    struct __type_traits<long double> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    //针对原生指针做偏特化
    template <class T>
    struct __type_traits<T*> {
        typedef __true_type has_trivial_default_constructor;
        typedef __true_type has_trivial_copy_constructor;
        typedef __true_type has_trivial_assignment_operatpr;
        typedef __true_type has_trivial_destructor;
        typedef __true_type is_POD_type;
    };
    
    template<class T>
    struct __type_traits<const T*>
    {
        typedef __true_type     has_trivial_default_constructor;
        typedef __true_type     has_trivial_copy_constructor;
        typedef __true_type     has_trivial_assignment_operator;
        typedef __true_type     has_trivial_destructor;
        typedef __true_type     is_POD_type;
    };
}
#endif /* Lwj_trait_h */
