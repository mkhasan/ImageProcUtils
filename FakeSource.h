#pragma once

#ifndef FAKE_SOURCE_H
#define FAKE_SOURCE_H

#include "ImageSource.h"

class FakeSource : public ImageSource {

	int index;
	char * data;
public:

	FakeSource(const std::string & root="");
	~FakeSource();
	
	void Reset() override;
	void GetNext(std::vector<char> & image, std::vector<char> & header) override;

private:
	int get_filesize(std::string filename);
	void get_data(char * data, int & len, std::string filename);
	
};

#endif // !FAKE_SOURCE_H