#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    currentMode = Mode::ssh;
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();

    initRos2ControlTab();
}

MainWindow::~MainWindow()
{
    delete ui;
}


// SLOT METHODS
void MainWindow::onTabChanged(int index)
{
    switch (index) 
    {
        case 0:
            currentMode = Mode::ssh;
            break;
        case 1:
            currentMode = Mode::ros2_control;
            break;
        case 2:
            currentMode = Mode::ekf;
            break;
        case 3:
            currentMode = Mode::nav2_1;
            break;
        case 4:
            currentMode = Mode::nav2_2;
            break;
        case 5:
            currentMode = Mode::nav2_3;
            break;
        case 6:
            currentMode = Mode::bt;
            break;
        case 7:
            currentMode = Mode::topic;
            break;
        case 8:
            currentMode = Mode::log;
            break;
        default:
            // Handle unexpected index
            break;
    }
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();
}



// OUR UI METHODS
void MainWindow::initRos2ControlTab()
{
    if (ui->ros2_control) 
    {
        cmd_vel_unstamped_ = new QcGaugeWidget(ui->ros2_control);
        cmd_vel_unstamped_->setMinimumSize(50, 50);
        cmd_vel_unstamped_->setRange(0, 70.0);
        cmd_vel_unstamped_->setValue(35);
        cmd_vel_unstamped_->setObjectName("cmdVelUnstamped");



        // Ensure the tab has a layout; create a horizontal layout if needed
        QLayout *existing = ui->ros2_control->layout();
        QHBoxLayout *layout = qobject_cast<QHBoxLayout*>(existing);
        if (!layout) {
            layout = new QHBoxLayout(ui->ros2_control);
            layout->setContentsMargins(8, 8, 8, 8);
        }


        layout->addWidget(cmd_vel_unstamped_);
    }
}
