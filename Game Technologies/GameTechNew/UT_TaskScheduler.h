

#include <ncltech\TaskScheduler.h>
#include <iostream>

namespace UT_TaskScheduler
{

	std::mutex m;
	int counter = 0;

	void worker_task()
	{
		std::lock_guard<std::mutex> lck(m);
		for (int i = 0; i < 10000; i++)
			counter--;
	}

	void worker_task2()
	{
		std::lock_guard<std::mutex> lck(m);
		for (int i = 0; i < 10000; i++)
			counter++;
	}

	void UT_TaskScheduler()
	{
		TaskScheduler::Instance();
		std::cout << "Task Scheduler Initialized\n";


		int queue_idx = TaskScheduler::Instance()->BeginNewTaskQueue();
		std::cout << "Task Queue Begun with queue_idx: " << queue_idx << "\n";

		int queue_idx2 = 2;

		for (int i = 0; i < 10000; i++)
		{
			TaskScheduler::Instance()->PostTaskToQueue(queue_idx, UT_TaskScheduler::worker_task);
			TaskScheduler::Instance()->PostTaskToQueue(queue_idx, UT_TaskScheduler::worker_task2);
		}
		std::cout << "Submitted 1000 Tasks to be completed\n";

		TaskScheduler::Instance()->WaitForTaskQueueToComplete(queue_idx2); //Should auto-complete as no work is queued for queue_id 2 and not wait for condition

		TaskScheduler::Instance()->WaitForTaskQueueToComplete(queue_idx);
		std::cout << "All tasks successfully completed\nCounter Value: " << counter << "\n";

		TaskScheduler::Release();
		std::cout << "TaskScheduler Shut Down Successfully\n";

		system("pause");
	}
}