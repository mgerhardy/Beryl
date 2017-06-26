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
		asm("movq (%1), %%rbp;"\
				"movq (%0), %%rsp;"\
				:\
				: "r"(&T->store->rsp), "r"(&T->store->rbp)\
				);


void create(const std::function<void(void)>& f, const char* name) {
	static std::atomic<uint64_t> tID(0);
	tID++;
	auto t = new Threadstate;
	auto *stack = static_cast<uint8_t*>(malloc(MAXSTACKSIZE));

	t->store->rbp = stack + 4098;
	t->store->rsp = stack + 4098;
	t->main = f;
	t->name = name;
	t->end_stack = stack;
	t->tID = tID++;

	threadpool.emplace_back(t);
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

const ThreadInfo& getInfo() {
	SAVESTATE(current);

	std::string s = std::string("test").append(std::to_string(current->tID));
	current->info->name = s;
	current->info->free_stack = current->store->rsp - current->end_stack;

	return reinterpret_cast<ThreadInfo&>(*current->info);

}
}
}

