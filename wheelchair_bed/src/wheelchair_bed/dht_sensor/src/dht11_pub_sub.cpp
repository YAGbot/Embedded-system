
#include "ros/ros.h"
#include "std_msgs/Bool.h"
//这个是我的包msg文件
#include "wheelchair_bed_msgs/dht.h"
#include "std_msgs/String.h"
#include <sstream>

using namespace std;

wheelchair_bed_msgs::dht dhtmsg;
std_msgs::String msg;

void object_detectedCallback(const std_msgs::String::ConstPtr& msg)
{
//此处代码忽略
    ROS_INFO("[%s]",msg->data.c_str());
}
 
int main(int argc,char **argv)
{
	ros::init(argc,argv,"sub_and_pub");
    ros::NodeHandle n;

	ros::Subscriber sub = n.subscribe("dht11_listener",1,object_detectedCallback);
    ros::Publisher identify_pub = n.advertise<wheelchair_bed_msgs::dht>("dht11_talker",1000);//锁定helloworld这个topic准备发布消息
    ros::Rate loop_rate(10);
    while(ros::ok())
    {
        dhtmsg.temp = 24.0;
        dhtmsg.humi = 24;
//        dhtmsg.stamp = ros::Time::now();
        ROS_INFO("%f", dhtmsg.temp);
        identify_pub.publish(dhtmsg);//向刚才锁定的helloworld这个topic发布消息
        ros::spinOnce();//这句就是同时发布节点和订阅节点的关键了
        loop_rate.sleep();
    }
	return 0;	
}
