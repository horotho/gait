#include <stdio.h>
#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "gait.h"

#define ESC_KEY (27)
#define dist(x, y)  ((x) * (x) + (y) * (y))

using namespace cv;
using namespace std;

const char *filename = "gait.mpeg";
const char *windowName = "Gait Test";

//int lH, lS, lV, hH, hS, hV;

void callback(int event, int x, int y, int flags, void *userdata)
{
    if (event == EVENT_LBUTTONDOWN)
    {
        printf("Left button of the mouse clicked at position (%d, %d) \n", x, y);
    }
    else if (event == EVENT_RBUTTONDOWN)
    {
        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MBUTTONDOWN)
    {
        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MOUSEMOVE)
    {
        //cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
    }
}

int main(int argc, char **argv)
{
    namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    setMouseCallback(windowName, callback, NULL);
    displayOverlay(windowName, "Please select the Left Hip Marker", 0);

//    createTrackbar("H Low", windowName, &lH, 360);
//    createTrackbar("H High", windowName, &hH, 360);
//    createTrackbar("S Low", windowName, &lS, 255);
//    createTrackbar("S High", windowName, &hS, 255);
//    createTrackbar("V Low", windowName, &lV, 255);
//    createTrackbar("V High", windowName, &hV, 255);

    // Load input video
    VideoCapture input(0);
    if (!input.isOpened())
    {
        cout << "Error opening input capture." << endl;
        return 0;
    }

    int width = (int) input.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = (int) input.get(CV_CAP_PROP_FRAME_HEIGHT);
    int codec = CV_FOURCC('M', 'P', 'E', 'G');

    int frameCount = 0;
    //int fourcc = input.get(CV_CAP_PROP_FOURCC);
    //double fps = input.get(CV_CAP_PROP_FPS);

    // Setup output video
    VideoWriter vout(filename, codec, 30, Size(width, height), true);

    if (!vout.isOpened())
    {
        cout << "Error opening output capture." << endl;
        return 0;
    }

    Mat frame, range, cont, hsv, output;

    while (1)
    {
        if (!input.read(frame)) break;

        frame.copyTo(output);
        //frameCount++;

        // Convert to hsv and threshold with the given values
        cvtColor(frame, hsv, CV_BGR2HSV);
//        inRange(hsv, Scalar(lH, lS, lV), Scalar(hH, hS, hV), range);
        inRange(hsv, Scalar(LOW_H, LOW_S, LOW_V), Scalar(HIGH_H, HIGH_S, HIGH_V), range);



        Mat s = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

        //morphological opening (remove small objects from the foreground)
        erode(range, range, s);
        dilate(range, range, s);

        //morphological closing (fill small holes in the foreground)
        dilate(range, range, s);
        erode(range, range, s);

        medianBlur(range, cont, 9);

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        findContours(cont, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);
        unsigned long ct = contours.size();

        vector<Moments> mu(ct);
        vector<Point2f> mc(ct);

        int tx = 0, ty = 0;
        for (int i = 0; i < ct; i++)
        {
            mu[i] = moments(contours[i], false);
            mc[i] = Point2f((float) (mu[i].m10 / mu[i].m00), (float) (mu[i].m01 / mu[i].m00));

            tx += mc[i].x;
            ty += mc[i].y;
        }

        if (ct != 0)
        {
            tx /= ct;
            ty /= ct;
        }

        float md;
        float dy;
        float dx;
        float cx = 0, cy = 0;
        float mx, my;

        md = 1000000;
        for (int i = 0; i < ct; i++)
        {
            if (mc[i].y < ty)
            {
                dx = mc[i].x - tx;
                dy = mc[i].y - ty;

                if ((dx * dx + dy * dy) < md)
                {
                    md = (dx * dx + dy * dy);
                    cx = mc[i].x;
                    cy = mc[i].y;
                }
            }
        }

//        circle(output, Point2f(cx, cy), 6, Scalar(0, 0, 255), -1);

        for (int i = 0; i < ct; i++)
        {
            rectangle(output, Point2f(mc[i].x - 10, mc[i].y - 10), Point2f(mc[i].x + 10, mc[i].y + 10), Scalar(0, 0, 255), 2);

////            circle(output, mc[i], 3, Scalar(255, 255, 255), -1);
//            md = 1000000;
//            mx = 0;
//            my = 0;
//
//            for (int j = 0; j < ct; j++)
//            {
//                // If we're not looking at the same point, as the distance
//                // would be 0
//                if (j != i)
//                {
//                    if (mc[j].x == cx && mc[j].y == cy) continue;
//
//                    dx = mc[j].x - mc[i].x;
//                    dy = mc[j].y - mc[i].y;
//
//                    if (dist(dx, dy) < md)
//                    {
//                        md = dist(dx, dy);
//                        mx = mc[j].x;
//                        my = mc[j].y;
//                    }
//                }
//            }

//            printf("Drawing from (%.0f, %.0f) to (%.0f, %.0f) \n", mc[i].x, mc[i].y, mx, my);

//            if (mx > 0 && my > 0 && mc[i].x > 0 && mc[i].y > 0)
//                line(output, mc[i], Point2f(mx, my), Scalar(255, 255, 255), 2);
        }

//        printf("Found %lu objects. \n", ct);


        //int num = 0, mi = 0;
        //mx = my = 0;
        //while(num < 4 && mc.size() > 0)
        //{
        //md = 100000;
        //mi = 0;
        //for(int i = 0; i < mc.size(); i++)
        //{
        //if(mc[i].x == cx && mc[i].y == cy) continue;

        //dx = cx - mc[i].x;
        //dy = cy - mc[i].y;

        //if(dist(dx, dy) < md)
        //{
        //md = dist(dx, dy);
        //mx = mc[i].x;
        //my = mc[i].y;
        //mi = i;
        //}
        //}

        //if(cx > 0 && cy > 0 && mx > 0 && my > 0)
        //{
        //line(output, Point2f(cx, cy), Point2f(mx, my), Scalar(255, 255, 255), 2);
        //}

        //mc.erase(mc.begin() + mi);
        //num++;
        //}

        imshow(windowName, output);

        //vout.write(output);

        char c = (char) cvWaitKey(1);
        if (c == ESC_KEY) break;

    }

    input.release();
    output.release();

    return 0;
}
