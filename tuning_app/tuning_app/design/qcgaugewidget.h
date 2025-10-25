#ifndef QCGAUGEWIDGET_H
#define QCGAUGEWIDGET_H


/***************************************************************************
** **
** QcGauge, for instrumentation, and real time data measurement **
** visualization widget for Qt. **
** Copyright (C) 2015 Hadj Tahar Berrima **
** **
** This program is free software: you can redistribute it and/or modify **
** it under the terms of the GNU Lesser General Public License as **
** published by the Free Software Foundation, either version 3 of the **
** License, or (at your option) any later version. **
** **
** This program is distributed in the hope that it will be useful, **
** but WITHOUT ANY WARRANTY; without even the implied warranty of **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the **
** GNU Lesser General Public License for more details. **
** **
***************************************************************************/


#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QColor>
#include <QList>
#include <cmath>


class QcGaugeWidget : public QWidget
{
Q_OBJECT


public:
explicit QcGaugeWidget(QWidget *parent = nullptr);
~QcGaugeWidget() override = default;


void setRange(float minValue, float maxValue);
void setValue(float value);
float value() const { return m_value; }


protected:
void paintEvent(QPaintEvent *event) override;


private:
float m_minValue {0.0f};
float m_maxValue {100.0f};
float m_value {0.0f};
};


#endif // QCGAUGEWIDGET_H