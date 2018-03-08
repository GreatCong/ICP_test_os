/**
  ******************************************************************************
  * @file   queue.h
  * @author  liucongjun
  * @version 1.0
  * @date    2017.4.13
  * @brief 由于单片机对malloc支持不好，采用静态数组建立循环队列
******************************************************************************
*/
#ifndef __MY_QUEUE_H__
#define __MY_QUEUE_H__

#include <stdint.h>
#include <stdbool.h>

//#define QUEUE_SIZE 250;

//typedef uint8_t Elemtype;
typedef int16_t Elemtype;

typedef struct{
  Elemtype *data;
	int front;
	int rear;
//	uint16_t length;
	int queue_max;
}QueueArray_type;

bool Init_queue(QueueArray_type *q,Elemtype *array,int num_max);
bool EnQueue(QueueArray_type *q,Elemtype e);
bool DeQueue(QueueArray_type *q,Elemtype *e);
bool IsQueue_empty(QueueArray_type q);
bool IsQueue_full(QueueArray_type q);
bool ReadQueue_n(QueueArray_type *q,Elemtype *array_print,int num);
bool ReadQueue_all(QueueArray_type *q,Elemtype *array_print);
int GetQueue_length(QueueArray_type q);


#endif
