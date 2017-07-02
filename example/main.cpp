/*
 * main.cpp
 *
 *  Created on: 19.06.2017
 *      Author: kevin
 */
#include <iostream>
#include <string>

#include <beryl>
using namespace std;

void __attribute__((noinline)) test(int a, int b) {
	auto info = beryl::utils::getInfo();

	cout << a + b << endl;
	cout << "thread: " << info.name << " has " << info.free_stack
			<< " Bytes free on its stack" << endl;

}

int main() {

	auto t =
			[]() {

				auto info = beryl::utils::getInfo();

				cout << "thread: " << info.name << " has " << info.free_stack << " Bytes free on its stack" << endl;


				test(5,6);
				beryl::yield();

				info = beryl::utils::getInfo();
				cout << "thread: " << info.name << " has " << info.free_stack << " Bytes free on its stack" << endl;
			};


	auto info = beryl::create(t);

	beryl::create([info](){
	    beryl::memory::resizeStack(info.threadId, 20000);
	  });
	
	beryl::go();

	return 0;
}
