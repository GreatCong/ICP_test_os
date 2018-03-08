/**
  ******************************************************************************
  * @file   queue.c
  * @author  liucongjun
  * @version 0.2
  * @date    2017.4.13
  * @brief 由于单片机对malloc支持不好，采用静态数组建立循环队列
	         0.1 3.30 基本实现了循环队列;
					 0.2 4.12 改进了ReadQueue_n函数;
					 1.0 4.13 修复ReadQueue_n函数中，如果n=max_num，队列的长度不会变的bug
******************************************************************************
*/
//#define USART_DEBUG

#include "my_queue.h"
#ifdef USART_DEBUG
#include "usart.h"
#endif

//  @ fuction:  
//  @ description:  初始化队列
//  @ input: 1.需要初始化的队列指针q  2.静态数组首地址 array
//  @ output:
//  @ note: 
bool Init_queue(QueueArray_type *q,Elemtype *array,int num_max){
   q->data = array;
	 q->front = 0;
	 q->rear = 0;
//	 q->length = 0;
	 q->queue_max = num_max;
	
	return true;
}

//  @ fuction:  
//  @ description:  入队
//  @ input:
//  @ output:
//  @ note: 
bool EnQueue(QueueArray_type *q,Elemtype e){
	//判断队列是否已满
  if((q->rear+1)%q->queue_max == q->front) return false;
	
	q->data[q->rear] = e;
	q->rear = (q->rear + 1) % q->queue_max;//rear指向尾数据的下一个空间
	
	return true;
}

//  @ fuction:  
//  @ description:  出队
//  @ input:
//  @ output:
//  @ note: 
bool DeQueue(QueueArray_type *q,Elemtype *e){
   //判断队列是否为空
  if(q->front == q->rear) return false;
	
	*e = q->data[q->front];
	q->front = (q->front + 1) % q->queue_max;
	
	return true;
}

//  @ fuction:  
//  @ description:  判断队列是否为空
//  @ input:
//  @ output:
//  @ note: 
bool IsQueue_empty(QueueArray_type q){
  if(q.front == q.rear) return true;
	
	return false;
}

//  @ fuction:  
//  @ description:  判断队列是否为满
//  @ input:
//  @ output:
//  @ note: 这里牺牲一个单元来区分队列空和满，入队时少用一个队列单元，即如果数组的大小为size 实际只能存放(size-1)个元素
bool IsQueue_full(QueueArray_type q){
  if((q.rear+1)%q.queue_max == q.front) return true;
	
	return false;
}

//  @ fuction:  
//  @ description:  从缓冲区中读取n个元素到array_print
//  @ input:
//  @ output:
//  @ note: num < 
bool ReadQueue_n(QueueArray_type *q,Elemtype *array_print,int num){
	int temp,temp_array;
  //判断队列是否为空
  if(q->front == q->rear){
		#ifdef USART_DEBUG
      printf("read empty queue!\r\n");
    #endif
	   return false;
	}
	if(num > q->queue_max){
		#ifdef USART_DEBUG
      printf("read full queue!\r\n");
    #endif
	  return false;
	}
	
	if(q->front < q->rear){ //正常输出
		for(temp = 0,temp_array = 0;temp<num;temp++){
		  array_print[temp_array++] = q->data[q->front+temp];
		} 
		q->front = (q->front+num)%q->queue_max;//出队
		
		if(num == q->queue_max) q->rear = q->front;//避免 如果num=max,即使出队了，长度还是没有变
	}
	else{
	  for(temp = 0,temp_array = 0;temp<num;temp++){
		  array_print[temp_array++] = q->data[(temp+q->front)%q->queue_max];
		}
		q->front = (q->front+num)%q->queue_max;//出队
		
		if(num == q->queue_max) q->rear = q->front;//避免 如果num=max,即使出队了，长度还是没有变
	}
	
	return true;
}

//  @ fuction:  
//  @ description:  从缓冲区读取所以的元素
//  @ input:
//  @ output:
//  @ note: 同时还要出队
bool ReadQueue_all(QueueArray_type *q,Elemtype *array_print){
  return ReadQueue_n(q,array_print,GetQueue_length(*q));
}

//  @ fuction:  
//  @ description:  获取当前的队列的长度
//  @ input:
//  @ output:
//  @ note: 
int GetQueue_length(QueueArray_type q){
	//int temp;
	
  if(q.front == q.rear) return 0;//empty
	if((q.rear+1)%q.queue_max == q.front) return q.queue_max;//full
	
	if(q.front < q.rear){ //正常输出
		//temp = q.rear - q.front;
		return (q.rear - q.front);
	}
	else{
	  //temp = q.queue_max - (q.front - q.rear);
		return (q.queue_max + q.rear - q.front);
	}
	
	//return temp;
}

