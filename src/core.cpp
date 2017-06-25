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
#include "beryl.hpp"
#include <sstream>

namespace beryl {

#define CREATED 0
#define RUNNING 1
#define SCHEDULER 99
#define DEAD 2
#define WAITING 3

struct RegisterStore {
	RegisterStore() :rbp(nullptr), rsp(nullptr), rax(nullptr), rdx(nullptr), rcx(nullptr) {};
	uint8_t* rbp;
	uint8_t* rsp;
	uint8_t* rax;
	uint8_t* rdx;
	uint8_t* rcx;

};

struct Threadstate {
	Threadstate() : store(new RegisterStore),
		 flags(0), name(nullptr), end_stack(
					nullptr), info(new utils::ThreadInfo), tID(0) {
	}
	;

	RegisterStore* store;
	uint8_t flags;
	std::function<void(void)> main;
	const char *name;
	uint8_t* end_stack;
	beryl::utils::ThreadInfo* info;
	uint32_t tID;
};

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

static std::deque<Threadstate*> threadpool;

static Threadstate* current = nullptr;

static const uint64_t MAXSTACKSIZE = 10240;

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

	threadpool.push_back(t);
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
		threadpool.push_back(current);
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
;
