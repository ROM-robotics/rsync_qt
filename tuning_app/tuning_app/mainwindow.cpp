#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QQuickWidget>
#include <QUrl>
#include <QVBoxLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <QQmlContext>
#include <QQuickItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);

    currentMode = Mode::ssh;
    qDebug() << " currentMode = " << ModeToString(currentMode).c_str();

    // create rosbridge client and connect to topic message signal
    communication_ = new RosBridgeClient("", "127.0.0.1", 9090, this);
    connect(communication_, &RosBridgeClient::topicMessageReceived, this, &MainWindow::onRosTopicMessage);
    connect(communication_, &RosBridgeClient::connected, this, &MainWindow::onRosConnected);
    communication_->connectToServer();

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
        // Create QQuickWidget to show QML and keep as member
        qmlView_ = new QQuickWidget(ui->ros2_control);
        qmlView_->setResizeMode(QQuickWidget::SizeRootObjectToView);

        // Load QML file (resource)
        qmlView_->setSource(QUrl(QStringLiteral("qrc:/Speed.qml")));

        // Keep root object for property updates
        qmlRoot_ = qmlView_->rootObject();

        // Ensure the tab has a layout; create a vertical layout if needed
        QLayout *existing = ui->ros2_control->layout();
        QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(existing);
        if (!layout) {
            layout = new QVBoxLayout(ui->ros2_control);
            layout->setContentsMargins(8, 8, 8, 8);
        }
        layout->addWidget(qmlView_);

        // Create a timer to update speed periodically for demo (only if enabled)
        if (useDemo_) {
            qmlUpdateTimer_ = new QTimer(this);
            connect(qmlUpdateTimer_, &QTimer::timeout, this, &MainWindow::updateQmlSpeed);
            qmlUpdateTimer_->start(250); // update every 250 ms
        }
    }
}

void MainWindow::onRosConnected()
{
    qDebug() << "RosBridge connected, subscribing to /cmd_vel_unstamped";
    if (communication_) {
        communication_->subscribeTopic("/cmd_vel_unstamped", "geometry_msgs/Twist", "");
    }
    // If demo timer was running, stop it (live data will drive the gauge)
    if (qmlUpdateTimer_ && qmlUpdateTimer_->isActive()) {
        qmlUpdateTimer_->stop();
    }
}

void MainWindow::updateQmlSpeed()
{
    if (!qmlRoot_)
        return;

    // generate a random speed between 0 and 70 for demo purposes
    // demo range now matches the speedometer (0..70 km/h)
    double value = QRandomGenerator::global()->generateDouble() * 70.0;
    qmlRoot_->setProperty("speed", value);
}


void MainWindow::onRosTopicMessage(const QString &topic, const QJsonObject &msg)
{
    Q_UNUSED(topic)
    // Expecting a geometry_msgs/Twist-like message with linear.x as forward speed
    if (!qmlRoot_) return;

    if (msg.contains("linear") && msg.value("linear").isObject()) {
        QJsonObject linear = msg.value("linear").toObject();
        double speed = 0.0;
        if (linear.contains("x")) {
            speed = linear.value("x").toDouble(0.0);
        }

        // convert speed from m/s to km/h (common ROS linear.x unit is m/s)
        double speedKph = speed * 3.6;
        // Clamp to 0..70 (speedometer range)
        double clamped = qBound(0.0, speedKph, 70.0);
        qmlRoot_->setProperty("speed", clamped);
    }
}



