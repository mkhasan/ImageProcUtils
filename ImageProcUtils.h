#pragma once

#ifndef UITLS_H
#define UTILS_H

#ifdef _WIN32
#include <Windows.h>
#endif

#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <memory>
#include <string.h>
#include <algorithm>
#include <stddef.h>

#define _MAX(X, Y) (X >= Y ? (X) : (Y) )

#define THROW_EX(exceptionClass, message) throw exceptionClass(__FILE__, \
__LINE__, (message) )


struct PointXYZ;

#ifdef _WIN32
#define _DLL_EXPORT __declspec(dllexport)
#else
#define _DLL_EXPORT
#endif

namespace image_proc_utils {


	

	class RobotException : public std::exception
	{
		// Disable copy constructors
		RobotException& operator=(const RobotException&);
		std::string file_;
		int line_;
		std::string e_what_;
		int errno_;
	public:
		explicit RobotException(std::string file, int line, int errnum)
			: file_(file), line_(line), errno_(errnum) {
			std::stringstream ss;
#if defined(_WIN32) && !defined(__MINGW32__)
			char error_str[1024];
			strerror_s(error_str, 1024, errnum);
#else
			char * error_str = strerror(errnum);
#endif
			ss << "Robot Exception (" << errno_ << "): " << error_str;
			ss << ", file " << file_ << ", line " << line_ << ".";
			e_what_ = ss.str();
		}
		explicit RobotException(std::string file, int line, const char * description)
			: file_(file), line_(line), errno_(0) {
			std::stringstream ss;
			ss << "Robot Exception: " << description;
			ss << ", file " << file_ << ", line " << line_ << ".";
			e_what_ = ss.str();
		}
		virtual ~RobotException() throw() {}
		RobotException(const RobotException& other) : line_(other.line_), e_what_(other.e_what_), errno_(other.errno_) {}

		int getErrorNumber() const { return errno_; }

		virtual const char* what() const throw () {
			return e_what_.c_str();
		}

	};


#ifdef _WIN32
	void _DLL_EXPORT SafeAccess(HANDLE handle, bool(*func)(char *), char * p);

	_DLL_EXPORT void Acquire(HANDLE handle);

	_DLL_EXPORT void Release(HANDLE handle);





	class _DLL_EXPORT Guard {
		HANDLE m_Handle;
	public:
		Guard(HANDLE handle) : m_Handle(handle) {

			Acquire(m_Handle);
		}

		~Guard() {
			Release(m_Handle);
		}


	};


	struct timezone
	{
		int  tz_minuteswest; /* minutes W of Greenwich */
		int  tz_dsttime;     /* type of dst correction */
	};

	int _DLL_EXPORT gettimeofday(struct timeval *tv, struct image_proc_utils::timezone *tz);

	_DLL_EXPORT void print_time();
	_DLL_EXPORT void print_time(char *str);
	_DLL_EXPORT void print_time_filename_Format(char *str);
	_DLL_EXPORT void date_time(char *str);
	_DLL_EXPORT unsigned char * readCSV(std::string filename);


#endif	
	class ShmData;

	class _DLL_EXPORT ImgData {
		friend class ShmData;
	public:
		static const int WIDTH = 1280;
		static const int HEIGHT = 720;
		static const int CHANNEL = 3;
		static const int MAX_LEN = 256;
		struct image_header {
			__int64 seq;
			unsigned int timestamp;
			int imageSize;
			float speed;
			float position;
			char format[MAX_LEN];
			char streamName[MAX_LEN];
			unsigned char quality;
			int nRobotNum;
			int nRobotPos;
			int nPan;
			int nTilt;
		} header;

		const static int GetBufferSize() {
			return (WIDTH * HEIGHT * CHANNEL + _MAX(1024, sizeof(struct image_header)));
		}
	
		std::vector<char> buffer;
		
		ImgData();

		ImgData(const image_header &header, const char * image = NULL);
		
		void SetData(const image_header &header, const char * image = NULL);
		
	};


#ifdef _WIN32
	class _DLL_EXPORT ShmData {

		struct Queue {
			char * buffer;
			int head;
			int tail;
			int buffer_size;
		
		};

		HANDLE hMutex;
		HANDLE hMapFile;
		char * pShmBuf;
		const int IMG_BUFFER_SIZE = ImgData::GetBufferSize() * 40;

		bool isSlave;
		const char* SHM_NAME = "IMG_PROC_SHM";
		const char * MUTEX_NAME = "IMG_PROC_MUTEX";

	public:
		ShmData(bool isSlave = true);
		ShmData(const char * shmName, int size, const char * mutexName, bool isSlave = true);

		~ShmData();

		int GetAvailableSpace() const;

		void InsertData(const ImgData & imgData);
		void RemoveData(ImgData & imgData);
		void Reset();

	private:
		void Construct(const char * shmName, int size, const char * mutexName, bool isSlave = true);
		void RemoveDataNoLock(ImgData & imgData);
		void InsertDataNoLock(const char * data, int len);
		void CopyData(char * dest, int len);
		

	};

	_DLL_EXPORT std::basic_string<TCHAR> TCHAR_STR(const std::string & str);
#endif

}





#ifndef RM_DEBUG
#define RM_DEBUG(...) {char str[1024]; sprintf(str, __VA_ARGS__); char curr_time[256]; print_time(curr_time); printf("%s: %s", curr_time, str); }
#endif


#endif // ! UTILS_H
