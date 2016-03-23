#include "stack.h"
#include <stdlib.h>

void stack_pop_each(Stack* this, void(*e)(void*))
{
	StackNode* curr = this->_last;
	while (curr != NULL)
	{
		StackNode* tf = curr;
		curr = curr->_prev;
		e(tf->node);
		free(tf);
	}
	this->_last = NULL;
	this->count = 0;
}

void* stack_pop(Stack* this)
{
	StackNode* curr = this->_last;
	this->_last = curr->_prev;
	void* ptr = curr->node;
	free(curr);
	return ptr;
}

int stack_push(Stack* this, void* node)
{
	StackNode* qNode = (StackNode*)malloc(sizeof(StackNode));

	if (qNode == NULL) return -1;

	qNode->node = node;
	qNode->_prev = NULL;
	
	if (this->count == 0)
	{
		this->_last = qNode;
	}
	else
	{
		this->_last->_prev = qNode;
		this->_last = qNode;
	}

	this->count++;

	return this->count;
}

void* stack_peek(Stack* this)
{
	return this->_last->node;
}

Stack* stack_new()
{
	Stack* stack = (Stack*)malloc(sizeof(Stack));

	stack->count = 0;
	stack->_last = NULL;

	return stack;
}
