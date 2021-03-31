#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

using namespace std;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    
    // Request service with velocities
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    // Call the DriveToTarget service and passing the command velocities
    if (!client.call(srv)) {
	    ROS_ERROR("Failed to call service /ball_chaser/command_robot/.");
	}
}


void process_image_callback(const sensor_msgs::Image img)
{
    int white_pixel = 255;
    int colm_index = 0;
    bool ball_seen = false;
    
    for (int i=0; i < img.height * img.step; i += 3)
    {
        if ((img.data[i] == white_pixel) && (img.data[i+1] == white_pixel) && (img.data[i+2] == white_pixel))
        {
            colm_index = i % img.step;
 
            // Moving the ball left
            if (colm_index < img.step/3)
                drive_robot(0.5, 1);
            // Moving the ball forward
            else if (colm_index < (img.step/3 * 2))
                drive_robot(0.5, 0); 
            // Moving the ball right
            else
                drive_robot(0.5, -1);
            ball_seen = true;
            break;
         }
     }

     if (ball_seen == false)
         drive_robot(0, 0);             
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subsribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
