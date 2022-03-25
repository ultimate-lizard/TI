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

	for (std::thread& thread : threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
}

void ThreadPool::pushTask(std::function<void()> task)
{
	{
		std::unique_lock<std::mutex> lock(workMutex);
		tasks.push(task);
	}

	conditionalVariable.notify_one();
}

void ThreadPool::startThread()
{
	while (!stopping)
	{
		std::function<void()> task;
		{
			std::unique_lock<std::mutex> lock(workMutex);

			// std::cout << "Watining for a task" << std::endl;

			conditionalVariable.wait(lock, [this]() {
				return !tasks.empty() || stopping;
				});

			if (!tasks.empty())
			{
				task = tasks.front();
				tasks.pop();
			}
		}

		// std::cout << "Working on the task" << std::endl;
		if (task)
		{
			task();
		}
		// std::cout << "Finished the task" << std::endl;
	}
}
