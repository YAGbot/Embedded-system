/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "../include/wheelchair_bed_ui/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace wheelchair_bed_ui {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"wheelchair_bed_ui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
  chatter_publisher = n.advertise<std_msgs::String>("wheelchair_bed_control", 1000);
  chatter_subscriber = n.subscribe("dht11_talker", 1000, &QNode::Callback, this);

	start();
	return true;
}
/*
bool QNode::init(const std::string &master_url, const std::string &host_url) {
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"wheelchair_bed_ui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.
	chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
	start();
	return true;
}
*/
void QNode::run() {
	ros::Rate loop_rate(1);
	int count = 0;
	while ( ros::ok() ) {

		std_msgs::String msg;
		std::stringstream ss;
		ss << "hello world " << count;
		msg.data = ss.str();
//		chatter_publisher.publish(msg);
//    log_pub(Info,std::string("I sent: ")+msg.data);
		ros::spinOnce();
		loop_rate.sleep();
		++count;
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::log_pub( const LogLevel &level, const std::string &msg) {
  logging_model_pub.insertRows(logging_model_pub.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
  logging_model_pub.setData(logging_model_pub.index(logging_model_pub.rowCount()-1),new_row);
  QString b_state(msg.c_str());//=QString::fromLocal8Bit("你好");
  Q_EMIT loggingUpdated_pub(b_state); // used to readjust the scrollbar
}
void QNode::log_sub(const float &temp, const float &humi)
{
  logging_model_sub.insertRows(logging_model_sub.rowCount(),1);
  std::stringstream logging_model_msg;
  //ROS_DEBUG_STREAM(data);

        logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << "temp:"<<temp<<"temp:"<<humi;


  QVariant new_row(QString(logging_model_msg.str().c_str()));
  logging_model_sub.setData(logging_model_sub.index(logging_model_sub.rowCount()-1),new_row);

 // QString b_state(state.c_str());//=QString::fromLocal8Bit("你好");
  Q_EMIT loggingUpdated_sub(temp,humi);

}
//add
void QNode::start_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "start";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}
//add
void QNode::stop_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "stop";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}

//add
void QNode::up_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "up";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}
//add
void QNode::down_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "down";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}
//add
void QNode::left_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "left";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}
//add
void QNode::right_cmd()
{
  if(ros::ok())
  {
    std_msgs::String msg;
    std::stringstream ss;
    ss << "right";
    msg.data = ss.str();
    chatter_publisher.publish(msg);
    log_pub(Info, std::string("I sent:"+msg.data));
    ros::spinOnce();
  }
}


void QNode::Callback(const wheelchair_bed_msgs::dht::ConstPtr &dht_data)
{

  log_sub(dht_data->temp,dht_data->humi);
}
}  // namespace wheelchair_bed_ui
