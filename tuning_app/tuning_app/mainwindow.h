#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "design/rom_structures.h"
#include "communication/ros_bridge_client.hpp"

#include "design/qcgaugewidget.h"

#include <QTimer>

class QQuickWidget;
class QQuickItem;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initRos2ControlTab();

private:
    Ui::MainWindow *ui;

    Mode currentMode;
    RosBridgeClient *communication_ = nullptr;

    QcGaugeWidget *cmd_vel_unstamped_;

    QcGaugeWidget *reserve1_;

    QcGaugeWidget *reserve2_;
    
    // QML view and root object for ros2_control tab
    QQuickWidget *qmlView_ = nullptr;
    QQuickItem *qmlRoot_ = nullptr;
    QTimer *qmlUpdateTimer_ = nullptr;
    bool useDemo_ = false; // when true, run demo timer; otherwise use live ROS data

private slots:
    void updateQmlSpeed();
    void onRosTopicMessage(const QString &topic, const QJsonObject &msg);
    void onRosConnected();
    void onTabChanged(int index);

    
};
#endif // MAINWINDOW_H
