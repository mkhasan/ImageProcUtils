
#include "DetectionInfo.h"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

void image_proc_utils::DetectionInfo::Apply(Mat & image) {
	int left = rep.left;
	int top = rep.top;
	int right = rep.right;
	int bottom = rep.bottom;

	rectangle(image, Point(left, top), Point(right, bottom), Scalar(255, 255, 255), 5);
	string label = format("%.2f", this->rep.confidence);
	
	if (this->rep.detectionClass >= (int)MAX_CLASS) {
		THROW_EX(RobotException, "Invalid detection");
	}

	label = classes[rep.detectionClass] + ":" + label;

	int baseLine;
	Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 1, 1, &baseLine);
	auto labelTop = max(top, labelSize.height);
	putText(image, label, Point(left, labelTop-10), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2);


}