#pragma once

#ifndef FRAME_QUEUE_H
#define FRAME_QUEUE_H
 
#include "ImageProcUtils.h"
#include "opencv2/core.hpp"

namespace image_proc_utils {
	class _DLL_EXPORT FrameQueue {
		enum {SUCCESS = 0, EMPTY_QUEUE=1, OLD_SEQ=2 };
	public:
		typedef struct  {
			__int64 seq;
			cv::Mat frame;
		} element_t;
	public:
		FrameQueue();
		void Add(const element_t & elem);
		void Remove(__int64 seq, element_t & elem);
		void Remove(element_t& elem);
		void Remove();
		void Clear();

	private:
		enum { SIZE = 100 };
		int head, tail;
		element_t queue[SIZE+1];
		CRITICAL_SECTION mutex;
		CONDITION_VARIABLE cond;


	};

}
#endif // !FRAME_QUEUE_H