#include "qcgaugewidget.h"
m_maxValue = maxValue;
update();
}


void QcGaugeWidget::setValue(float value)
{
m_value = std::clamp(value, m_minValue, m_maxValue);
update();
}


void QcGaugeWidget::paintEvent(QPaintEvent *event)
{
Q_UNUSED(event);


QPainter painter(this);
painter.setRenderHint(QPainter::Antialiasing);


const int side = qMin(width(), height());
painter.translate(width() / 2.0, height() / 2.0);
painter.scale(side / 200.0, side / 200.0);


// Draw outer circle
painter.setPen(Qt::NoPen);
painter.setBrush(QColor(60, 60, 60));
painter.drawEllipse(QPointF(0, 0), 100, 100);


// Draw tick marks
painter.setPen(QPen(Qt::white, 2));
for (int i = 0; i < 100; i += 10)
{
painter.save();
painter.rotate((i - m_minValue) * 270.0 / (m_maxValue - m_minValue) - 135);
painter.drawLine(0, -85, 0, -100);
painter.restore();
}


// Draw needle
float angle = (m_value - m_minValue) * 270.0 / (m_maxValue - m_minValue) - 135.0f;
painter.save();
painter.rotate(angle);


QPolygon needle;
needle << QPoint(0, -80) << QPoint(-4, 0) << QPoint(4, 0);
painter.setBrush(Qt::red);
painter.drawPolygon(needle);
painter.restore();


// Draw center cap
painter.setBrush(Qt::black);
painter.drawEllipse(QPointF(0, 0), 5, 5);


// Draw current value text
painter.setPen(Qt::white);
QFont font = painter.font();
font.setPointSize(10);
painter.setFont(font);
painter.drawText(QRectF(-50, 30, 100, 30), Qt::AlignCenter,
QString::number(m_value, 'f', 1));