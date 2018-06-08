//
//  Lwj_alloc.h
//  S
//
//  Created by 练文健 on 2018/6/8.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_alloc_h
#define Lwj_alloc_h
#include <cstdlib>
#if 0
#    include <new>
#    define __THROW_BAD_ALLOC throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
#    include <iostream>
#    define __THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl;exit(0);
#endif


namespace Lwj_stl{
    //----------------------------------------------第一级配置器------------------------------------------------------------
    class __malloc_alloc_template {
    private:
        //oom:out of memory
        static void *oom_malloc(size_t);
        static void *oom_realloc(void*, size_t);
        static void(*__malloc_alloc_oom_handler)();
    public:
        static void* allocate(size_t n) {
            void* result = malloc(n);//第一级配置器直接使用malloc()，成功则返回void*
            //oom情况
            if (result == 0)
                result = oom_malloc(n);
            return result;
        }
        
        static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
            void* result = realloc(p, new_sz);//第一级配置器直接使用realloc(),成功则返回void*
            //oom情况
            if (result == 0)
                result = oom_realloc(p, new_sz);
            return result;
        }
        
        static void deallocate(void* p, size_t n) {
            free(p);//第一级配置器直接使用free()
        }
        
        static void(*set_malloc_handler(void(*f)()))(){
            void(*old)() = __malloc_alloc_oom_handler;
            __malloc_alloc_oom_handler = f;
            return (old);
        }
    };
    
    void (*__malloc_alloc_template::__malloc_alloc_oom_handler)() = 0;
    void * __malloc_alloc_template::oom_malloc(size_t n) {
        void (* my_alloc_handler)();
        void *result;
        for (;;) {//不断尝试释放、配置、再释放、再配置。
            my_alloc_handler = __malloc_alloc_oom_handler;
            if (my_alloc_handler == 0){
                __THROW_BAD_ALLOC;
            }
            (*my_alloc_handler)();//再调用处理例程，企图释放内存
            result = malloc(n);//再次尝试配置内存
            if (result)
                return result;
        }
    }
    

    void* __malloc_alloc_template::oom_realloc(void* p, size_t n) {
        void (* my_alloc_handler)();
        void* result;
        for (;;) {
            my_alloc_handler = __malloc_alloc_oom_handler;
            if (my_alloc_handler == 0) {
                __THROW_BAD_ALLOC;
            }
            (* my_alloc_handler)();
            result = realloc(p, n);
            if (result)
                return result;
        }
    }
    //-------------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------第二级配置器，默认-----------------------------------------------------------------
    enum{__ALIGN = 8};//小型区块的上调边界
    enum{__MAX_BYTES = 128};//小型区块的上限
    enum{__NFREELISTS = __MAX_BYTES/__ALIGN};//free-lists个数
    
    //————default_alloc_template
    class Lwj_alloc {
    private:
        static size_t ROUND_UP(size_t bytes) {        //上调至8的倍数
            return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
        }
    private:
        union obj {
            union obj* free_list_link;
            char client_data[1];
        };
    private:
        //16个free-lists
        static obj* volatile free_list[__NFREELISTS];
        
        //根据区块大小，决定使用第n号free-list,n从0起算
        static size_t FREELIST_INDEX(size_t bytes) {
            return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
        }
        
        //返回一个大小为n的对象，并可能加入大小为n的其他区块到free-list
        static void *refill(size_t n);
        
        //配置一大块空间，可容纳nobjs个大小为"size"的区块
        //如果配置nobjs个区块有所不便，nobjs可能会降低
        static char *chunk_alloc(size_t size, int &nobjs);
        
        //Chunk allocation state
        static char *start_free;//内存池起始位置，只在chunk_alloc()中变化
        static char *end_free;//内存池结束位置，只在chunk_alloc()中变化
        static size_t heap_size;
        
    public:
        static void *allocate(size_t n);
        static void deallocate(void *p, size_t n);
        static void *reallocate(void *p, size_t old_sz, size_t new_sz);
    };
    
    //static data mamber的定义和初值设定
    char *Lwj_alloc::start_free = 0; //初始化
    char *Lwj_alloc::end_free = 0; //初始化
    size_t Lwj_alloc::heap_size = 0; //初始化

    typename Lwj_alloc::obj * volatile
    Lwj_alloc::free_list[__NFREELISTS] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    
    /*空间配置函数*/

    void *Lwj_alloc::allocate(size_t n) {
        obj * volatile *my_free_list;
        obj *result;
        if (n > (size_t)__MAX_BYTES)
            return (__malloc_alloc_template::allocate(n));
        my_free_list = free_list + FREELIST_INDEX(n);
        result = *my_free_list;
        if (result == 0) {
            void *r = refill(ROUND_UP(n));//没有找到可用区块，准备重新填充free-list
            return r;
        }
        *my_free_list = result->free_list_link;
        return result;
    }
    
    /*空间释放函数*/
    void Lwj_alloc::deallocate(void* p, size_t n) {
        obj *q = (obj*)p;
        obj *volatile *my_free_list;
        if (n > (size_t)__MAX_BYTES) {
            __malloc_alloc_template::deallocate(p,n);
            return;
        }
        my_free_list = free_list + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;
        *my_free_list = q;
    }
    
    //重新填充函数，没有可用区块时，从内存池取空间并调整free-list
    void* Lwj_alloc::refill(size_t n) {
        int nobjs = 20;
        char* chunk = chunk_alloc(n, nobjs);//注意这里nobjs是传引用，也就是说内存池空间不足20个时nobjs会变小
        obj * volatile * my_free_list;
        obj *result;
        obj *current_obj, *next_obj;
        int i;
        if (nobjs == 1)//如果只获得一个区块，这个区块就分配给调用者用,free-list无新节点
            return (obj* ) chunk;
        my_free_list = free_list + FREELIST_INDEX(n);
        result = (obj *) chunk;
        *my_free_list = next_obj = (obj *)(chunk + n);//引导free-list指向新配置的空间
        for (i = 1;; ++i) {//从1开始，因为第0个将返回给client
            current_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n);
            if (i == nobjs - 1) {
                current_obj->free_list_link = 0;
                break;
            }
            else
                current_obj->free_list_link = next_obj;
        }
        return result;
    }
    
    //从内存池取空间给free-list使用
    //size是区块大小，nobjs是区块数量
    char * Lwj_alloc::chunk_alloc(size_t size, int &nobjs) {
        char *result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;//内存池剩余空间
        if (bytes_left >= total_bytes) {
            //内存池剩余空间完全满足需求量
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if (bytes_left >= size) {
            //内存池剩余空间不能完全满足需求量，但足够供应一个以上区块
            nobjs = int(bytes_left / size);
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else {
            //内存池剩余空间连一个区块的大小都无法提供
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            //试着让内存池中的残余零头还有利用价值，配给适当的free-list
            if (bytes_left > 0) {
                //寻找适当的free-list
                obj * volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
                //调整free-list,将内存池中的残余空间编入
                ((obj *)start_free)->free_list_link = *my_free_list;
                *my_free_list = (obj *)start_free;
            }
            //配置heap空间，用来补充内存池
            start_free = (char*)malloc(bytes_to_get);
            if (start_free == 0) {
                //heap空间不足,malloc()失败
                size_t i;
                obj * volatile * my_free_list, *p;
                //搜寻free-list中尚有未用区块，且区块大小大于size，并配置
                for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (p != 0) {
                        //free-list中比size大的区域尚有未用区块，调整free-list以释放出未用区块
                        *my_free_list = p->free_list_link;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        return(chunk_alloc(size, nobjs));
                    }
                }
                //到这里意味着山穷水尽，到处都没有内存可用了，调用第一级配置器，使用oom机制
                end_free = 0;
                start_free = (char *)__malloc_alloc_template::allocate(bytes_to_get);
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return (chunk_alloc(size, nobjs));
        }
    }
    
    
    
    //-------------------------------------------------------------------------------------------------------------------
    //---------------------------------------------------接口-------------------------------------------------------------
    
    template<class T, class Alloc>
    class simple_alloc {
        
    public:
        static T *allocate(size_t n)
        { return 0 == n? 0 : (T*) Alloc::allocate(n * sizeof (T)); }
        static T *allocate(void)
        { return (T*) Alloc::allocate(sizeof (T)); }
        static void deallocate(T *p, size_t n)
        { if (0 != n) Alloc::deallocate(p, n * sizeof (T)); }
        static void deallocate(T *p)
        { Alloc::deallocate(p, sizeof (T)); }
    };

}

#endif /* Lwj_alloc_h */
