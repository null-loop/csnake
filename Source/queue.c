#include "../Headers/queue.h"
#include <stdlib.h>

void queue_enqueue(Queue* this, void* node)
{
	QueueNode* qNode = (QueueNode*)malloc(sizeof(QueueNode));
	qNode->node = node;
	qNode->_next = NULL;

	if (this->count == 0)
	{
		this->_first = qNode;
		this->_last = qNode;
	}
	else 
	{
		this->_last->_next = qNode;
		this->_last = qNode;
	}
	
	this->count++;
}

void* queue_dequeue(Queue* this)
{
	QueueNode* q = this->_first;
	void* n = q->node;
	QueueNode* qN = q->_next;

	this->_first = qN;
	free(q);
	this->count--;

	return n;
}

void* queue_peek(Queue* this)
{
	return this->_first;
}

void queue_dequeue_each(Queue* this, void(*e)(void*))
{
	QueueNode* curr = this->_first;
	while (curr != NULL)
	{
		QueueNode* tf = curr;
		curr = curr->_next;
		e(tf->node);
		free(tf);
	}
	this->_last = NULL;
	this->_first = NULL;
	this->count = 0;
}

Queue* queue_new()
{
	Queue* q = (Queue*)malloc(sizeof(Queue));
	q->count = 0;
	q->_first = NULL;
	q->_last = NULL;
	return q;
}