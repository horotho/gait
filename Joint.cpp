//
// Created by trent on 5/2/15.
//

#include"gait.h"

const int DISTANCE_THESHOLD = 50;

inline int dist(Point2f, Point2f);

class Joint
{
    private:
        String name;
        Joint* connection;
        Point2f center;


    public:
        Joint(){};

        Joint(String inName, Point2f initialLocation)
        {
            name = inName;
            connection = NULL;
            center = initialLocation;
        }

        void updateLocation(Vector<Point2f> joints)
        {

            int minDistance = 999999;
            int index = -1;

            for(int i = 0; i < joints.size(); i++)
            {
                Point2f point = joints[i];
                int distance = dist(point, center);

                if(distance < minDistance && distance < DISTANCE_THESHOLD)
                {
                    minDistance = distance;
                    index = i;
                }
            }

            if(index != -1)
            {
                center = joints[index];
            }
        }

        Point2f getLocation()
        {
            return center;
        }

        void setConnection(Joint* joint)
        {
            connection =  joint;
        }

        Joint * getConnection()
        {
            return connection;
        }

};

inline int dist(Point2f point1, Point2f point2)
{
    int dx = point2.x - point1.x;
    int dy = point2.y - point1.y;

    return (dx*dx + dy*dy);
}
