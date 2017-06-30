/*
 * Thread.hpp
 *
 *  Created on: 19.06.2017
 *      Author: kevin
 */

#ifndef BERYL_HPP_
#define BERYL_HPP_

#include <functional>
#include <stdint.h>
#include <string>
namespace beryl {


struct ThreadInfo {

	ThreadInfo() :
			threadId(0), name(), free_stack(0) {
	}
	uint32_t threadId;
	std::string name;
	uint32_t free_stack;
};


namespace utils {

void sleep(uint32_t seconds);


const ThreadInfo getInfo();
};

void go();

const ThreadInfo create(const std::function<void(void)>&, const char* name = nullptr);

void yield();

void test_setup(const std::function<void(void)>&);

void test_run();

namespace memory {

void resizeStack(uint32_t threadId, uint64_t nStackSize);
}

};

#endif /* BERYL_HPP_ */
