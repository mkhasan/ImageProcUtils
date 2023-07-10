
#include "DetectionManager.h"

#include <sstream>
#include <iostream>

using namespace std;

image_proc_utils::DetectionManager::DetectionManager(int maxObjects) : maxObjects(maxObjects) {
	// communicate with the DetectionReader to store detection info so that the main process can get them
	for (int i = 0; i<NO_OF_ROBOTS; ++i) {
		currentSeqNumber[i] = -1;
	}

	if (maxObjects > DetectionInfo::MAX_CLASS) {
		THROW_EX(RobotException, "invalid value of maxObjects");

	}
	InitializeCriticalSection(&mutex);
	
	for (int i = 0; i < NO_OF_ROBOTS; ++i) {
		
		ghSeqSet[i] = CreateEvent(
			NULL,               // default security attributes
			FALSE,               // manual-reset event
			FALSE,              // initial state is nonsignaled
			NULL  // object name
			);
	}
}

int image_proc_utils::DetectionManager::MaxObjexts() const {	// MaxObjects we are interested in 
	return maxObjects;
}

void image_proc_utils::DetectionManager::SetInfo(const DetectionInfo & info) {
	// DetectionReader calls this method to store detection results associated to a frame
	//DetectionInfo info;
	int temp = (int)info->detectionClass;
	if (temp < 0 || temp >= maxObjects) {
		THROW_EX(RobotException, "Infovalid parameter");
	}
	int robotNo = info->robotNum;
	EnterCriticalSection(&mutex);
	infoList[robotNo][temp].push_back(info);		// one list for each class
	LeaveCriticalSection(&mutex);

	//cout << "robot id " << robotNo << " class: " << info->detectionClass << " info seq " << info->seq << endl;


}

void image_proc_utils::DetectionManager::GetInfoList(int robotNum, vector<DetectionInfo> * info, __int64& seq) {
	// the main program calls this method to retrieve detection info
	

	EnterCriticalSection(&mutex);
	for (int i = 0; i < maxObjects; i++) {
		info[i] = infoList[robotNum][i];
	}
	seq = currentSeqNumber[robotNum];
	//printf("current seq: %06d\n", seq);
	LeaveCriticalSection(&mutex);
}

void image_proc_utils::DetectionManager::UpdateSeqNumber(int robotNum, __int64 seq) {
	
	//cout << "robot no: " << robotNum << " seq: " << seq << endl;
	EnterCriticalSection(&mutex);
	currentSeqNumber[robotNum] = seq; // hightest seq of the fram for which all detection results have beeen received
	
	for (int i = 0; i < DetectionInfo::MAX_CLASS; i++) {
		vector<DetectionInfo> & list = infoList[robotNum][i];
		if (list.size() > 0) {
			vector<DetectionInfo> temp;
			for (int k = 0; k < list.size(); k++) {
				if (list[k]->seq >= currentSeqNumber[robotNum])		// in case of out of seq arrival
					temp.push_back(list[k]);		// temp only contains detection information of with current seq no
			}
			infoList[robotNum][i] = temp;

		}
	}
	
	LeaveCriticalSection(&mutex);
	if (!SetEvent(ghSeqSet[robotNum])) { // all the detection info of a frame has been received...let's inform the main process
		THROW_EX(RobotException, "Error in setting event");	// inform the waiter that all detection results of the farme with seq currentSeqNumber have arrieved 
	}
}

/*
void image_proc_utils::DetectionManager::GetCurrentSeqNumber(int& seq) {
	EnterCriticalSection(&mutex);
	seq = currentSeqNumber;
	LeaveCriticalSection(&mutex);

}
*/