#pragma once

typedef struct Queue Queue;
typedef struct QueueNode QueueNode;

struct Queue {
	int count;
	QueueNode* _first;
	QueueNode* _last;
};

struct QueueNode {
	QueueNode* _next;
	void* node;
};

void queue_dequeue_each(Queue* this, void(*e)(void*));

void queue_enqueue(Queue* this, void* node);
void* queue_dequeue(Queue* this);
void* queue_peek(Queue* this);


Queue* queue_new();