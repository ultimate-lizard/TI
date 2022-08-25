#include "ThreadPool.h"

#include <iostream>

ThreadPool::ThreadPool() :
	stopping(false)
{
	// const size_t THREAD_COUNT = std::thread::hardware_concurrency();
	const size_t THREAD_COUNT = 4;
	for (size_t i = 0; i < THREAD_COUNT; ++i)
	{
		threads.push_back(std::thread(&ThreadPool::startThread, this));
	}
}

ThreadPool::~ThreadPool()
{
	stopping = true;
	conditionalVariable.notify_all();

	for (std::thread& thread : threads)
	{
		while (thread.joinable())
		{
			thread.join();
		}
	}
}

void ThreadPool::pushTask(std::function<void()> task)
{
	if (!threads.empty())
	{
		{
			std::unique_lock<std::mutex> lock(workMutex);
			tasks.push(task);
		}

		conditionalVariable.notify_one();
	}
}

void ThreadPool::startThread()
{
	while (!stopping)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(workMutex);

			conditionalVariable.wait(lock, [this]() {
				return !tasks.empty() || stopping;
				});

			if (!tasks.empty())
			{
				task = tasks.front();
				tasks.pop();
			}
		}

		if (task)
		{
			task();
		}
	}
}
