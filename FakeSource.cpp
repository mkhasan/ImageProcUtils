#include "FakeSource.h"
#include "ImageProcUtils.h"

#include <fstream>
#include <iostream>

using namespace std;
using namespace image_proc_utils;

FakeSource::FakeSource(const string & root) : ImageSource(root), index(0) {

	data = new char[ImgData::GetBufferSize()];

}

FakeSource::~FakeSource() {

	cout << "In FakeSource Destructor" << endl;
	delete[] data;
}

void FakeSource::Reset() {
	index = 0;
}

void FakeSource::GetNext(vector<char>& image, vector<char>& header) {
	char filename[255];

	sprintf(filename, "%s%06d.jpg", root.c_str(), index);

	int len;
	get_data(data, len, filename);
	if (len < 0) {
		THROW_EX(RobotException, "Error in getting data !!!");
	}

	image.resize(len);
	copy(data, data + len, image.begin());


	ImgData::image_header h;
	
	h.nRobotNum = 0;
	h.seq = index;
	h.quality = 0x5e;

	header.resize(sizeof(h));
	char *p = (char *)&h;

	copy(p, p + header.size(), header.begin());
	//copy((char *)&h, sizeof(h), header.begin());
	
	index = (index + 1) % 200;

}

int FakeSource::get_filesize(string filename) {
	streampos begin, end;
	ifstream myfile(filename, ios::binary);
	begin = myfile.tellg();
	myfile.seekg(0, ios::end);
	end = myfile.tellg();
	myfile.close();
	return end - begin;
}

void FakeSource::get_data(char * data, int & len, string filename) {


	len = -1;

	ifstream myfile(filename, ios::binary);
	if (myfile.is_open()) {
		int size = get_filesize(filename); // myfile.tellg();
		//assert(size <= ImgData::GetBufferSize());
		myfile.seekg(0, ios::beg);
		myfile.read(data, size);
		len = size;

	}

}
