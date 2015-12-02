
#include "TSingleton.h"
#include <functional>
#include <vector>
#include <queue>
#include <unordered_map>

#include <thread>
#include <mutex>
#include <condition_variable>

/*void my_task()
{
		//Do Things
}

void main()
{
	int queue_id = BeginNewTaskQueue();

	PostTaskToQueue(queue_id, &my_task);
	PostTaskToQueue(queue_id, &my_task);
	PostTaskToQueue(queue_id, &my_task);
	PostTaskToQueue(queue_id, &my_task);
	PostTaskToQueue(queue_id, &my_task);

	WaitForTaskQueueToComplete(queue_id);

}*/

#define MAX_QUEUE_INDICIES			100
#define NUM_WORKER_THREADS			8

struct Task
{
	int queue_idx;							//Queue Index or -1 if not associated with a given queue
	std::function<void()> task_function;		//Function to call
	std::function<void()> task_callback;		//Optional callback function to execute upon completion
};

class TaskScheduler : public TSingleton<TaskScheduler>
{
public:
	
	int  BeginNewTaskQueue();													//Start a new task queue, acquiring an index to use when submiting new tasks to this work-queue
	void PostTaskToQueue(int queue_idx, const std::function<void()>& task);		//Submit tasks to a given queue index, these will be processed as soon as possible after submittion
	void WaitForTaskQueueToComplete(int queue_idx);								//Wait for all tasks in the given queue index to complete 


//	void PostIndividualTask(const std::function<void>& task);														//Post a one-off task that will complete when it can
//	void PostIndividualTaskWithCallback(const std::function<void>& task, const std::function<void>& callback);		//Post a one-off task that will call the provided function when it completes.

public:
	TaskScheduler();
	~TaskScheduler();

	void ThreadWorkLoop();

protected:
	std::queue<int>							m_UnassignedQueueIndices;
	std::unordered_map<int, unsigned int>	m_ActiveQueues;

	std::queue<Task>						m_QueuedTasks;


	std::mutex					m_mDataMutex;
	std::condition_variable		m_cvTaskReadyForProcessing;
	std::condition_variable		m_cvTaskCompleted;

	std::thread					m_WorkerThreads[NUM_WORKER_THREADS];
	bool						m_IsTerminating;
};