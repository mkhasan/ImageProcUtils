#pragma once
#ifndef _DETECTION_READER_H 
#define BASIC_DETECTION_READER_H 

#include "ImageProcUtils.h"
#include "DetectionManager.h"



namespace image_proc_utils {
	class _DLL_EXPORT AbsDetectionReader {
	public:
		AbsDetectionReader();
		~AbsDetectionReader();
		virtual void OnMesage(char * buff, int len) = 0; // not making buff constant so that strtok can be directly applied on it
	private:
		bool quit;
		DWORD TID;
		HANDLE hThread;
		
		static DWORD WINAPI ThreadHandler(LPVOID lpParam);
		static void ReadMessages(AbsDetectionReader *reader, SOCKET);
		SOCKET MasterSocket;
		sockaddr_in ServerAddr;                                   // server address parameters
		
		

	};

	class _DLL_EXPORT BasicDetectionReader : public AbsDetectionReader {
	public:
		BasicDetectionReader(DetectionManager* detectionManager_);
		~BasicDetectionReader();
		void OnMesage(char * buff, int len) override;
	private:
		DetectionManager* detectionManager;
	};

}

#endif