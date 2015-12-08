/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QtOpenGL>
#include "facePCA.h"
#include "clm.h"

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE
class GLWidget;


class MyQLabel: public QLabel
{
	Q_OBJECT
public:
	MyQLabel();
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	inline void currentPixmap(QPixmap _pixmap){ m_pixmap = _pixmap;};
	QPixmap m_pixmap;
	QPoint lastPos;
	bool b_pickmode;
	int pickId;
	int currentView;   // 0 side view  1 profile view

	std::vector<QPoint> pickedPoints;
signals:
	void targetPointsPicked();

};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();


private slots:
	void loadData();
	void loadImage();
	void showImage();  //display loaded image
    void renderIntoPixmap();
    void grabFrameBuffer();
    void getFeaturePointsID();
	void projectModel();
	void pasteTexture();
	void saveToPLY();
	void saveProject();
	void loadProject();

	void fitLighting();
	void fitModelXY();
	void fitModelZ();
	void resetModel();

    void about();
	void featurePointsONOFF();
	void monoImageModeONOFF();
	void axisONOFF();
	void textureONOFF();
	void specularONOFF();

	void diffImages(); //subtract rendered image from input image (MONO image)
	void updateDiffImage(); //update diff image
	void highlightSegment();  //for high lighting segements.
	void pauseTimer();
	void trackGL();
	void showFeaturePointIndex();
	void getFrontviewParameters();
	void getSideviewParameters();
	void getProfileviewParameters();

	void featurePointPickMode();
	void targetSidePointPickMode();
	void targetProfilePointPickMode();

	void updateTargetPoints();
	
	void blendEar();

	void testFunction();
	void demo();
private:
    void createActions();
    void createMenus();
    QSlider *createSlider(const char *changedSignal, const char *setterSlot);
    void setPixmap(const QPixmap &pixmap, int ind);
    QSize getSize();
	QPixmap overlayImages(QImage im1, QImage im2);


    QWidget *centralWidget;
    QScrollArea *glWidgetArea;
    QScrollArea *pixmapLabelArea;
	QScrollArea *pixmapLabelArea2;
	QScrollArea *pixmapLabelArea3;

    GLWidget *glWidget;
	//GLWidget *glWidget2;

    MyQLabel *pixmapLabel;
	MyQLabel *pixmapLabel2;
	MyQLabel *pixmapLabel3;
    QSlider *xSlider;
    QSlider *ySlider;
    QSlider *zSlider;

	QSlider *ambLightSlider;
	QSlider *lightPosSliderX;
	QSlider *lightPosSliderY;
	

    QMenu *fileMenu;
	QMenu *fittingMenu;
	QMenu *featurepointMenu;
	QMenu *testMenu;
    QMenu *helpMenu;
	
	QAction *loadDataAct;
	QAction *loadProjectAct;
    QAction *grabFrameBufferAct;
    QAction *loadImageAct;
    QAction *getFeaturePointsIDAct;   //to be removed
	QAction *featurePointsONOFFAct;
	QAction *monoImageModeONOFFAct;
	QAction *axisONOFFAct;
	QAction *textureONOFFAct;
	QAction *specularONOFFAct;  //to be implemented

	QAction *projectModelAct;
	QAction *pasteTextureAct;
	QAction *saveToPLYAct;
	QAction *saveProjectAct;
	QAction *fitLightingAct;
	QAction *fitModelXYAct;
	QAction *fitModelZAct;
	QAction *resetModelAct;
	QAction *diffImageAct;
	QAction *pauseTimerAct;
	QAction *trackGLAct;
	QAction *highlightSegmentAct;
	QAction *getFrontMatrixAct;
	QAction *getSideMatrixAct;
	QAction *getProfileMatrixAct;
	QAction *blendEarAct;

	QAction *targetSidePointPickModeAct;
	QAction *targetProfilePointPickModeAct;
	QAction *testAct;

    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

	QImage m_subjectImage;
	QImage m_renderedImage;

	QImage m_subjectImageMono;
	QImage m_renderedImageMono;

	std::vector<cv::Point> m_subjectFeaturePoints;
	std::vector<cv::Point> m_subjectFeaturePointsContour;  //only uses first 10 elements.
	std::vector<cv::Point> m_subjectFeaturePointsProfile;  //10 elements

	bool b_backgroundloaded;
	bool b_monoImageMode;

	CFaceTracker m_FaceTrackerGL;
	CFaceTracker m_FaceTrackerIM;

	QTimer *timer;
	QTimer *timer_trackGL;
	QTimer *timer_image;
	QTimer *timer_demo;

	bool isIncreasing;
};

#endif
