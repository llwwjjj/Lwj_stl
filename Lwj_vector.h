//
//  Lwj_vector.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_vector_h
#define Lwj_vector_h
#include "Lwj_alloc.h"
#include "Lwj_construct.h"
#include "Lwj_uninitialized.h"
#include "Lwj_iterator.h"
#include <algorithm>
namespace Lwj_stl {
    template<class T,class Alloc=Lwj_alloc>
    class Lwj_vector {
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef value_type* iterator;
        typedef value_type& reference;
        typedef size_t size_type;
        typedef ptrdiff_t    difference_type;
    protected:
        typedef simple_alloc<value_type, Alloc> data_allocator;
        iterator start;                //表示目前使用空间的头
        iterator finish;            //表示目前使用空间的尾
        iterator end_of_storage;        //表示目前可用空间的尾
        void insert_aux(iterator position, const T& x) {
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2 * old_size : 1;
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = uninitialized_copy(start, position, new_start);
                Lwj_stl::construct(new_finish, x);
                ++new_finish;
            }
            catch (...) {
                Lwj_stl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            Lwj_stl::destroy(begin(), end());
            deallocate();
            
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
        void deallocate() {
            if (start)
                data_allocator::deallocate(start, end_of_storage - start);
        }
        void fill_initialize(size_type n, const T& value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
    public:
        iterator begin() { return start; }
        iterator end() { return finish; }
        size_type size() const { return finish-start; }
        size_type capacity() const { return size_type(end_of_storage - begin()); }
        bool empty() const { return begin() == end(); }
        reference operator[](size_type n) { retuen(begin() + n); }
                                                      
        Lwj_vector():start(0),finish(0),end_of_storage(0){}
        Lwj_vector(size_type n, const T& value) { fill_initialize(n, value); }
        Lwj_vector(int n, const T& value) { fill_initialize(n, value); }
        Lwj_vector(long n, const T& value) { fill_initialize(n, value); }
        explicit Lwj_vector(size_type n) { fill_initialize(n, T()); }
        ~Lwj_vector() {
            Lwj_stl::destroy(start, finish);
            deallocate();
        }
        reference feont() { return *begin(); }        //第一个元素
        reference back() { return (*end() - 1); }    //最后一个元素
        void push_back(const T& x) {
            if (finish != end_of_storage) {
                Lwj_stl::construct(finish, x);
                ++finish;
            }
            else
                insert_aux(end(), x);
        }
        void pop_back() {
            --finish;
            Lwj_stl::destroy(finish);
        }
        iterator erase(iterator position) {//清除某位置上的元素
            if (position + 1 != end())
                copy(position + 1, finish, position);
            --finish;
            Lwj_stl::destroy(finish);
            return position;
        }
        void resize(size_type new_size, const T& x) {
            if (new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }
    protected:
        iterator allocate_and_fill(size_type n, const T& x) {
            iterator result = data_allocator::allocate(n);
            Lwj_stl::uninitialized_fill_n(result, n, x);
            return result;
        }
    public:
        void insert(iterator position, size_type n, const T& x) {
            if (n != 0) {
                if (size_type(end_of_storage - finish) >= n) {//备用空间大于等于新元素个数
                    T x_copy = x;
                    const size_type elems_after = finish - position;
                    iterator old_finish = finish;
                    if (elems_after > n) {//插入点之后的鲜鱿元素个数大于新增元素个数
                        Lwj_stl::uninitialized_copy(finish - n, finish, finish);
                        finish += n;
                        copy_backward(position, old_finish - n, old_finish);
                        fill(position, position + n, x_copy);
                    }
                    else {//插入点之后的现有元素个数小于等于新增元素个数
                        Lwj_stl::uninitialized_fill_n(finish, n - elems_after, x_copy);
                        finish += n - elems_after;
                        Lwj_stl::uninitialized_copy(position, old_finish, finish);
                        finish += elems_after;
                        fill(position, old_finish, x_copy);
                    }
                }
                else {
                    //备用空间小于新增元素个数
                    //首先决定新长度：就长度的两倍或就长度+新增元素个数
                    const size_type old_size = size();
                    const size_type len = old_size + std::max(old_size, n);
                    iterator new_start = data_allocator::allocate(len);
                    iterator new_finish = new_start;
                    try {
                        new_finish = uninitialized_copy(start, position, new_start);
                        new_finish = uninitialized_fill_n(new_finish, n, x);
                        new_finish = uninitialized_copy(position, finish, new_finish);
                    }
                    catch (...) {
                        Lwj_stl::destroy(new_start, new_finish);
                        data_allocator::deallocate(new_start, len);
                        throw;
                    }
                    Lwj_stl::destroy(begin(), end());
                    deallocate();
                    
                    start = new_start;
                    finish = new_finish;
                    end_of_storage = new_start + len;
                }
            }
        }
    };
}

#endif /* Lwj_vector_h */
