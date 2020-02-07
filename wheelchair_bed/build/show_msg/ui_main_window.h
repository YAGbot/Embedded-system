/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowDesign
{
public:
    QAction *action_Quit;
    QAction *action_Preferences;
    QAction *actionAbout;
    QAction *actionAbout_Qt;
    QWidget *centralwidget;
    QWidget *tab_manager;
    QGroupBox *groupBox_12;
    QListView *view_logging_sub;
    QLabel *label_4;
    QPushButton *start;
    QPushButton *quit;
    QLineEdit *l_speed;
    QLineEdit *l_steer_angle;
    QLineEdit *l_error1;
    QLineEdit *l_error2;
    QLineEdit *l_state;
    QLabel *subscribe_3;
    QLabel *subscribe_4;
    QLabel *subscribe_5;
    QLabel *subscribe_6;
    QLabel *subscribe_2;
    QLabel *subscribe_7;
    QLabel *subscribe_8;
    QFrame *frame;

    void setupUi(QMainWindow *MainWindowDesign)
    {
        if (MainWindowDesign->objectName().isEmpty())
            MainWindowDesign->setObjectName(QString::fromUtf8("MainWindowDesign"));
        MainWindowDesign->resize(944, 704);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindowDesign->setWindowIcon(icon);
        MainWindowDesign->setLocale(QLocale(QLocale::English, QLocale::Australia));
        action_Quit = new QAction(MainWindowDesign);
        action_Quit->setObjectName(QString::fromUtf8("action_Quit"));
        action_Quit->setShortcutContext(Qt::ApplicationShortcut);
        action_Preferences = new QAction(MainWindowDesign);
        action_Preferences->setObjectName(QString::fromUtf8("action_Preferences"));
        actionAbout = new QAction(MainWindowDesign);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout_Qt = new QAction(MainWindowDesign);
        actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
        centralwidget = new QWidget(MainWindowDesign);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tab_manager = new QWidget(centralwidget);
        tab_manager->setObjectName(QString::fromUtf8("tab_manager"));
        tab_manager->setGeometry(QRect(-10, 0, 926, 686));
        tab_manager->setMinimumSize(QSize(100, 0));
        tab_manager->setLocale(QLocale(QLocale::English, QLocale::Australia));
        groupBox_12 = new QGroupBox(tab_manager);
        groupBox_12->setObjectName(QString::fromUtf8("groupBox_12"));
        groupBox_12->setGeometry(QRect(90, 40, 481, 451));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_12->sizePolicy().hasHeightForWidth());
        groupBox_12->setSizePolicy(sizePolicy);
        groupBox_12->setStyleSheet(QString::fromUtf8("\n"
"QGroupBox{\n"
"	\n"
"border-width:1px;\n"
"border-style:solid;\n"
"border-color:black;\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"QGroupBox::title{\n"
"subcontrol-origin:margin;\n"
"subcontrol-position:to left;\n"
"left:10px;\n"
"margin-left:0px;\n"
"padding: 0 1px;\n"
"}"));
        view_logging_sub = new QListView(groupBox_12);
        view_logging_sub->setObjectName(QString::fromUtf8("view_logging_sub"));
        view_logging_sub->setGeometry(QRect(30, 240, 425, 177));
        view_logging_sub->setStyleSheet(QString::fromUtf8(""));
        label_4 = new QLabel(groupBox_12);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(110, 50, 81, 20));
        start = new QPushButton(groupBox_12);
        start->setObjectName(QString::fromUtf8("start"));
        start->setGeometry(QRect(370, 40, 80, 22));
        quit = new QPushButton(groupBox_12);
        quit->setObjectName(QString::fromUtf8("quit"));
        quit->setGeometry(QRect(370, 70, 80, 22));
        l_speed = new QLineEdit(groupBox_12);
        l_speed->setObjectName(QString::fromUtf8("l_speed"));
        l_speed->setGeometry(QRect(120, 120, 101, 22));
        l_steer_angle = new QLineEdit(groupBox_12);
        l_steer_angle->setObjectName(QString::fromUtf8("l_steer_angle"));
        l_steer_angle->setGeometry(QRect(120, 160, 101, 22));
        l_error1 = new QLineEdit(groupBox_12);
        l_error1->setObjectName(QString::fromUtf8("l_error1"));
        l_error1->setGeometry(QRect(330, 120, 125, 22));
        l_error2 = new QLineEdit(groupBox_12);
        l_error2->setObjectName(QString::fromUtf8("l_error2"));
        l_error2->setGeometry(QRect(330, 160, 125, 22));
        l_state = new QLineEdit(groupBox_12);
        l_state->setObjectName(QString::fromUtf8("l_state"));
        l_state->setGeometry(QRect(220, 50, 101, 22));
        subscribe_3 = new QLabel(groupBox_12);
        subscribe_3->setObjectName(QString::fromUtf8("subscribe_3"));
        subscribe_3->setGeometry(QRect(190, 220, 121, 16));
        subscribe_4 = new QLabel(groupBox_12);
        subscribe_4->setObjectName(QString::fromUtf8("subscribe_4"));
        subscribe_4->setGeometry(QRect(280, 120, 44, 22));
        subscribe_5 = new QLabel(groupBox_12);
        subscribe_5->setObjectName(QString::fromUtf8("subscribe_5"));
        subscribe_5->setGeometry(QRect(280, 160, 44, 22));
        subscribe_6 = new QLabel(groupBox_12);
        subscribe_6->setObjectName(QString::fromUtf8("subscribe_6"));
        subscribe_6->setGeometry(QRect(20, 160, 101, 22));
        subscribe_2 = new QLabel(groupBox_12);
        subscribe_2->setObjectName(QString::fromUtf8("subscribe_2"));
        subscribe_2->setGeometry(QRect(22, 120, 91, 22));
        subscribe_7 = new QLabel(groupBox_12);
        subscribe_7->setObjectName(QString::fromUtf8("subscribe_7"));
        subscribe_7->setGeometry(QRect(230, 120, 44, 22));
        subscribe_8 = new QLabel(groupBox_12);
        subscribe_8->setObjectName(QString::fromUtf8("subscribe_8"));
        subscribe_8->setGeometry(QRect(230, 160, 44, 22));
        frame = new QFrame(groupBox_12);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(40, 40, 31, 21));
        frame->setStyleSheet(QString::fromUtf8("image: url(:/images/bit.jpeg);"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        view_logging_sub->raise();
        label_4->raise();
        start->raise();
        quit->raise();
        l_state->raise();
        subscribe_3->raise();
        l_error2->raise();
        subscribe_6->raise();
        subscribe_5->raise();
        subscribe_2->raise();
        l_error1->raise();
        subscribe_4->raise();
        subscribe_7->raise();
        subscribe_8->raise();
        frame->raise();
        l_steer_angle->raise();
        l_speed->raise();
        MainWindowDesign->setCentralWidget(centralwidget);

        retranslateUi(MainWindowDesign);
        QObject::connect(action_Quit, SIGNAL(triggered()), MainWindowDesign, SLOT(close()));

        QMetaObject::connectSlotsByName(MainWindowDesign);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowDesign)
    {
        MainWindowDesign->setWindowTitle(QApplication::translate("MainWindowDesign", "QRosApp", 0, QApplication::UnicodeUTF8));
        action_Quit->setText(QApplication::translate("MainWindowDesign", "&Quit", 0, QApplication::UnicodeUTF8));
        action_Quit->setShortcut(QApplication::translate("MainWindowDesign", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        action_Preferences->setText(QApplication::translate("MainWindowDesign", "&Preferences", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("MainWindowDesign", "&About", 0, QApplication::UnicodeUTF8));
        actionAbout_Qt->setText(QApplication::translate("MainWindowDesign", "About &Qt", 0, QApplication::UnicodeUTF8));
        groupBox_12->setTitle(QApplication::translate("MainWindowDesign", "UI DESIGN", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MainWindowDesign", "Vehicle State:", 0, QApplication::UnicodeUTF8));
        start->setText(QApplication::translate("MainWindowDesign", "Start", 0, QApplication::UnicodeUTF8));
        quit->setText(QApplication::translate("MainWindowDesign", "Quit", 0, QApplication::UnicodeUTF8));
        subscribe_3->setText(QApplication::translate("MainWindowDesign", "Update data\357\274\232", 0, QApplication::UnicodeUTF8));
        subscribe_4->setText(QApplication::translate("MainWindowDesign", "Error1\357\274\232", 0, QApplication::UnicodeUTF8));
        subscribe_5->setText(QApplication::translate("MainWindowDesign", "Error2\357\274\232", 0, QApplication::UnicodeUTF8));
        subscribe_6->setText(QApplication::translate("MainWindowDesign", "Steering angle:", 0, QApplication::UnicodeUTF8));
        subscribe_2->setText(QApplication::translate("MainWindowDesign", "       Speed:", 0, QApplication::UnicodeUTF8));
        subscribe_7->setText(QApplication::translate("MainWindowDesign", "km/h", 0, QApplication::UnicodeUTF8));
        subscribe_8->setText(QApplication::translate("MainWindowDesign", "\345\272\246", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindowDesign: public Ui_MainWindowDesign {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
