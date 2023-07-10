#pragma once
#ifndef DETECTION_INFO_H
#define DETECTION_INFO_H

#ifdef _WIN32
#include "opencv2/core.hpp"
#endif

#include <string>
#include <sstream>

#include "ImageProcUtils.h"

namespace image_proc_utils {


class _DLL_EXPORT DetectionInfo {
public:
	typedef enum { FIRE, SPARK, HUMAN, SMOKE, FLOOD, MAX_CLASS } class_t;

	std::string classes[MAX_CLASS];// = { "fire", "spark", "human", "smoke", "flood" };
	/*
	class_t detectionClass;
    int top, left, bottom, right;
	float confidence;
	int seq;
	int robotNum;
	*/

public:
	typedef struct {
		class_t detectionClass;
		int top;
		int left;
		int bottom;
		int right;
		float confidence;
		__int64 seq;
		int robotNum;
	} rep_t;

private:
	rep_t rep;

	void InitClasses() {
		const char * str[MAX_CLASS] = { "fire", "spark", "person", "smoke", "flood" };
		for (int i = 0; i < MAX_CLASS; i++) {
			classes[i] = str[i];
		}

	}
	
public:
	DetectionInfo(class_t detectionClass=MAX_CLASS, int top=0, int left=0, int bottom=0, int right=0, float confidence=0.0, __int64 seq=-1, int robotNum=-1)
		//: rep.detectionClass(detectionClass), top(top), left(left), bottom(bottom), right(right), confidence(confidence), seq(seq), robotNum(robotNum)
	{
		rep.detectionClass = detectionClass;
		rep.top = top;
		rep.left = left;
		rep.bottom = bottom;
		rep.right = right;
		rep.confidence = confidence;
		rep.seq = seq;
		rep.robotNum = robotNum;

		InitClasses();

	}

	DetectionInfo(const rep_t & rep_) {
		int len = sizeof(rep_t);
		memcpy(&this->rep, &rep_, len);
		InitClasses();

	}

	/*
	DetectionInfo(rep_t * rep_) {
		int len = sizeof(rep_t);
		memcpy(&this->rep, (char *)rep_, len);
		InitClasses();

	}
	*/


	DetectionInfo(const std::string & info) {
		int temp;
		int ret = sscanf(info.c_str(), "%d %d %d %d %d %f %d %d", &temp, &rep.top, &rep.left, &rep.bottom, &rep.right, &rep.confidence, &rep.seq, &rep.robotNum);

		if (ret != 8) {


			THROW_EX(RobotException, "Error in parsing");
		}

		if(temp < 0 || temp > MAX_CLASS) {
			THROW_EX(RobotException, "Invalid class");
		}

		rep.detectionClass = (class_t) temp;
		InitClasses();

	}

	const rep_t& Rep() const {
		return rep;
	}

	const DetectionInfo & operator=(const DetectionInfo & info){
		memcpy(&this->rep, &info.rep, sizeof(rep_t));
		return *this;
	}

	const DetectionInfo& operator=(const DetectionInfo::rep_t& rep_) {
		memcpy(&this->rep, &rep_, sizeof(rep_t));
		return *this;
	}

	const rep_t *operator->() const {
		//rep_t *p = new rep_t;
		return &rep;
	}


	std::string ToString() const {
		char str[256];
		std::stringstream ss;
		ss << rep.detectionClass << " " << rep.top << " " << rep.left << " " << rep.bottom << " " << rep.right << " " << rep.confidence << " " << rep.seq << rep.robotNum;
		return std::string(ss.str());
	}

#ifdef _WIN32
	void Apply(cv::Mat & image);

#endif


};

}

#endif // !DETECTION_INFO_H
