/**
 * @file /include/wheelchair_bed_ui/main_window.hpp
 *
 * @brief Qt based gui for wheelchair_bed_ui.
 *
 * @date November 2010
 **/
#ifndef wheelchair_bed_ui_MAIN_WINDOW_H
#define wheelchair_bed_ui_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace wheelchair_bed_ui {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/

	void on_button_connect_clicked(bool check );
  void on_quit_button_clicked(bool check );

    /******************************************
    ** Manual connections
    *******************************************/
    void updateLoggingView_pub(QString data); // no idea why this can't connect automatically
//    void updateLoggingView_sub(QString state,float speed, float steer_angle,float error1, double error2); //add
    void updateLoggingView_sub(float temp, float humi); //add

    void start_cmd(); //add
    void stop_cmd(); //add
    void up_cmd(); //add
    void down_cmd(); //add
    void left_cmd(); //add
    void right_cmd(); //add
private:
	Ui::MainWindowDesign ui;
	QNode qnode;
};

}  // namespace wheelchair_bed_ui

#endif // wheelchair_bed_ui_MAIN_WINDOW_H
