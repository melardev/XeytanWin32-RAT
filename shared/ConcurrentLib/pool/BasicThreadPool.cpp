#include "BasicThreadPool.h"
#include <cassert>

BasicThreadPool::BasicThreadPool(size_t startingSize)
	: stop(false)
{
	assert(startingSize > 0);

	for (size_t i = 0; i < startingSize; ++i)
		workers.emplace_back(
			[this]
			{
				for (;;)
				{
					std::function<void()> runnable;

					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
						                     [this] { return this->stop || !this->runnables.empty(); });
						if (this->stop && this->runnables.empty())
							return;
						runnable = std::move(this->runnables.front());
						this->runnables.pop();
					}

					runnable();
				}
			}
		);
}

void BasicThreadPool::post(const std::function<void()>& runnable)
{
	{
		std::lock_guard<std::mutex> lock(this->queue_mutex);
		this->runnables.push(runnable);
	}
	this->condition.notify_one();
}

BasicThreadPool::~BasicThreadPool()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}

	condition.notify_all();
	for (std::thread& worker : workers)
		worker.join();
}
