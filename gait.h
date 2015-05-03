//
// Created by aaron on 5/2/15.
//

#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>

using namespace cv;
using namespace std;

#ifndef GAIT_GAIT_H
#define GAIT_GAIT_H

#define ESC_KEY     27
#define PI 3.14159235
//#define HSV_CAL
//#define SHITTY_CAMERA

#ifdef SHITTY_CAMERA
#define LOW_H 30
#define LOW_S 82
#define LOW_V 49
#define HIGH_H 106
#define HIGH_S 180
#define HIGH_V 255
#else
#define LOW_H 36
#define LOW_S 45
#define LOW_V 59
#define HIGH_H 86
#define HIGH_S 157
#define HIGH_V 255
#endif

void callback(int event, int x, int y, int flags, void *userdata);
float cosAngle(Point2f hip, Point2f knee, Point2f ankle);


#endif //GAIT_GAIT_H
