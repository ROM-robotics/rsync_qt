#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include "design/rom_structures.h"
#include "communication/ros_bridge_client.hpp"
#include <QString>
#include <QQuickWidget>

#include "design/rom_design.hpp"
#include "design/covarianceDisplay.hpp"

using namespace rom_dynamics::communication;
using namespace rom_dynamics::data_types;

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

    void createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port);

    void initRos2ControlTab();
    void activateRos2ControlTab();
    void deactivateRos2ControlTab();

    void initEkfTab();
    void activateEkfTab();
    void deactivateEkfTab();

    void initCartoTab();
    void activateCartoTab();
    void deactivateCartoTab();

    void initNav2_1Tab();
    void activateNav2_1Tab();
    void deactivateNav2_1Tab();

    void initNav2_2Tab();
    void activateNav2_2Tab();
    void deactivateNav2_2Tab();

    void initNav2_3Tab();
    void activateNav2_3Tab();
    void deactivateNav2_3Tab();

    void initBtTab();
    void activateBtTab();
    void deactivateBtTab();

    void initTopicTab();
    void activateTopicTab();
    void deactivateTopicTab();

    void initLogTab();
    void activateLogTab();
    void deactivateLogTab();

    void robotVelocityToWheelRpms(double linear_velocity, double angular_velocity, double wheel_radius, double wheel_seperation, int &left_rpm, int &right_rpm);

    double quaternionToYawDegrees(double &qx, double &qy, double &qz, double &qw);
    double yawDegreesToQuaternion(double &yaw_degrees, double &qx, double &qy, double &qz, double &qw);

signals:
    //void createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port);

private slots:
    void onTabChanged(int index);
    void on_closeBtn_clicked();
    void on_rsyncBtn_clicked();
    void on_connectBtn_clicked();
    void on_hostTerminalBtn_clicked();
    void on_robotTerminalBtn_clicked();
    void on_ekfTuningGuideBtn_clicked();

    // from web socket
    void onReceivedTopicMessage(const QString &topic, const QJsonObject &msg);


protected:
    // test();

private:
    Ui::MainWindow *ui;

    QString robotIp_         = "192.168.1.xx";
    QString robotPort_       = "9090";
    QString password_        = "ghostman";
    QString robotNamespace_  = "";
    bool isConnected_        = false;

    int previousCmdColorIndex = -1;

    Mode currentMode;
    RosBridgeClient *communication_ = nullptr;

    // QRC speed meter
    QVector<QQuickWidget*> ros2ControlQmlView_;
    QQuickItem *qmlRoot_ = nullptr;
    QTimer *qmlUpdateTimer_ = nullptr;
    QVector<QTimer*> speedTimeoutTimers_;

    // robotspecs
    double wheel_radius_ = 0.100; // meters
    double wheel_seperation_ = 0.400; // meters

    // ekf
    rom_dynamics::ui::qt::RomPolarHeadingGraph *odomDiffOdomImuHeadingGraphPtr_ = nullptr;
    rom_dynamics::ui::qt::RomPositionGraph *odomDiffOdomPositionGraphPtr_ = nullptr;
    rom_dynamics::ui::qt::RomPositionCovarianceGraph *ekfPositionCovarianceGraphPtr_ = nullptr;
    rom_dynamics::ui::qt::RomYawCovarianceGraph *ekfHeadingCovarianceGraphPtr_ = nullptr;

};
#endif // MAINWINDOW_H
