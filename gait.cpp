#include "gait.h"
#include "Joint.cpp"
#include <memory>
#include  <cmath>
#include <fstream>

const char *windowName = "Gait Test";

// ifdef for Displaying the HSV Sliders
#ifdef HSV_CAL
int lH = LOW_H, lS = LOW_S, lV = LOW_V, hH = HIGH_H, hS = HIGH_S, hV = HIGH_V;
#endif

// Objects for holding the joints, names, and an index
const vector<String> jointNames{"Hip", "Thigh", "Knee", "Shin", "Ankle"};
int jointNumber;
float kneeAngle;
vector<shared_ptr<Joint>> joints;

int main(int argc, char **argv)
{
    // Create a window and set a mounse callback
    namedWindow(windowName, CV_WINDOW_AUTOSIZE);
    setMouseCallback(windowName, callback, NULL);

    // Displays the initial joint selection
    jointNumber = 0;
    displayOverlay(windowName, "Please select the " + jointNames[jointNumber] + " Marker", 0);


    // ifdef for Displaying the HSV Sliders
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

    // Sets the height and width of the video and the encoding codec
    int width = (int) input.get(CV_CAP_PROP_FRAME_WIDTH);
    int height = (int) input.get(CV_CAP_PROP_FRAME_HEIGHT);;
    int codec = CV_FOURCC('M', 'P', 'E', 'G');

    unsigned int frameCount = 0;
    //int fourcc = input.get(CV_CAP_PROP_FOURCC);
    //double fps = input.get(CV_CAP_PROP_FPS);

    // Setup output video
    VideoWriter vout("/tmp/gait6.mpeg", codec, 30, Size(width, height), true);

    // Checks the video stream is open
    if (!vout.isOpened())
    {
        cout << "Error opening output capture." << endl;
        return 0;
    }


    // Creates the output video
    ofstream fs;
    fs.open("/tmp/joint6.txt");

    if (!fs.is_open())
    {
        cout << "Error opening joint save file." << endl;
        return 0;
    }

    // Sets variables for holding frames, countours, and object centers
    Mat frame, range, cont, hsv, output, tmp;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    vector<Point2f> mc;
    vector<Rect> objects;
    setMouseCallback(windowName, callback, &objects);
    char buffer[50];

    while (1)
    {
        if (!input.read(frame)) break;

        frame.copyTo(output);
        frameCount++;

        if (jointNumber == jointNames.size())
        {
            for (int i = 0; i < joints.size(); i++)
            {
                Joint joint = *joints[i];
                Point p = joint.getLocation();
                fs << joint.getName() << "," << p.x << "," << p.y << endl;
            }

            fs << kneeAngle << endl;

            frameCount++;
        }

        // Convert to hsv and threshold with the given values
        cvtColor(frame, hsv, CV_BGR2HSV);

#ifdef HSV_CAL
        inRange(hsv, Scalar(lH, lS, lV), Scalar(hH, hS, hV), range);
#else
        inRange(hsv, Scalar(LOW_H, LOW_S, LOW_V), Scalar(HIGH_H, HIGH_S, HIGH_V), range);
#endif

        // Apply blur to get rid of noise
        medianBlur(range, range, 15);
        range.copyTo(cont);
        range.copyTo(tmp);

        // Clears the values
        contours.clear();
        hierarchy.clear();
        mc.clear();
        objects.clear();

        // Find the contours of the objects
        findContours(cont, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
        unsigned long ct = contours.size();

        // Allocates size of the vectors
        mc.reserve(ct);
        objects.reserve(ct);

        Moments m;
        Rect obj;

        // Find the center of each of the contours
        for (int i = 0; i < ct; i++)
        {
            m = moments(contours[i], true);
            mc.push_back(Point2f((float) (m.m10 / m.m00), (float) (m.m01 / m.m00)));

            circle(tmp, mc[i], 5, Scalar(0, 0, 255), -1);

            obj = boundingRect(contours[i]);
            rectangle(output, obj, Scalar(0, 0, 255), 2);
            objects.push_back(obj);
        }

        // Updates joint locations
        for (vector<shared_ptr<Joint>>::iterator it = joints.begin(); it != joints.end(); ++it)
        {
            shared_ptr<Joint> joint = (*it);

            // Draws cicle at joint center
            joint->updateLocation(&mc);
        }


        for (vector<shared_ptr<Joint>>::iterator it = joints.begin(); it != joints.end(); ++it)
        {
            shared_ptr<Joint> joint = (*it);

            circle(output, joint->getLocation(), 8, Scalar(0, 0, 255), -1);

            Joint *connection = joint->getConnection();

            // Draws lines between joints
            if (connection != NULL)
            {
                line(output, joint->getLocation(), joint->getConnection()->getLocation(), Scalar(255, 255, 255), 5, 4, 0);
            }
        }

        // Calculates the knee angle and puts it on the screen
        if (joints.size() >= jointNames.size())
        {
            Point2f hip = (*joints[0]).getLocation();
            Point2f knee = (*joints[2]).getLocation();
            Point2f ankle = (*joints[4]).getLocation();
            kneeAngle = cosAngle(hip, knee, ankle);

            sprintf(buffer, "%2.2f deg", kneeAngle);
            putText(output, buffer, Point2f(knee.x + 15, knee.y), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255), 2, 8, false);
        }

#ifdef HSV_CAL
        imshow(windowName, range);
#else
        imshow(windowName, output);
        vout.write(output);
#endif

        // Looks for the esc key
        char c = (char) cvWaitKey(1);
        if (c == ESC_KEY) break;

    }

    printf("Performing cleanup. \n");
    input.release();
    vout.release();
    fs.close();

    return 0;
}

void callback(int event, int x, int y, int flags, void *userdata)
{
    // Cast the vector back from a void pointer
    vector<Rect> *objects = reinterpret_cast< vector<Rect> * >(userdata);

    if (event == EVENT_LBUTTONDOWN)
    {
        for (vector<Rect>::iterator it = objects->begin(); it != objects->end(); ++it)
        {
            if (it->contains(Point(x, y)))
            {
                printf("Clicked inside an object. \n");

                if (jointNumber < jointNames.size())
                {
                    Point2f center = Point2f(it->x + it->width / 2, it->y + it->height / 2);
                    Joint *newJoint = new Joint(jointNames[jointNumber], center);

                    auto ptr = shared_ptr<Joint>(newJoint);
                    joints.push_back(ptr);

                    if (jointNumber != 0)
                    {
                        (*joints[jointNumber - 1]).setConnection(newJoint);
                    }

                    jointNumber++;
                    if (jointNumber < jointNames.size())
                        displayOverlay(windowName, "Please select the " + jointNames[jointNumber] + " Marker", 0);
                    else
                        displayOverlay(windowName, "Joints Selected", 3000);


                    break;
                }
            }
        }
    }
}

float cosAngle(Point2f hip, Point2f knee, Point2f ankle)
{
    float a = norm(Mat(knee), Mat(ankle));
    float b = norm(Mat(hip), Mat(ankle));
    float c = norm(Mat(hip), Mat(knee));

    float cosB = ((a * a) - (b * b) + (c * c)) / (2 * a * c);

    return acos(cosB) * (180 / PI);
}
