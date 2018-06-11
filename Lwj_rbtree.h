//
//  Lwj_rbtree.h
//  S
//
//  Created by 练文健 on 2018/6/9.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_rbtree_h
#define Lwj_rbtree_h

#include "Lwj_alloc.h"
#include "Lwj_construct.h"
#include "Lwj_uninitialized.h"
#include "Lwj_iterator.h"
#include <algorithm>

namespace Lwj_stl{
    const bool __rb_tree_red = false;//红色为0
    const bool __rb_tree_black = true;//黑色为1
    
    struct __rb_tree_node_base{//节点基类
        typedef __rb_tree_node_base* base_ptr;
        bool color;
        base_ptr parent;
        base_ptr left;
        base_ptr right;
        
        static base_ptr minimum(base_ptr x){
            while(x->left!=0)
                x=x->left;
            return x;
        }
        static base_ptr maximum(base_ptr x){
            while(x->right!=0)
                x=x->right;
            return x;
        }
    };
    
    template<class T>
    struct __rb_tree_node : public __rb_tree_node_base{//节点派生类
        T data;
    };
    
    //迭代器基类
    struct __rb_tree_base_iterator{
        typedef __rb_tree_node_base::base_ptr base_ptr;
        typedef bidirectional_iterator_tag iterator_catagory;
        typedef ptrdiff_t difference_type;
        
        base_ptr node;
        
        void increment(){
            if(node->right!=0){//有右子节点，那就是右子节点最左子节点
                node=node->right;
                while(node->left!=0)
                    node=node->left;
            }
            else{//没有右子节点，就找父节点，直到不是他的右儿子为止
                base_ptr tmp=node->parent;
                while(node==tmp->right){
                    node=tmp;
                    tmp=tmp->parent;
                }
                //如果根节点没有右儿子，那么树里没有比它大的值，等于本身，其他等于此时父节点
                if(node->right!=tmp)
                    node=tmp;
            }
        }
        
        void decrement(){
            if(node->color==__rb_tree_red && node->parent->parent==node)//header和root互为父节点，且header为红
                node=node->right;
            else if(node->left!=0){
                node=node->left;
                while(node->right!=0)
                    node=node->right;
            }
            else{
                base_ptr tmp=node->parent;
                while(node==tmp->left){
                    node=tmp;
                    tmp=tmp->right;
                }
                node=tmp;//不知为何不处理根节点没有左子树的情况了
            }
        }
    };
    
    //迭代器派生类
    template<class T,class Ref,class Ptr>
    struct __rb_tree_iterator:public __rb_tree_base_iterator{
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef __rb_tree_iterator<T,T&,T*> iterator;
        typedef __rb_tree_iterator<T,const T&,const T*> const_iterator;
        typedef __rb_tree_iterator<T,Ref,Ptr> selfs;
        typedef __rb_tree_node<T>* link_type;
        
        __rb_tree_iterator(){}
        __rb_tree_iterator(link_type x){node=x;}
        __rb_tree_iterator(const iterator& it){node=it.node;}
        
        reference operator*() const{return link_type(node)->data;}
        pointer operator->() const{return &(operator*());}
        
        selfs& operator++(){
            increment();
            return *this;
            
        }
        selfs operator++(int){
            selfs tmp=*this;
            increment();
            return tmp;
        }
        selfs& operator--(){
            decrement();
            return *this;
            
        }
        selfs operator--(int){
            selfs tmp=*this;
            decrement();
            return tmp;
        }
        
        inline bool operator==(const __rb_tree_base_iterator& rhs) {
            return node == rhs.node;
            // 兩個迭代器相等，意指其所指的節點相等。
        }
        inline bool operator!=(const __rb_tree_base_iterator& rhs) {
            return node != rhs.node;
            // 兩個迭代器相等，意指其所指的節點相等。
        }
    };

    
    template<class Key,class Value,class KeyOfValue,class Compare,class Alloc=Lwj_stl::Lwj_alloc>
    class Lwj_rbtree{
    public:
        typedef void* void_pointer;
        typedef __rb_tree_node_base* base_ptr;
        typedef __rb_tree_node<Value> rb_tree_node;
        typedef simple_alloc<rb_tree_node,Alloc> rb_tree_node_allocator;
        
        typedef Key key_type;
        typedef Value value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef rb_tree_node* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        
        size_type node_count;//追踪记录树的大小
        link_type header;//实现上的一个技巧
        Compare key_compare;//节点间的键值大小比较准则，应该会是个函数对象
        
        link_type get_node(){
            return rb_tree_node_allocator::allocate();
        }
        
        link_type create_node(const value_type& x){
            link_type tmp=get_node();
            Lwj_stl::construct(&tmp->data,x);
            return tmp;
        }
        
        void put_node(link_type p){
            rb_tree_node_allocator::deallocate(p);
        }
        
        void destroy_node(link_type p){
            Lwj_stl::destroy(&p->data);
            put_node(p);
        }
        
        link_type clone_node(link_type x){
            link_type tmp=create_node(x->data);
            tmp->color=x->color;
            tmp->left=0;
            tmp->right=0;
            //不知为何不设置父节点
            return tmp;
        }
        
        //以下三个函数用来方便去取得header的成员
        link_type& root() const{return (link_type&) header->parent;}//root是header的父亲
        link_type& leftmost() const{return (link_type&) header->left;}
        link_type& rightmost() const{return (link_type&) header->right;}
        
        //以下12个函数用来方便取得节点x的成员
        static link_type& left(link_type x){return (link_type&)(x->left);}
        static link_type& right(link_type x){return (link_type&) (x->right);}
        static link_type& parent(link_type x){return (link_type&) (x->parent);}
        static reference value(link_type x){return x->data;}
        static const Key& key(link_type x){return KeyOfValue()(value(x));}
        static bool& color(link_type x){return (bool&)(x->color);}
        
        static link_type& left(base_ptr x){return (link_type&) (x->left);}
        static link_type& right(base_ptr x){return (link_type&) (x->right);}
        static link_type& parent(base_ptr x){return (link_type&) (x->parent);}
        static reference value(base_ptr x){return ((link_type)x)->data;}
        static const Key& key(base_ptr x){return KeyOfValue()(value(link_type(x)));}
        static bool& color(base_ptr x){return (bool&)(x->color);}
        
        //求最大值最小值
        static link_type minimum(link_type x){ return (link_type) __rb_tree_node_base::minimum(x);}
        static link_type maximum(link_type x){ return (link_type) __rb_tree_node_base::maximum(x);}
        
        
    public:
        typedef __rb_tree_iterator<value_type, reference, pointer> iterator;
        typedef __rb_tree_iterator<value_type, const_reference, const_pointer> const_iterator;
    private:
        link_type __copy(link_type x,link_type p);
        void __erase(link_type x){
            while (x != 0) {
                __erase(right(x));
                link_type y = left(x);
                destroy_node(x);
                x = y;
            }
        }
        void init(){
            header=get_node();//产生一个节点空间，令header指向他
            color(header)=__rb_tree_red;//令header为红色，用来区分header和root，在operator--中
            root()=0;
            leftmost()=header;//令header右子节点指向自己
            rightmost()=header;//令header左子节点指向自己
        }
    public:
        Lwj_rbtree(const Compare& comp=Compare()):node_count(0),key_compare(comp){init();}
        ~Lwj_rbtree(){
            clear();
            put_node(header);
        }
        Lwj_rbtree<Key,Value,KeyOfValue,Compare,Alloc>& operator=(const Lwj_rbtree<Key,Value,KeyOfValue,Compare,Alloc>& x);
    public:
        //访问器
        Compare key_comp() const{return key_compare;}
        iterator begin(){return leftmost();}//RB树的起头为最左节点
        iterator end(){return header;}//RB树的终点为header所指处
        bool empty() const {return node_count==0;}
        size_type size() const {return node_count;}
        size_type max_size() const {return size_type(-1);}
        
        void clear() {
            if (node_count != 0) {
                __erase(root());
                leftmost() = header;
                root() = 0;
                rightmost() = header;
                node_count = 0;
            }
        }
        
    //-----------------------------------------------------插入操作---------------------------------------------------------------
    public:
        //将x插入到RB-Tree中，保持节点值独一无二
        std::pair<iterator,bool> insert_unique(const value_type& v){//返回第二元素表示插入成功与否
            link_type y=header;
            link_type x=root();
            bool comp=true;
            while(x!=0){
                y=x;
                comp=key_compare(KeyOfValue()(v),key(x));
                x=comp?left(x):right(x);
            }
            //离开while后，y所指即插入点的父节点(必为叶节点)
            iterator j=iterator(y);
            if(comp){
                if(j==begin())//插入点比树中任意键值都小
                    return std::pair<iterator,bool>(__insert(x,y,v),true);
                else
                    --j;//回头准备测试
            }
            if(key_compare(key(j.node),KeyOfValue()(v)))
                return std::pair<iterator,bool>(__insert(x,y,v),true);
            return std::pair<iterator,bool>(j,false);
        }
        
        
        //将x插入到RB-Tree中，允许节点值重复
        iterator insert_equal(const value_type& v){
            link_type y=header;
            link_type x=root();
            while(x!=0){//从根节点开始，往下寻找适当的插入点
                y=x;
                x=key_compare(KeyOfValue()(v),key(x))?left(x):right(x);//比v键值大则往左，小于等于则往右
            }
            return __insert(x,y,v);
        }
        
    private:
        
        
        iterator __insert(base_ptr x,base_ptr y,const Value& v){
            link_type z;
            if(y==header || x!=0 || key_compare(KeyOfValue()(v),key(y))){
                z=create_node(v);
                left(y)=z;
                if(y==header){
                    root()=z;
                    rightmost()=z;
                }
                else if(y==leftmost())
                    leftmost()=z;
            }
            else{
                z=create_node(v);
                right(y)=z;
                if(y==rightmost())
                    rightmost()=z;
            }
            z->parent=y;
            left(z)=0;
            right(z)=0;
            __rb_tree_rebalance(z,header->parent);
            ++node_count;
            return iterator(z);
        }
        
        
        inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root)
        {
            x->color = __rb_tree_red;        // 新節點必為紅
            while (x != root && x->parent->color == __rb_tree_red) { // 父節點為紅
                if (x->parent == x->parent->parent->left) { // 父節點為祖父節點之左子節點
                    __rb_tree_node_base* y = x->parent->parent->right;    // 令y 為伯父節點
                    if (y && y->color == __rb_tree_red) {         // 伯父節點存在，且為紅
                        x->parent->color = __rb_tree_black;          // 更改父節點為黑
                        y->color = __rb_tree_black;                // 更改伯父節點為黑
                        x->parent->parent->color = __rb_tree_red;     // 更改祖父節點為紅
                        x = x->parent->parent;
                    }
                    else {    // 無伯父節點，或伯父節點為黑
                        if (x == x->parent->right) { // 如果新節點為父節點之右子節點
                            x = x->parent;
                            __rb_tree_rotate_left(x, root); // 第一參數為左旋點
                        }
                        x->parent->color = __rb_tree_black;    // 改變顏色
                        x->parent->parent->color = __rb_tree_red;
                        __rb_tree_rotate_right(x->parent->parent, root); // 第一參數為右旋點
                    }
                }
                else {    // 父節點為祖父節點之右子節點
                    __rb_tree_node_base* y = x->parent->parent->left; // 令y 為伯父節點
                    if (y && y->color == __rb_tree_red) {        // 有伯父節點，且為紅
                        x->parent->color = __rb_tree_black;        // 更改父節點為黑
                        y->color = __rb_tree_black;                 // 更改伯父節點為黑
                        x->parent->parent->color = __rb_tree_red;     // 更改祖父節點為紅
                        x = x->parent->parent;    // 準備繼續往上層檢查...
                    }
                    else {    // 無伯父節點，或伯父節點為黑
                        if (x == x->parent->left) {    // 如果新節點為父節點之左子節點
                            x = x->parent;
                            __rb_tree_rotate_right(x, root);     // 第一參數為右旋點
                        }
                        x->parent->color = __rb_tree_black;    // 改變顏色
                        x->parent->parent->color = __rb_tree_red;
                        __rb_tree_rotate_left(x->parent->parent, root); // 第一參數為左旋點
                    }
                }
            }    // while 結束
            root->color = __rb_tree_black;    // 根節點永遠為黑
        }
        
        inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
        {
            // x 為旋轉點
            __rb_tree_node_base* y = x->right;    // 令y 為旋轉點的右子節點
            x->right = y->left;
            if (y->left !=0)
                y->left->parent = x;        // 別忘了回馬槍設定父節點
            y->parent = x->parent;
            
            // 令 y 完全頂替 x 的地位（必須將 x 對其父節點的關係完全接收過來）
            if (x == root)                    // x 為根節點
                root = y;
            else if (x == x->parent->left)    // x 為其父節點的左子節點
                x->parent->left = y;
            else                            // x 為其父節點的右子節點
                x->parent->right = y;
            y->left = x;
            x->parent = y;
        }
        
        inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
        {
            // x 為旋轉點
            __rb_tree_node_base* y = x->left;    // y 為旋轉點的左子節點
            x->left = y->right;
            if (y->right != 0)
                y->right->parent = x;     // 別忘了回馬槍設定父節點
            y->parent = x->parent;
            
            // 令 y 完全頂替 x 的地位（必須將 x 對其父節點的關係完全接收過來）
            if (x == root)                    // x 為根節點
                root = y;
            else if (x == x->parent->right)    // x 為其父節點的右子節點
                x->parent->right = y;
            else                            // x 為其父節點的左子節點
                x->parent->left = y;
            y->right = x;
            x->parent = y;
        }
        void insert_unique(iterator first,iterator last){
            for ( ; first != last; ++first)
                insert_unique(*first);
        }
        void insert_equal(iterator first,iterator last){
            for ( ; first != last; ++first)
                insert_equal(*first);
        }
    };

}
#endif /* Lwj_rbtree_h */
