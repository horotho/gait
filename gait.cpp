#include "gait.h"
#include "Joint.cpp"

#define ESC_KEY (27)
#define dist(x, y)  ((x) * (x) + (y) * (y))

const char *filename = "gait.mpeg";
const char *windowName = "Gait Test";

#ifdef HSV_CAL
int lH = LOW_H, lS = LOW_S, lV = LOW_V, hH = HIGH_H, hS = HIGH_S, hV = HIGH_V;
#endif

static const String jointNames[] = {"Hip", "Thigh", "Knee", "Shin", "Ankle"};
int jointNumber = 0;

Vector<Joint> joints;

void callback(int event, int x, int y, int flags, void *userdata)
{
    vector<Rect> *objects = reinterpret_cast< vector<Rect> * >(userdata);
    if (event == EVENT_LBUTTONDOWN)
    {
        for (vector<Rect>::iterator it = objects->begin(); it != objects->end(); ++it)
        {
            printf("Point: (%d, %d) \n", it->x, it->y);
            if (it->contains(Point(x, y))) 
            {
                printf("Clicked inside an object. \n");


                if (jointNumber <= jointNames->size())
                {
                    Point2f center = Point2f(it->x + it->width/2, it->y + it->height/2);
                    Joint newJoint = Joint(jointNames[jointNumber], center);
                    joints.push_back(newJoint);

                    jointNumber++;
                    displayOverlay(windowName, "Please select the " + jointNames[jointNumber] + " Marker", 0);
                    break;
                }
                else
                {
                    displayOverlay(windowName, "Joints Selected", 3000);
                    break;
                }
            }
        }
    }
    else if (event == EVENT_RBUTTONDOWN)
    {
//        cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MBUTTONDOWN)
    {
//        cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    }
    else if (event == EVENT_MOUSEMOVE)
    {
//        cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

    }
}

int main(int argc, char **argv)
{
    namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    setMouseCallback(windowName, callback, NULL);
    displayOverlay(windowName, "Please select the " + jointNames[jointNumber] + " Marker", 0);

#ifdef HSV_CAL
    createTrackbar("H Low", windowName, &lH, 179);
    createTrackbar("H High", windowName, &hH, 179);
    createTrackbar("S Low", windowName, &lS, 255);
    createTrackbar("S High", windowName, &hS, 255);
    createTrackbar("V Low", windowName, &lV, 255);
    createTrackbar("V High", windowName, &hV, 255);

#endif

    // Open the input
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

    Mat frame, range, cont, hsv, output, canny;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<Point2f> mc;
    vector<Rect> objects;

    while (1)
    {
        if (!input.read(frame)) break;

        frame.copyTo(output);
        //frameCount++;

        // Convert to hsv and threshold with the given values
        cvtColor(frame, hsv, CV_BGR2HSV);

#ifdef HSV_CAL
        inRange(hsv, Scalar(lH, lS, lV), Scalar(hH, hS, hV), range);
#else
        inRange(hsv, Scalar(LOW_H, LOW_S, LOW_V), Scalar(HIGH_H, HIGH_S, HIGH_V), range);
#endif

        medianBlur(range, range, 9);
        Canny(range, canny, 50, 100, 3, true);
        canny.copyTo(cont);

        contours.clear();
        hierarchy.clear();
        mc.clear();
        objects.clear();

        findContours(cont, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_TC89_L1);
        unsigned long ct = contours.size();

        mc.reserve(ct);
        objects.reserve(ct);

        Moments m;
        Rect obj;
        for (int i = 0; i < ct; i++)
        {
            m = moments(contours[i], false);
            mc[i] = Point2f((float) (m.m10 / m.m00), (float) (m.m01 / m.m00));
            obj = Rect((int) mc[i].x - OBJECT_SIZE / 2, (int) mc[i].y - OBJECT_SIZE / 2, OBJECT_SIZE, OBJECT_SIZE);
            rectangle(output, obj, Scalar(0, 0, 255), 2);
            circle(output, mc[i], 2, Scalar(255, 255, 255));

            objects.push_back(obj);
        }

        setMouseCallback(windowName, callback, &objects);

        imshow(windowName, output);

        //vout.write(output);

        char c = (char) cvWaitKey(1);
        if (c == ESC_KEY) break;

    }

    input.release();
    output.release();

    return 0;
}
