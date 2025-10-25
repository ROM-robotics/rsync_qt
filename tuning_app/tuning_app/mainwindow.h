#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "design/rom_structures.h"
#include "communication/ros_bridge_client.hpp"

#include "design/qcgaugewidget.h"

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
    
private slots:
    void onTabChanged(int index);

    
};
#endif // MAINWINDOW_H
