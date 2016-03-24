#include "../Headers/loop.h"
#include <stdlib.h>

typedef struct LoopNode LoopNode;

struct LoopNode {
	void* node;
	LoopNode* __prev;
	LoopNode* __next;
};

Loop* loop_new()
{
	Loop* loop = (Loop*)malloc(sizeof(Loop));
	loop->count = 0;
	loop->__start = NULL;
	return loop;
}

void __remove_node(Loop* loop, LoopNode* node)
{
	LoopNode* p = node->__prev;
	LoopNode* n = node->__next;

	p->__next = n;
	n->__prev = p;

	if (node == loop->__start)
	{
		loop->__start = n;
	}
	loop->count--;

	free(node);
}

void loop_run(Loop* loop, LoopStepResult(*e)(void* node))
{
	LoopNode* cNode = loop->__start;

	if (cNode == NULL) return;

	LoopStepResult result;

	while (loop->count > 0 && (result = e(cNode->node)).next_step != Finished)
	{
		cNode = cNode->__next;

		if (result.next_step == InsertNode)
		{
			loop_add(loop, result.node);
		}
		else if (result.next_step == RemoveNode)
		{
			__remove_node(loop, cNode->__prev);
		}
	}
}

void loop_each_with_state(Loop* loop, void* state, void(*e)(void*,void*))
{
	LoopNode* start = loop->__start;
	LoopNode* curr = loop->__start;

	while (curr != NULL)
	{
		LoopNode* tf = curr;
		curr = curr->__next;
		e(curr->node,state);

		if (curr == start) break;
	}
}

int loop_add(Loop* loop, void* node)
{
	LoopNode* lNode = (LoopNode*)malloc(sizeof(LoopNode));
	lNode->node = node;

	if (loop->count == 0)
	{
		loop->__start = lNode;
		loop->__start->__next = lNode;
		loop->__start->__prev = lNode;
	}
	else
	{
		LoopNode* pNode = loop->__start->__prev;

		pNode->__next = lNode;
		lNode->__prev = pNode;

		loop->__start->__prev = lNode;
		lNode->__next = loop->__start;
	}
	loop->count++;
	return loop->count;
}

void loop_clear(Loop* loop, void(*e)(void* node))
{
	LoopNode* lNode = loop->__start;
	while (lNode != NULL)
	{
		e(lNode->node);
		LoopNode* t = lNode;
		lNode = lNode->__next;
		free(t);
	}
	loop->count = 0;
	loop->__start = NULL;
}
