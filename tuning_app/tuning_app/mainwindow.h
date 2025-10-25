#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include "design/rom_structures.h"
#include "communication/ros_bridge_client.hpp"


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


private:
    Ui::MainWindow *ui;

    QString RobotIp = "192.168.1.9";
    QString RobotPort = "9090";

    Mode currentMode;
    RosBridgeClient *communication_ = nullptr;

    // --- Window Drag Members ---
    QPoint m_dragPosition;
    bool m_isDragging = false;
    // ---------------------------
    
private slots:
    void onTabChanged(int index);
    void on_closeBtn_clicked();

protected:
    // --- Window Drag Functionality ---
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
};
#endif // MAINWINDOW_H
