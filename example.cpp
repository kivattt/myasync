#include <iostream>
#include <string>

#include "async.hpp"
using namespace myasync;

using std::string;

struct Data {
	string text;
};

int main() {
	std::srand(std::time(nullptr));

	Async<Data> async;
	async.set_function([]() -> Data* {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		Data *d = new Data();
		d->text = "hello! random number: " + std::to_string(rand() % 100);
		return d;
	});

	async.try_start();

	while (true) {
		async.lock();
		Data *d = async.get_data();

		if (d != nullptr) {
			std::cout << d->text << '\n';
			//async.reset_data();
			async.try_start();
		}
		async.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(70));
	}
}
