//
//  Lwj_iterator.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_iterator_h
#define Lwj_iterator_h

namespace Lwj_stl {
    //五种迭代器类型
    struct input_iterator_tag{};
    struct output_iterator_tag{};
    struct forward_iterator_tag {};
    struct bidirectional_iterator_tag {};
    struct random_access_iterator_tag {};
    template<class Category, class T, class Distance = ptrdiff_t, class Pointer=T*,class Reference=T&>
    struct iterator {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };
    template<class Iterator>
    struct iterator_traits {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type value_type;
        typedef typename Iterator::difference_type difference_type;
        typedef typename Iterator::pointer pointer;
        typedef typename Iterator::reference reference;
    };
    template<class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
    };
    template<class T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T value_type;
        typedef ptrdiff_t difference_type;
        typedef const T* pointer;
        typedef const T& reference;
    };
    
    //这个函数可以很方便地决定某个迭代器的类型(category)
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_catagory(const Iterator&) {
        typedef typename iterator_traits<Iterator>::category category;
        return category();
    }
    //这个函数可以很方便地决定某个迭代器的distance type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }
    //这个函数可以很方便地决定某个迭代器的value type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }
    
    
}
   
#endif /* Lwj_iterator_h */
