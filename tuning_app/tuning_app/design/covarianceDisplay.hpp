#ifndef COVARIANCEDISPLAY_HPP
#define COVARIANCEDISPLAY_HPP

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QScatterSeries>
#include <QValueAxis>
#include <QtCharts>

//#include "rclcpp/rclcpp.hpp"
//#include "nav_msgs/msg/odometry.hpp"
#include <Eigen/Dense>

namespace rom_dynamics {

class CovarianceDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit CovarianceDisplay(QWidget *parent = nullptr){}
    ~CovarianceDisplay(){}

private:
    // ROS 2 Components
    //rclcpp::Node::SharedPtr ros_node_;
    //rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    //QTimer *ros_timer_; // To process ROS events

    // Qt Chart Components
    QChartView *chart_view_position_;
    QChart *chart_position_;
    QScatterSeries *ellipse_series_;
    QValueAxis *axisX_;
    QValueAxis *axisY_;

    // Visualization Data
    double current_x_ = 0.0;
    double current_y_ = 0.0;
    double current_yaw_ = 0.0;
    std::array<double, 36> current_covariance_;

    // Methods
    //void initializeRosNode();
    void initializeChart();
    //void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg);
    void updateVisualization();

    // Custom drawing functions
    void drawPositionEllipse(const Eigen::Matrix2d& covariance_xy) {}
    void drawYawSector(double yaw, double yaw_variance){}

private slots:
    //void spinRos();
};

}

#endif // COVARIANCEDISPLAY_HPP
