//
// Created by aaron on 5/2/15.
//

#include <stdio.h>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

#ifndef GAIT_GAIT_H
#define GAIT_GAIT_H

#include <stdio.h>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define ESC_KEY (27)
#define OBJECT_SIZE 25
//#define HSV_CAL
//#define SHITTY_CAMERA

#ifndef SHITTY_CAMERA
#define LOW_H 56
#define LOW_S 82
#define LOW_V 49
#define HIGH_H 86
#define HIGH_S 146
#define HIGH_V 255
#else
#define LOW_H 33
#define LOW_S 71
#define LOW_V 0
#define HIGH_H 93
#define HIGH_S 255
#define HIGH_V 255
#endif


#endif //GAIT_GAIT_H
