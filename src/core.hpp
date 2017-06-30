/*
 * core.hpp
 *
 *  Created on: 26.06.2017
 *      Author: kevin
 */

#ifndef SRC_CORE_HPP_
#define SRC_CORE_HPP_

#include <beryl>
#include <memory>
namespace beryl {

#define CREATED 0
#define RUNNING 1
#define SCHEDULER 99
#define DEAD 2
#define WAITING 3

struct ThreadInfo;

struct RegisterStore {
	RegisterStore() :
			rbp(nullptr), rsp(nullptr), rax(nullptr), rdx(nullptr), rcx(nullptr) {
	}
	;
	uint8_t* rbp;
	uint8_t* rsp;
	uint8_t* rax;
	uint8_t* rdx;
	uint8_t* rcx;

};

struct Threadstate {
	Threadstate() :
			store(new RegisterStore), flags(0), name(nullptr), end_stack(
					nullptr), begin_stack(nullptr), info(new ThreadInfo), tID(0) {
	}

	~Threadstate() {
		delete store;
	}

	RegisterStore* store;
	uint8_t flags;
	std::function<void(void)> main;
	const char *name;
	uint8_t* end_stack;
	uint8_t* begin_stack;
	ThreadInfo *info;
	uint32_t tID;
};

}

#endif /* SRC_CORE_HPP_ */
