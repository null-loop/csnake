#pragma once

typedef struct Loop Loop;
typedef struct LoopNode LoopNode;
typedef enum NextStep NextStep;
typedef struct LoopStepResult LoopStepResult;

struct Loop {
	int count;
	LoopNode* __start;
};

enum NextStep {
	ContinueLoop = 0,
	Finished = 1,
	RemoveNode = 2,
	InsertNode = 3
};

struct LoopStepResult
{
	NextStep next_step;
	void* node;
};

Loop* loop_new();
void loop_run(Loop* loop, LoopStepResult(*e)(void* node));
void loop_each_with_state(Loop* loop, void* state, void(*e)(void*,void*));

int loop_add(Loop* loop, void* node);

void loop_clear(Loop* loop, void(*e)(void* node));

