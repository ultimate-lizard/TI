#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;

	void pushTask(std::function<void()> task);

private:
	void startThread();

private:
	std::vector<std::thread> threads;
	std::queue<std::function<void()>> tasks;

	std::mutex workMutex;
	std::condition_variable conditionalVariable;

	bool stopping;
};
