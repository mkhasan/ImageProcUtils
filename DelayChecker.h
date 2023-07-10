#pragma once
#ifndef DELAY_CHECKER_H
#define DELAY_CHECKER_H

#include "ImageProcUtils.h"
#include <string>
#include <vector>

namespace image_proc_utils {
	class _DLL_EXPORT DelayChecker {
		typedef struct {
			char send_time[256];
			char arrival_time[256];
		} delay_info_t;

	public:
		DelayChecker(const std::string & filename, int infoSize, int startIndex = 0);
		~DelayChecker();
		void UpdateSendTime(int seq);
		void UpdateArrivalTime(int seq);
		void WriteIntoFile();
		void Print();
	private:
		
		
		std::vector<delay_info_t> infoList;

		std::string filename;
		int infoSize;
		int startIndex;
	};

}
#endif // !DELAY_CHECKER_H
