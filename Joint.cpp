//
// Created by trent on 5/2/15.
//

#include"gait.h"

const int DISTANCE_THESHOLD = 1000;

inline int dist(Point2f, Point2f);

class Joint
{
private:
    String name;
    Joint *connection;
    Point2f center;
    
public:
    Joint()
    { };

    Joint(String inName, Point2f initialLocation)
    {
        name = inName;
        connection = NULL;
        center = initialLocation;
    }

    void updateLocation(vector<Point2f> *points)
    {

        int minDistance = 999999;
        int index = -1;

        for (int i = 0; i < points->size(); i++)
        {
            Point2f point = (*points)[i];
            int distance = dist(point, center);

            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }

        if (index != -1)
        {
            center = (*points)[index];
            points->erase(points->begin() + index);
        }
        /*
        else
        {
            printf("Could not update %s joint \n", name.c_str());
            printf("Located at X:%f, Y:%f", center.x, center.y);
        }
         */
    }

    Point2f getLocation()
    {
        return center;
    }

    void setConnection(Joint *joint)
    {
        connection = joint;
    }

    Joint *getConnection()
    {
        return connection;
    }

    String getname()
    {
        return name;
    }

};

inline int dist(Point2f point1, Point2f point2)
{
    int dx = point2.x - point1.x;
    int dy = point2.y - point1.y;

    return (dx * dx + dy * dy);
}
