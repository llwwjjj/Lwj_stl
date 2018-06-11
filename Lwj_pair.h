//
//  Lwj_pair.h
//  S
//
//  Created by 练文健 on 2018/6/11.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_pair_h
#define Lwj_pair_h
namespace Lwj_stl{
    template<class T1,class T2>
    struct pair{
        typedef T1 first_type;
        typedef T2 second_type;
        T1 first;
        T2 second;
        pair():first(T1()),second(T2()){}
        pair(const T1& a,const T2& b):first(a),second(b){}
        template <class U1, class U2>
        pair(const pair<U1, U2>& p) : first(p.first), second(p.second) {}
    };
}

#endif /* Lwj_pair_h */
