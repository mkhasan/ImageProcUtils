
#include "ImageSource.h"
#include "ImageProcUtils.h"

using namespace std;
using namespace image_proc_utils;

ImageSource::ImageSource(const string & root) : root(root) {

}

void ImageSource::GetNext(vector<char> & image) {
	vector<char> h;
	GetNext(image, h);
}

ImageSource::~ImageSource() {

}
