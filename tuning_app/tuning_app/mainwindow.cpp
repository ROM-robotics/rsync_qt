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

#include "design/rom_design.hpp"

#include <QDir>
#include "design/readmeviewer.h"
#include "design/covarianceDisplay.hpp"
#include "sdk/rom_map_widget.hpp"

using rom_dynamics::ui::qt::RomMapWidget;
using rom_dynamics::ui::qt::RomPolarHeadingGraph;
using rom_dynamics::ui::qt::RomPositionGraph;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800, 600);
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    
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

    ui->ipLineEdit->setFocus();

    // signal and slots 
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    currentMode = Mode::ssh;
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();

    initRos2ControlTab();
    initEkfTab();
    //initCartoTab();
}

MainWindow::~MainWindow()
{
    delete ui;
}


// SLOT METHODS
void MainWindow::onTabChanged(int index)
{
    qDebug() << "is connected status: " << this->isConnected_; 
    switch (index) 
    {
        case 0:
            currentMode = Mode::ssh;
            break;
        case 1:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::ros2_control;
            break;
        case 2:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::ekf;
            break;
        case 3:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::carto;
            break;
        case 4:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_1;
            break;
        case 5:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_2;
            break;
        case 6:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::nav2_3;
            break;
        case 7:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::bt;
            break;
        case 8:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::topic;
            break;
        case 9:
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
            currentMode = Mode::log;
            break;
        default:
            // Handle unexpected index
            if(!this->isConnected_) { ui->tabWidget->setCurrentIndex(0); return; }
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
        case Mode::carto:
            deactivateRos2ControlTab();
            deactivateEkfTab();

            activateCartoTab();

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
    //qDebug() << "Done";
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

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
        });

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

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->passwordLineEdit) ui->passwordLineEdit->setFocus();
        });

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
    this->robotIp_ = ip;
    this->password_ = password;
    this->robotNamespace_ = robot_ns;

    createCommunicationClient(this->robotNamespace_, this->robotIp_, this->robotPort_.toInt());

    qDebug() << "robotIp_: " << this->robotIp_;
    qDebug() << "password_ : " << this->password_;
    qDebug() << "robotNamespace_ : " << this->robotNamespace_;

    QDir currentDir = QDir::currentPath();
    qDebug() << "Current Directory: " << currentDir.path();
    currentDir.cdUp(); currentDir.cdUp();currentDir.cdUp();
    qDebug() << "Up three directories: " << currentDir.path();

    QString rsync_app = currentDir.path() + "/rsync_qt/apprsync_qt";

    QStringList arguments;
    arguments << "--ip" << robotIp_ << "--password" << password;
    
    QProcess *proc = new QProcess(this);
    proc->start(rsync_app, arguments);
    //proc->start(rsync_app);
}
void MainWindow::on_connectBtn_clicked()
{
    qDebug() << "Connection button clicked!";

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

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->ipLineEdit) ui->ipLineEdit->setFocus();
        });

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

        //QObject::connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
        QObject::connect(okBtn, &QPushButton::clicked, this, [this, dlgPtr = &dlg]() {
            dlgPtr->accept();
            if (ui && ui->passwordLineEdit) ui->passwordLineEdit->setFocus();
        });

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
    this->robotIp_ = ip;
    this->password_ = password;
    this->robotNamespace_ = robot_ns;

    createCommunicationClient(this->robotNamespace_, this->robotIp_, this->robotPort_.toInt());

    qDebug() << "robotIp_: " << this->robotIp_;
    qDebug() << "password_ : " << this->password_;
    qDebug() << "robotNamespace_ : " << this->robotNamespace_;
}
void MainWindow::on_closeBtn_clicked()
{
    QCoreApplication::quit();
}
void MainWindow::on_hostTerminalBtn_clicked()
{
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
    qDebug() << "robotIp_ set to: " << robotIp_;
    qDebug() << "input_password set to: " << password_;

    QString sshCmd = QString("sshpass -p '%1' ssh mr_robot@%2").arg(password_, robotIp_);
    QStringList args;
    args << "--" << "bash" << "-c" << sshCmd;
    QProcess::startDetached("gnome-terminal", args);
}
void MainWindow::createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port)
{
    qDebug() << "Hacked" ;
    this->isConnected_ = true;

    if(communication_)
    {
        communication_->deleteLater();
        communication_ = nullptr;
    }
    // bridge driver နဲ့ ဆက်သွယ်ဖို့ 
    communication_ = new RosBridgeClient(robot_ns, host, port, this);
    connect(communication_, &RosBridgeClient::receivedTopicMessage, this, &MainWindow::onReceivedTopicMessage);
}
void MainWindow::on_ekfTuningGuideBtn_clicked()
{
    //QString ekf_tuning_guide_url = "https://docs.ros.org/en/foxy/Tutorials/Localization/Understanding-EKF-Localization-With-Nav2.html";
    //QDesktopServices::openUrl(QUrl(ekf_tuning_guide_url));

    QDir dir = QDir::current();
    dir.cdUp(); dir.cdUp(); dir.cdUp();
    QString upTwoDirs = dir.path();
    QString filePath = upTwoDirs + "/README/ekf_guide.md";
    
    qDebug() << "EKF guide path:" << filePath;

    ReadmeDialog dlg(this, filePath);
    dlg.exec();
}
void MainWindow::on_initialNoiseCovToggleBtn_clicked()
{
    bool currentState = ui->initialNoiseXBtn->isVisible();

    //=================================
    ui->initialNoiseXBtn->setVisible(!currentState);
    ui->initialNoiseYBtn->setVisible(!currentState);
    ui->initialNoiseZBtn->setVisible(!currentState);

    ui->initialNoiseRollBtn->setVisible(!currentState);
    ui->initialNoisePitchBtn->setVisible(!currentState);
    ui->initialNoiseYawBtn->setVisible(!currentState);

    ui->initialNoiseVXBtn->setVisible(!currentState);
    ui->initialNoiseVYBtn->setVisible(!currentState);
    ui->initialNoiseVZBtn->setVisible(!currentState);

    ui->initialNoiseVRollBtn->setVisible(!currentState);
    ui->initialNoiseVPitchBtn->setVisible(!currentState);
    ui->initialNoiseVYawBtn->setVisible(!currentState);

    ui->initialNoiseAXBtn->setVisible(!currentState);
    ui->initialNoiseAYBtn->setVisible(!currentState);
    ui->initialNoiseAZBtn->setVisible(!currentState);
}

void MainWindow::on_processNoiseCovToggleBtn_clicked()
{
    bool currentState = ui->processNoiseXBtn->isVisible();
    //=================================
    ui->processNoiseXBtn->setVisible(!currentState);
    ui->processNoiseYBtn->setVisible(!currentState);
    ui->processNoiseZBtn->setVisible(!currentState);

    ui->processNoiseRollBtn->setVisible(!currentState);
    ui->processNoisePitchBtn->setVisible(!currentState);
    ui->processNoiseYawBtn->setVisible(!currentState);

    ui->processNoiseVXBtn->setVisible(!currentState);
    ui->processNoiseVYBtn->setVisible(!currentState);
    ui->processNoiseVZBtn->setVisible(!currentState);

    ui->processNoiseVRollBtn->setVisible(!currentState);
    ui->processNoiseVPitchBtn->setVisible(!currentState);
    ui->processNoiseVYawBtn->setVisible(!currentState);

    ui->processNoiseAXBtn->setVisible(!currentState);
    ui->processNoiseAYBtn->setVisible(!currentState);
    ui->processNoiseAZBtn->setVisible(!currentState);
}

// OUR UI METHODS
void MainWindow::initRos2ControlTab()
{
    if (ui->ros2_control)
    {
        // Remove any existing layout
        QLayout *existing = ui->ros2_control->layout();
        if (existing) 
        {
            delete existing;
        }

        // --- Create a vertical layout (main container) ---
        QVBoxLayout *vLayout = new QVBoxLayout(ui->ros2_control);
        vLayout->setContentsMargins(8, 8, 8, 8);  // 8px padding around edges
        vLayout->setSpacing(8);                   // Space between elements (layouts)
        ui->ros2_control->setLayout(vLayout);

        // ---------------------------------------------------------------------------- create 3 meters
        // --- Create a horizontal layout for the 3 meters ---
        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setSpacing(16); 
        hLayout->setAlignment(Qt::AlignLeft);
        ros2ControlQmlView_.clear();

        QQuickWidget *speed_meter   = new QQuickWidget(ui->ros2_control);
        QQuickWidget *leftRpm_meter = new QQuickWidget(ui->ros2_control);
        QQuickWidget *rightRpm_meter= new QQuickWidget(ui->ros2_control);

        QVector<QQuickWidget*> meterWidgets = { speed_meter, leftRpm_meter, rightRpm_meter };

        for(int i = 0; i < meterWidgets.size(); ++i)
        {
            QQuickWidget *meter = meterWidgets[i];
            meter->setResizeMode(QQuickWidget::SizeRootObjectToView);

            // Set fixed height (e.g., 60% of container height)
            int meterHeight = static_cast<int>(ui->ros2_control->height() * 0.45);
            meter->setMinimumHeight(meterHeight);
            meter->setMaximumHeight(meterHeight);

            meter->setClearColor(QColor("#2e2e2e"));
            
            meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            
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
            ros2ControlQmlView_.append(meter);
        }

        // --- Add the horizontal layout to the vertical layout ---
        vLayout->addLayout(hLayout);
        vLayout->addStretch(1);

        // ---------------------------------------------------------------------------- next 3 meters
        // --- Create a horizontal layout for the Actual 3 meters ---
        QHBoxLayout *lower_hLayout = new QHBoxLayout();
        lower_hLayout->setSpacing(16); 
        
        //ros2ControlQmlView_.clear();

        QQuickWidget *actual_speed_meter   = new QQuickWidget(ui->ros2_control);
        QQuickWidget *actualLeftRpm_meter  = new QQuickWidget(ui->ros2_control);
        QQuickWidget *actualRightRpm_meter = new QQuickWidget(ui->ros2_control);

        QVector<QQuickWidget*> lower_meterWidgets = { actual_speed_meter, actualLeftRpm_meter, actualRightRpm_meter };

        for(int i = 0; i < lower_meterWidgets.size(); ++i)
        {
            QQuickWidget *meter = lower_meterWidgets[i];
            meter->setResizeMode(QQuickWidget::SizeRootObjectToView);

            // Set fixed height (e.g., 60% of container height)
            int meterHeight = static_cast<int>(ui->ros2_control->height() * 0.45);
            meter->setMinimumHeight(meterHeight);
            meter->setMaximumHeight(meterHeight);

            meter->setClearColor(QColor("#2e2e2e"));
            
            meter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            
            if (i == 0) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualSpeed.qml")));
            } else if (i == 1) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualLeftSpeed.qml")));
            } else if (i == 2) {
                meter->setSource(QUrl(QStringLiteral("qrc:/ActualRightSpeed.qml")));
            }

            if (meter->status() != QQuickWidget::Ready) {
                qWarning() << "Failed to load QML for meter" << i << ":" << meter->errors();
            }

            // Add to layout and store reference
            lower_hLayout->addWidget(meter, 1);  // equal width
            ros2ControlQmlView_.append(meter);
        }

        // --- Add the horizontal layout to the vertical layout ---
        vLayout->addLayout(lower_hLayout);
        vLayout->addStretch(1);
        
        ui->desireVelHzLabel->raise();ui->desireVelHzUnitLabel->raise();
        ui->desireLeftWheelRpmLabel->raise();ui->desireLeftWheelRpmUnitLabel->raise();
        ui->desireRightWheelRpmLabel->raise();ui->desireRightWheelRpmUnitLabel->raise();

        ui->actualVelHzLabel->raise();ui->actualVelHzUnitLabel->raise();
        ui->actualLeftWheelRpmLabel->raise();ui->actualLeftWheelRpmUnitLabel->raise();
        ui->actualRightWheelRpmLabel->raise();ui->actualRightWheelRpmUnitLabel->raise();
        
        
    }
}
void MainWindow::activateRos2ControlTab()
{
    if (!communication_) return;

    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString cmd_vel_msg_type   = "geometry_msgs/msg/Twist";
    communication_->subscribeTopic(cmd_vel_topic_name, cmd_vel_msg_type);

    QString odom_topic_name = "/diff_controller/odom";
    QString odom_msg_type   = "nav_msgs/msg/Odometry";
    communication_->subscribeTopic(odom_topic_name, odom_msg_type);

    QString js_topic_name = "/joint_states";
    QString js_msg_type   = "sensor_msgs/msg/JointState";
    communication_->subscribeTopic(js_topic_name, js_msg_type);

    qDebug() << "Subscribed to " << cmd_vel_topic_name << "," << odom_topic_name << "," << js_topic_name;
}
void MainWindow::deactivateRos2ControlTab()
{
    if (!communication_) return;
    
    QString cmd_vel_topic_name = "/diff_controller/cmd_vel_unstamped";
    QString odom_topic_name = "/diff_controller/odom";
    QString js_topic_name = "/joint_states";
    communication_->unsubscribeTopic(cmd_vel_topic_name);
    communication_->unsubscribeTopic(odom_topic_name);
    communication_->unsubscribeTopic(js_topic_name);

    qDebug() << "Unsubscribed from " << cmd_vel_topic_name << "," << odom_topic_name << "," << js_topic_name;
}


void MainWindow::initEkfTab()
{
    if (ui->ekf)
    {
        qDebug() << " Initializing EKF Tab UI components ";
        QLayout *existing = ui->ekf->layout();
        if (existing) 
        {
            delete existing;
            qDebug() << " Deleted existing layout in EKF Tab ";
        }
        
        QGridLayout* grid = new QGridLayout(ui->ekf);
        grid->setContentsMargins(20, 20, 20, 20);
        grid->setSpacing(0);

        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 1);
        grid->setColumnStretch(0, 1);
        grid->setColumnStretch(1, 1);

        odomDiffOdomImuHeadingGraphPtr_ = new RomPolarHeadingGraph(ui->ekf);
        odomDiffOdomPositionGraphPtr_   = new RomPositionGraph(ui->ekf);

        ekfPositionCovarianceGraphPtr_  = new rom_dynamics::ui::qt::RomPositionCovarianceGraph(ui->ekf);
        ekfHeadingCovarianceGraphPtr_   = new rom_dynamics::ui::qt::RomYawCovarianceGraph(ui->ekf);

        
        grid->addWidget(ekfPositionCovarianceGraphPtr_, 0, 1);
        grid->addWidget(odomDiffOdomPositionGraphPtr_, 0, 0);
        grid->addWidget(odomDiffOdomImuHeadingGraphPtr_, 1, 0);
        grid->addWidget(ekfHeadingCovarianceGraphPtr_, 1, 1);

    
        // Optionally, add more widgets or adjust grid layout as needed

        ui->ekf->setLayout(grid);

        //=================================
        ui->controllerLegendLabel->raise();
        ui->controllerPoseLegendLabel->raise();
        ui->ekfLegendLabel->raise();
        ui->ekfPoseLegendLabel->raise();
        ui->imuLegendLabel->raise();

        ui->processNoiseXBtn->raise();
        ui->processNoiseYBtn->raise();
        ui->processNoiseZBtn->raise();

        ui->processNoiseRollBtn->raise();
        ui->processNoisePitchBtn->raise();
        ui->processNoiseYawBtn->raise();

        ui->processNoiseVXBtn->raise();
        ui->processNoiseVYBtn->raise();
        ui->processNoiseVZBtn->raise();

        ui->processNoiseVRollBtn->raise();
        ui->processNoiseVPitchBtn->raise();
        ui->processNoiseVYawBtn->raise();

        ui->processNoiseAXBtn->raise();
        ui->processNoiseAYBtn->raise();
        ui->processNoiseAZBtn->raise();



        ui->initialNoiseXBtn->raise();
        ui->initialNoiseYBtn->raise();
        ui->initialNoiseZBtn->raise();

        ui->initialNoiseRollBtn->raise();
        ui->initialNoisePitchBtn->raise();
        ui->initialNoiseYawBtn->raise();

        ui->initialNoiseVXBtn->raise();
        ui->initialNoiseVYBtn->raise();
        ui->initialNoiseVZBtn->raise();

        ui->initialNoiseVRollBtn->raise();
        ui->initialNoiseVPitchBtn->raise();
        ui->initialNoiseVYawBtn->raise();

        ui->initialNoiseAXBtn->raise();
        ui->initialNoiseAYBtn->raise();
        ui->initialNoiseAZBtn->raise();

        ui->initialNoiseXBtn->setVisible(false);
        ui->initialNoiseYBtn->setVisible(false);
        ui->initialNoiseZBtn->setVisible(false);

        ui->initialNoiseRollBtn->setVisible(false);
        ui->initialNoisePitchBtn->setVisible(false);
        ui->initialNoiseYawBtn->setVisible(false);

        ui->initialNoiseVXBtn->setVisible(false);
        ui->initialNoiseVYBtn->setVisible(false);
        ui->initialNoiseVZBtn->setVisible(false);

        ui->initialNoiseVRollBtn->setVisible(false);
        ui->initialNoiseVPitchBtn->setVisible(false);
        ui->initialNoiseVYawBtn->setVisible(false);

        ui->initialNoiseAXBtn->setVisible(false);
        ui->initialNoiseAYBtn->setVisible(false);
        ui->initialNoiseAZBtn->setVisible(false);

        ui->processNoiseXBtn->setVisible(false);
        ui->processNoiseYBtn->setVisible(false);
        ui->processNoiseZBtn->setVisible(false);

        ui->processNoiseRollBtn->setVisible(false);
        ui->processNoisePitchBtn->setVisible(false);
        ui->processNoiseYawBtn->setVisible(false);

        ui->processNoiseVXBtn->setVisible(false);
        ui->processNoiseVYBtn->setVisible(false);
        ui->processNoiseVZBtn->setVisible(false);

        ui->processNoiseVRollBtn->setVisible(false);
        ui->processNoiseVPitchBtn->setVisible(false);
        ui->processNoiseVYawBtn->setVisible(false);

        ui->processNoiseAXBtn->setVisible(false);
        ui->processNoiseAYBtn->setVisible(false);
        ui->processNoiseAZBtn->setVisible(false);

    }
}
void MainWindow::activateEkfTab()
{
    if (!communication_) return;

    QString odom_topic_name = "/diff_controller/odom";
    QString odom_msg_type   = "nav_msgs/msg/Odometry";
    communication_->subscribeTopic(odom_topic_name, odom_msg_type);

    QString ekf_odom_topic_name = "/odom";
    QString cmd_vel_msg_type   = "geometry_msgs/msg/Twist";
    communication_->subscribeTopic(ekf_odom_topic_name, odom_msg_type);

    QString imu_topic_name = "/imu/out";
    QString imu_msg_type   = "sensor_msgs/msg/Imu";
    communication_->subscribeTopic(imu_topic_name, imu_msg_type);

    qDebug() << "Subscribed to " << odom_topic_name << "," << ekf_odom_topic_name << "," << imu_topic_name;
}
void MainWindow::deactivateEkfTab()
{
    QString ekf_odom_topic_name = "/odom";
    QString odom_topic_name = "/diff_controller/odom";
    QString imu_topic_name = "/imu/out";
    communication_->unsubscribeTopic(ekf_odom_topic_name);
    communication_->unsubscribeTopic(odom_topic_name);
    communication_->unsubscribeTopic(imu_topic_name);

    qDebug() << "Unsubscribed from " << odom_topic_name << "," << ekf_odom_topic_name << "," << imu_topic_name;
}

void MainWindow::initCartoTab()
{
    if (ui->carto)
    {
        qDebug() << " Initializing Carto Tab UI components ";

        QLayout *existing = ui->ekf->layout();
        if (existing) 
        {
            delete existing;
            qDebug() << " Deleted existing layout in EKF Tab ";
        }

        // ========================================        Create map widget
        QWidget *mapWidgetPtr = new RomMapWidget(ui->ekf);
        mapWidgetPtr->setStyleSheet("background:#2a2f36; color:#d3d9e3; border-radius:6px; font-size:20px;");
        mapWidgetPtr->show();

        // Set initial layout
        QVBoxLayout* mainPanelLayout = new QVBoxLayout(ui->ekf);
        mainPanelLayout->setContentsMargins(0,0,0,0);
        mainPanelLayout->addWidget(mapWidgetPtr);

        ui->ekf->setLayout(mainPanelLayout);
    }
}
void MainWindow::activateCartoTab()
{
    if (!communication_) return;

    QString map_topic_name = "/map";
    QString map_msg_type   = "nav_msgs/msg/OccupancyGrid";
    communication_->subscribeTopic(map_topic_name, map_msg_type);

    QString constraint_list_topic_name = "/constraint_list";
    QString constraint_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    communication_->subscribeTopic(constraint_list_topic_name, constraint_list_msg_type);

    QString trajectory_node_list_topic_name = "/trajectory_node_list";
    QString trajectory_node_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    communication_->subscribeTopic(trajectory_node_list_topic_name, trajectory_node_list_msg_type);

    QString scan_matched_points_topic_name = "/scan_matched_points2";
    QString scan_matched_points_msg_type   = "sensor_msgs/msg/PointCloud2";
    communication_->subscribeTopic(scan_matched_points_topic_name, scan_matched_points_msg_type);

    QString landmark_poses_list_topic_name = "/landmark_poses_list";
    QString landmark_poses_list_msg_type   = "visualization_msgs/msg/MarkerArray";
    communication_->subscribeTopic(landmark_poses_list_topic_name, landmark_poses_list_msg_type);

    qDebug() << "Subscribed to " << map_topic_name << "," << constraint_list_topic_name << "," << trajectory_node_list_topic_name;

}
void MainWindow::deactivateCartoTab()
{
    QString map_topic_name = "/map";
    QString constraint_list_topic_name = "/constraint_list";
    QString trajectory_node_list_topic_name = "/trajectory_node_list";
    QString scan_matched_points_topic_name = "/scan_matched_points2";
    QString landmark_poses_list_topic_name = "/landmark_poses_list";
  
    communication_->unsubscribeTopic(map_topic_name);
    communication_->unsubscribeTopic(constraint_list_topic_name);
    communication_->unsubscribeTopic(trajectory_node_list_topic_name);
    communication_->unsubscribeTopic(scan_matched_points_topic_name);
    communication_->unsubscribeTopic(landmark_poses_list_topic_name);

    qDebug() << "Unsubscribed from " << map_topic_name << "," << constraint_list_topic_name << "," << trajectory_node_list_topic_name;

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


void MainWindow::onReceivedTopicMessage(const QString &topic, const QJsonObject &msg)
{
    /* ROS2 CONTROL TAB */
    if( currentMode == Mode::ros2_control )
    {
       // topic name က /diff_controller/cmd_vel_unstamped 
        if( topic == "/diff_controller/cmd_vel_unstamped" )
        {
            if (ros2ControlQmlView_.size() != 6) return;

            QObject* root = ros2ControlQmlView_[0] ? ros2ControlQmlView_[0]->rootObject() : nullptr;
            
            if ( msg.isEmpty() || !msg.contains("linear") ) 
            {
                if (root) 
                {
                    root->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonObject linear = msg.value("linear").toObject();
            double vx = linear.value("x").toDouble();
            //double vy = linear.value("y").toDouble();

            if ( vx < 0 ) { vx *= -1; }

            double speed = vx;
            if (speed > 1.0) { speed = 1.0; } // cap at 1.0 m/s
            
            speed = speed * 100.0; // convert to m/s for display


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
        
            // left rpm
            QObject* left_root = ros2ControlQmlView_[1] ? ros2ControlQmlView_[1]->rootObject() : nullptr;
            if (left_root)
            {
                QVariant qmlLeftRpm = QVariant::fromValue(left_rpm);
                left_root->setProperty("speed", qmlLeftRpm);
            }
            // right rpm
            QObject* right_root = ros2ControlQmlView_[2] ? ros2ControlQmlView_[2]->rootObject() : nullptr;
            if (right_root)
            {
                QVariant qmlRightRpm = QVariant::fromValue(right_rpm);
                right_root->setProperty("speed", qmlRightRpm);
            }

        }
        // topic name က /diff_controller/odom for Actual Robot Velocity
        else if( topic == "/diff_controller/odom" )
        {
            if (ros2ControlQmlView_.size() != 6) return;

            QObject* root = ros2ControlQmlView_[3] ? ros2ControlQmlView_[3]->rootObject() : nullptr;
            
            if ( msg.isEmpty() || !msg.contains("twist") ) 
            {
                if (root) 
                {
                    root->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonObject twist = msg.value("twist").toObject();
            QJsonObject child_twist = twist.value("twist").toObject();
            QJsonObject linear = child_twist.value("linear").toObject();
            double vx = linear.value("x").toDouble();
            //double vy = linear.value("y").toDouble();

            
            if ( vx < 0 ) { vx *= -1; }

            double speed = vx;
            if (speed > 1.0) { speed = 1.0; } // cap at 1.0 m/s
            
            speed = speed * 100.0; // convert to m/s for display

            if (root) 
            {
                QVariant qmlSpeed = QVariant::fromValue(speed);
                root->setProperty("speed", qmlSpeed);
            }
        }
        // topic name က /joint_states for Actual Robot RPMs
        else if( topic == "/joint_states" )
        {
            if (ros2ControlQmlView_.size() != 6) return;
            QObject* leftActualRpmRoot  = ros2ControlQmlView_[4] ? ros2ControlQmlView_[4]->rootObject() : nullptr;
            QObject* rightActualRpmRoot = ros2ControlQmlView_[5] ? ros2ControlQmlView_[5]->rootObject() : nullptr;

            if ( msg.isEmpty() || !msg.contains("name") || !msg.contains("velocity") ) 
            {
                if (leftActualRpmRoot) 
                {
                    leftActualRpmRoot->setProperty("speed", 0.0);
                }
                if (rightActualRpmRoot) 
                {
                    rightActualRpmRoot->setProperty("speed", 0.0);
                }
                return;
            }

            QJsonArray nameArray = msg.value("name").toArray();
            QJsonArray velocityArray = msg.value("velocity").toArray();
            double left_wheel_velocity  = 0.0;
            double right_wheel_velocity = 0.0;

            for (int i = 0; i < nameArray.size(); ++i) 
            {
                QString joint_name = nameArray[i].toString();
                double joint_velocity = velocityArray[i].toDouble();

                if (joint_name == "left_wheel_joint") 
                {
                    left_wheel_velocity = joint_velocity;
                } 
                else if (joint_name == "right_wheel_joint") 
                {
                    right_wheel_velocity = joint_velocity;
                }
            }
            left_wheel_velocity = left_wheel_velocity * (60.0 / (2.0 * M_PI)); // convert rad/s to RPM
            right_wheel_velocity = right_wheel_velocity * (60.0 / (2.0 * M_PI)); // convert rad/s to RPM

            if( left_wheel_velocity < 0 ) { left_wheel_velocity *= -1; }
            if( right_wheel_velocity < 0 ) { right_wheel_velocity *= -1; }

            if (leftActualRpmRoot)
            {
                QVariant qmlLeftActualRpm = QVariant::fromValue(static_cast<int>(left_wheel_velocity));
                leftActualRpmRoot->setProperty("speed", qmlLeftActualRpm);
            }
            if (rightActualRpmRoot)
            {
                QVariant qmlRightActualRpm = QVariant::fromValue(static_cast<int>(right_wheel_velocity));
                rightActualRpmRoot->setProperty("speed", qmlRightActualRpm);
            }
        }
    }
    

    /* EKF TAB */
    else if( currentMode == Mode::ekf ) 
    {
    QString ekf_odom_topic_name = "/odom";
    QString odom_topic_name = "/diff_controller/odom";
    QString imu_topic_name = "/imu/out";
        
    static QPointF ekf_position;
    static QPointF odom_position;
    static double imu_heading = 0.0;
    static double odom_heading = 0.0;
    static double ekf_heading = 0.0;

    // Covariances
    static double ekf_x  = 0.0; static double ekf_y = 0.0; static double ekf_yaw = 0.0;

    static double xx_cov = 0.0; static double xy_cov = 0.0;
    static double yx_cov = 0.0; static double yy_cov = 0.0;
    static double yaw_cov = 0.0;

        if( topic == ekf_odom_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("pose") ) 
            {
                return;
            }

            QJsonObject pose = msg.value("pose").toObject();
            QJsonObject pose_child = pose.value("pose").toObject();
            QJsonObject position = pose_child.value("position").toObject();
            double x = position.value("x").toDouble();
            double y = position.value("y").toDouble();
            ekf_position = QPointF(x, y);

            QJsonObject orientation = pose_child.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();
            ekf_heading = quaternionToYawDegrees(qx, qy, qz, qw);

           QJsonArray covariance_array = pose.value("covariance").toArray();

            if (covariance_array.isEmpty() || covariance_array.size() != 36) 
            {
                qDebug() << "Covariance array is invalid or incomplete.";
                return; 
            }
            
            xx_cov = covariance_array.at(0).toDouble();
            xy_cov = covariance_array.at(1).toDouble();
            yx_cov = covariance_array.at(6).toDouble();
            yy_cov = covariance_array.at(7).toDouble();
            yaw_cov = covariance_array.at(35).toDouble();

            Eigen::Matrix2d covariance_xy_matrix;
            covariance_xy_matrix << xx_cov, xy_cov, yx_cov, yy_cov;

            qDebug() << " EKF Odom Position: " << ekf_position << ", Heading: " << ekf_heading;
            qDebug() << " Diff Odom Position: " << odom_position << ", Heading: " << odom_heading;
            qDebug() << " EKF Odom Position Covariance: " << xx_cov << "," << xy_cov << "," << yx_cov << "," << yy_cov << "," << yaw_cov;

           
            ekf_x = x; ekf_y = y; //ekf_yaw = ekf_heading; ======================== for check

            // xx_cov = position_covariance_obj.value("0").toDouble();
            // xy_cov = position_covariance_obj.value("1").toDouble();
            // yx_cov = position_covariance_obj.value("6").toDouble();
            // yy_cov = position_covariance_obj.value("7").toDouble();
            // yaw_cov= position_covariance_obj.value("35").toDouble();

            if( ekfPositionCovarianceGraphPtr_ )
            {
                ekfPositionCovarianceGraphPtr_->updateGraph(ekf_x, ekf_y, covariance_xy_matrix);
            }
            if( ekfHeadingCovarianceGraphPtr_ )
            {
                ekfHeadingCovarianceGraphPtr_->updateGraph(ekf_heading, yaw_cov);
            }
            if( odomDiffOdomImuHeadingGraphPtr_ )
            {
                odomDiffOdomImuHeadingGraphPtr_->updateGraph(odom_heading, imu_heading, ekf_heading);
            }
            if( odomDiffOdomPositionGraphPtr_ )
            {
                odomDiffOdomPositionGraphPtr_->updateGraph(odom_position, ekf_position);
            }
        }
        else if( topic == odom_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("pose") ) 
            {
                return;
            }

            QJsonObject pose = msg.value("pose").toObject();
            QJsonObject pose_child = pose.value("pose").toObject();
            QJsonObject position = pose_child.value("position").toObject();
            double x = position.value("x").toDouble();
            double y = position.value("y").toDouble();
            odom_position = QPointF(x, y);

            QJsonObject orientation = pose_child.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();
            odom_heading = quaternionToYawDegrees(qx, qy, qz, qw);
        }
        else if( topic == imu_topic_name )
        {
            if ( msg.isEmpty() || !msg.contains("orientation") ) 
            {
                return;
            }

            QJsonObject orientation = msg.value("orientation").toObject();
            double qx = orientation.value("x").toDouble();
            double qy = orientation.value("y").toDouble();
            double qz = orientation.value("z").toDouble();
            double qw = orientation.value("w").toDouble();

            // Convert quaternion to yaw angle in degrees
            double siny_cosp = 2.0 * (qw * qz + qx * qy);
            double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
            double yaw = std::atan2(siny_cosp, cosy_cosp);
            double yaw_degrees = yaw * (180.0 / M_PI);
            imu_heading = yaw_degrees;
        }
    }

    /* CARTO TAB */
    else if( currentMode == Mode::carto ) {}

    /* NAV2 1 TAB */
    else if( currentMode == Mode::nav2_1 ) {}

    /* NAV2 2 TAB */
    else if( currentMode == Mode::nav2_2 ) {}

    /* NAV2 3 TAB */
    else if( currentMode == Mode::nav2_3 ) {}

    /* BT TAB */
    else if( currentMode == Mode::bt ) {}

    /* TOPIC TAB */
    else if( currentMode == Mode::topic ) {}

    /* LOG TAB */
    else if( currentMode == Mode::log ) {}
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

double MainWindow::quaternionToYawDegrees(double &qx, double &qy, double &qz, double &qw)
{
    double siny_cosp = 2.0 * (qw * qz + qx * qy);
    double cosy_cosp = 1.0 - 2.0 * (qy * qy + qz * qz);
    double yaw = std::atan2(siny_cosp, cosy_cosp);
    double yaw_degrees = yaw * (180.0 / M_PI);
    return yaw_degrees;
}

double MainWindow::yawDegreesToQuaternion(double &yaw_degrees, double &qx, double &qy, double &qz, double &qw)
{
    double half_yaw = yaw_degrees * 0.5 * (M_PI / 180.0);
    qw = std::cos(half_yaw);
    qx = 0.0;
    qy = 0.0;
    qz = std::sin(half_yaw);
}
