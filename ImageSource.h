#pragma once

#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include <string>
#include <vector>

class ImageSource {
	
protected:
	std::string root;

	ImageSource(const std::string & rootDirecotry="");
public:

	virtual void Reset() = 0;
	virtual void GetNext(std::vector<char> & image);
	virtual void GetNext(std::vector<char> & image, std::vector<char> & header) = 0;
	virtual ~ImageSource();
};
#endif // !IMAGE_SOURCE_H
