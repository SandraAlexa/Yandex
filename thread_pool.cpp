#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <iostream>
#include "thread_pool.h"
thread_pool::thread_pool(int32_t num) {
	stop = false;
	for (int i = 0; i < num; i++) {
		workers.emplace_back([this]() -> void {
			curPool = this;
			while (true) {
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> l(q_mutex);
					cond_var.wait(l, [this] {return this->stop || !q.empty(); });
					if (stop) {
						return;
					}
					task = std::move(q.front());
					q.pop();
				};
				task();
			}
		});
	};
}

void thread_pool::execute(std::function<void()> const & f) {
	{
		std::unique_lock<std::mutex> l(q_mutex);
		if (stop) {
			return;
		}
		q.push(std::move(f));
	}
	cond_var.notify_one();
}


thread_pool::~thread_pool() {
	{
		std::unique_lock<std::mutex> l(q_mutex);
		stop = true;
	}
	cond_var.notify_all();
	for (auto & w : workers) {
		w.join();
	}
}

thread_local thread_pool * thread_pool::curPool = nullptr;