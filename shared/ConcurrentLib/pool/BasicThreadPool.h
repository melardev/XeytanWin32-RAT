#pragma once
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

class BasicThreadPool
{
public:
	BasicThreadPool(size_t startingSize = 5);
	void post(const std::function<void()>& runnable);

	~BasicThreadPool();
private:

	std::vector<std::thread> workers;

	std::queue<std::function<void()>> runnables;

	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};
