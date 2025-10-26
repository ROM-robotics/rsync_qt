#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QTabBar>
#include <QtGui/QMouseEvent>
#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QRandomGenerator>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    // allow the main window to have a transparent background so QQuickWidget alpha shows through
    //this->setAttribute(Qt::WA_TranslucentBackground);
    
    if (ui->tabWidget && ui->tabWidget->tabBar()) 
    {   // tab bar တွေညီအောင်လို့။
        int tabCount = ui->tabWidget->count();
        if (tabCount > 0) 
        {
            int totalW = ui->tabWidget->width() - 30;
            int perTabW = qMax(1, totalW / tabCount);
            QString tabStyle = QString("QTabBar::tab { min-width: %1px; max-width: %1px; }").arg(perTabW);
            ui->tabWidget->tabBar()->setStyleSheet(tabStyle);
        }
    }

    // bridge driver နဲ့ ဆက်သွယ်ဖို့ 
    communication_ = new RosBridgeClient("", RobotIp, RobotPort.toInt(), this);

    // signal and slots 
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(communication_, &RosBridgeClient::receivedRos2controlMessage, this, &MainWindow::onRos2ControlVelocity);


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
void MainWindow::on_rsyncBtn_clicked()
{
    qDebug() << "Rsync button clicked!";

    QString ip = ui->ipLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString robot_ns = ui->nsLineEdit->text().trimmed();

    if (ip.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing IP");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the robot IP address.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);
        QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }
    if (password.isEmpty()) 
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Missing Password");
        dlg.setFixedSize(400, 100);
        QLabel *label = new QLabel("Please enter the password.", &dlg);
        label->setAlignment(Qt::AlignCenter);
        QPushButton *okBtn = new QPushButton("OK", &dlg);
        okBtn->setGeometry(150, 150, 100, 30);
        QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(okBtn);
        dlg.setLayout(layout);
        // Center dialog over main window
        QPoint center = this->geometry().center();
        QPoint globalCenter = this->mapToGlobal(center);
        int dlgX = globalCenter.x() - dlg.width() / 2;
        int dlgY = globalCenter.y() - dlg.height() / 2;
        dlg.move(dlgX, dlgY);
        dlg.exec();
        return;
    }

    // Store in class variable
    RobotIp = ip;
    password_ = password;
    RobotNamespace = robot_ns;
    qDebug() << "RobotIp set to: " << RobotIp;
    qDebug() << "RobotNamespace set to: " << RobotNamespace;

    QDir currentDir = QDir::currentPath();
    qDebug() << "Current Directory: " << currentDir.path();
    currentDir.cdUp(); currentDir.cdUp();currentDir.cdUp();
    qDebug() << "Up three directories: " << currentDir.path();

    QString rsync_app = currentDir.path() + "/rsync_qt/apprsync_qt";

    QStringList arguments;
    arguments << "--ip" << RobotIp << "--password" << password;
    
    QProcess *proc = new QProcess(this);
    proc->start(rsync_app, arguments);
    //proc->start(rsync_app);
}
void MainWindow::on_closeBtn_clicked()
{
    QCoreApplication::quit();
}
void MainWindow::on_hostTerminalBtn_clicked()
{
    // QString echo_style1  = "echo -e '\e]11;rgb:00/AA/BB\a'";
    // QString class_black  = "echo -e '\e]11;rgb:00/00/00\a'";
    // QString bright_white = "echo -e '\e]11;rgb:FF/FF/FF\a'";
    // QString navy_blue    = "echo -e '\e]11;rgb:00/00/80\a'";
    // QString forest_green = "echo -e '\e]11;rgb:22/8B/22\a'";
    // QString vibrant_cyan = "echo -e '\e]11;rgb:00/FF/FF\a'";
    // QString warm_sepia   = "echo -e '\e]11;rgb:F0/E6/8C\a'";
    // QString deep_purple  = "echo -e '\e]11;rgb:4B/00/82\a'";
    // QString soft_gray    = "echo -e '\e]11;rgb:A9/A9/A9\a'";
    // QString bold_old     = "echo -e '\e]11;rgb:FF/A5/00\a'";
    // QString hot_pink     = "echo -e '\e]11;rgb:FF/69/B4\a'";

    // QStringList args;
    // args << "--" << "bash" << "-c" << echo_style1 + "; exec bash";
    // QProcess::startDetached("gnome-terminal", args);

    QStringList colorCmds = {
        "echo -e '\\e]11;rgb:00/AA/BB\\a'", // echo_style1
        "echo -e '\\e]11;rgb:00/00/00\\a'", // class_black
        "echo -e '\\e]11;rgb:FF/FF/FF\\a'", // bright_white
        "echo -e '\\e]11;rgb:00/00/80\\a'", // navy_blue
        "echo -e '\\e]11;rgb:22/8B/22\\a'", // forest_green
        "echo -e '\\e]11;rgb:00/FF/FF\\a'", // vibrant_cyan
        "echo -e '\\e]11;rgb:F0/E6/8C\\a'", // warm_sepia
        "echo -e '\\e]11;rgb:4B/00/82\\a'", // deep_purple
        "echo -e '\\e]11;rgb:A9/A9/A9\\a'", // soft_gray
        "echo -e '\\e]11;rgb:FF/A5/00\\a'", // bold_old
        "echo -e '\\e]11;rgb:FF/69/B4\\a'"  // hot_pink
    };

    int idx = QRandomGenerator::global()->bounded(colorCmds.size());
    while( idx == previousCmdColorIndex )
    {
        idx = QRandomGenerator::global()->bounded(colorCmds.size());
    }

    previousCmdColorIndex = idx;
    QString colorCmd = colorCmds[idx]; 

    QStringList args;
    args << "--geometry=80x25-0+0";
    args << "--" << "bash" << "-c" << colorCmd + "; exec bash";
    QProcess::startDetached("gnome-terminal", args);
   
}
void MainWindow::on_robotTerminalBtn_clicked()
{
    qDebug() << "RobotIp set to: " << RobotIp;
    qDebug() << "input_password set to: " << password_;

    QString sshCmd = QString("sshpass -p '%1' ssh mr_robot@%2").arg(password_, RobotIp);
    QStringList args;
    args << "--" << "bash" << "-c" << sshCmd;
    QProcess::startDetached("gnome-terminal", args);
}


// OUR UI METHODS
void MainWindow::initRos2ControlTab()
{
    if (ui->ros2_control)
    {
         // Remove any existing layout
        QLayout *existing = ui->ros2_control->layout();
        if (existing) {
            delete existing;
        }

        // --- Create a vertical layout (main container) ---
        QVBoxLayout *vLayout = new QVBoxLayout(ui->ros2_control);
        vLayout->setContentsMargins(8, 8, 8, 8);  // 8px padding around edges
        vLayout->setSpacing(8);                   // Space between elements (layouts)
        ui->ros2_control->setLayout(vLayout);

        // --- Create a horizontal layout for the 3 meters ---
        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setSpacing(16);  // space between meters

        // --- Prepare meters ---
        qmlView_.clear();

        QQuickWidget *speed_meter   = new QQuickWidget(ui->ros2_control);
        QQuickWidget *leftRpm_meter = new QQuickWidget(ui->ros2_control);
        QQuickWidget *rightRpm_meter= new QQuickWidget(ui->ros2_control);

        QVector<QQuickWidget*> meterWidgets = { speed_meter, leftRpm_meter, rightRpm_meter };

        for(int i = 0; i < meterWidgets.size(); ++i)
        {
            QQuickWidget *meter = meterWidgets[i];
            meter->setResizeMode(QQuickWidget::SizeRootObjectToView);

            // Set fixed height (e.g., 60% of container height)
            int meterHeight = static_cast<int>(ui->ros2_control->height() * 0.6);
            meter->setMinimumHeight(meterHeight);
            meter->setMaximumHeight(meterHeight);

            meter->setClearColor(QColor("#2e2e2e"));


            // Make them expand horizontally but fixed vertically
            meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            // Load the QML
            if (i == 0) {
                meter->setSource(QUrl(QStringLiteral("qrc:/Speed.qml")));
            } else if (i == 1) {
                meter->setSource(QUrl(QStringLiteral("qrc:/LeftSpeed.qml")));
            } else if (i == 2) {
                meter->setSource(QUrl(QStringLiteral("qrc:/RightSpeed.qml")));
            }

            if (meter->status() != QQuickWidget::Ready) {
                qWarning() << "Failed to load QML for meter" << i << ":" << meter->errors();
            }

            // Add to layout and store reference
            hLayout->addWidget(meter, 1);  // equal width
            qmlView_.append(meter);
        }

        // --- Add the horizontal layout to the vertical layout ---
        vLayout->addLayout(hLayout);

        // --- Add a stretch below so meters stay at the top ---
        vLayout->addStretch(1);
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


void MainWindow::onRos2ControlVelocity(const QString &topic, const QJsonObject &msg)
{
    // topic name က /diff_controller/cmd_vel_unstamped 
    if( topic == "/diff_controller/cmd_vel_unstamped" )
    {
        if (qmlView_.size() != 3) return;

        QObject* root = qmlView_[0] ? qmlView_[0]->rootObject() : nullptr;

        // If the message is empty or does not contain 'linear', treat as no data (publisher exists but not publishing)
        if ( msg.isEmpty() || !msg.contains("linear") ) {
            if (root) {
                root->setProperty("speed", 0.0);
            }
            return;
        }

        QJsonObject linear = msg.value("linear").toObject();
        double vx = linear.value("x").toDouble();
        //double vy = linear.value("y").toDouble();
        if ( vx < 0 ) { vx *= -1; }

        double speed = vx; 

        if (root) 
        {
            QVariant qmlSpeed = QVariant::fromValue(speed);
            root->setProperty("speed", qmlSpeed);
        }

        int right_rpm = 0;
        int left_rpm  = 0;

        QJsonObject angular = msg.value("angular").toObject();
        double vz = angular.value("z").toDouble();
        robotVelocityToWheelRpms(vx, vz, wheel_radius_, wheel_seperation_, left_rpm, right_rpm);

        if( right_rpm < 0 ) { right_rpm *= -1; }
        if( left_rpm  < 0 ) { left_rpm  *= -1; }

        // right rpm
        QObject* right_root = qmlView_[2] ? qmlView_[2]->rootObject() : nullptr;
        if (right_root)
        {
            QVariant qmlRightRpm = QVariant::fromValue(right_rpm);
            right_root->setProperty("speed", qmlRightRpm);
        }
        // left rpm
        QObject* left_root = qmlView_[1] ? qmlView_[1]->rootObject() : nullptr;
        if (left_root)
        {
            QVariant qmlLeftRpm = QVariant::fromValue(left_rpm);
            left_root->setProperty("speed", qmlLeftRpm);
        }

    }

}


void MainWindow::robotVelocityToWheelRpms(double linear_velocity, double angular_velocity, double wheel_radius, double wheel_seperation, int &left_rpm, int &right_rpm)
{
    // Calculate wheel linear velocities
    double v_left = linear_velocity - (angular_velocity * wheel_seperation / 2.0);
    double v_right = linear_velocity + (angular_velocity * wheel_seperation / 2.0);

    // Convert linear velocities to angular velocities (rad/s)
    double omega_left = v_left / wheel_radius;
    double omega_right = v_right / wheel_radius;

    // Convert angular velocities to RPM
    left_rpm = static_cast<int>((omega_left * 60.0) / (2.0 * M_PI));
    right_rpm = static_cast<int>((omega_right * 60.0) / (2.0 * M_PI));
}



// OVERRIDE METHODS



// QDIALOG BOX
//  QDialog dlg(this);
//         dlg.setWindowTitle("Missing IP");
//         dlg.setFixedSize(400, 100);
//         QLabel *label = new QLabel("Please enter the robot IP address.", &dlg);
//         label->setAlignment(Qt::AlignCenter);
//         QPushButton *okBtn = new QPushButton("OK", &dlg);
//         okBtn->setGeometry(150, 150, 100, 30);
//         QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
//         QVBoxLayout *layout = new QVBoxLayout;
//         layout->addWidget(label);
//         layout->addWidget(okBtn);
//         dlg.setLayout(layout);
//         // Center dialog over main window
//         QPoint center = this->geometry().center();
//         QPoint globalCenter = this->mapToGlobal(center);
//         int dlgX = globalCenter.x() - dlg.width() / 2;
//         int dlgY = globalCenter.y() - dlg.height() / 2;
//         dlg.move(dlgX, dlgY);
//         dlg.exec();
//         return;
