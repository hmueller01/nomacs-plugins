/*******************************************************************************************************
 DkImgTransformationsPlugin.h
 Created on:	01.06.2014

 nomacs is a fast and small image viewer with the capability of synchronizing multiple instances

 Copyright (C) 2011-2014 Markus Diem <markus@nomacs.org>
 Copyright (C) 2011-2014 Stefan Fiel <stefan@nomacs.org>
 Copyright (C) 2011-2014 Florian Kleber <florian@nomacs.org>

 This file is part of nomacs.

 nomacs is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 nomacs is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 *******************************************************************************************************/

#pragma once

#include <QObject>
#include <QtPlugin>
#include <QImage>
#include <QStringList>
#include <QString>
#include <QMessageBox>
#include <QAction>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QToolbar>
#include <QMainWindow>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QListWidget>
#include <QSlider>
#include <QPushButton>
#include <QtCore/qmath.h>

#include "DkPluginInterface.h"
#include "DkNoMacs.h"
#include "DkSettings.h"
#include "DkUtils.h"
#include "DkBaseViewport.h"
#include "DkImageStorage.h"
#include "DkMath.h"
#include "DkSkewEstimator.h"

namespace nmc {

class DkImgTransformationsViewPort;
class DkImgTransformationsToolBar;
class DkInteractionRects;

enum {
	mode_scale = 0,
	mode_rotate,
	mode_shear,

	mode_end,
};

class DkImgTransformationsPlugin : public QObject, DkViewPortInterface {
    Q_OBJECT
    Q_INTERFACES(nmc::DkViewPortInterface)

public:

	DkImgTransformationsPlugin();
	~DkImgTransformationsPlugin();

	QString pluginID() const;
    QString pluginName() const;
    QString pluginDescription() const;
    QImage pluginDescriptionImage() const;
    QString pluginVersion() const;

    QStringList runID() const;
    QString pluginMenuName(const QString &runID = QString()) const;
    QString pluginStatusTip(const QString &runID = QString()) const;
    QImage runPlugin(const QString &runID = QString(), const QImage &image = QImage()) const;
	DkPluginViewPort* getViewPort();
	void deleteViewPort();

protected:
	DkPluginViewPort* viewport;

protected slots:
	void viewportDestroyed();
};

class DkImgTransformationsViewPort : public DkPluginViewPort {
	Q_OBJECT

public:

	DkImgTransformationsViewPort(QWidget* parent = 0, Qt::WindowFlags flags = 0);
	~DkImgTransformationsViewPort();

	bool isCanceled();
	QImage getTransformedImage();

public slots:
	void setPanning(bool checked);
	void applyChangesAndClose();
	void discardChangesAndClose();
	virtual void setVisible(bool visible);
	void setScaleXValue(double val);
	void setScaleYValue(double val);
	void setShearXValue(double val);
	void setShearYValue(double val);
	void setRotationValue(double val);
	void calculateAutoRotation();
	void setCropEnabled(bool enabled);

protected slots:
		
	void setMode(int mode);

protected:

	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent*event);
	void paintEvent(QPaintEvent *event);
	QPoint map(const QPointF &pos);
	virtual void init();

	bool cancelTriggered;
	bool panning;
	DkImgTransformationsToolBar* imgTransformationsToolbar;
	QCursor defaultCursor;
	DkInteractionRects* intrRect;
	QPointF scaleValues;
	QPointF shearValues;
	QPointF shearValuesTemp;
	QPointF shearValuesDir;
	bool insideIntrRect;
	int intrIdx;
	int selectedMode;
	int defaultMode;
	double rotationValue;
	double rotationValueTemp;
	QPoint referencePoint;
	QPoint rotationCenter;
	double imgRatioAngle;
	QCursor rotatingCursor;
	bool rotCropEnabled;
};


class DkImgTransformationsToolBar : public QToolBar {
	Q_OBJECT


public:

	enum {
		apply_icon = 0,
		cancel_icon,
		pan_icon,
		scale_icon,
		rotate_icon,
		shear_icon,

		icons_end,
	};

	DkImgTransformationsToolBar(const QString & title, int defaultMode, QWidget * parent = 0);
	virtual ~DkImgTransformationsToolBar();

	void setRotationValue(double val);
	void setScaleValue(QPointF val);
	void setShearValue(QPointF val);

public slots:
	void on_applyAction_triggered();
	void on_cancelAction_triggered();
	void on_panAction_toggled(bool checked);
	void on_scaleAction_toggled(bool checked);
	void on_rotateAction_toggled(bool checked);	
	void on_shearAction_toggled(bool checked);
	void on_scaleXBox_valueChanged(double val);
	void on_scaleYBox_valueChanged(double val);
	void on_shearXBox_valueChanged(double val);
	void on_shearYBox_valueChanged(double val);
	void on_rotationBox_valueChanged(double val);
	void on_cropEnabledBox_stateChanged(int val);
	void on_autoRotateButton_clicked();
	virtual void setVisible(bool visible);

signals:
	void applySignal();
	void cancelSignal();
	void scaleXValSignal(double val);
	void scaleYValSignal(double val);
	void shearXValSignal(double val);
	void shearYValSignal(double val);
	void rotationValSignal(double val);
	void calculateAutoRotationSignal();
	void cropEnabledSignal(bool enabled);
	void panSignal(bool checked);
	void modeChangedSignal(int mode);

protected:
	void createLayout(int defaultMode);
	void createIcons();
	void modifyLayout(int mode);

	QDoubleSpinBox* scaleXBox;
	QDoubleSpinBox* scaleYBox;
	QDoubleSpinBox* shearXBox;
	QDoubleSpinBox* shearYBox;
	QDoubleSpinBox* rotationBox;
	QCheckBox* cropEnabledBox;
	QPushButton* autoRotateButton;
	QMap<QString, QAction*> toolbarWidgetList;

	QAction* panAction;
	QAction* scaleAction;
	QAction* rotateAction;
	QAction* shearAction;
	QVector<QIcon> icons;		// needed for colorizing

};


class DkInteractionRects : public QWidget {
	Q_OBJECT

public:
	
	DkInteractionRects(QRect imgRect, QWidget* parent = 0, Qt::WindowFlags f = 0);
	DkInteractionRects(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~DkInteractionRects();

	void draw(QPainter* painter);
	void updateRects(QRect imgRect);
	QCursor getCursorShape(int idx);
	QVector<QRect> getInteractionRects();
	void setInitialValues(QRect rect);
	QSize getInitialSize();
	QPointF getInitialPoint(int idx);
	QPoint getCenter();

protected:

	void init();

	QVector<QRect> intrRect;
	QVector<QCursor> intrCursors;
	QVector<QPointF> initialPoints;
	QSize initialSize;

	QSize size;
};

};