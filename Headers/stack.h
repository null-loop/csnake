#pragma once

typedef struct Stack Stack;
typedef struct StackNode StackNode;

struct Stack {
	int count;
	StackNode* _last;
};

struct StackNode {
	StackNode* _prev;
	void* node;
};

void stack_pop_each(Stack* this, void(*e)(void*));

int stack_push(Stack* this, void* node);
void* stack_pop(Stack* this);
void* stack_peek(Stack* this);
void* stack_peek_at(Stack* this, int skip);
stack_for_each_with_state(Stack* this, void(*e)(void*, void*), void* s);

Stack* stack_new();