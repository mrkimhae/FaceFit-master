/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/



#include "glwidget.h"
#include "mainwindow.h"
#include "facePCA.h"
#include <string>
#include "qtcvutility.h"

MyQLabel::MyQLabel()
{
	b_pickmode = false;
}

void MyQLabel::mousePressEvent(QMouseEvent *event)
{
	
	if (event->button() == Qt::LeftButton)
	{
		lastPos = event->pos();
		QColor c = m_pixmap.toImage().pixel(lastPos);
		qDebug() << c.red() << " "<<c.green()<<" " << c.blue();

		if(b_pickmode)
		{
			if (pickId < 11)
			{
				pickedPoints.push_back(lastPos);
				pickId ++ ;
				if (pickId == 11)
				{
					qDebug() << "Picked 10 contour points, exit pick mode...";
					b_pickmode = false;
					emit targetPointsPicked();
				}
			}
			else
			{
				//qDebug() << "Picked 10 contour points, exit pick mode...";
				b_pickmode = false;
			}

		}

	}
}

void MyQLabel::mouseMoveEvent(QMouseEvent *event)
{
	
	
}

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    glWidget = new GLWidget(this);
	glWidget->grabKeyboard();


    pixmapLabel = new MyQLabel;
	pixmapLabel2 = new MyQLabel;
	pixmapLabel3 = new MyQLabel;


    glWidgetArea = new QScrollArea;
    glWidgetArea->setWidget(glWidget);
    glWidgetArea->setWidgetResizable(true);
    glWidgetArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    glWidgetArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    glWidgetArea->setMinimumSize(50, 50);

    pixmapLabelArea = new QScrollArea;
    pixmapLabelArea->setWidget(pixmapLabel);
	pixmapLabelArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pixmapLabelArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pixmapLabelArea->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    pixmapLabelArea->setMinimumSize(50, 50);

	pixmapLabelArea2 = new QScrollArea;
    pixmapLabelArea2->setWidget(pixmapLabel2);
	pixmapLabelArea2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pixmapLabelArea2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pixmapLabelArea2->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    pixmapLabelArea2->setMinimumSize(50, 50);

	pixmapLabelArea3 = new QScrollArea;
    pixmapLabelArea3->setWidget(pixmapLabel3);
	pixmapLabelArea3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	pixmapLabelArea3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    pixmapLabelArea3->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    pixmapLabelArea3->setMinimumSize(50, 50);

    //xSlider = createSlider(SIGNAL(xRotationChanged(int)),
    //                      SLOT(setXRotation(int)));
    //ySlider = createSlider(SIGNAL(yRotationChanged(int)),
    //                       SLOT(setYRotation(int)));
    //zSlider = createSlider(SIGNAL(zRotationChanged(int)),
    //                       SLOT(setZRotation(int)));

	ambLightSlider = createSlider(SIGNAL(camPosChanged(int)),
                           SLOT(setAmbLightValue(int)));
	ambLightSlider->setRange(0, 100);

	lightPosSliderX = createSlider(SIGNAL(lightPosChanged(int)),
                          SLOT(setLightPositionX(int)));
	lightPosSliderY = createSlider(SIGNAL(lightPosChanged(int)),
                          SLOT(setLightPositionY(int)));
	lightPosSliderY->setRange(-100,100);


    createActions();
    createMenus();

    QGridLayout *centralLayout = new QGridLayout;
    centralLayout->addWidget(glWidgetArea, 0, 0);
    centralLayout->addWidget(pixmapLabelArea, 0, 1);
	centralLayout->addWidget(pixmapLabelArea2, 1, 0);
	centralLayout->addWidget(pixmapLabelArea3, 1, 1);

    centralLayout->addWidget(lightPosSliderX, 2, 0);
	centralLayout->addWidget(lightPosSliderY, 2, 1);
    //centralLayout->addWidget(ySlider, 2, 0, 1, 2);
    //centralLayout->addWidget(zSlider, 3, 0, 1, 2);
	centralLayout->addWidget(ambLightSlider, 3, 0, 1,2);
    centralWidget->setLayout(centralLayout);

    lightPosSliderX->setValue(180);
	ambLightSlider->setValue(0);
    //ySlider->setValue(345 * 16);
    //zSlider->setValue(0 * 16);

    setWindowTitle(tr("FaceFit"));
    resize(1200, 960);

	b_backgroundloaded = false;
	b_monoImageMode =false;
	
	m_FaceTrackerGL = CFaceTracker();   //opengl rendered image face tracker.
	m_FaceTrackerIM = CFaceTracker();   //input image face tracker.

	timer = new QTimer(this);
	timer_image = new QTimer(this);
	timer_trackGL = new QTimer(this);
	timer_demo = new QTimer(this);
	isIncreasing = false;

	connect(timer, SIGNAL(timeout()), this, SLOT(grabFrameBuffer()));
	connect(timer_image, SIGNAL(timeout()), this, SLOT(showImage()));
	connect(timer_trackGL, SIGNAL(timeout()), this, SLOT(trackGL()));
	connect(glWidget, SIGNAL(LightChanged()), this, SLOT(updateDiffImage()));
	connect(pixmapLabel3, SIGNAL(targetPointsPicked()), this, SLOT(updateTargetPoints()));
	connect(timer_demo, SIGNAL(timeout()), this, SLOT(demo()));
}

void MainWindow::loadData()
{
	this->glWidget->loadData();
	
}

void MainWindow::renderIntoPixmap()
{
    QSize size = getSize();
    if (size.isValid()) {
        QPixmap pixmap = glWidget->renderPixmap(size.width(), size.height());
        setPixmap(pixmap,1);
    }
}


void MainWindow::getFeaturePointsID()
{
	/*QImage image = glWidget->grabFrameBuffer();
	m_renderedImage = image.copy();
	QImage imagebackup = image.copy();
    QImage result_image;
	m_FaceTrackerGL.DetectCurrentImage(image, result_image);*/

	glWidget->m_featurePoints = m_FaceTrackerGL.GetFeaturePoints();
    glWidget->getFeaturePointTriangleIDs();
	WriteToFile("featurePtsCountour.txt", glWidget->m_featurePointsTriangleID);
}

void MainWindow::about()
{
	glWidget->myProjectFeaturePoints();
	//glWidget->projectFeaturePoints();
	//glWidget->b_needProject = true;

	for (int i = 0; i < 10; i++)
	{
		qDebug() << glWidget->m_projectedFeaturePoints[i].x << " " << glWidget->m_projectedFeaturePoints[i].y << endl;
		qDebug() << glWidget->m_myProjectedFeaturePoints[i].x << " " << glWidget->m_myProjectedFeaturePoints[i].y << endl;
	}

	qDebug() << "testing done" << endl;
}

void MainWindow::createActions()
{
	loadProjectAct = new QAction(tr("&Load Project"), this);
	connect(loadProjectAct, SIGNAL(triggered()), this, SLOT(loadProject()));

    loadImageAct = new QAction(tr("&Load Image..."), this);
    loadImageAct->setShortcut(tr("Ctrl+R"));
    connect(loadImageAct, SIGNAL(triggered()),
		this, SLOT(loadImage()));

    grabFrameBufferAct = new QAction(tr("&Overlay Model to Image"), this);
    grabFrameBufferAct->setShortcut(tr("Ctrl+G"));
    connect(grabFrameBufferAct, SIGNAL(triggered()),
            this, SLOT(grabFrameBuffer()));

    getFeaturePointsIDAct = new QAction(tr("&Get Feature Points Triangle ID"), this);		//to be removed
    getFeaturePointsIDAct->setShortcut(tr("Ctrl+L"));										//to be removed
    connect(getFeaturePointsIDAct, SIGNAL(triggered()), this, SLOT(getFeaturePointsID()));	//to be removed

	projectModelAct = new QAction(tr("&Project Model to Image"), this);
	connect(projectModelAct, SIGNAL(triggered()), this, SLOT(projectModel()));

	pasteTextureAct = new QAction(tr("&Paste Texture from Image"), this);
	connect(pasteTextureAct, SIGNAL(triggered()), this, SLOT(pasteTexture()));

	saveToPLYAct = new QAction(tr("&Save Model to PLY File"), this);
	connect(saveToPLYAct, SIGNAL(triggered()), this, SLOT(saveToPLY()));

	saveProjectAct = new QAction(tr("&Save Current Project"), this);
	connect(saveProjectAct, SIGNAL(triggered()), this, SLOT(saveProject()));

	resetModelAct = new QAction(tr("&Reset Model"), this);
	connect(resetModelAct, SIGNAL(triggered()), this, SLOT(resetModel()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	featurePointsONOFFAct = new QAction(tr("Show/Hide Feature Points"), this);
    connect(featurePointsONOFFAct, SIGNAL(triggered()), this, SLOT(featurePointsONOFF()));

	monoImageModeONOFFAct = new QAction(tr("Mono/Color Images"), this);
    connect(monoImageModeONOFFAct, SIGNAL(triggered()), this, SLOT(monoImageModeONOFF()));

	axisONOFFAct = new QAction(tr("Show/Hide AXIS"), this);
    connect(axisONOFFAct, SIGNAL(triggered()), this, SLOT(axisONOFF()));

	textureONOFFAct = new QAction(tr("Show/Hide Texture"), this);
    connect(textureONOFFAct, SIGNAL(triggered()), this, SLOT(textureONOFF()));

	specularONOFFAct = new QAction(tr("ON/OFF Specular Light"), this);
    connect(specularONOFFAct, SIGNAL(triggered()), this, SLOT(specularONOFF()));

	fitModelXYAct = new QAction(tr("Start Fitting Model"), this);
	connect(fitModelXYAct, SIGNAL(triggered()), this, SLOT(fitModelXY()));

	fitModelZAct = new QAction(tr("Start Fitting Details"), this);
	connect(fitModelZAct, SIGNAL(triggered()), this, SLOT(fitModelZ()));

	fitLightingAct = new QAction(tr("Start Fitting Lighting"), this);
	connect(fitLightingAct, SIGNAL(triggered()), this, SLOT(fitLighting()));

	diffImageAct = new QAction(tr("Show Diff Image"), this);
	connect(diffImageAct, SIGNAL(triggered()), this, SLOT(diffImages()));

	highlightSegmentAct = new QAction(tr("Highlight Segment"), this);
	connect(highlightSegmentAct, SIGNAL(triggered()), this, SLOT(highlightSegment()));

	pauseTimerAct = new QAction(tr("Pasue Timer"), this);
	connect(pauseTimerAct, SIGNAL(triggered()), this, SLOT(pauseTimer()));

	trackGLAct = new QAction(tr("Track GL"), this);
	connect(trackGLAct, SIGNAL(triggered()), this, SLOT(trackGL()));

	getFrontMatrixAct = new QAction(tr("Get Frontal Matrix"), this);
	connect(getFrontMatrixAct, SIGNAL(triggered()), this, SLOT(getFrontviewParameters()));

	getSideMatrixAct = new QAction(tr("Get Side Matrix"), this);
	connect(getSideMatrixAct, SIGNAL(triggered()), this, SLOT(getSideviewParameters()));

	getProfileMatrixAct = new QAction(tr("Get Profile Matrix"), this);
	connect(getProfileMatrixAct, SIGNAL(triggered()), this, SLOT(getProfileviewParameters()));

	testAct = new QAction(tr("testing Function"), this);
	connect(testAct, SIGNAL(triggered()), this, SLOT(testFunction()));

	targetSidePointPickModeAct = new QAction(tr("Pick Side Target Points"), this);
	connect(targetSidePointPickModeAct, SIGNAL(triggered()), this, SLOT(targetSidePointPickMode()));

	
	targetProfilePointPickModeAct = new QAction(tr("Pick Profile Target Points"), this);
	connect(targetProfilePointPickModeAct, SIGNAL(triggered()), this, SLOT(targetProfilePointPickMode()));

	blendEarAct = new QAction(tr("Blend Ear"), this);
	connect(blendEarAct, SIGNAL(triggered()), this, SLOT(blendEar()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(loadImageAct);
	fileMenu->addAction(loadProjectAct);
	fileMenu->addAction(saveToPLYAct);
	fileMenu->addAction(saveProjectAct);
	fileMenu->addAction(resetModelAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	fittingMenu = menuBar()->addMenu(tr("&Fitting"));
    fittingMenu->addAction(grabFrameBufferAct);
	fittingMenu->addAction(getFrontMatrixAct);
	fittingMenu->addAction(getSideMatrixAct);
	fittingMenu->addAction(getProfileMatrixAct);
	fittingMenu->addAction(fitModelXYAct);
	fittingMenu->addAction(fitLightingAct);
	fittingMenu->addAction(fitModelZAct);
	fittingMenu->addAction(pasteTextureAct);
	fittingMenu->addAction(blendEarAct);

	featurepointMenu = menuBar()->addMenu(tr("&FeaturePoints"));
	featurepointMenu->addAction(getFeaturePointsIDAct);  //to be removed
	featurepointMenu->addAction(featurePointsONOFFAct);
	featurepointMenu->addAction(pauseTimerAct);
	featurepointMenu->addAction(trackGLAct);
	featurepointMenu->addAction(targetSidePointPickModeAct);
	featurepointMenu->addAction(targetProfilePointPickModeAct);
	

	testMenu = menuBar()->addMenu(tr("&Test"));
	testMenu->addAction(monoImageModeONOFFAct);
	testMenu->addAction(axisONOFFAct);
	testMenu->addAction(textureONOFFAct);
	testMenu->addAction(specularONOFFAct);
	testMenu->addAction(projectModelAct);
	testMenu->addAction(diffImageAct);
	testMenu->addAction(highlightSegmentAct);
	testMenu->addAction(testAct);
	
	

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

QSlider *MainWindow::createSlider(const char *changedSignal,
                                  const char *setterSlot)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 180);
    slider->setSingleStep(1);
    slider->setPageStep(1);
    //slider->setTickInterval(1);
    //slider->setTickPosition(QSlider::TicksRight);
    connect(slider, SIGNAL(valueChanged(int)), glWidget, setterSlot);
    //connect(glWidget, changedSignal, slider, SLOT(setValue(int)));
    return slider;
}

void MainWindow::setPixmap(const QPixmap &pixmap, int ind)
{
	QSize size;
	QPixmap pixmap2;
	switch (ind){
	case 1:

		pixmap2 = pixmap.scaledToWidth(pixmapLabelArea->width());

		//qDebug() << pixmap2.size() << pixmapLabelArea->size() <<endl;
		pixmapLabel->setPixmap(pixmap2);
		pixmapLabel->currentPixmap(pixmap2);
		pixmapLabel->resize(pixmap2.size());
		//pixmapLabel->resize(pixmapLabelArea3->maximumViewportSize());
		break;
	case 2:
		pixmap2 = pixmap.scaledToWidth(pixmapLabelArea2->width());

		//qDebug() << pixmap2.size() << pixmapLabelArea2->size() <<endl;
		pixmapLabel2->setPixmap(pixmap2);
		pixmapLabel2->currentPixmap(pixmap2);
		pixmapLabel2->resize(pixmap2.size());
		//std::cout << "case 2 " << std::endl;
		break;
	case 3:
		pixmap2 = pixmap.scaledToWidth(pixmapLabelArea3->width());
		pixmapLabel3->setPixmap(pixmap2);
		pixmapLabel3->currentPixmap(pixmap2);
		pixmapLabel3->resize(pixmapLabelArea3->maximumViewportSize());
		break;
	}	
}

QSize MainWindow::getSize()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Grabber"),
                                         tr("Enter pixmap size:"),
                                         QLineEdit::Normal,
                                         tr("%1 x %2").arg(glWidget->width())
                                                      .arg(glWidget->height()),
                                         &ok);
    if (!ok)
        return QSize();

    QRegExp regExp(tr("([0-9]+) *x *([0-9]+)"));
    if (regExp.exactMatch(text)) {
        int width = regExp.cap(1).toInt();
        int height = regExp.cap(2).toInt();
        if (width > 0 && width < 2048 && height > 0 && height < 2048)
            return QSize(width, height);
    }

    return glWidget->size();
}

void MainWindow::loadImage()
{
	QString filename = QFileDialog::getOpenFileName(this,  tr("Open Image"), " ", tr("Image Files (*.png *.jpg *.bmp)"));
	if (filename == NULL)
		return;
	m_subjectImage = QImage(filename);
	qDebug() << filename << endl;
	if (m_subjectImage.isNull())
	{
		qDebug() << "Failed to Load Image" << endl;
		return;
	}
	m_subjectImage = m_subjectImage.scaledToWidth(pixmapLabelArea->width());

	if (!timer_image->isActive())
	{
		timer_image->start(300);
	}

	//showImage();
	

	b_backgroundloaded = true;
}


void MainWindow::showImage()
{
	
	QImage tmpImage = m_subjectImage.copy(); //memory mark
	
	QImage resultImage;
	m_FaceTrackerIM.DetectCurrentImage(tmpImage, resultImage);
	if (resultImage.isNull() == true)   //check null
		setPixmap(QPixmap::fromImage(tmpImage), 1);
	else
		setPixmap(QPixmap::fromImage(resultImage), 1);
	
	m_subjectFeaturePoints = m_FaceTrackerIM.GetFeaturePoints();
	

}

void MainWindow::updateTargetPoints()   //manually select contour points
{
	if (pixmapLabel3->currentView == 0)
	{
		if (pixmapLabel3->pickedPoints.size() > 0)
		{
			for (int i = 0; i < pixmapLabel3->pickedPoints.size(); i++)
			{
				cv::Point pt;
				pt.x  = pixmapLabel3->pickedPoints[i].x();
				pt.y = pixmapLabel3->pickedPoints[i].y();
				m_subjectFeaturePointsContour.push_back(pt);
			}
			qDebug() << "Target Points Updated...";
			qDebug() << m_subjectFeaturePointsContour.size();
			QImage tmpImage = m_subjectImage.copy();
		
			for (int i = 0; i < m_subjectFeaturePointsContour.size(); i++)
			{
				tmpImage.setPixel(m_subjectFeaturePointsContour[i].x, m_subjectFeaturePointsContour[i].y, qRgb(0,0,255));
			}
			setPixmap(QPixmap::fromImage(tmpImage), 1);
		}
	}
	else
	{
		if (pixmapLabel3->pickedPoints.size() > 0)
		{
			for (int i = 0; i < pixmapLabel3->pickedPoints.size(); i++)
			{
				cv::Point pt;
				pt.x  = pixmapLabel3->pickedPoints[i].x();
				pt.y = pixmapLabel3->pickedPoints[i].y();
				m_subjectFeaturePointsProfile.push_back(pt);
			}
			qDebug() << "Target Points Updated...";
			qDebug() << m_subjectFeaturePointsProfile.size();
			QImage tmpImage = m_subjectImage.copy();
		
			for (int i = 0; i < m_subjectFeaturePointsProfile.size(); i++)
			{
				tmpImage.setPixel(m_subjectFeaturePointsProfile[i].x, m_subjectFeaturePointsProfile[i].y, qRgb(255,0,0));
			}
			setPixmap(QPixmap::fromImage(tmpImage), 1);
		}
	}
}

void MainWindow::saveToPLY()
{
	QString filename = QFileDialog::getSaveFileName(this,  tr("Save Model As PLY File"), " ", tr("PLY Files (*.ply)"));
	if (filename == NULL)
		return;
	glWidget->writeToPLYFile(filename);
}

void MainWindow::resetModel()
{
	glWidget->resetModel();
}

void MainWindow::pauseTimer()
{
	//qDebug() << "stop";
	timer_image->stop();
	timer_trackGL->stop();
}

void MainWindow::featurePointsONOFF()
{
	if (glWidget->b_showHighlight == true)
		glWidget->b_showHighlight = false;
	else
	{
		glWidget->b_showHighlight = true;
		showFeaturePointIndex();
	}
	
	
	
}


void MainWindow::axisONOFF()
{
	if (glWidget->b_showAxis == true)
		glWidget->b_showAxis = false;
	else
		glWidget->b_showAxis = true;
	
}

void MainWindow::textureONOFF()
{
	glWidget->hideTexture();
}

void MainWindow::specularONOFF()
{
	glWidget->specularON();
}

void MainWindow::monoImageModeONOFF()
{
	if (b_monoImageMode == true)
	{
		b_monoImageMode = false;
		setPixmap(QPixmap::fromImage(m_subjectImage), 1);
	}
	else
	{
		b_monoImageMode = true;
		QImageColorToGray(m_subjectImage, m_subjectImageMono);
		setPixmap(QPixmap::fromImage(m_subjectImageMono), 1);
	}
}

QPixmap MainWindow::overlayImages(QImage background, QImage forground)
{

	QImage im1 = background.convertToFormat(QImage::Format_ARGB32_Premultiplied);
	QImage im2 = forground.convertToFormat(QImage::Format_ARGB32_Premultiplied);


	QPixmap backpix = QPixmap::fromImage(im1);
	QPixmap forpix = QPixmap::fromImage(im2);

	//backpix = backpix.scaled(forpix.size());

	QPixmap resultpixmap(forpix.width(),forpix.height());

	resultpixmap.fill(Qt::transparent);
	QPainter p(&resultpixmap);
	//p.setOpacity(0.9);
	p.drawPixmap(0, 0, backpix);
	p.setOpacity(0.5);
	p.drawPixmap(0, 0, forpix);
	p.end();

	return resultpixmap;
}

void MainWindow::grabFrameBuffer()
{
    QImage image = glWidget->grabFrameBuffer();
	m_renderedImage = image.copy();
	QImage imagebackup = image.copy();
    QImage result_image;
	m_FaceTrackerGL.DetectCurrentImage(image, result_image);
    
	if (b_backgroundloaded)
	{
		QPixmap overlayedpixmap = overlayImages(m_subjectImage, imagebackup);
		setPixmap(overlayedpixmap, 3);

		if (!timer->isActive())
		{
			 timer->start(50);
		}
       
	}

	if (b_monoImageMode)
	{
		QImageColorToGray(m_renderedImage, m_renderedImageMono);
		result_image = m_renderedImageMono;
	}

	if (result_image.isNull() == true)
		setPixmap(QPixmap::fromImage(image), 2);
	else
		setPixmap(QPixmap::fromImage(result_image), 2);

}


void MainWindow::trackGL()
{
	QImage image = glWidget->grabFrameBuffer();
	m_renderedImage = image.copy();
	QImage imagebackup = image.copy();
    QImage result_image;
	m_FaceTrackerGL.DetectCurrentImage(image, result_image);
    
	

	if (!timer_trackGL->isActive())
	{
		 timer_trackGL->start(50);
	}
       
	

	setPixmap(QPixmap::fromImage(result_image), 2);

}


void MainWindow::showFeaturePointIndex()
{
	glWidget->projectFeaturePoints();
	
	//glWidget->m_projectedFeaturePoints;
	QImage image = glWidget->grabFrameBuffer();
	cv::Mat im;
	QImageToCvMat(image, im);
	//cv::imshow("feature point indices1", im);

	//qDebug() <<  glWidget->m_projectedFeaturePoints.size();
	for (int i = 0; i < glWidget->m_projectedFeaturePoints.size(); i++)
	{
		//qDebug() << glWidget->m_projectedFeaturePoints[i].x << glWidget->m_projectedFeaturePoints[i].y;
		cv::putText(im, to_string(i), glWidget->m_projectedFeaturePoints[i],  FONT_HERSHEY_SCRIPT_SIMPLEX, 0.3, cvScalar(200,200,250), 1, 8);
	}
	
	cv::imshow("feature point indices", im);
}


void MainWindow::projectModel()
{
	glWidget->projectVertices();
	std::cout << "Projection Done" << std::endl;

	//projection test///////////////////////////////////////////////////////////////////

	//QImage image = glWidget->grabFrameBuffer();
	//QImage imagebackup = QImage(image.width(),image.height(),QImage::Format_ARGB32_Premultiplied);
	//// comment out feature point projection
	////std::vector<cv::Point> fps = glWidget->m_FaceTracker.GetFeaturePoints();

	////glWidget->setFeaturePoints(fps);
	//std::cout << imagebackup.size().width() << " " << imagebackup.height() << endl; 
	//std::vector<cv::Point> projFeaturePoints = glWidget->m_projectedVertices;

	//for (int i = 0; i < projFeaturePoints.size(); i++)
	//{
	//	imagebackup.setPixel(projFeaturePoints[i].x, projFeaturePoints[i].y, qRgb(1,1,1));
	//}
	//setPixmap(QPixmap::fromImage(imagebackup), 3);
	/////////////////////////////////////////////////////////////////////////////////////

}

void MainWindow::getFrontviewParameters()
{
	//pauseTimer();
	glWidget->saveFrontMatrix();
	m_subjectFeaturePoints = m_FaceTrackerIM.GetFeaturePoints();
	qDebug() << "Frontal View Parameters Saved...";
}
void MainWindow::getSideviewParameters()
{
	glWidget->saveSideMatrix();
	//m_subjectFeaturePointsContour = m_FaceTrackerIM.GetFeaturePoints();   
	qDebug() << "Side View Parameters Saved...";
}

void MainWindow::getProfileviewParameters()
{
	glWidget->saveProfileMatrix();
	//m_subjectFeaturePointsContour = m_FaceTrackerIM.GetFeaturePoints();   
	qDebug() << "Profile View Parameters Saved...";
}



void MainWindow::pasteTexture()
{
	blendEar();
	glWidget->projectVertices();
	if (!glWidget->b_isProjectionDone)
	{
		std::cout << "Projection in process, please wait and try again later" << std::endl;
	}
	else
	{
		if (!m_subjectImage.isNull())
		{
			if(b_monoImageMode)
			{
				QImageColorToGray(m_subjectImage, m_subjectImageMono);
				glWidget->pasteTexture(m_subjectImageMono);
			}
			else
				glWidget->pasteTexture(m_subjectImage);
		
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			/*QImage image = glWidget->grabFrameBuffer();
			QImage imagebackup = QImage(image.width(),image.height(),QImage::Format_ARGB32_Premultiplied);
			std::vector<cv::Point> projFeaturePoints = glWidget->m_projectedVertices;

			for (int i = 0; i < projFeaturePoints.size(); i++)
			{
				imagebackup.setPixel(projFeaturePoints[i].x, projFeaturePoints[i].y, qRgb(glWidget->m_FaceModel.ave_text[i][0],glWidget->m_FaceModel.ave_text[i][1],glWidget->m_FaceModel.ave_text[i][2]));
			}
			setPixmap(QPixmap::fromImage(imagebackup), 3);*/
			/////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		else
			std::cout << "Please Load Subject Image Before Texture Pasting" << std::endl;
	}
}

void MainWindow::fitModelXY()
{
	bool ok;
	int thd = QInputDialog::getInt(this, tr("Set Threshold"),
                                         tr("Value:"), 25000, 10000, 35000, 1000, &ok);

	if (ok == true)
	{
		std::cout << "Fitting Model...." << std::endl;
		double dist = 999999;
		double predist;
		for(int i = 0; i < 20; i++)
		{
			predist = dist;
			dist = glWidget->updateModel(this->m_subjectFeaturePoints, this->m_subjectFeaturePointsContour, this->m_subjectFeaturePointsProfile);
			qDebug() << "Distance: " << dist;
		    //qDebug() << dist - predist;
			if (1)
			{
				QImage im = glWidget->grabFrameBuffer();
				QString fn = QString::number(i) + ".png";
				QImageWriter writer(fn, "png");
				writer.write(im);

			}
			if ((dist < thd)||(predist - dist < 1000 ))
			   break;
		}
		std::cout << "Done" << std::endl;
		pasteTexture();
	}
	else
		qDebug() << "Fitting Canceled";
}

void MainWindow::fitModelZ()
{
	std::cout << "Fitting Model Details...." << std::endl;
	QImage testImage;
	//double error = glWidget->updateModelZ(m_subjectImage, testImage);
	double error = glWidget->updateModelZMethod2(m_subjectImage,testImage);
	setPixmap(QPixmap::fromImage(testImage), 2);
	//double error = glWidget->updateModelZMethod2(m_subjectImage);
	std::cout << "Done" << std::endl;
	
}


void MainWindow::fitLighting()
{
	glWidget->optimizeDirectionLight(m_subjectImage);
	glWidget->optimizeAmbLight(m_subjectImage);

}


void MainWindow::diffImages()
{
	if (m_subjectImage.isNull())
	{
		std::cout << "Please Load Subject Image Before Diff." << std::endl;
		return;
	}

	QImage image = glWidget->grabFrameBuffer();
	//m_renderedImage = image.copy();
	
	QImage resultImage;

	cv::Mat diffim = DiffQImages(image, m_subjectImage, resultImage);
	cv::imshow("test" , diffim);
	glWidget->projectVertices();
	glWidget->m_FaceModel.updateAlbedo(glWidget->m_projectedVertices, diffim);
	//setPixmap(QPixmap::fromImage(resultImage), 1);
	
}


void MainWindow::updateDiffImage()
{
	diffImages();
}

void MainWindow::saveProject()
{
	glWidget->saveCurrentProject();
}

void MainWindow::loadProject()
{
	glWidget->loadProject();
}

void MainWindow::highlightSegment()
{
	std::vector<int> index;
	bool ok;
	int segmentID = QInputDialog::getInt(this, tr("Highlight Segment ID"),
                                         tr("Segment ID:"), 0, 0, 8, 1, &ok);
	if (segmentID == 0)
		index = glWidget->m_visibleVertexID;
	else
		index = glWidget->getSegmentIndex(segmentID);
	std::vector<cv::Point> _projectPoints;
	glWidget->projectVertices(index, _projectPoints);
	QImage img = glWidget->grabFrameBuffer();
	for (int i = 0; i < _projectPoints.size(); i++)
	{
		img.setPixel(_projectPoints[i].x, _projectPoints[i].y, qRgb(255,0,0));
	}

	setPixmap(QPixmap::fromImage(img), 2);qDebug() << segmentID;

}

void MainWindow::featurePointPickMode()
{
	bool ok;
	int vertexID = QInputDialog::getInt(this, tr("Highlight Segment ID"),
                                         tr("User name:"), 0, 0, 8, 1, &ok);


}

void MainWindow::targetSidePointPickMode()
{
	qDebug() << "Target Point Pick Mode";
	pixmapLabel3->currentView = 0;
	pixmapLabel3->b_pickmode = true;
	pixmapLabel3->pickId = 0;
	pixmapLabel3->pickedPoints.clear();
	timer_image->stop();
}

void MainWindow::targetProfilePointPickMode()
{
	qDebug() << "Target Profile Point Pick Mode";
	pixmapLabel3->currentView = 1;
	pixmapLabel3->b_pickmode = true;
	pixmapLabel3->pickId = 0;
	pixmapLabel3->pickedPoints.clear();
	timer_image->stop();

}

void MainWindow::blendEar()
{
	glWidget->blendEar();
	qDebug() << "Blend Ear Done";
}

void MainWindow::testFunction()
{
	//qDebug() << "here";
	timer_demo->start(10);
	//test show feature point index
	//this->showFeaturePointIndex();


	//glWidget->m_FaceModel.findAdjacentVertex();
	//glWidget->getVertexColorTest();

	//QImage testImage;
	//glWidget->testRendering(m_subjectImage, testImage);
	//setPixmap(QPixmap::fromImage(testImage), 2);
	////////////////////////////////////////////////////
	// highlight segment test

	/*std::vector<int> index = glWidget->getSegmentIndex(1);
	std::vector<cv::Point> _projectPoints;
	glWidget->projectVertices(index, _projectPoints);
	QImage img = glWidget->grabFrameBuffer();
	for (int i = 0; i < _projectPoints.size(); i++)
	{
		img.setPixel(_projectPoints[i].x, _projectPoints[i].y, qRgb(255,0,0));
	}

	setPixmap(QPixmap::fromImage(img), 2);
	*/


	/////////////////////////////////////////////////////
	//closet vertex test
	//std::vector<std::vector<int>> cp = glWidget->m_FaceModel.findAdjacentVertex();
	//std::vector<int> tr = glWidget->m_FaceModel.findTriangles(1000);
	//WriteToFile("closestVertex.txt", cp);
	//WriteToFile("triangle.txt", tr);

	/////////////////////////////////////////////////////
	// cylindar coordinate test
	//vec2d cycos = glWidget->m_FaceModel.cylindarCoordinate();

	//glWidget->writeToPLYFile("test.ply", cycos);


	//////////////////////////////////////////////////////
	// this part is lighting computation test
	/*std::vector<int> visibleIndices;
	ReadFromFile("visibleIndices.txt", visibleIndices);*/
	//std::vector<int> index = glWidget->getSegmentIndex(2);

	//QImage image = glWidget->grabFrameBuffer();
	////for (int i = 0; i < glWidget->m_FaceModel.shape.size(); i++)
	//QImage im = QImage(image.size(), QImage::Format_ARGB32_Premultiplied);
	//vec2d colors = glWidget->getVertexColor();
	//
	//glWidget->projectVertices();

	//for (int i = 0; i < index.size(); i++)
	//{
	//	im.setPixel(glWidget->m_projectedVertices[index[i]].x, glWidget->m_projectedVertices[index[i]].y, qRgb(colors[index[i]][0], colors[index[i]][1], colors[index[i]][2]));
	//}

	//qDebug()<<"done";
	//setPixmap(QPixmap::fromImage(im), 2);






	///////////////////////////////////////////////////////////////////////////////////////////////
	//QImage image = glWidget->grabFrameBuffer();
	//glWidget->projectVertices(glWidget->m_featurePointsVertexID_z, glWidget->m_projectedfeaturePointZ);
	////qDebug() << glWidget->m_projectedEvaluationPoints.size();
	//HighLightPixels(image, glWidget->m_projectedfeaturePointZ);
	//setPixmap(QPixmap::fromImage(image), 2);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//DiffEvaluationPoints(image, m_subjectImage, glWidget->m_projectedEvaluationPoints);

	/*std::vector<int> visIndices = glWidget->getVisibleIndex();
	
	std::vector<cv::Point> pp;
	
	glWidget->projectVertices(visIndices, pp);
	std::cout << pp.size() << std::endl;
	QImage image = glWidget->grabFrameBuffer();
	
	HighLightPixels(image, pp);
	
	setPixmap(QPixmap::fromImage(image), 1);
	WriteToFile("visibleIndices.txt", visIndices);*/
}

void MainWindow::demo()
{
	//qDebug() << "demo test";
	double zrot = glWidget->zRotation();
	//glWidget->normalizeAngle(&zrot);
	//qDebug() << zrot << isIncreasing ;
	if (zrot == 5730)
		isIncreasing = true;
	
	if (zrot == 30)
		isIncreasing = false;

	if (isIncreasing)
		
		 glWidget-> setZRotation(zrot+0.5);
	else 
		glWidget->setZRotation(zrot-0.5);
	glWidget->updateGL();
}