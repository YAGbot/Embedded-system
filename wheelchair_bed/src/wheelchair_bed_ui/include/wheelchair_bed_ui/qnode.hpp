/**
 * @file /include/wheelchair_bed_ui/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef wheelchair_bed_ui_QNODE_HPP_
#define wheelchair_bed_ui_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include<sstream>
#include<vector>
#include <QStringListModel>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include "show_msg/info.h"
#include "wheelchair_bed_msgs/dht.h"


/*****************************************************************************
** Namespaces
*****************************************************************************/
using namespace std;
namespace wheelchair_bed_ui {

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();

	void run();
  void up_cmd();  //add
  void down_cmd();  //add
  void left_cmd();  //add
  void right_cmd(); //add
  void start_cmd();  //add
  void stop_cmd(); //add
	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

  QStringListModel* loggingModel_pub() { return &logging_model_pub; }
  void log_pub( const LogLevel &level, const std::string &msg);

  QStringListModel* loggingModel_sub() { return &logging_model_sub; }
//  void log_sub( const string &state,const float &speed, const float &steer_angle,const float &error1, const double &error2);
//  void Callback(const show_msg::info::ConstPtr &data);
  void log_sub(const float &temp, const float &humi);
  void Callback(const wheelchair_bed_msgs::dht::ConstPtr &dht_data);
Q_SIGNALS:
  void loggingUpdated_pub(QString);
    void rosShutdown();
//  void loggingUpdated_sub(QString,float,float,float,double);  //
  void loggingUpdated_sub(float,float);  //
private:
	int init_argc;
	char** init_argv;
	ros::Publisher chatter_publisher;
  QStringListModel logging_model_pub;
  ros::Subscriber chatter_subscriber; //add

  QStringListModel logging_model_sub; //add
};

}  // namespace wheelchair_bed_ui

#endif /* wheelchair_bed_ui_QNODE_HPP_ */
