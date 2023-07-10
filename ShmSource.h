#pragma once

#ifndef SHM_SOURCE_H
#define SHM_SOURCE_H


#include "ImageSource.h"
#include "ImageProcUtils.h"

#include <string>

class ShmSource : public ImageSource {

	std::unique_ptr<image_proc_utils::ShmData> shm;
	image_proc_utils::ImgData imgData;
public:

	ShmSource(const std::string & root);		// in order to make it compatible with fake source
	~ShmSource();

	void Reset() override;
	void GetNext(std::vector<char> & image, std::vector<char> & header) override;

private:
	

};

#endif // !SHM_SOURCE_H
