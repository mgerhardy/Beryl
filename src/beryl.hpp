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


namespace utils {

	struct ThreadInfo {
	std::string name;
	uint32_t free_stack;
	};

	void sleep(uint32_t seconds);

	const ThreadInfo& getInfo();

};


void go();

void create(const std::function<void(void)>&, const char* name = nullptr);

void yield();

void test_setup(const std::function<void(void)>&);

void test_run();


namespace memory {
	void resize();
};



};

#endif /* BERYL_HPP_ */
