
#include "ShmSource.h"
#include "ImageProcUtils.h"

#include <iostream>


using namespace std;
using namespace image_proc_utils;

ShmSource::ShmSource(const string & root) : ImageSource(), shm(make_unique<image_proc_utils::ShmData>(true)) {
	
}

ShmSource::~ShmSource() {

}

void ShmSource::GetNext(std::vector<char> & image, std::vector<char> & header) {
	shm->RemoveData(imgData);
	if (imgData.header.imageSize < 0) {
		image.resize(0);
		header.resize(0);
		cout << "No Image" << endl;
		return;
	}

	
	//cout << "Image size is " << imgData.header.imageSize << endl;
	image.resize(imgData.header.imageSize);
	copy(&imgData.buffer[0], (&imgData.buffer[0]) + image.size(), image.begin());

	header.resize(sizeof(imgData.header));
	char * p = (char *)&imgData.header;
	copy(p, p + header.size(), header.begin());

	//printf("header is is %d\n", header.size());
	return; 
}

void ShmSource::Reset() {
	shm->Reset();
}

