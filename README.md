## Do not use this!
This is something I did to learn how to do something asynchronously in a GUI program without blocking the main graphics thread for long.\
It has not been properly tested or anything, its a terrible API and has caveats like we only expect you to call `async.set_function()` once.

The only blocking thing is changing the value of a pointer, so the slowest part you'd be doing in your main thread is `async.try_start()` which launches a std::thread (takes ~0.15 milliseconds on my fastest computer).

To accomplish the fast `async.get_data()`, it internally swaps between 2 pointers, meaning the memory footprint is 2 times what your `async.set_function()` function returns.

If your `async.set_function()` function allocates and returns a struct of 2 GB, it will use 4 GB ram at peak on any successive calls to `async.try_start()` after the first one.

## Basic usage
You can also look at `example.cpp` for an example.

```cpp
#include "async.hpp"
using namespace myasync;

struct Data {
    std::string text;
};

Async <Data> async; // Type is Data, not *Data here...
async.set_function([]() -> Data* {
    Data *data = new Data(); // The Async class handles destruction of this
    data->text = "Hello world!";
    return data;
});

// Main graphics loop
while (true) {
    if (/*Some key is pressed*/) {
        bool started = async.try_start();
        if (! started) {
            std::cout << "Already running!" << std::endl;
        }
    }

    async.lock();
    Data *data = async.get_data();
    if (data != nullptr) {
        std::cout << data->text << std::endl;
    }
    async.unlock();
}
```

## `namespace myasync`
- `void Async::set_function(std::function<T*()>)` Set the function to be ran asynchronously
- `bool Async::try_start()` Returns false if it was already running and couldn't start
- `T* Async::get_data()` (Requires lock/unlock functions) Returns a pointer to the return value
- `void Async::reset_data()` (Requires lock/unlock functions) Essentially resets the output of `Async::get_data()` to nullptr
- `bool Async::is_running()` Returns true if it is already running because of `Async::try_start()`

Needed for `T* Async::get_data()` and `void Async::reset_data()`:
- `void Async::lock()`
- `void Async::unlock()` 
