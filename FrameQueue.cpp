

#include "FrameQueue.h"
#include <iostream>
 
using namespace std;

image_proc_utils::FrameQueue::FrameQueue() : head(0), tail(0) {
	InitializeCriticalSection(&mutex);
	InitializeConditionVariable(&cond);

}


void image_proc_utils::FrameQueue::Add(const element_t & elem) {

	EnterCriticalSection(&mutex);
	int next = -1;
	do {
		
		next = (tail + 1) % (SIZE + 1);
		if (next != head) 
			break;

		SleepConditionVariableCS(&cond, &mutex, INFINITE);
	} while (1);

	queue[tail].seq = elem.seq;
	queue[tail].frame = elem.frame.clone();
	tail = next;
	int occupied = tail - head;
	if (occupied < 0)
		occupied += (SIZE + 1);
	//cout << "occupied " << occupied << " of " << SIZE << endl;
	LeaveCriticalSection(&mutex);


}

void image_proc_utils::FrameQueue::Remove(__int64 seq, element_t & elem) {
	// get the elem with seq == seq...
	if (seq < 0) {
		THROW_EX(RobotException, "Invalid param");
	}
	
	
	EnterCriticalSection(&mutex);

	if (head == tail) { // empty queue
		elem.seq = -EMPTY_QUEUE;
		LeaveCriticalSection(&mutex);
		return;
	}


	if (queue[head].seq > seq) {
		elem.seq = -OLD_SEQ;		// old detection ignore it
		LeaveCriticalSection(&mutex);
		return;
	}

	if ((tail + 1) % (SIZE + 1) == head) {		// currently the queue is full
		WakeConditionVariable(&cond);
	}

	int temp = head;
	
	while (temp != tail) {		// queue became empty
		if (queue[temp].seq == seq) 
			break;
		temp = (temp + 1) % (SIZE + 1);
	}
	
	if (temp == tail)
		elem.seq = -EMPTY_QUEUE;
		
	else {
		elem.seq = queue[temp].seq;
		elem.frame = queue[temp].frame.clone();
	}
	head = temp;
	//printf("Going to remove with seq: %06d\n", elem.seq);
	LeaveCriticalSection(&mutex);
}

void image_proc_utils::FrameQueue::Remove() {
	element_t elem;
	Remove(elem);
}
void image_proc_utils::FrameQueue::Remove(element_t& elem) {

	EnterCriticalSection(&mutex);
	if (head == tail) {
		elem.seq = -EMPTY_QUEUE;
		LeaveCriticalSection(&mutex);
		return;
	}

	if ((tail + 1) % (SIZE + 1) == head) {		// currently the queue is full
		WakeConditionVariable(&cond);
	}
	elem = queue[head];
	head = (head + 1) % (SIZE + 1);
	LeaveCriticalSection(&mutex);
}


void image_proc_utils::FrameQueue::Clear() {
	EnterCriticalSection(&mutex);
	if ((tail + 1) % (SIZE + 1) == head) {		// currently queue is full
		WakeConditionVariable(&cond);
	}
	head = tail;
	LeaveCriticalSection(&mutex);
}