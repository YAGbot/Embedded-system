/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/wheelchair_bed_ui/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace wheelchair_bed_ui {

using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent)
	, qnode(argc,argv)
{
	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
  this->setWindowTitle(QString::fromLocal8Bit("HMI DESIGNED BY ZJ"));

//    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application


	setWindowIcon(QIcon(":/images/icon.png"));
	ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

	/*********************
	** Logging
	**********************/
  ui.view_logging_pub->setModel(qnode.loggingModel_pub());
    QObject::connect(&qnode, SIGNAL(loggingUpdated_pub(QString)), this, SLOT(updateLoggingView_pub(QString)));
    ui.view_logging_sub->setModel(qnode.loggingModel_sub());
   QObject::connect(&qnode, SIGNAL(loggingUpdated_sub(float,float)), this, SLOT(updateLoggingView_sub(float,float)));
// QObject::connect(&qnode, SIGNAL(loggingUpdated_sub(QString,float,float,float,double)), this, SLOT(updateLoggingView_sub(QString,float,float,float,double)));
   QObject::connect(ui.button_start, SIGNAL(clicked()), this, SLOT(start_cmd()));
   QObject::connect(ui.button_stop, SIGNAL(clicked()), this, SLOT(stop_cmd()));
   QObject::connect(ui.button_up, SIGNAL(clicked()), this, SLOT(up_cmd()));
   QObject::connect(ui.button_down, SIGNAL(clicked()), this, SLOT(down_cmd()));
   QObject::connect(ui.button_left, SIGNAL(clicked()), this, SLOT(left_cmd()));
   QObject::connect(ui.button_right, SIGNAL(clicked()), this, SLOT(right_cmd()));


}

MainWindow::~MainWindow() {}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
    close();
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */

void MainWindow::on_button_connect_clicked(bool check ) {

		if ( !qnode.init() ) {
			showNoMasterMessage();
		} else {
			ui.button_connect->setEnabled(false);
		}
}
void MainWindow::on_quit_button_clicked(bool check )
{
    close();
//    std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;

}

/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
void MainWindow::updateLoggingView_pub(QString data) {
  ui.l_data->setText(data);
  ui.view_logging_pub->scrollToBottom();

}

void MainWindow::updateLoggingView_sub(float temp, float humi)
{
//         ui.l_state->setText(state);
         ui.l_speed->setText(QString::number(temp));
         ui.l_steer_angle->setText(QString::number(humi));
//         ui.l_error1->setText(QString::number(error1));
//         ui.l_error2->setText(QString::number(error2));
         ui.view_logging_sub->scrollToBottom();
}
void MainWindow::start_cmd()
{
  qnode.start_cmd();
}
void MainWindow::stop_cmd()
{
  qnode.stop_cmd();
}
void MainWindow::up_cmd()
{
  qnode.up_cmd();
}
void MainWindow::down_cmd()
{
  qnode.down_cmd();
}
void MainWindow::left_cmd()
{
  qnode.left_cmd();
}
void MainWindow::right_cmd()
{
  qnode.right_cmd();
}
void MainWindow::closeEvent(QCloseEvent *event)
{

	QMainWindow::closeEvent(event);

}

}  // namespace wheelchair_bed_ui

