class OdomPoseChart : public QWidget
    
    
{
    Q_OBJECT
public:
    explicit OdomPoseChart(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        qDebug() << "OdomPoseChart constructed, parent:" << parent;
    setStyleSheet("border: 2px solid red;");
    setMinimumSize(400, 300); // DEBUG: ensure widget is large enough

    // --- Create simple line series for debugging ---
    m_pathSeries = new QLineSeries();
    m_pathSeries->setName("Debug Line");
    m_pathSeries->setColor(Qt::red);
    m_pathSeries->append(0, 0);
    m_pathSeries->append(1, 1);
    m_pathSeries->append(2, 0.5);

    m_chart = new QChart();
    m_chart->addSeries(m_pathSeries);
    m_chart->setTitle("Debug Chart");
    m_chart->setBackgroundBrush(QColor("#ffffff")); // DEBUG: set to white for visibility
    m_chart->legend()->setLabelColor(Qt::black);

    m_axisX = new QValueAxis;
    m_axisX->setTitleText("X");
    m_axisX->setRange(0, 3);
    m_axisX->setLabelsColor(Qt::black);

    m_axisY = new QValueAxis;
    m_axisY->setTitleText("Y");
    m_axisY->setRange(0, 2);
    m_axisY->setLabelsColor(Qt::black);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_pathSeries->attachAxis(m_axisX);
    m_pathSeries->attachAxis(m_axisY);

        // --- Chart view ---
    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setGeometry(rect());
    m_chartView->show(); // DEBUG: force show

        // --- Optional: Add demo data at startup ---
        loadDemoData();
    }

    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        qDebug() << "OdomPoseChart resizeEvent, size:" << size();
        if (m_chartView)
            m_chartView->setGeometry(rect());  // make chart fill entire widget
    }

    // --- Add a single point (x, y) ---
    void addRobotPoint(double x, double y)
    {
        m_pathSeries->append(x, y);
        m_pointSeries->append(x, y);
    }

    // --- Demo data for testing ---
    void loadDemoData()
    {
        // Example trajectory (x, y in meters)
        QList<QPointF> demoPoints = {
            {0.0, 0.0},
            {0.5, 0.4},
            {1.2, 0.9},
            {2.0, 1.5},
            {3.0, 2.0},
            {4.0, 2.8}
        };

        for (const QPointF &p : demoPoints)
            addRobotPoint(p.x(), p.y());
    }

private:
    QChart *m_chart;
    QChartView *m_chartView;
    QLineSeries *m_pathSeries;
    QScatterSeries *m_pointSeries;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;
};
