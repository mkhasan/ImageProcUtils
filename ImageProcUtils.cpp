#include "ImageProcUtils.h"

#include <iostream>
using namespace std;
#define THROW THROW_EX

_DLL_EXPORT void image_proc_utils::Acquire(HANDLE handle) {

	DWORD dwWaitResult = WaitForSingleObject(
		handle,    // handle to mutex
		INFINITE);  // no time-out interval

	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:

		// TODO: Write to the database

		break;

		// The thread got ownership of an abandoned mutex
		// The database is in an indeterminate state
	case WAIT_ABANDONED:
		return THROW(RobotException, "Error in accessing mutex");
	}

}

_DLL_EXPORT void image_proc_utils::Release(HANDLE handle) {

	if (!ReleaseMutex(handle))
	{
		THROW(RobotException, "Error in releasing mutex ");
	}

}


void image_proc_utils::SafeAccess(HANDLE handle, bool(*func)(char *), char * p) {
	DWORD dwWaitResult = WaitForSingleObject(
		handle,    // handle to mutex
		INFINITE);  // no time-out interval


	switch (dwWaitResult)
	{
		// The thread got ownership of the mutex
	case WAIT_OBJECT_0:

		// TODO: Write to the database
		if (func(p) == false) {
			ReleaseMutex(handle);
			THROW(image_proc_utils::RobotException, "Error");
		}


		if (!ReleaseMutex(handle))
		{
			THROW(image_proc_utils::RobotException, "Error in releasing mutex ");
		}

		break;

		// The thread got ownership of an abandoned mutex
		// The database is in an indeterminate state
	case WAIT_ABANDONED:
		return THROW(image_proc_utils::RobotException, "Error in accessing mutex");
	}
}

#include < time.h >

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int _DLL_EXPORT image_proc_utils::gettimeofday(struct timeval *tv, struct image_proc_utils::timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
}

void _DLL_EXPORT image_proc_utils::print_time() {
	char str[256];
	print_time(str);
	printf("%20s: %s\n ", "Now: ", str);
}
void _DLL_EXPORT image_proc_utils::print_time(char *str) {
	//date_time(str);

	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);

	__time64_t long_time;

	_time64(&long_time);             // Get time as 64-bit integer.
									   // Convert to local time.
	tm = _localtime64(&long_time); // C4996

	sprintf(str, "%d:%02d:%02d_%06d", tm->tm_hour, tm->tm_min,
		tm->tm_sec, tv.tv_usec);
}

void _DLL_EXPORT image_proc_utils::print_time_filename_Format(char *str) {
	//date_time(str);

	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);

	__time64_t long_time;

	_time64(&long_time);             // Get time as 64-bit integer.
									   // Convert to local time.
	tm = _localtime64(&long_time); // C4996

	sprintf(str, "%d_%02d_%02d_%d", tm->tm_hour, tm->tm_min,
		tm->tm_sec, tv.tv_usec);
}




void _DLL_EXPORT image_proc_utils::date_time(char *str)   // a function from a DLL 
{
	struct tm *newtime;
	char am_pm[] = "AM";
	__time64_t long_time;

	_time64(&long_time);             // Get time as 64-bit integer.
									   // Convert to local time.
	newtime = _localtime64(&long_time); // C4996
	// Note: _localtime64 deprecated; consider _localetime64_s

	if (newtime->tm_hour > 12)        // Set up extension.
		strcpy_s(am_pm, sizeof(am_pm), "PM");
	if (newtime->tm_hour > 12)        // Convert from 24-hour
		newtime->tm_hour -= 12;        //   to 12-hour clock.
	if (newtime->tm_hour == 0)        // Set hour to 12 if midnight.
		newtime->tm_hour = 12;


	char buff[30];
	asctime_s(buff, sizeof(buff), newtime);
	sprintf(str, "%.19s %s\n", buff, am_pm);
}

std::basic_string<TCHAR> image_proc_utils::TCHAR_STR(const std::string & str) {



	std::basic_ostringstream<TCHAR> ss;
	for (int i = 0; i < str.length(); i++)
		ss << str.at(i);

	//std::basic_string<TCHAR> ts;
	//ts = ss.str();
	return ss.str();

}


image_proc_utils::ImgData::ImgData() {
	memset(&header, 0, sizeof(header));

	buffer.resize(GetBufferSize());
}
image_proc_utils::ImgData::ImgData(const image_proc_utils::ImgData::image_header & _header, const char * image) {
	buffer.resize(GetBufferSize());
	
	SetData(_header, image);
}

void image_proc_utils::ImgData::SetData(const image_proc_utils::ImgData::image_header &_header, const char * image) {
	header = _header;
	if (header.imageSize < 0 || header.imageSize > GetBufferSize())
		THROW(RobotException, "Inappropriate image size");

	//int k = GetBufferSize();
	if (header.imageSize > 0 && image != NULL) {
		
		memcpy(&buffer[0], image, header.imageSize);
	}

}


image_proc_utils::ShmData::ShmData(bool _isSlave)
	: isSlave(_isSlave)
{
	Construct(SHM_NAME, IMG_BUFFER_SIZE, MUTEX_NAME, _isSlave);
	
}

image_proc_utils::ShmData::ShmData(const char * shmName, int size, const char * mutexName, bool slave)
	: isSlave(slave)
{
	Construct(shmName, size, mutexName, slave);
}
void image_proc_utils::ShmData::Construct(const char * shmName, int size, const char * mutexName, bool slave) 
{

	size += 1;		// on byte extra to handle head and tail position 

	std::basic_string<TCHAR> szName = TCHAR_STR(shmName);
	std::basic_string<TCHAR> mName = TCHAR_STR(mutexName);

	if (slave)
	{
		hMapFile = OpenFileMapping(
			FILE_MAP_ALL_ACCESS,   // read/write access
			FALSE,                 // do not inherit the name
			szName.c_str());               // name of mapping object
	}
	else {
		hMapFile = CreateFileMapping(
			INVALID_HANDLE_VALUE,    // use paging file
			NULL,                    // default security
			PAGE_READWRITE,          // read/write access
			0,                       // maximum object size (high-order DWORD)
			sizeof(Queue) + size,                // maximum object size (low-order DWORD)
			szName.c_str());                 // name of mapping object


	}

	if (hMapFile == NULL)
	{
		char str[256];
		sprintf(str, "Could not create file mapping object (%d).\n",
			GetLastError());

		THROW(RobotException, str);
	}

	pShmBuf = (char *)MapViewOfFile(hMapFile,   // handle to map object
		FILE_MAP_ALL_ACCESS, // read/write permission
		0,
		0,
		sizeof(Queue) + size);

	if (pShmBuf == NULL)
	{
		char str[256];
		sprintf(str, "Could not map view of file (%d).\n",
			GetLastError());

		CloseHandle(hMapFile);

		THROW(RobotException, str);
	}

	if (slave == false) {
		memset(pShmBuf, 0, sizeof(Queue)+size);
		Queue * queue = (Queue *)pShmBuf;
		queue->head = queue->tail = 0;
		queue->buffer_size = size;
		
		
	}


	hMutex = CreateMutex(
		NULL,                        // default security descriptor
		FALSE,                       // mutex not owned
		mName.c_str());  // object name

	if (hMutex == NULL) {
		char str[256];
		sprintf(str, "CreateMutex error: %d\n", GetLastError());
		THROW(RobotException, str);
	}
	else
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			printf("CreateMutex opened an existing mutex\n");
		else printf("CreateMutex created a new mutex.\n");


	
}

image_proc_utils::ShmData::~ShmData() {
	cout << "going to unmap" << endl;
	UnmapViewOfFile(pShmBuf);

	CloseHandle(hMapFile);

	CloseHandle(hMutex);
}

int image_proc_utils::ShmData::GetAvailableSpace() const {
	Queue * q = (Queue *)pShmBuf;
	q->buffer = &pShmBuf[sizeof(Queue)];
	int & head = q->head;
	int & tail = q->tail;
	int & buffer_size = q->buffer_size;

	if (head == tail)
		return (buffer_size - 1);

	int end = tail;
	if (end < head)
		end += buffer_size;

	int used = end - head;
	return (buffer_size - 1 - used);


}

void image_proc_utils::ShmData::InsertData(const image_proc_utils::ImgData & imgData) {

	Acquire(hMutex);
	int available = GetAvailableSpace();
	int required = sizeof(image_proc_utils::ImgData::image_header) + imgData.header.imageSize;

	/*
	if (required < available && isSlave == true) {		
		Release(hMutex);
		THROW(RobotException, "check available space first");
	}
	*/

	while (required > available) {
		image_proc_utils::ImgData::image_header header;
		image_proc_utils::ImgData temp;
		RemoveDataNoLock(temp);

		if (temp.header.imageSize < 0) {		// if buffer does not have any data header.imgsize will be -1

			Release(hMutex);
			THROW(RobotException, "Inconsistent state !!!");
		}

		available = GetAvailableSpace();		// since data is removed we need to calculate availability again
		
	}

	//printf("buffer used before insertion : %10d bytes\n", IMG_BUFFER_SIZE - available);

	try {
		int headerSize = sizeof(image_proc_utils::ImgData::image_header);

		InsertDataNoLock((const char *)&imgData.header, headerSize);
		if (imgData.header.imageSize > 0)
			InsertDataNoLock((char *)&imgData.buffer[0], imgData.header.imageSize);
	}
	catch (const std::exception & exp) {

		
		Release(hMutex);
		
		std::string msg = std::string("Error: " + std::string(exp.what()));
		THROW(RobotException, msg.c_str());
	}

	Release(hMutex);
}


void image_proc_utils::ShmData::InsertDataNoLock(const char * data, int len) {

	Queue * q = (Queue *)pShmBuf;
	q->buffer = &pShmBuf[sizeof(Queue)];
	int & head = q->head;
	int & tail = q->tail;
	int & buffer_size = q->buffer_size;

	//cout << "tail is " << q->tail << endl;
	
	if (tail + len <= buffer_size) {
		memcpy(&q->buffer[tail], data, len);
		tail += len;
		if (tail == buffer_size)
			tail = 0;
		return;
	}

	int right = buffer_size - tail;
	int left = len - right;

	memcpy(&q->buffer[tail], &data[0], right);
	memcpy(&q->buffer[0], &data[right], left);
	tail = left;
}


void image_proc_utils::ShmData::RemoveData(ImgData & imgData) {
	Acquire(hMutex);
	try {
		RemoveDataNoLock(imgData);
	}
	catch (const std::exception e) {
		Release(hMutex);

		std::string msg = std::string("Error: " + std::string(e.what()));
		THROW(RobotException, msg.c_str());
	}

	Release(hMutex);
}

void image_proc_utils::ShmData::RemoveDataNoLock(ImgData & imgData) {		// should be called only when mutex is acquired

	Queue * q = (Queue *)pShmBuf;
	q->buffer = &pShmBuf[sizeof(Queue)];
	int & head = q->head;
	int & tail = q->tail;
	int & buffer_size = q->buffer_size;

	int end = tail;
	if (end < head) {
		end += buffer_size;

	}

	if (end < head) {
		THROW(RobotException, "Inconsistent state !!!");
	}

	//printf("data remaining %06d\n", end - head);
	int headerSize = sizeof(imgData.header);
	if ((end - head) < headerSize) {
		imgData.header.imageSize = -1;
		return;
	}

	CopyData((char *)&imgData.header, headerSize);
	if (imgData.header.imageSize < 0) {
		THROW(RobotException, "Inconsistent state !!!");
	}

	end = tail;
	if (end < head)
		end += buffer_size;
	if (end - head < imgData.header.imageSize) {
		THROW(RobotException, "Inconsistent state !!!");
	}

	CopyData((char *)&imgData.buffer[0], imgData.header.imageSize);
		


	
}

void image_proc_utils::ShmData::CopyData(char * dest, int len) {

	Queue * q = (Queue *)pShmBuf;
	q->buffer = &pShmBuf[sizeof(Queue)];
	int & head = q->head;
	int & tail = q->tail;
	int & buffer_size = q->buffer_size;

	if (head + len <= buffer_size) {
		memcpy(dest, &q->buffer[head], len);
		head += len;
		if (head == buffer_size)
			head = 0;
		return;
	}

	int right = buffer_size - head;
	int left = len - right;
	memcpy(dest, &q->buffer[head], right);
	memcpy(&dest[right], &q->buffer[0], left);
	head = left;
}

void image_proc_utils::ShmData::Reset() {
	Acquire(hMutex);
	Queue * q = (Queue *)pShmBuf;
	q->head = q->tail;

	Release(hMutex);
}
