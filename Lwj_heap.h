//
//  Lwj_heap.h
//  S
//
//  Created by 练文健 on 2018/5/30.
//  Copyright © 2018年 练文健. All rights reserved.
//

#ifndef Lwj_heap_h
#define Lwj_heap_h
namespace Lwj_stl{
    //先对vector进行push_back后使用push_heap进行调度
    template <class RandomAccessIterator>
    inline void push_heap(RandomAccessIterator first,RandomAccessIterator last){
        __push_heap_aux(first,last,distance_type(first),value_type(first));
    }
    template<class RandomAccessIterator,class Distance,class T>
    inline void __push_heap_aux(RandomAccessIterator first,RandomAccessIterator last,Distance*,T*){
        __push_heap(first,Distance((last-first)-1),Distance(0),T(*(last-1)));
    }
    template<class RandomAccessIterator,class Distance,class T>
    void __push_heap(RandomAccessIterator first,Distance holeIndex,Distance topIndex,T value){
        Distance parent=(holeIndex-1)/2;//找到父节点
        while(holeIndex > topIndex && *(first+parent)<value){
            //当尚未达到顶端，且父节点小于新值时，交换
            *(first+holeIndex)=*(first+parent);//父值给洞值
            holeIndex=parent;//洞号调整
            parent=(holeIndex-1)/2;//调整父号
        }
        *(first+holeIndex)=value;//新值给洞值
    }
    
    
    
    //首先设定欲调整值为尾值，然后将首值调整置尾节点，然后重整
    template <class RandomAccessIterator>
    inline void pop_heap(RandomAccessIterator first,RandomAccessIterator last){
        __pop_heap_aux(first,last,value_type(first));
    }
    
    template<class RandomAccessIterator,class T>
    inline void __pop_heap_aux(RandomAccessIterator first,RandomAccessIterator last,T*){
        __pop_heap(first,last-1,last-1,T(*(last-1)),distance_type(first));
    }
    
    template<class RandomAccessIterator,class Distance,class T>
    inline void __pop_heap(RandomAccessIterator first,RandomAccessIterator last,RandomAccessIterator result,T value,Distance holeIndex){
        *result=*first;//设定尾值为首值，又pop_back()取出尾值
        __adjust_heap(first,Distance(0),Distance(last-first),value);//重新调整
    }
    
    
    template<class RandomAccessIterator,class Distance,class T>
    void __adjust_heap(RandomAccessIterator first,Distance holeIndex,Distance len,T value){
        Distance topIndex=holeIndex;
        Distance secondChild=2*(holeIndex+1);
        while(secondChild<len){
            //比较两节点大小
            if(*(first+secondChild) < *(first+secondChild-1))
                secondChild--;
            //令较大值为洞值，再令洞号下移至较大字节点处
            *(first+holeIndex) = *(first+secondChild);
            holeIndex=secondChild;
            secondChild=2*(holeIndex+1);//字节点中的右节点
        }
        if (secondChild == len) { // 沒有右子節點，只有左子節點
            // Percolate down：令左子值為洞值，再令洞號下移至左子節點處。
            *(first + holeIndex) = *(first + (secondChild - 1));
            holeIndex = secondChild - 1;
        }
        __push_heap(first, holeIndex, topIndex, value);//可能此时尚未满足次序特性，需要执行一次push_heap操作
    }
    
    template<class RandomAccessIterator>
    void sort_heap(RandomAccessIterator first,RandomAccessIterator last){
        //每执行一次pop_heap就把当前范围内最大值放尾端，且每次把范围减一
        while(last-first>1)
            pop_heap(first,last--);
    }
    
    //将一段现有的数据转化为一个heap
    template<class RandomAccessIterator,class T,class Distance>
    void __make_heap(RandomAccessIterator first,RandomAccessIterator last,T*,Distance*){
        if(last-first<2) return;//如果长度为0或1，不必重新排列
        Distance len =last-first;
        Distance parent=(len-2)/2;
        while(true){
            __adjust_heap(first,parent,len,T(*(first+parent)));
            if(parent==0)
                return;
            parent--;
        }
    }
}
#endif /* Myheap_h */















