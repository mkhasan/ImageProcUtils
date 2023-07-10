#pragma once
#ifndef DECTION_MANAGER_H
#define DECTION_MANAGER_H

#include "DetectionInfo.h"

#include "Config.h"

namespace image_proc_utils {

	class _DLL_EXPORT DetectionManager {
		
	public:
		DetectionManager(int maxObjcts = 3);
		int MaxObjexts() const;
		void SetInfo(const DetectionInfo & info);
		void GetInfoList(int robotNum, std::vector<DetectionInfo> * info, __int64& seq);
		void UpdateSeqNumber(int robotNum, __int64 seq);


		HANDLE ghSeqSet[NO_OF_ROBOTS];

	private:
		int maxObjects;
		CRITICAL_SECTION mutex;
		std::vector<DetectionInfo> infoList[NO_OF_ROBOTS][DetectionInfo::MAX_CLASS];
		int currentSeqNumber[NO_OF_ROBOTS];

	};

}
#endif // !DECTION_MANAGER_H