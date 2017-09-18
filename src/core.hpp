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
	uint8_t* rbp = nullptr;
	uint8_t* rsp = nullptr;
	uint8_t* rax = nullptr;
	uint8_t* rdx = nullptr;
	uint8_t* rcx = nullptr;
};

struct Threadstate {
	RegisterStore store;
	uint8_t flags = 0u;
	std::function<void(void)> main;
	const char *name = nullptr;
	uint8_t* end_stack = nullptr;
	uint8_t* begin_stack = nullptr;
	ThreadInfo info;
	uint32_t tID = 0u;
};

}

#endif /* SRC_CORE_HPP_ */
