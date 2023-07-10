
#include <winsock2.h>

#include "DetectionInfo.h"
#include "BasicDetectionReader.h"
#include "ImageProcUtils.h"
#include "Config.h"

#include <iostream>
#include <asferr.h>


#pragma comment(lib, "Ws2_32.lib")

using namespace std;
image_proc_utils::AbsDetectionReader::AbsDetectionReader() : quit(false), TID(0), hThread(NULL) {

	// receives detection info from the Image Classifier unit
	// starts a server at port DETECTION_READER_CUSTOM_PORT and parses the detection results 
	// once msg is received from the image classifer module (which can in another computer) the OnMessage of this abstrct class is called
	// the inherited class must impliment OnMessage method	
	sockaddr_in ServerAddr;                                   // server address parameters
	
	
	WSADATA     WsaData;

	int ret = WSAStartup(0x101, &WsaData);
	if (ret != 0) {
		THROW_EX(RobotException, "Error in initializing");
	}
	

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = INADDR_ANY;
	ServerAddr.sin_port = htons( DETECTION_READER_CUSTOM_PORT);                 // listen on RTSP port 8554
	MasterSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (bind(MasterSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr)) != 0) {
		THROW_EX(RobotException, "Error in binding");
	}
	
	if (listen(MasterSocket, 5) != 0) {
		THROW_EX(RobotException, "Error in listening");
	}
	
	


	hThread = CreateThread(NULL, 0, ThreadHandler, this, 0, &TID);
	if (hThread == NULL) {
		THROW_EX(RobotException, "Error in creating thread");
	}


}

image_proc_utils::AbsDetectionReader::~AbsDetectionReader() {

	quit = true;
	closesocket(MasterSocket);
	WSACleanup();

	if (hThread)
		WaitForSingleObject(hThread, INFINITE);
}

DWORD WINAPI image_proc_utils::AbsDetectionReader::ThreadHandler(LPVOID ptr) {

	AbsDetectionReader * reader = (AbsDetectionReader *)ptr;

	SOCKET      ClientSocket;                                 
	sockaddr_in ClientAddr;
	int ClientAddrLen = sizeof(ClientAddr);

	DWORD temp;
	while (reader->quit == false)
	{   // loop forever to accept client connections
		ClientSocket = accept(reader->MasterSocket, (struct sockaddr*)&ClientAddr, &ClientAddrLen);
		
		//CreateThread(NULL, 0, ReadMessages, &ClientSocket, 0, &temp);
		ReadMessages(reader, ClientSocket);
		//CreateThread(NULL, 0, SessionThreadHandler, &ClientSocket, 0, &TID);
		//printf("Client connected. Client address: %s\r\n", inet_ntoa(ClientAddr.sin_addr));
	}

	return 0;
}

using namespace std;

void image_proc_utils::AbsDetectionReader::ReadMessages(AbsDetectionReader *reader, SOCKET Client) {

	
	static char RecvBuf[MAX_DETECTION_MSG_SIZE];
	//static char buffer[MAX_DETECTION_MSG_SIZE];
	const int MAX_INFO = 10;
	const int INFO_SIZE = sizeof(DetectionInfo::rep_t);
	assert(sizeof(DetectionInfo::rep_t) * MAX_INFO < MAX_DETECTION_MSG_SIZE);
	
	WSAEVENT SockEvent = WSACreateEvent();      // 
	
	
	WSAEventSelect(Client, SockEvent, FD_READ | FD_CLOSE);   // select socket read event


	

	bool closed = false;
	int index = 0;
	
	
	while (!reader->quit && !closed)
	{
		int ret = WaitForSingleObject(SockEvent, 5000);
		switch(ret)
		{

			case WAIT_OBJECT_0:
			{   // read client socket
				//cout << "got event" << endl;
				WSANETWORKEVENTS NetworkEvents;
				
				int ret = WSAEnumNetworkEvents(Client, SockEvent, &NetworkEvents);
				
				//printf("ret: %d event val is %x FD_READ %x FC_CLOSE %x\n", ret, NetworkEvents.lNetworkEvents, FD_READ, FD_CLOSE);
				if (NetworkEvents.lNetworkEvents & FD_READ) {
					//WSAResetEvent(SockEvent);
					//cout << "Reading" << endl;
					

					//memset(RecvBuf, 0x00, sizeof(RecvBuf));
					do {
						int req = INFO_SIZE - (index % INFO_SIZE);
						int res = recv(Client, &RecvBuf[index], req, 0);
						assert(res <= req);
						//cout << "received: " << res << " bytes\n";
						if (res <= 0) {
							//cout << "received 0" << endl;
							break;
						}
						else if (res < req) {
							index += res;
							break;
						}
						else {
							
							index+=req;
							if (index >= MAX_INFO * INFO_SIZE) {
								reader->OnMesage(RecvBuf, index);
								index = 0;
								continue;
							}
							int prev = index - INFO_SIZE;
							DetectionInfo::rep_t *p = (DetectionInfo::rep_t *)&RecvBuf[prev];
							if (p->seq > 0 && p->detectionClass == DetectionInfo::MAX_CLASS) {
								//char str[256];
								//print_time(str);
								//printf("seq: %06d at %s\n", p->seq, str);
								reader->OnMesage(RecvBuf, index);
								index = 0;
							}

						}
					} while (!reader->quit);

				}
			    if (NetworkEvents.lNetworkEvents & FD_CLOSE) {
					//WSAResetEvent(SockEvent);
					//cout << "Closing" << endl;
					closed = true;
					break;
				}

				break;
			}
			case WAIT_TIMEOUT:
			{
				cout << "CustomDetectionReader: time out" << endl;
				break;
			};
		};
	};

	cout << "done" << endl;
	closesocket(Client);

}

image_proc_utils::BasicDetectionReader::BasicDetectionReader(DetectionManager* detectionManager_) : AbsDetectionReader(), detectionManager(detectionManager_) {

};

image_proc_utils::BasicDetectionReader::~BasicDetectionReader() {

}

void image_proc_utils::BasicDetectionReader::OnMesage(char * buff, int len) {
	try {

		if (len < sizeof(DetectionInfo::rep_t)) {
			THROW_EX(RobotException, "Data format error");
		}

		DetectionInfo info;
		int index = 0;
		while (index < len) {

			const DetectionInfo::rep_t *p = (DetectionInfo::rep_t *) &buff[index];
			info = *p;
			if (info->seq >= 0) {
				if (info->detectionClass < DetectionInfo::MAX_CLASS) {	// info->detectionClass == MAX_CLASS indicates that all detection of current seq has been sent
					//cout << info.ToString() << endl;
					detectionManager->SetInfo(info);
				}
				else {
					detectionManager->UpdateSeqNumber(info->robotNum, info->seq); // notify detectionManager so that detectionManager can inform the main process
				}
			}

			index += sizeof(DetectionInfo::rep_t);
		}
	}
	catch (const std::exception& exp) {
		cout << "Error: " << exp.what() << endl;
	}
}
