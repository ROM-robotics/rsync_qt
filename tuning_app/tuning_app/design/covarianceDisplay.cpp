#include "covarianceDisplay.hpp" // Assumed header location
#include <QVBoxLayout>
#include <cmath>
#include <QtMath>
#include <limits> // For numeric_limits

using namespace rom_dynamics::ui::qt;
using namespace Eigen;

//================================================================================
// 1. RomPositionCovarianceGraph Implementation
//================================================================================

rom_dynamics::ui::qt::RomPositionCovarianceGraph::RomPositionCovarianceGraph(QWidget *parent)
    : QWidget(parent)
{
    initializeChart();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chart_view_);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::initializeChart()
{
    // Chart for X-Y Position Ellipse
    chart_ = new QChart();
    chart_->setTitle("EKF Position Covariance x,y Ellipse");
    chart_->legend()->hide();
    
    ellipse_series_ = new QScatterSeries();
    ellipse_series_->setMarkerSize(3.0);
    ellipse_series_->setColor(QColor(255, 0, 0)); // Red
    ellipse_series_->setName("95% Confidence Ellipse");
    chart_->addSeries(ellipse_series_);

    // Axis setup
    axisX_ = new QValueAxis();
    axisX_->setTitleText("X Position (m)");
    axisY_ = new QValueAxis();
    axisY_->setTitleText("Y Position (m)");

    // Initial Range
    axisX_->setRange(-1.0, 1.0);
    axisY_->setRange(-1.0, 1.0);
    
    chart_->addAxis(axisX_, Qt::AlignBottom);
    chart_->addAxis(axisY_, Qt::AlignLeft);
    ellipse_series_->attachAxis(axisX_);
    ellipse_series_->attachAxis(axisY_);
    
    chart_view_ = new QChartView(chart_);
    chart_view_->setRenderHint(QPainter::Antialiasing);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::updateGraph(double ekf_pose_x, 
                                             double ekf_pose_y, 
                                             const Eigen::Matrix2d& covariance_xy)
{
    current_x_ = ekf_pose_x;
    current_y_ = ekf_pose_y;
    
    drawPositionEllipse(covariance_xy);
}

void rom_dynamics::ui::qt::RomPositionCovarianceGraph::drawPositionEllipse(const Eigen::Matrix2d& covariance_xy)
{
    // Calculate Eigenvalues and Eigenvectors for the Ellipse
    EigenSolver<Matrix2d> es(covariance_xy);
    Vector2d eigenvalues = es.eigenvalues().real();
    Matrix2d eigenvectors = es.eigenvectors().real();

    // 95% confidence chi-squared value for 2 DOF
    double k = 2.4477; 
    double major_axis = k * std::sqrt(eigenvalues(0));
    double minor_axis = k * std::sqrt(eigenvalues(1));
    double angle = std::atan2(eigenvectors(1, 0), eigenvectors(0, 0)); // Angle of major axis

    ellipse_series_->clear();
    
    // Generate Ellipse points
    const int num_points = 50;
    for (int i = 0; i <= num_points; ++i) 
    {
        double theta = 2.0 * M_PI * i / num_points;
        double x_rotated = major_axis * std::cos(theta);
        double y_rotated = minor_axis * std::sin(theta);

        // Rotate and Translate to current (x, y) pose
        double x_point = current_x_ + (x_rotated * std::cos(angle) - y_rotated * std::sin(angle));
        double y_point = current_y_ + (x_rotated * std::sin(angle) + y_rotated * std::cos(angle));

        ellipse_series_->append(x_point, y_point);
    }
    
    // Update chart view to follow the current position 
    double range = 0.5 + std::max(major_axis, minor_axis);
    axisX_->setRange(current_x_ - range, current_x_ + range);
    axisY_->setRange(current_y_ - range, current_y_ + range);
}

//================================================================================
// 2. RomYawCovarianceGraph Implementation
//================================================================================

rom_dynamics::ui::qt::RomYawCovarianceGraph::RomYawCovarianceGraph(QWidget *parent)
    : QWidget(parent)
{
    initializeChart();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(chart_view_);
    setLayout(layout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::initializeChart()
{
    chart_ = new QPolarChart();
    chart_->setTitle("EKF Yaw Covariance yaw Polar View");
    chart_->legend()->hide();
    
    // Axis setup (Polar Chart uses Radial and Angular axes)
    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setRange(0, 1); // Normalized radius
    radialAxis->setLabelsVisible(false);
    radialAxis->setLinePen(Qt::NoPen);
    chart_->addAxis(radialAxis, QPolarChart::PolarOrientationRadial); 

    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setRange(0, 360); // Full circle in degrees
    angularAxis->setTickCount(13); // 0, 30, 60, ..., 330
    chart_->addAxis(angularAxis, QPolarChart::PolarOrientationAngular); 
    
    // Series for Yaw Arc
    yaw_arc_series_ = new QLineSeries();
    yaw_arc_series_->setColor(QColor(0, 0, 255)); // Blue
    QPen pen = yaw_arc_series_->pen();
    pen.setWidth(4); // Thicker line for sector boundary
    yaw_arc_series_->setPen(pen);
    chart_->addSeries(yaw_arc_series_);
    yaw_arc_series_->attachAxis(radialAxis);
    yaw_arc_series_->attachAxis(angularAxis);
    
    // Dummy scatter to correctly map chart axes
    QScatterSeries *dummyScatter = new QScatterSeries();
    dummyScatter->append(0, 0);
    dummyScatter->setMarkerSize(0);
    chart_->addSeries(dummyScatter);
    dummyScatter->attachAxis(radialAxis); 
    dummyScatter->attachAxis(angularAxis); 
    
    chart_view_ = new QChartView(chart_);
    chart_view_->setRenderHint(QPainter::Antialiasing);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::updateGraph(double ekf_pose_yaw, double yaw_variance)
{
    current_yaw_ = ekf_pose_yaw;
    drawYawSector(ekf_pose_yaw, yaw_variance);
}

void rom_dynamics::ui::qt::RomYawCovarianceGraph::drawYawSector(double yaw, double yaw_variance)
{
    double sigma_yaw = std::sqrt(yaw_variance);
    // 95% confidence interval (approx k=2 for normal distribution)
    double k = 2.0; 
    
    double yaw_min = yaw - k * sigma_yaw;
    double yaw_max = yaw + k * sigma_yaw;

    // Radius for the arc (1.0 is the max radial range)
    const double radius = 0.95; 

    yaw_arc_series_->clear();

    // Start point of the arc (Center of the polar chart: radius=0, angle=0)
    yaw_arc_series_->append(0, 0); 

    const int num_segments = 30;
    double yaw_step = (yaw_max - yaw_min) / num_segments;

    // Generate points for the arc
    for (int i = 0; i <= num_segments; ++i) 
    {
        double current_angle = yaw_min + i * yaw_step;
        
        // Polar Chart coordinates are (radius, angle)
        // Convert the angle from radians to QPolarChart's degrees [0, 360]
        qreal angle_degrees = radiansToDegrees(current_angle);
        
        yaw_arc_series_->append(radius, angle_degrees);
    }
    
    // End point of the arc (back to the center to close the sector)
    yaw_arc_series_->append(0, 0);

    // Update the title to show uncertainty value
    double confidence_95_deg = k * sigma_yaw * 180.0 / M_PI; 
    QString title = QString("EKF Yaw Covariance Sector (Polar View)\n"
                            "Yaw Uncertainty (2σ): ±%1 degrees").arg(confidence_95_deg, 0, 'f', 2);
    chart_->setTitle(title);
}