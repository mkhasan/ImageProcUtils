
#include "DelayChecker.h"


#include <iostream>
#include <fstream>
#include <string>


using namespace std;

image_proc_utils::DelayChecker::DelayChecker(const string & filename, int infoSize, int startIndex)
	: filename(filename)
	, infoSize(infoSize)
	, startIndex(startIndex)
{
	infoList.resize(infoSize);
	for (int i = 0; i < infoSize; i++) {
		infoList[i].send_time[0] = infoList[i].arrival_time[0] = 0;
	}
}

image_proc_utils::DelayChecker::~DelayChecker() {
	WriteIntoFile();
}

void image_proc_utils::DelayChecker::UpdateSendTime(int seq) {
	if (seq >= startIndex && seq < startIndex + infoSize) {
		print_time(infoList[seq - startIndex].send_time);
	}
}

void image_proc_utils::DelayChecker::UpdateArrivalTime(int seq) {
	if (seq >= startIndex && seq < startIndex + infoSize) {
		print_time(infoList[seq - startIndex].arrival_time);
	}
}

void image_proc_utils::DelayChecker::WriteIntoFile() {
	ofstream myfile;
	myfile.open(filename);
	cout << "going to writing in file: " << filename << endl;
	if (myfile.is_open()) {
		cout << "writing to file: " << filename << endl;
		for (int i = 0; i < infoSize; i++) {

			char str[1024];
			sprintf(str, "seq %5d: send time: %s arrival time: %s", i+startIndex, infoList[i].send_time, infoList[i].arrival_time);
			myfile << str << endl;
			
		}
		myfile.flush();
		cout << "done \n";
	}
	else {
		cout << "Errro in opening file for writing" << endl;
	}

	myfile.close();
}

void image_proc_utils::DelayChecker::Print() {

	for (int i = 0; i < infoSize; i++) {

		char str[1024];
		sprintf(str, "seq %5d: send time: %s arrival time: %s", i + startIndex, infoList[i].send_time, infoList[i].arrival_time);
		cout << str << endl;
		//string filename = format("%s/%06d.jpg", src_dir.c_str(), info.seq);
	}

}