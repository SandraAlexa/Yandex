#pragma once
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <iostream>
class thread_pool
{
public:
	thread_pool(int32_t num);
	void execute(std::function<void()> const & f);
	~thread_pool();
	static thread_local thread_pool * curPool;
private:
	std::queue<std::function<void()> > q;
	std::mutex q_mutex;
	std::vector<std::thread> workers;
	std::condition_variable cond_var;
	std::atomic<bool> stop;
};