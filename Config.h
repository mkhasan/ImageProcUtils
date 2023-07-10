/*
 * Config.h
 *
 *  Created on: Sep 8, 2020
 *      Author: hasan
 */

#ifndef IMAGEPROCUTILS_CONFIG_H_
#define IMAGEPROCUTILS_CONFIG_H_


#define CHECK_TIME_ 1

const unsigned char JPEG_QUALITY = 80;

const float FRAME_RATE = 20;

namespace image_proc_utils {
const int MAX_DETECTION_MSG_SIZE = (1024*4);
const int DETECTION_READER_CUSTOM_PORT = 9885;
const int NO_OF_ROBOTS = 1;


}



#endif /* IMAGEPROCUTILS_CONFIG_H_ */
