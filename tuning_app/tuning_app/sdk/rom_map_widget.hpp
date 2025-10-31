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
#include <QListWidget>
#include <QMouseEvent>
#include <QColor>

namespace rom_dynamics::ui::qt 
{

using rom_dynamics::data_types::RomMap;

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


inline RomMapWidget::RomMapWidget(QWidget* parent) : QGraphicsView(parent)
    {
        setFrameShape(QFrame::NoFrame);
        setBackgroundBrush(QColor("#2a2f36"));
        setRenderHint(QPainter::Antialiasing, false);
        setRenderHint(QPainter::SmoothPixmapTransform, true);
        setRenderHint(QPainter::TextAntialiasing, true);
        setDragMode(QGraphicsView::ScrollHandDrag); // basic pan for now
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

        scene_ = new QGraphicsScene(this);
        setScene(scene_);

        mapItem_ = scene_->addPixmap(QPixmap());
    }

// Update the displayed occupancy grid; cheap if same size.
inline void RomMapWidget::setMap(const RomMap& map)
    {
        QImage img = romMapToImage_local(map);
        if (img.isNull()) return;

        const QSize newSize = img.size();
        mapItem_->setPixmap(QPixmap::fromImage(img));
        mapItem_->setOffset(-img.width() / 2.0, -img.height() / 2.0); // center around (0,0)
        scene_->setSceneRect(mapItem_->boundingRect());

        // Cache metadata for conversions
        mapWidth_ = map.width();
        mapHeight_ = map.height();
        mapResolution_ = map.resolution();
        mapOriginX_ = map.originX();
        mapOriginY_ = map.originY();
        
        // if (!mapOriginAxes_) {
        //     mapOriginAxes_ = new MapOriginCoordinateFrame();
        //     scene_->addItem(mapOriginAxes_);
        //     mapOriginAxes_->setVisible(axesVisible_);
        // }
        const int w = map.width();
        const int h = map.height();
        const float res = map.resolution();
        if (w > 0 && h > 0 && res > 0.0f) 
        {
            const double ox = static_cast<double>(map.originX());
            const double oy = static_cast<double>(map.originY());
            const double pixel_x = -ox / res;
            const double pixel_y = (static_cast<double>(h) - 1.0) + oy / res;

            const double scene_x = pixel_x - static_cast<double>(w) / 2.0;
            const double scene_y = pixel_y - static_cast<double>(h) / 2.0;
            //mapOriginAxes_->setPos(scene_x, scene_y);
            //mapOriginAxes_->setVisible(axesVisible_);
        }
        // else if (mapOriginAxes_) {
        //     mapOriginAxes_->setVisible(false);
        // }

        if (autoFit_ && newSize != lastMapSize_) {
            fitInView(mapItem_, Qt::KeepAspectRatio);
            lastMapSize_ = newSize;
        }
    }
    
inline void RomMapWidget::clearMap()
    {
        mapItem_->setPixmap(QPixmap());
        scene_->setSceneRect(QRectF());
        lastMapSize_ = QSize();
    }

// Fit the current map pixmap to the view keeping aspect ratio.
inline void RomMapWidget::fitToView()
    {
        if (mapItem_ && !mapItem_->pixmap().isNull()) {
            // Re-enable autoFit for this operation, and reset user zoom flag
            autoFit_ = true;
            userZoomed_ = false;
            fitInView(mapItem_, Qt::KeepAspectRatio);
        }
    }

// Optional: control whether the view auto-fits when a new map size arrives
inline void RomMapWidget::setAutoFitEnabled(bool enabled) { autoFit_ = enabled; }

inline bool RomMapWidget::autoFitEnabled() const { return autoFit_; }

inline void RomMapWidget::mousePressEvent(QMouseEvent* event)
{
    if (!mapItem_ || mapItem_->pixmap().isNull()) {
		QGraphicsView::mousePressEvent(event);
		return;
	}
	if (event->button() != Qt::LeftButton) {
		QGraphicsView::mousePressEvent(event);
		return;
	}

	// Begin waypoint placement/rotation
	const QPointF scenePos = mapToScene(event->pos());
    //waypointPressScene_ = scenePos;
    //waypointDragging_ = true;
    //waypointAngleDeg_ = 0.0;

	// Create preview icon if needed
    // if (!waypointPreview_) {
    // 	QPixmap pix(":/icons/wp_waypoint.png");
    // 	if (!pix.isNull()) {
    // 		const int W = 25, H = 25;
    // 		QPixmap scaled = pix.scaled(W, H, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 		waypointPreview_ = scene_->addPixmap(scaled);
    // 		waypointPreview_->setTransformationMode(Qt::SmoothTransformation);
    // 		waypointPreview_->setOffset(-W/2.0, -H/2.0);
    // 		waypointPreview_->setZValue(990.0);
    // 		waypointPreview_->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    // 	}
    // }
    // if (waypointPreview_) {
    // 	waypointPreview_->setPos(scenePos);
    // 	waypointPreview_->setRotation(0.0);
    // 	waypointPreview_->setVisible(true);
    // }
	event->accept();
}

inline void RomMapWidget::mouseMoveEvent(QMouseEvent* event)
{
    // if (!waypointModeEnabled_ || !waypointDragging_) {
    // 	QGraphicsView::mouseMoveEvent(event);
    // 	return;
    // }
    //const QPointF scenePos = mapToScene(event->pos());
    //const QPointF d = scenePos - waypointPressScene_;
    //double angleRad = std::atan2(d.y(), d.x()); // scene coords: +x right, +y down
    //double angleDeg = angleRad * 180.0 / M_PI;
    // waypointAngleDeg_ = angleDeg; // image angle convention
    // if (waypointPreview_) {
    // 	waypointPreview_->setPos(waypointPressScene_);
    // 	waypointPreview_->setRotation(waypointAngleDeg_);
    // }
    //event->accept();
}

inline void RomMapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    // if (!waypointModeEnabled_ || !waypointDragging_) {
    // 	QGraphicsView::mouseReleaseEvent(event);
    // 	return;
    // }
	
    // if (event->button() != Qt::LeftButton) {
    // 	QGraphicsView::mouseReleaseEvent(event);
    // 	return;
    // }

	// Finalize waypoint placement
    //const QPointF scenePos = waypointPressScene_;
    //const double image_x = scenePos.x();
    //const double image_y = scenePos.y();
    //const double image_theta_deg = waypointAngleDeg_;

	// Convert to world (map meters). Convert image deg to world yaw radians.
    //const QPointF w = sceneToWorld_mapFrame(image_x, image_y, mapWidth_, mapHeight_, mapResolution_, mapOriginX_, mapOriginY_);
	
	// Inverse of Option A from RomRobotUi::imagePoseToTfPose: robot_theta_deg = -image_theta_deg
    //const double world_theta_rad = (-image_theta_deg) * M_PI / 180.0;

    //emit waypointPlaced(image_x, image_y, image_theta_deg, w.x(), w.y(), world_theta_rad);

	// Keep the preview visible at the placed location until next interaction (optional)
	// Or hide it after placement; here we hide to reduce clutter
    // if (waypointPreview_) {
    // 	waypointPreview_->setVisible(false);
    // }
    // waypointDragging_ = false;
    // event->accept();
}

inline void RomMapWidget::resizeEvent(QResizeEvent* e)
{
    // QGraphicsView::resizeEvent(e);
    // if (autoFit_ && mapItem_ && !mapItem_->pixmap().isNull()) {
    // 	fitInView(mapItem_, Qt::KeepAspectRatio);
    // }
}

inline void RomMapWidget::wheelEvent(QWheelEvent* event)
{
    // if (!mapItem_ || mapItem_->pixmap().isNull()) {
    // 	QGraphicsView::wheelEvent(event);
    // 	return;
    // }

    // // Disable auto-fit after the user starts zooming manually
    // autoFit_ = false;
    // userZoomed_ = true;

    // constexpr double zoomInFactor = 1.15;
    // constexpr double zoomOutFactor = 1.0 / zoomInFactor;

    // const double factor = (event->angleDelta().y() > 0) ? zoomInFactor : zoomOutFactor;

    // // Compute new scale while clamping to min/max
    // QTransform t = transform();
    // // current uniform scale assuming no shear
    // const double currentScale = std::sqrt(t.m11() * t.m11() + t.m12() * t.m12());
    // double newScale = currentScale * factor;
    // if (newScale < minScale_) newScale = minScale_;
    // if (newScale > maxScale_) newScale = maxScale_;

    // // Apply by resetting scale to target factor relative to current
    // const double applyFactor = newScale / currentScale;
    // scale(applyFactor, applyFactor);

    // event->accept();
}

}
#endif

