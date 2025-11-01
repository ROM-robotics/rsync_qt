#ifndef ROM_SDK_MAP_WIDGET_HPP
#define ROM_SDK_MAP_WIDGET_HPP

#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QScreen>
#include <QEasingCurve>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QListWidget>
#include <QMouseEvent>
#include <QColor>
#include <QJsonObject>
#include <QJsonArray>

#include <cstdint>
#include "../design/rom_structures.h"

namespace rom_dynamics::ui::qt 
{

using rom_dynamics::data_types::RomMap;
//class RomMap;

static QImage romMapToImage_local(const RomMap& map)
{// Convert RomMap occupancy grid to grayscale QImage
	const int w = map.width();
	const int h = map.height();
	QImage img(w, h, QImage::Format_Grayscale8);
	if (w <= 0 || h <= 0) return img;

	const auto& data = map.data();
	const int expected = w * h;
	if ((int)data.size() < expected) 
    {
		img.fill(127);
		return img;
	}

	for (int y = 0; y < h; ++y) 
    {
		uchar* line = img.scanLine(h - 1 - y); // flip Y
		const int rowOff = y * w;
		for (int x = 0; x < w; ++x) 
        {
			const int8_t occ = data[rowOff + x];
			uchar v = 127; // unknown
			if (occ >= 0) 
            {
				int clamped = occ;
				if (clamped > 100) clamped = 100;
				v = static_cast<uchar>(255 - (clamped * 255) / 100);
			}
			line[x] = v;
		}
	}
	return img;
}


class RomMapWidget : public QGraphicsView 
{
    Q_OBJECT
public:
    RomMapWidget(QWidget* parent);

    // Update the displayed occupancy grid; cheap if same size.
    void setMap(const RomMap& map);
    // Parse ROS OccupancyGrid JSON (from rosbridge) and update the map view
    void updateMap(const QJsonObject& msg);
    
    void clearMap();

    // Fit the current map pixmap to the view keeping aspect ratio.
    void fitToView();

    // Optional: control whether the view auto-fits when a new map size arrives
    void setAutoFitEnabled(bool enabled);
    bool autoFitEnabled() const;

signals:
    // void waypointPlaced();

protected:
    void resizeEvent(QResizeEvent* e) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QGraphicsScene* scene_{nullptr};
    QGraphicsPixmapItem* mapItem_{nullptr};
    QGraphicsItem* viewAxes_{nullptr};
    QGraphicsItem* mapOriginAxes_{nullptr};
    
    QSize lastMapSize_{};
    bool autoFit_{true};
    bool userZoomed_{false};
    double minScale_{0.1};
    double maxScale_{10.0};

    // Cache for conversions between image(scene) and world
    int mapWidth_{0};
    int mapHeight_{0};
    double mapResolution_{0.0};
    double mapOriginX_{0.0};
    double mapOriginY_{0.0};
};


}
#endif

