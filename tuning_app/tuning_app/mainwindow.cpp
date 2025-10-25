#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QTabBar>
#include <QtGui/QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
        this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
        //this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    
    if (ui->tabWidget && ui->tabWidget->tabBar()) 
    {
        int tabCount = ui->tabWidget->count();
        if (tabCount > 0) 
        {
            int totalW = ui->tabWidget->width() - 30;
            int perTabW = qMax(1, totalW / tabCount);
            QString tabStyle = QString("QTabBar::tab { min-width: %1px; max-width: %1px; }").arg(perTabW);
            ui->tabWidget->tabBar()->setStyleSheet(tabStyle);
        }
    }

    // initialize communication client
    communication_ = new RosBridgeClient("", RobotIp, RobotPort.toInt(), this);

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


    switch (currentMode) 
    {
        case Mode::ssh:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::ros2_control:
            activateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            qDebug() << " activateRos2ControlTab called  ";
            break;
        case Mode::ekf:
            deactivateRos2ControlTab();
            activateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::nav2_1:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            activateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::nav2_2:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            activateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::nav2_3:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            activateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::bt:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            activateBtTab();
            deactivateTopicTab();
            deactivateLogTab();
            break;
        case Mode::topic:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            activateTopicTab();
            deactivateLogTab();
            break;
        case Mode::log:
            deactivateRos2ControlTab();
            deactivateEkfTab();
            deactivateCartoTab();
            deactivateNav2_1Tab();
            deactivateNav2_2Tab();
            deactivateNav2_3Tab();
            deactivateBtTab();
            deactivateTopicTab(); 
            activateLogTab();
            break;
        default:
            // Handle unexpected index
            break;
    }


}



// OUR UI METHODS
void MainWindow::initRos2ControlTab()
{
    if (ui->ros2_control) 
    {

    }
}
void MainWindow::activateRos2ControlTab()
{
    if (!communication_) return;
    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString cmd_vel_msg_type   = "geometry_msgs/msg/Twist";

    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    communication_->subscribeTopic(cmd_vel_topic_name, cmd_vel_msg_type);
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    qDebug() << "Subscribed to " << cmd_vel_topic_name << ","; // << example_topic_name;
}
void MainWindow::deactivateRos2ControlTab()
{
    if (!communication_) return;
    
    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString cmd_vel_msg_type   = "geometry_msgs/msg/Twist";

    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    communication_->unsubscribeTopic(cmd_vel_topic_name);
    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    qDebug() << "Unsubscribed from " << cmd_vel_topic_name << ","; // << example_topic_name;
}

void MainWindow::initEkfTab()
{
    
}
void MainWindow::activateEkfTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;
}
void MainWindow::deactivateEkfTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;
}

void MainWindow::initCartoTab()
{

}
void MainWindow::activateCartoTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateCartoTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initNav2_1Tab()
{

}
void MainWindow::activateNav2_1Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateNav2_1Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initNav2_2Tab()
{

}
void MainWindow::activateNav2_2Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateNav2_2Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initNav2_3Tab()
{

}
void MainWindow::activateNav2_3Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateNav2_3Tab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initBtTab()
{

}
void MainWindow::activateBtTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateBtTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initTopicTab()
{

}
void MainWindow::activateTopicTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateTopicTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::initLogTab()
{

}
void MainWindow::activateLogTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";
    
    //communication_->subscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Subscribed to " << example_topic_name;

}
void MainWindow::deactivateLogTab()
{
    //QString example_topic_name = "/diff_controller/cmd_vel_unstamped";
    //QString example_msg_type   = "geometry_msgs/msg/Twist";

    //communication_->unsubscribeTopic(example_topic_name, example_msg_type);

    //qDebug() << "Unsubscribed to " << example_topic_name;

}

void MainWindow::on_closeBtn_clicked()
{
    QCoreApplication::quit();
}


// OVERRIDE METHODS
// =====================================================================================
// --- WINDOW DRAG EVENT HANDLERS ---
// =====================================================================================

/**
 * @brief Handles mouse button press events.
 *
 * Checks if the left button is pressed. If so, it starts the dragging process by:
 * 1. Setting the dragging flag to true.
 * 2. Calculating and storing the relative drag position (the offset of the mouse
 * click from the top-left corner of the window's frame).
 */
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = true;
        // Calculate position relative to the window's top-left corner
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
    QMainWindow::mousePressEvent(event);
}

/**
 * @brief Handles mouse movement events.
 *
 * If the dragging flag is true and the left mouse button is held down, it moves
 * the window to the new position calculated by subtracting the stored drag offset
 * from the current global mouse position.
 */
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    // Check if dragging is enabled and the left button is currently held down
    if (m_isDragging && (event->buttons() & Qt::LeftButton)) {
        // Move the window to the new position
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

/**
 * @brief Handles mouse button release events.
 *
 * If the left button is released, it stops the dragging process by resetting
 * the dragging flag to false.
 */
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_isDragging = false;
        event->accept();
    }
    QMainWindow::mouseReleaseEvent(event);
}