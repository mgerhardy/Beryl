/*
 * Thread.cpp
 *
 *  Created on: 19.06.2017
 *      Author: kevin
 */

#include <deque>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic>
#include "core.hpp"
#include <sstream>
#include <string.h>
#include <assert.h>

namespace beryl {

static std::deque<Threadstate*> threadpool;
static Threadstate* current = nullptr;
static const uint64_t MAXSTACKSIZE = 10240;

#define SAVESTATE(T) \
	asm( "movq %%rbp, %1;"\
			"movq %%rsp, %0;"\
			"movq %%rax, %2;"\
			"movq %%rcx, %3;"\
			"movq %%rdx, %4"\
			:"=r"(T->store->rsp), "=r"(T->store->rbp), "=r"(T->store->rax), "=r"(T->store->rcx)\
			 ,"=r"(T->store->rdx)\
			:\
);

#define RESTORESTATE(T) \
		asm("movq %1, %%rbp;"\
				"movq %0, %%rsp;"\
				"movq %2, %%rax;"\
				"movq %3, %%rcx;"\
				"movq %4, %%rdx;"\
				:\
				: "r"(T->store->rsp), "r"(T->store->rbp), "r"((T->store->rax)), "r"(T->store->rcx)\
				 ,"r"(T->store->rdx)\
				);

static const ThreadInfo* generateInfo(Threadstate *state) {
	std::string s = std::string("test").append(std::to_string(state->tID));
	state->info->name = s;
	state->info->threadId = state->tID;
	state->info->free_stack = state->store->rsp - state->end_stack;
	return state->info;


}

const ThreadInfo create(const std::function<void(void)>& f, const char* name) {
	static std::atomic<uint32_t> tID(0);
	tID++;
	auto t = new Threadstate;
	auto *stack = static_cast<uint8_t*>(malloc(MAXSTACKSIZE));

	t->store->rbp = stack + MAXSTACKSIZE - 1;
	t->store->rsp = stack + MAXSTACKSIZE - 1;
	t->main = f;
	t->name = name;
	t->end_stack = stack;
	t->begin_stack = stack + MAXSTACKSIZE - 1;
	t->tID = tID++;

	threadpool.emplace_back(t);

	return *generateInfo(t);
}

void __always_inline terminate() {
	current->flags = DEAD;
	yield();
}

static void __attribute__((noinline)) spawn() {
	current->flags = 1;
	current->main();
	terminate();
	yield();
}

void __attribute__((noinline)) yield() {
	//save the state of the current state if not dead
	if (current->flags != DEAD) {
		SAVESTATE(current);
		//store thread for reinvocation
		threadpool.emplace_back(current);
	} else {
		delete current;
	}
	Threadstate* t = threadpool.front();
	threadpool.pop_front();
	current = t;
	if (t->flags == CREATED) {
		RESTORESTATE(t);
		spawn();
		return;
	}
	RESTORESTATE(t);

}

void __attribute__((noinline)) go() {
	current = new Threadstate;
	current->flags = SCHEDULER;
	current->name = "_scheduler";
	while (true) {
		if (threadpool.size() == 0) {
			return;
		}
		yield();

	}

}

namespace utils {

const ThreadInfo getInfo() {
	SAVESTATE(current);
	return *generateInfo(current);

}
}

namespace memory {

void resizeStack(uint32_t threadId, uint64_t nStackSize) {

	//thread should never resize itself when its running
	assert(current->tID != threadId);
	for (Threadstate* iter : threadpool) {
		if (iter->tID == threadId) {
			uint8_t* nStack = static_cast<uint8_t *>(malloc(nStackSize));
			memccpy((void*) (nStack + nStackSize - 1), iter->store->rsp,
					iter->end_stack - iter->store->rsp, 1);

			//iter->store->rsp = nStack + iter->begin_stack - iter->store->rsp - 1;

		}
	}
}
}
}

