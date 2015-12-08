/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/


#include <QtGui>
#include <QtOpenGL>
#include <gl\GLU.h>
#include <math.h>

#include "glwidget.h"
#include "qtcvutility.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
	m_params = new GLParameters();

    gear1 = 0;
    gear2 = 0;
    gear3 = 0;

	xRot = m_params->xRot;
	yRot = m_params->yRot;
    zRot = m_params->zRot;

    gear1Rot = 0;

	lightPos[0] =  m_params->lightPos[0];
	lightPos[1] =  m_params->lightPos[1];
	lightPos[2] =  m_params->lightPos[2];
	lightPos[3] =  m_params->lightPos[3];

	ambLightValue =  m_params->ambLightValue;

	camx =  m_params->camx;
	camy =  m_params->camy;
	camz =  m_params->camz;

	fx =  m_params->fx;
	fy =  m_params->fy;
	fz =  m_params->fz;

	modelx = 0.0;
	modely = 0.0;
	modelz = 0.0;

	nearplane = 100;
	farplane = 2000;

	
	//m_FaceTracker.Init();
	this->loadData();


	b_showAxis = false;
	b_showHighlight = false;
	b_showTexture = true;
	b_specluarON = false;

    QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(upDateGL()));
    timer->start(100);
}

GLWidget::~GLWidget()
{
    makeCurrent();
    glDeleteLists(gear1, 1);
    glDeleteLists(gear2, 1);
    glDeleteLists(gear3, 1);
}



void GLWidget::setLightPositionX(int angle)
{
	double angf = (double)angle/180 * PI;
	lightPos[0] = (float) (std::cos(angf) * 1);
	lightPos[2] = (float) (std::sin(angf) * 1);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	emit LightChanged();
    updateGL();
    
}

void GLWidget::setLightPositionY(int y)
{
	lightPos[1] = (float) y/10;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	emit LightChanged();
    updateGL();
    
}

void GLWidget::setXRotation(int angle)
{
	normalizeAngle(&angle);
    if (angle != (int)xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != (int)yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    normalizeAngle(&angle);
    if (angle != (int)zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setXRotation(double angle)
{
	normalizeAngle(&angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(double angle)
{
    normalizeAngle(&angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(double angle)
{
    normalizeAngle(&angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setCamPosition(int z)
{
    if (z != camz) {
        camz = z;
		//std::cout <<" "<< camx <<" " << camy <<" " <<camz <<std::endl;
        emit camPosChanged(z);
        updateGL();
    }
}


void GLWidget::setAmbLightValue(int x)
{
	ambLightValue = (float)x/100.0f;
	GLfloat AmbientLight[] = {ambLightValue, ambLightValue, ambLightValue}; 
	glLightfv (GL_LIGHT1, GL_AMBIENT, AmbientLight); 
	emit LightChanged();
	updateGL();
}

void GLWidget::hideTexture()
{
	glDeleteLists(face, 1);
	if (b_showTexture)
	{
		b_showTexture = false;
		face = makeFace(this->m_FaceModel);
	}
	else
	{
		b_showTexture = true;
		face = makeFace(this->m_FaceModel);
	}
}


 void GLWidget::specularON()
 {
	glDeleteLists(face, 1);
	if (b_specluarON == true)
	{
		b_specluarON = false;
		face = makeFace(this->m_FaceModel);
		qDebug() << "Specular OFF";
	}
	else
	{
		b_specluarON = true;
		face = makeFace(this->m_FaceModel);
		qDebug() << "Specular ON";
	}
 }

void GLWidget::resetModel()
{
	xRot = 90.0;
    yRot = 180.0;
    zRot = 0.0;
    gear1Rot = 0;

	camx = 0.0;
	camy = 30.0;
	camz = 280.0;

	fx = 0.0;
	fy = 0.0;
	fz = 0.0;

	modelx = 0.0;
	modely = 0.0;
	modelz = 0.0;

	nearplane = 200;
	farplane = 500;

	m_FaceModel.generateZeroCoef();
	m_FaceModel.generateModel(); 
	
	//static const GLfloat reflectance1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face = makeFace(this->m_FaceModel);
	updateGL();
}

void GLWidget::initializeGL()
{
    
    static const GLfloat reflectance1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat dlr = 0.5f,dlg = 0.5f, dlb = 0.5f;              // light 0
	GLfloat slr = 0.5f,slg = 0.5f, slb = 0.5f;              // light 1

	GLfloat DiffuseLight[] = {dlr, dlg, dlb, 1.0};
	GLfloat SpecluarLight[] = {slr, slg, slb, 1.0};
	GLfloat AmbientLight[] = {ambLightValue, ambLightValue, ambLightValue, 1.0}; 
	float As[4] = {0.1f, 0.1f, 0.1f, 1.0f };
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, As );

	glLightfv (GL_LIGHT0, GL_DIFFUSE, DiffuseLight);
	glLightfv (GL_LIGHT1, GL_AMBIENT, AmbientLight); 
	glLightfv (GL_LIGHT2, GL_SPECULAR, SpecluarLight);


    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT2, GL_POSITION, lightPos);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
    glEnable(GL_DEPTH_TEST);

	
	
	face = makeFace(this->m_FaceModel);
	aveface = makeAverageFace(reflectance1, this->m_FaceModel);
    //gear1 = makeGear(reflectance1, 1.0, 4.0, 1.0, 0.7, 20);
    //gear2 = makeGear(reflectance2, 0.5, 2.0, 2.0, 0.7, 10);
    //gear3 = makeGear(reflectance3, 1.3, 2.0, 0.5, 0.7, 10);
	
	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);    
}



void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*if (b_specluarON)
		glEnable(GL_LIGHT2);
	else
		glDisable(GL_LIGHT2);*/

    glPushMatrix();

	

	gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0);
	
	/*if (vertex_w.size() == 3)
	{
		GLUquadricObj *quadric = gluNewQuadric();
		gluQuadricNormals(quadric, GLU_SMOOTH);
		glPushMatrix();
		glTranslated(vertex_w[0], vertex_w[1], vertex_w[2]);
		glColor3d(255,0,0);
		gluSphere(quadric, 1, 10, 10);
		glPopMatrix();
		glColor3d(255,255,255);
		
	}*/
	if (b_showAxis)
    {
		drawAxis();

		glPushMatrix();
		glBegin(GL_LINES);
		glColor3d(255, 255, 255);
		glVertex3d(0, 0, 0);
		glVertex3d(100*lightPos[0], 100*lightPos[1], 100*lightPos[2]);
		glEnd();
		glPopMatrix();
		
		/*glPushMatrix();
		glBegin(GL_LINES);
		glColor3d(255, 255, 255);
		glVertex3d(0, 0, 0);
		glVertex3d(1000*0.05, 1000*0.23, 1000*0.97);


		glEnd();

		glPopMatrix();*/

		//glColor3d(255,255,255);
	}

    glRotated(xRot, 1.0, 0.0, 0.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(zRot, 0.0, 0.0, 1.0);
	
    //drawGround();
	

	glScaled( 0.5, 0.5, 0.5);
	glTranslated( modelx, modely, modelz);
	drawFace(face, 0.0, 0.0, 0.0, 0.0); 
	//drawFace(aveface, 0.0, 0.0, 0.0, 0.0);
	if (b_showHighlight)
	{
		highlightFeaturePoints();
	}
    //drawGear(gear1, 0.0, 0.0, 0.0, gear1Rot / 16.0);
    //drawGear(gear2, +3.1, -2.0, 0.0, -2.0 * (gear1Rot / 16.0) - 9.0);

    //glRotated(+90.0, 1.0, 0.0, 0.0);
    //drawGear(gear3, -3.1, -1.8, -2.2, +2.0 * (gear1Rot / 16.0) - 2.0);

    glPopMatrix();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
	//glViewport(0, 0, width, height);
	//std::cout << float(width) /float(height) <<std::endl;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-1.0, 1.0, -1.0, 1.0, 5.0, 60.0);
	//gluPerspective(20, 1, 1.0, 60.0);
	gluPerspective(30, 1, nearplane, farplane);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //glTranslated(0.0, 0.0, -40.0);
	
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
	if (event->buttons() & Qt::RightButton) {
		GLuint selectBuf[512];
		GLint hits;
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glSelectBuffer (512, selectBuf);
		(void) glRenderMode (GL_SELECT);

		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble) event->x(), (GLdouble) (viewport[3] - event->y()), 1,1, viewport);

		gluPerspective(30, 1, nearplane, farplane);
		glMatrixMode(GL_MODELVIEW);
		paintGL();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		hits = glRenderMode (GL_RENDER);
		//processHits (hits, selectBuf);
		//for (int i = 0 ; i <hits; i++ )
		int tid = selectBuf[3];
		int vid = m_FaceModel.trilist[selectBuf[3]-1][0];
		WriteToFile("selectedvertices.txt", vid);
		std::cout << "picked triangle ID: " << tid << std::endl;
		std::cout << "picked vertex ID: " << vid << std::endl;
		qDebug() << "picked vertex coord: " << m_FaceModel.shape[vid][0] <<  m_FaceModel.shape[vid][1]<<  m_FaceModel.shape[vid][2];
	}
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + dy);
        setZRotation(zRot + dx);
    } else if (event->buttons() & Qt::RightButton) {
		setXRotation(xRot + dy);
        setYRotation(yRot + dx);
	} else if (event->buttons() & Qt::MiddleButton) {
		if (abs(dx)> abs(dy))
		{
			camx = camx - dx;
			fx = fx - dx;
		}
		else
		{
			camy = camy + dy;
			fy = fy + dy;
		}

	}


    lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	double d = 0.5;
	
	if(event->key() == Qt::Key_Up)
	{
		camy = camy - d;
		fy = fy - d;
	}
	if(event->key() == Qt::Key_Down)
	{
		camy = camy + d;
		fy = fy + d;
	}
	if(event->key() == Qt::Key_Left)
	{
		camx = camx + d;
		fx = fx + d;
	}
	if(event->key() == Qt::Key_Right)
	{
		camx = camx - d;
		fx = fx - d;
	}


	if(event->key() == Qt::Key_O)
	{
		setZRotation(zRot-0.2);
	}
	if(event->key() == Qt::Key_P)
	{
		setZRotation(zRot+0.2);
	}
}


void GLWidget::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
	setCamPosition(camz + numSteps);
    
}




void GLWidget::upDateGL()
{
    //gear1Rot += 2 * 16;
    updateGL();
}

GLuint GLWidget::makeGear(const GLfloat *reflectance, GLdouble innerRadius,
                          GLdouble outerRadius, GLdouble thickness,
                          GLdouble toothSize, GLint toothCount)
{
    const double Pi = 3.14159265358979323846;

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

    GLdouble r0 = innerRadius;
    GLdouble r1 = outerRadius - toothSize / 2.0;
    GLdouble r2 = outerRadius + toothSize / 2.0;
    GLdouble delta = (2.0 * Pi / toothCount) / 4.0;
    GLdouble z = thickness / 2.0;
    int i, j;

    glShadeModel(GL_FLAT);

    for (i = 0; i < 2; ++i) {
        GLdouble sign = (i == 0) ? +1.0 : -1.0;

        glNormal3d(0.0, 0.0, sign);

        glBegin(GL_QUAD_STRIP);
        for (j = 0; j <= toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
	    glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
	    glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
	    glVertex3d(r0 * cos(angle), r0 * sin(angle), sign * z);
	    glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
                       sign * z);
        }
        glEnd();

        glBegin(GL_QUADS);
        for (j = 0; j < toothCount; ++j) {
            GLdouble angle = 2.0 * Pi * j / toothCount;
	    glVertex3d(r1 * cos(angle), r1 * sin(angle), sign * z);
	    glVertex3d(r2 * cos(angle + delta), r2 * sin(angle + delta),
                       sign * z);
	    glVertex3d(r2 * cos(angle + 2 * delta), r2 * sin(angle + 2 * delta),
                       sign * z);
	    glVertex3d(r1 * cos(angle + 3 * delta), r1 * sin(angle + 3 * delta),
                       sign * z);
        }
        glEnd();
    }

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i < toothCount; ++i) {
        for (j = 0; j < 2; ++j) {
            GLdouble angle = 2.0 * Pi * (i + (j / 2.0)) / toothCount;
            GLdouble s1 = r1;
            GLdouble s2 = r2;
            if (j == 1)
                qSwap(s1, s2);

	    glNormal3d(cos(angle), sin(angle), 0.0);
	    glVertex3d(s1 * cos(angle), s1 * sin(angle), +z);
	    glVertex3d(s1 * cos(angle), s1 * sin(angle), -z);

	    glNormal3d(s2 * sin(angle + delta) - s1 * sin(angle),
                       s1 * cos(angle) - s2 * cos(angle + delta), 0.0);
	    glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), +z);
	    glVertex3d(s2 * cos(angle + delta), s2 * sin(angle + delta), -z);
        }
    }
    glVertex3d(r1, 0.0, +z);
    glVertex3d(r1, 0.0, -z);
    glEnd();

    glShadeModel(GL_SMOOTH);

    glBegin(GL_QUAD_STRIP);
    for (i = 0; i <= toothCount; ++i) {
	GLdouble angle = i * 2.0 * Pi / toothCount;
	glNormal3d(-cos(angle), -sin(angle), 0.0);
	glVertex3d(r0 * cos(angle), r0 * sin(angle), +z);
	glVertex3d(r0 * cos(angle), r0 * sin(angle), -z);
    }
    glEnd();

    glEndList();

    return list;
}

void GLWidget::drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                        GLdouble angle)
{
    glPushMatrix();
    glTranslated(dx, dy, dz);
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(gear);
    glPopMatrix();
}

void GLWidget::normalizeAngle(int *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}
void GLWidget::normalizeAngle(double *angle)
{
    while (*angle < 0)
        *angle += 360 * 16;
    while (*angle > 360 * 16)
        *angle -= 360 * 16;
}


void GLWidget::loadData()
{
	this->m_FaceModel.loadPCAData();
	this->m_FaceModel.generateZeroCoef();
	this->m_FaceModel.generateAverageFace();
	this->m_FaceModel.generateModel(); 
	ReadFromFile("featurePts.txt", m_featurePointsVertexID);   //feature points for updating xy plane.
	ReadFromFile("evaluationPts.txt", m_evaluationPointVertexID);   //feature points for updating lighting.
	ReadFromFile("featurePtsZ.txt", m_featurePointsVertexID_z);   //feature points for updating z value.
	ReadFromFile("visibleIndices.txt", m_visibleVertexID);
	ReadFromFile("contourPts.txt", m_faceContourVertexID);
	ReadFromFile("profilePts.txt", m_faceProfileVertexID);
}

void GLWidget::loadGLParams(std::string filename)
{
	std::cout << "Reading from file: " << filename << "...";
	std::ifstream infile;
	infile.open(filename);

	infile >> xRot;
	infile >> yRot;
	infile >> zRot;
	infile >> lightPos[0];
	infile >> lightPos[1];
	infile >> lightPos[2];
	infile >> lightPos[3];
	infile >> ambLightValue;
	infile >> camx;
	infile >> camy;
	infile >> camz;
	infile >> fx;
	infile >> fy;
	infile >> fz;

	updateGL();
	//std::cout << lightPos[0]<<" " << lightPos [1]<<" " << lightPos[2] << std::endl;
	std::cout << "Done" << std::endl;
}

void GLWidget::writeGLParams(std::string filename)
{
	std::ofstream outfile;

	outfile.open(filename, std::ostream::out);
	
	outfile << xRot << std::endl;
	outfile << yRot << std::endl;
	outfile << zRot << std::endl;
	outfile << lightPos[0] << std::endl;
	outfile << lightPos[1] << std::endl;
	outfile << lightPos[2] << std::endl;
	outfile << lightPos[3] << std::endl;
	outfile << ambLightValue << std::endl;

	outfile << camx << std::endl;
	outfile << camy << std::endl;
	outfile << camz << std::endl;

	outfile << fx << std::endl;
	outfile << fy << std::endl;
	outfile << fz << std::endl;

	outfile.close();
	//std::cout << lightPos[0]<<" " << lightPos [1]<<" " << lightPos[2] << std::endl;
}

void GLWidget::saveCurrentProject()
{
	QString filename = QFileDialog::getSaveFileName(this,  tr("Save Project File"), " ", tr("Project Files (*.prj)"));
	if (filename == NULL)
		return;
	std::string s = filename.toLocal8Bit().constData();
	//std::cout << s << std::endl;
	//std::cout<< filename.toStdString();
	writeGLParams(s);

	std::string t = s.substr(0, s.length() - 4) + ".txt";
	WriteToFile(t, m_FaceModel.shapeCoef);
}
void GLWidget::loadProject()
{
	QString filename = QFileDialog::getOpenFileName(this,  tr("Open Project File"), " ", tr("Project Files (*.prj)"));
	if (filename == NULL)
		return;
	std::string s = filename.toLocal8Bit().constData();
	
	loadGLParams(s);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	GLfloat AmbientLight[] = {ambLightValue, ambLightValue, ambLightValue}; 
	glLightfv (GL_LIGHT1, GL_AMBIENT, AmbientLight); 

	std::string t = s.substr(0, s.length() - 4) + ".txt";
	m_FaceModel.loadCoeffs(t);
	m_FaceModel.generateModel(); 
	static const GLfloat reflectance1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face = makeFace(this->m_FaceModel);
	updateGL();
}

void GLWidget::getCurrentMatrices()
{
	glGetDoublev(GL_MODELVIEW_MATRIX, m_modelviewMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projectionMatrix);
	glGetIntegerv(GL_VIEWPORT, m_viewportMatrix);
}
void GLWidget::drawFace(GLuint face, GLdouble dx, GLdouble dy, GLdouble dz,
                        GLdouble angle)
{
    glPushMatrix();
    glTranslated(dx, dy, dz);
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(face);
	getCurrentMatrices();
	/*if (b_needProject)
	{
		projectFeaturePoints();
		projectVertices();
	}*/
    glPopMatrix();
}

std::vector<int> GLWidget::getVisibleIndex()
{
	vec2d _shape = m_FaceModel.shape;

	std::vector<int> visbleIndices;
	for (int i = 0; i < _shape.size(); i++)
	{
		if (_shape[i][1] > 25)
		{
			visbleIndices.push_back(i+1);
		}
	}
	
	return visbleIndices;
}

std::vector<int> GLWidget::getSegmentIndex(int segment)   //returned index starts from 0
{
	std::vector<int> indices;
	for (int i = 0; i < m_FaceModel.segmentMask.size(); i++)
	{
		if (m_FaceModel.segmentMask[i] == segment)
				indices.push_back(i);
	}

	return indices;
}


GLuint GLWidget::makeFace(const CFacePCA& model)
{
	static const GLfloat modelamb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLfloat modeldiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
    const double Pi = 3.14159265358979323846;
	vec2d _shape = model.shape;
	vec2d _normal = model.normal;
	vec2d _texture = model.text;
	vec2d _trilist = model.trilist;
	vec1d _ab = model.albedo;

    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glMaterialfv(GL_FRONT, GL_AMBIENT, modelamb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, modeldiffuse);

	if (b_specluarON)
	{
		static const GLfloat modelspecular[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, modelspecular);
		glMateriali(GL_FRONT, GL_SHININESS, 5);
	}
	else
	{
		static const GLfloat modelspecular[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glMaterialfv(GL_FRONT, GL_SPECULAR, modelspecular);
		glMateriali(GL_FRONT, GL_SHININESS, 5);
	}

    int i, j;

    glShadeModel(GL_SMOOTH);

    for (i = 0; i < _trilist.size(); i++) {
        
		if (!b_showTexture)
			glColor4f(1.0f,1.0f,1.0f,1.0f);
        int inda = _trilist[i][0]-1;
		int indb = _trilist[i][1]-1;
		int indc = _trilist[i][2]-1;

		glLoadName(i);
        glBegin(GL_TRIANGLES);
        
		if (b_showTexture)
		{
			glColor3f(_texture[inda][0]/255, _texture[inda][1]/255, _texture[inda][2]/255);
			glNormal3f(_normal[inda][0], _normal[inda][1], _normal[inda][2]);
			glVertex3d(_shape[inda][0], _shape[inda][1], _shape[inda][2]);

			glColor3f(_texture[indb][0]/255, _texture[indb][1]/255, _texture[indb][2]/255);
			glNormal3f(_normal[indb][0], _normal[indb][1], _normal[indb][2]);
			glVertex3d(_shape[indb][0], _shape[indb][1], _shape[indb][2]);

			glColor3f(_texture[indc][0]/255, _texture[indc][1]/255, _texture[indc][2]/255);
			glNormal3f(_normal[indc][0], _normal[indc][1], _normal[indc][2]);
			glVertex3d(_shape[indc][0], _shape[indc][1], _shape[indc][2]);
		}
		else
		{
			if (_ab.size() != 0)
				glColor3d((_ab[inda]+200)/255, (_ab[inda]+200)/255, (_ab[inda]+200)/255);
			glNormal3f(_normal[inda][0], _normal[inda][1], _normal[inda][2]);
			glVertex3d(_shape[inda][0], _shape[inda][1], _shape[inda][2]);
			if (_ab.size() != 0)
				glColor3d((_ab[indb]+200)/255, (_ab[indb]+200)/255, (_ab[indb]+200)/255);
			glNormal3f(_normal[indb][0], _normal[indb][1], _normal[indb][2]);
			glVertex3d(_shape[indb][0], _shape[indb][1], _shape[indb][2]);
			if (_ab.size() != 0)
				glColor3d((_ab[indc]+200)/255, (_ab[indc]+200)/255, (_ab[indc]+200)/255);
			glNormal3f(_normal[indc][0], _normal[indc][1], _normal[indc][2]);
			glVertex3d(_shape[indc][0], _shape[indc][1], _shape[indc][2]);
		}
        
        glEnd();

        
    }


    glEndList();

    return list;
}

GLuint GLWidget::makeAverageFace(const GLfloat *reflectance, const CFacePCA& model)
{
    const double Pi = 3.14159265358979323846;
	vec2d _shape = model.ave_shape;
	vec2d _normal = model.ave_normal;
	vec2d _texture = model.ave_text;
	vec2d _trilist = model.trilist;


    GLuint list = glGenLists(2);
    glNewList(list, GL_COMPILE);
    //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, reflectance);

   
    int i, j;

    glShadeModel(GL_SMOOTH);

    for (i = 0; i < _trilist.size(); i++) {
        

        int inda = _trilist[i][0]-1;
		int indb = _trilist[i][1]-1;
		int indc = _trilist[i][2]-1;

		glLoadName(i);
        glBegin(GL_TRIANGLES);
        
		glColor3f(_texture[inda][0]/255, _texture[inda][1]/255, _texture[inda][2]/255);
		glNormal3f(_normal[inda][0], _normal[inda][1], _normal[inda][2]);
	    glVertex3d(_shape[inda][0], _shape[inda][1], _shape[inda][2]);

		glColor3f(_texture[indb][0]/255, _texture[indb][1]/255, _texture[indb][2]/255);
		glNormal3f(_normal[indb][0], _normal[indb][1], _normal[indb][2]);
	    glVertex3d(_shape[indb][0], _shape[indb][1], _shape[indb][2]);

		glColor3f(_texture[indc][0]/255, _texture[indc][1]/255, _texture[indc][2]/255);
		glNormal3f(_normal[indc][0], _normal[indc][1], _normal[indc][2]);
	    glVertex3d(_shape[indc][0], _shape[indc][1], _shape[indc][2]);
	    
        
        glEnd();

        
    }


    glEndList();

    return list;
}
void GLWidget::drawAxis()
{
	glBegin(GL_LINES);
	glColor3d(1.0, 0, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(100, 0, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 100, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 0, 100);
	glEnd();
}

void GLWidget::drawGround()
{
	glBegin(GL_LINES);
		for (int i = 0; i < 401; i++)
		{
			glColor3f(1.0,1.0,1.0);
			glVertex3f(-200.0f+i, 25.0f,-200.0f);
			glVertex3f(-200.0f+i, 25.0f, 200.0f);
		}
	glEnd();
	glBegin(GL_LINES);
		for (int i = 0; i < 401; i++)
		{
			glColor3f(1.0,1.0,1.0);
			glVertex3f(-200.0f, 25.0f,-200.0f+i);
			glVertex3f(200.0f, 25.0f,-200.0f+i);
		}
	glEnd();

}

std::vector<int> GLWidget::getFeaturePointTriangleIDs()
{
	qDebug() << " test" ;
	int x, y;
	std::vector<int> fpIndex;
	for (int i = 0; i < m_featurePoints.size(); i++)
	{
		x = m_featurePoints[i].x;
		y = m_featurePoints[i].y;

		GLuint selectBuf[512];
		GLint hits;
		GLint viewport[4];

		glGetIntegerv(GL_VIEWPORT, viewport);
		glSelectBuffer (512, selectBuf);
		(void) glRenderMode (GL_SELECT);

		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), 1,1, viewport);

		gluPerspective(30, 1, nearplane, farplane);
		glMatrixMode(GL_MODELVIEW);
		paintGL();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		hits = glRenderMode (GL_RENDER);
		//processHits (hits, selectBuf);
		//for (int i = 0 ; i <hits; i++ )
		int tid = selectBuf[3];
		int vid;
		if (tid >= m_FaceModel.trilist.size())
		    vid = 0;
		else
			vid	 = m_FaceModel.trilist[selectBuf[3]-1][0];

		std::cout << "picked vertex ID: " << vid << std::endl;
		fpIndex.push_back(vid);
	}
	m_featurePointsTriangleID = fpIndex;
	return fpIndex;

}

void GLWidget::highlightTriangles()
{
	vec2d _shape = m_FaceModel.ave_shape;
	vec2d _normal = m_FaceModel.ave_normal;
	vec2d _texture = m_FaceModel.ave_text;
	vec2d _trilist = m_FaceModel.trilist;

	int vertexid;
	double x,y,z;
	if (m_featurePointsTriangleID.size() >0)
	{
		for (int i = 0; i < m_featurePointsTriangleID.size(); i++)
		{
			vertexid = _trilist[m_featurePointsTriangleID[i]-1][0];
			x = _shape[vertexid][0];
			y = _shape[vertexid][1];
			z = _shape[vertexid][2];
			GLUquadricObj *quadric = gluNewQuadric();
			gluQuadricNormals(quadric, GLU_SMOOTH);
			glPushMatrix();
			glTranslated(x, y, z);
			glColor3d(255,0,0);
			gluSphere(quadric, 1, 10, 10);
			glColor3d(255,255,255);
			glPopMatrix();

		}
		b_showHighlight = true;
		update();
	}
}

void GLWidget::highlightFeaturePoints()
{
	vec2d _shape = m_FaceModel.ave_shape;
	vec2d _normal = m_FaceModel.ave_normal;
	vec2d _texture = m_FaceModel.ave_text;
	vec2d _trilist = m_FaceModel.trilist;

	int vertexid;
	double x,y,z;
	if (m_featurePointsVertexID.size() >0)
	{
		for (int i = 0; i < m_featurePointsVertexID.size(); i++)
		{
			vertexid = m_featurePointsVertexID[i] - 1;
			x = _shape[vertexid][0];
			y = _shape[vertexid][1];
			z = _shape[vertexid][2];
			GLUquadricObj *quadric = gluNewQuadric();
			gluQuadricNormals(quadric, GLU_SMOOTH);
			glPushMatrix();
			glTranslated(x, y, z);
			glColor3d(255,0,0);
			gluSphere(quadric, 1, 10, 10);
			glColor3d(255,255,255);
			glPopMatrix();

		}
		
	}
	if (m_faceContourVertexID.size() >0)
	{
		for (int i = 0; i < 10; i++)
		{
			vertexid = m_faceContourVertexID[i] - 1;
			x = _shape[vertexid][0];
			y = _shape[vertexid][1];
			z = _shape[vertexid][2];
			GLUquadricObj *quadric = gluNewQuadric();
			gluQuadricNormals(quadric, GLU_SMOOTH);
			glPushMatrix();
			glTranslated(x, y, z);
			glColor3d(0,255,0);
			gluSphere(quadric, 1, 10, 10);
			glColor3d(255,255,255);
			glPopMatrix();

		}
		
	}

	if (m_faceProfileVertexID.size() > 0)
	{
		for (int i = 0; i < 11; i++)
		{
			vertexid = m_faceProfileVertexID[i] - 1;
			x = _shape[vertexid][0];
			y = _shape[vertexid][1];
			z = _shape[vertexid][2];
			GLUquadricObj *quadric = gluNewQuadric();
			gluQuadricNormals(quadric, GLU_SMOOTH);
			glPushMatrix();
			glTranslated(x, y, z);
			glColor3d(0,0,255);
			gluSphere(quadric, 1, 10, 10);
			glColor3d(255,255,255);
			glPopMatrix();

		}
		
	}

	b_showHighlight = true;
	update();
}


void GLWidget::projectEvaluationPoints()
{
	QSize imagesize = this->grabFrameBuffer().size();
	vec2d _shape = m_FaceModel.shape;
	m_projectedEvaluationPoints.clear();
	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;
	for (int i = 0; i < m_evaluationPointVertexID.size(); i++)
	{
		objx = _shape[m_evaluationPointVertexID[i]-1][0];
		objy = _shape[m_evaluationPointVertexID[i]-1][1];
		objz = _shape[m_evaluationPointVertexID[i]-1][2];
		gluProject(objx, objy, objz, 
			m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix,
			&winx, &winy, &winz);

		cv::Point pt;
		pt.x = winx;
		pt.y = imagesize.height() - winy;
		m_projectedEvaluationPoints.push_back(pt);
	}
	
}

void GLWidget::projectVertices(std::vector<int>& _vertexIndices, std::vector<cv::Point>& _projectedCoords)
{
	QSize imagesize = this->grabFrameBuffer().size();
	vec2d _shape = m_FaceModel.shape;
	_projectedCoords.clear();
	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;

	for (int i = 0; i < _vertexIndices.size(); i++)
	{
		objx = _shape[_vertexIndices[i]][0];
		objy = _shape[_vertexIndices[i]][1];
		objz = _shape[_vertexIndices[i]][2];
		gluProject(objx, objy, objz, 
			m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix,
			&winx, &winy, &winz);

		cv::Point pt;
		pt.x = winx;
		pt.y = imagesize.height() - winy;
		_projectedCoords.push_back(pt);
	}
	
}

void GLWidget::projectFeaturePoints()
{
	QSize imagesize = this->grabFrameBuffer().size();
	vec2d _shape = m_FaceModel.shape;
	m_projectedFeaturePoints.clear();
	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;
	for (int i = 0; i < m_featurePointsVertexID.size(); i++)
	{
		objx = _shape[m_featurePointsVertexID[i]-1][0];
		objy = _shape[m_featurePointsVertexID[i]-1][1];
		objz = _shape[m_featurePointsVertexID[i]-1][2];
		gluProject(objx, objy, objz, 
			m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix,
			&winx, &winy, &winz);

		cv::Point pt;
		pt.x = winx;
		pt.y = imagesize.height() - winy;
		m_projectedFeaturePoints.push_back(pt);
	}
	
	b_needProject = false;
}

void GLWidget::projectVertices()  //test average shape
{
	QSize imagesize = this->grabFrameBuffer().size();
	vec2d _shape = m_FaceModel.shape;
	m_projectedVertices.clear();
	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;
	for (int i = 0; i < _shape.size(); i++)
	{
		objx = _shape[i][0];
		objy = _shape[i][1];
		objz = _shape[i][2];
		gluProject(objx, objy, objz, 
			m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix,
			&winx, &winy, &winz);

		cv::Point pt;
		pt.x = winx;
		pt.y = imagesize.height() - winy;
		m_projectedVertices.push_back(pt);
	}
	b_needProject = false;
	b_isProjectionDone = true;
}

void GLWidget::pasteTexture(QImage inputImage)
{
	vec2d _shape = m_FaceModel.shape;
	QColor color;
	GLdouble winx, winy, winz;
	newTexture.clear();
	for (int i = 0; i < _shape.size() ; i++)
	{
		vec1d rgb;
		winx = m_projectedVertices[i].x;
		winy = m_projectedVertices[i].y;
		color = inputImage.pixel(winx, winy);
		rgb.push_back(color.red());
		rgb.push_back(color.green());
		rgb.push_back(color.blue());
		newTexture.push_back(rgb);
	}

	m_FaceModel.text = newTexture;
	static const GLfloat reflectance1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face = makeFace(this->m_FaceModel);
	std::cout << "Texture Pasting Done" << std::endl;
}

void GLWidget::writeToPLYFile(QString filename, vec2d data)
{
	vec2d _shape = data;
	vec2d _text = m_FaceModel.text;
	vec2d _tl = m_FaceModel.trilist;

	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QTextStream out(&file);

	out << "ply" << endl;
	out << "format ascii 1.0" << endl;
	out << "element vertex " << _shape.size() << endl;
	out << "property float x" << endl << "property float y" << endl << "property float z" << endl;
	out << "property uchar red" << endl << "property uchar green" << endl << "property uchar blue" << endl;
	out << "element face " <<_tl.size() << endl;
	out << "property list uchar int vertex_indices" << endl;
	out << "end_header" << endl;

	for (int i = 0 ; i < _shape.size(); i ++) //x y z, r, g, b
		out << _shape[i][0] << " " << _shape[i][1] << " " << _shape[i][2] <<" "<<
				(int)_text[i][0] << " " <<(int)_text[i][1] << " " << (int)_text[i][2] << endl; 
	

	for (int i = 0; i < _tl.size(); i++)  //num_vertex_per_face , faces, vertex index should be subtracted by 1
		out << 3 <<" " << _tl[i][0]-1 << " " << _tl[i][1]-1 << " " <<_tl[i][2]-1 << endl; 

	file.close();
	std::cout << "Done" << std::endl;
}


void GLWidget::writeToPLYFile(QString filename)
{
	qDebug() << "Writing to file: " << filename << "...";

	vec2d _shape = m_FaceModel.shape;
	vec2d _text = m_FaceModel.text;
	vec2d _tl = m_FaceModel.trilist;

	QFile file(filename);
	file.open(QIODevice::WriteOnly);

	QTextStream out(&file);

	out << "ply" << endl;
	out << "format ascii 1.0" << endl;
	out << "element vertex " << _shape.size() << endl;
	out << "property float x" << endl << "property float y" << endl << "property float z" << endl;
	out << "property uchar red" << endl << "property uchar green" << endl << "property uchar blue" << endl;
	out << "element face " <<_tl.size() << endl;
	out << "property list uchar int vertex_indices" << endl;
	out << "end_header" << endl;

	for (int i = 0 ; i < _shape.size(); i ++) //x y z, r, g, b
		out << _shape[i][0] << " " << _shape[i][1] << " " << _shape[i][2] <<" "<<
			(int)_text[i][0] << " " <<(int)_text[i][1] << " " << (int)_text[i][2] << endl; 
	

	for (int i = 0; i < _tl.size(); i++)  //num_vertex_per_face , faces, vertex index should be subtracted by 1
		out << 3 <<" " << _tl[i][0]-1 << " " << _tl[i][1]-1 << " " <<_tl[i][2]-1 << endl; 

	file.close();
	std::cout << "Done" << std::endl;
}


void GLWidget::myProjectFeaturePoints() //Test use only...
{
	QSize imagesize = this->grabFrameBuffer().size();
	vec2d _shape = m_FaceModel.shape;

	

	GLdouble tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	
	glGetDoublev(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	for (int i = 0; i < 16; i++)
	{
		qDebug() << tempMatrix[i] << " " ;
	}
	qDebug() << endl;

	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;
	for (int i = 0; i < m_featurePointsVertexID.size(); i++)
	{
		objx = _shape[m_featurePointsVertexID[i]-1][0];
		objy = _shape[m_featurePointsVertexID[i]-1][1];
		objz = _shape[m_featurePointsVertexID[i]-1][2];

		//glhProjectf(objx, objy, objz, m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix, winx, winy);

		double v1 = tempMatrix[0] * objx + tempMatrix[4]* objy + tempMatrix[8] * objz + tempMatrix[12];
		double v2 = tempMatrix[1] * objx + tempMatrix[5]* objy + tempMatrix[9] * objz + tempMatrix[13];
		double v4 = tempMatrix[3] * objx + tempMatrix[7]* objy + tempMatrix[11] * objz + tempMatrix[15];

		v1 = v1/v4;
		v2 = v2/v4;

		winx = m_viewportMatrix[0] + m_viewportMatrix[2]*(v1 + 1) /2;
		winy = m_viewportMatrix[1] + m_viewportMatrix[3]*(v2 + 1) /2;

		cv::Point pt;
		pt.x = winx;
		pt.y = imagesize.height() - winy;
		m_myProjectedFeaturePoints.push_back(pt);
	}
	qDebug() << "my projected points size: "<<m_featurePointsVertexID.size() << endl;
}
void GLWidget::saveFrontMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_frontMatrix);
	glPopMatrix();

	glGetIntegerv(GL_VIEWPORT, m_frontViewportMatrix);
}
void GLWidget::saveSideMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_sideMatrix);
	glPopMatrix();

	glGetIntegerv(GL_VIEWPORT, m_sideViewportMatrix);
}

void  GLWidget::saveProfileMatrix()
{
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_profileMatrix);
	glPopMatrix();

	glGetIntegerv(GL_VIEWPORT, m_profileViewportMatrix);
}

void GLWidget::blendEar()
{
	std::vector<int> earIndex = getSegmentIndex(8);
	for (int i = 0; i < earIndex.size(); i++)
	{
		for (int j = 0; j < 3; j++)
			m_FaceModel.shape[earIndex[i]][j] = m_FaceModel.ave_shape[earIndex[i]][j];
	}
	std::vector<int> earIndex1 = getSegmentIndex(7);
	for (int i = 0; i < earIndex1.size(); i++)
	{
		for (int j = 0; j < 3; j++)
			m_FaceModel.shape[earIndex1[i]][j] = m_FaceModel.ave_shape[earIndex1[i]][j];
	}
	std::vector<int> earIndex2 = getSegmentIndex(6);
	for (int i = 0; i < earIndex2.size(); i++)
	{
		for (int j = 0; j < 3; j++)
			m_FaceModel.shape[earIndex2[i]][j] = m_FaceModel.ave_shape[earIndex2[i]][j];
	}
	glDeleteLists(face, 1);
	face = makeFace(this->m_FaceModel);
	updateGL();
}

double GLWidget::updateModel(const std::vector<cv::Point>& _targetFeaturePointsCoord, const std::vector<cv::Point>& _targetContourPointsCoord, const std::vector<cv::Point>& _targetProfilePointsCoord)
{
	// projection matrix * model view matrix = tempo matrix

	/*GLdouble m_frontMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	
	glGetDoublev(GL_MODELVIEW_MATRIX, m_frontMatrix);
	glPopMatrix();*/

	vec2d _shape = m_FaceModel.shape;
	QSize imagesize = this->grabFrameBuffer().size();

	GLdouble winx, winy, winz;
	GLdouble objx, objy, objz;

	double delta = 0.1;
	double dist = 0;

	int n_fp =  _targetFeaturePointsCoord.size();
	int n_cp =  _targetContourPointsCoord.size();
	int n_pp =  _targetProfilePointsCoord.size();

	if (m_featurePointsVertexID.size())
	{
		for (int j = 0; j < m_FaceModel.shapeCoef.size(); j++) // coeffient index
		{
			double da = 0;
			for (int i = 0; i < n_fp + n_cp + n_pp; i++) // feature points index
			{
				if (i < n_fp)
				{
					if (i<17||i>35)   //only optimize eyes, checks and mouth
					{
					objx = _shape[m_featurePointsVertexID[i]-1][0];
					objy = _shape[m_featurePointsVertexID[i]-1][1];
					objz = _shape[m_featurePointsVertexID[i]-1][2];

					//glhProjectf(objx, objy, objz, m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix, winx, winy);

					double v1 = m_frontMatrix[0] * objx + m_frontMatrix[4]* objy + m_frontMatrix[8] * objz + m_frontMatrix[12];
					double v2 = m_frontMatrix[1] * objx + m_frontMatrix[5]* objy + m_frontMatrix[9] * objz + m_frontMatrix[13];
					double v4 = m_frontMatrix[3] * objx + m_frontMatrix[7]* objy + m_frontMatrix[11] * objz + m_frontMatrix[15];

					v1 = v1/v4;
					v2 = v2/v4;

					winx = m_frontViewportMatrix[0] + m_frontViewportMatrix[2]*(v1 + 1) /2;
					winy = m_frontViewportMatrix[1] + m_frontViewportMatrix[3]*(v2 + 1) /2;
					winy = imagesize.height() - winy;

					double dx = winx - _targetFeaturePointsCoord[i].x; 
					double dy = winy - _targetFeaturePointsCoord[i].y;

					dist += qSqrt( dx*dx + dy*dy);
					///////////////////////////////////////////////////////////////

					int xind = 3*(m_featurePointsVertexID[i]-1) + 1;   //PC index
					int yind = xind + 1;
					int zind = xind + 2;

					da += dx *m_frontViewportMatrix[2]* (m_frontMatrix[0] * m_FaceModel.shapePC[xind-1][j] + m_frontMatrix[4] * m_FaceModel.shapePC[yind-1][j] + m_frontMatrix[8] * m_FaceModel.shapePC[zind-1][j])
						  - dy *m_frontViewportMatrix[3]* (m_frontMatrix[1] * m_FaceModel.shapePC[xind-1][j] + m_frontMatrix[5] * m_FaceModel.shapePC[yind-1][j] + m_frontMatrix[9] * m_FaceModel.shapePC[zind-1][j]);
				
					}
				}
				else if(i < n_fp + n_cp)
				{
					objx = _shape[m_faceContourVertexID[i-n_fp]-1][0];
					objy = _shape[m_faceContourVertexID[i-n_fp]-1][1];
					objz = _shape[m_faceContourVertexID[i-n_fp]-1][2];

					//glhProjectf(objx, objy, objz, m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix, winx, winy);

					double v1 = m_sideMatrix[0] * objx + m_sideMatrix[4]* objy + m_sideMatrix[8] * objz + m_sideMatrix[12];
					double v2 = m_sideMatrix[1] * objx + m_sideMatrix[5]* objy + m_sideMatrix[9] * objz + m_sideMatrix[13];
					double v4 = m_sideMatrix[3] * objx + m_sideMatrix[7]* objy + m_sideMatrix[11] * objz + m_sideMatrix[15];

					v1 = v1/v4;
					v2 = v2/v4;

					winx = m_sideViewportMatrix[0] + m_sideViewportMatrix[2]*(v1 + 1) /2;
					winy = m_sideViewportMatrix[1] + m_sideViewportMatrix[3]*(v2 + 1) /2;
					winy = imagesize.height() - winy;

					double dx = winx - _targetContourPointsCoord[i-n_fp].x; 
					double dy = winy - _targetContourPointsCoord[i-n_fp].y;

					dx = 0.8*dx;  
					dy = 0.8*dy;

					dist += qSqrt( dx*dx + dy*dy);
					///////////////////////////////////////////////////////////////

					int xind = 3*(m_faceContourVertexID[i-n_fp]-1) + 1;   //PC index
					int yind = xind + 1;
					int zind = xind + 2;

					da += dx *m_sideViewportMatrix[2]* (m_sideMatrix[0] * m_FaceModel.shapePC[xind-1][j] + m_sideMatrix[4] * m_FaceModel.shapePC[yind-1][j] + m_sideMatrix[8] * m_FaceModel.shapePC[zind-1][j])
						  - dy *m_sideViewportMatrix[3]* (m_sideMatrix[1] * m_FaceModel.shapePC[xind-1][j] + m_sideMatrix[5] * m_FaceModel.shapePC[yind-1][j] + m_sideMatrix[9] * m_FaceModel.shapePC[zind-1][j]);
				}
			
			
				else if(i < n_fp + n_cp + n_pp)
				{
					objx = _shape[m_faceProfileVertexID[i-n_fp- n_cp]-1][0];
					objy = _shape[m_faceProfileVertexID[i-n_fp- n_cp]-1][1];
					objz = _shape[m_faceProfileVertexID[i-n_fp- n_cp]-1][2];

					//glhProjectf(objx, objy, objz, m_modelviewMatrix, m_projectionMatrix, m_viewportMatrix, winx, winy);

					double v1 = m_profileMatrix[0] * objx + m_profileMatrix[4]* objy + m_profileMatrix[8] * objz + m_profileMatrix[12];
					double v2 = m_profileMatrix[1] * objx + m_profileMatrix[5]* objy + m_profileMatrix[9] * objz + m_profileMatrix[13];
					double v4 = m_profileMatrix[3] * objx + m_profileMatrix[7]* objy + m_profileMatrix[11] * objz + m_profileMatrix[15];

					v1 = v1/v4;
					v2 = v2/v4;

					winx = m_profileViewportMatrix[0] + m_profileViewportMatrix[2]*(v1 + 1) /2;
					winy = m_profileViewportMatrix[1] + m_profileViewportMatrix[3]*(v2 + 1) /2;
					winy = imagesize.height() - winy;

					double dx = winx - _targetProfilePointsCoord[i-n_fp- n_cp].x; 
					double dy = winy - _targetProfilePointsCoord[i-n_fp- n_cp].y;

					dx = 0.8*dx;  
					dy = 0.8*dy;

					dist += qSqrt( dx*dx + dy*dy);
					///////////////////////////////////////////////////////////////

					int xind = 3*(m_faceProfileVertexID[i- n_fp- n_cp]-1) + 1;   //PC index
					int yind = xind + 1;
					int zind = xind + 2;

					da += dx *m_profileViewportMatrix[2]* (m_profileMatrix[0] * m_FaceModel.shapePC[xind-1][j] + m_profileMatrix[4] * m_FaceModel.shapePC[yind-1][j] + m_profileMatrix[8] * m_FaceModel.shapePC[zind-1][j])
						  - dy *m_profileViewportMatrix[3]* (m_profileMatrix[1] * m_FaceModel.shapePC[xind-1][j] + m_profileMatrix[5] * m_FaceModel.shapePC[yind-1][j] + m_profileMatrix[9] * m_FaceModel.shapePC[zind-1][j]);
				}
			}

			m_FaceModel.shapeCoef[j] -= delta*da;
		}
		
	}
	//WriteToFile("alpha.txt", m_FaceModel.shapeCoef);

	m_FaceModel.generateModel();
	//blendEar();
	glDeleteLists(face, 1);
	face = makeFace(this->m_FaceModel);
	updateGL();

	return dist;
}

//precondition: 1. projection done.
//
double GLWidget::updateModelZ(const QImage& _subjectImage, QImage& testImage)  //working on here 
{
	//projectVertices();
	QImage monoImage;
	QImageColorToGray(_subjectImage, monoImage);
	testImage = QImage(_subjectImage.size(), QImage::Format_ARGB32_Premultiplied);


	GLfloat tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0);
	glRotated(xRot, 1.0, 0.0, 0.0);
	glRotated(yRot, 0.0, 1.0, 0.0);
	glRotated(zRot, 0.0, 0.0, 1.0);
	glScaled( 0.5, 0.5, 0.5);
	glTranslated( modelx, modely, modelz);
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	cv::Mat matrix;
	openglMatrixToCvMat(tempMatrix, matrix);
	cv::Mat invmat = matrix.t().inv();  //get inverse matrix to tranfer light direction vector into face coordinate system
	
	vec1d light;
	light.push_back(lightPos[0]*invmat.at<float>(0,0) + lightPos[1]*invmat.at<float>(0,1)+lightPos[2]*invmat.at<float>(0,2)+ invmat.at<float>(0,3));
	light.push_back(lightPos[0]*invmat.at<float>(1,0) + lightPos[1]*invmat.at<float>(1,1)+lightPos[2]*invmat.at<float>(1,2)+ invmat.at<float>(1,3));
	light.push_back(lightPos[0]*invmat.at<float>(2,0) + lightPos[1]*invmat.at<float>(2,1)+lightPos[2]*invmat.at<float>(2,2)+ invmat.at<float>(2,3));

	vec1d l = normalize(light);  //compute light direction vector under face coord system.

	std::vector<int> index = getSegmentIndex(2); //test update nose, vertex index starts from 0

	vec2d _shape = m_FaceModel.shape;
	vec2d _texture = m_FaceModel.text;
	vec2d _normal = m_FaceModel.normal;
	vec2d _adjacentVertices = m_FaceModel.adjacentVertices;   //vertex index starts from 0

	double delta = 0.001;
	double beta = 2;

	for (int k = 0; k < 20; k++)   //10 iterations
	{

	vec2d colors = getVertexColor();
	double sumd = 0;
	
	for (int i = 0; i < index.size(); i++)
	{
		
		int vid = index[i];

		int eastInd = _adjacentVertices[vid][0];
		int northInd = _adjacentVertices[vid][1];
		int westInd = _adjacentVertices[vid][2];
		int southInd = _adjacentVertices[vid][3];

		int westnorthInd = _adjacentVertices[westInd][1];
		int southeastInd = _adjacentVertices[southInd][0];

		if (eastInd == -1 || northInd == -1|| westInd == -1 || southInd == -1) //if vertex is boundary, pass
			continue;
		
		QColor targetColor = _subjectImage.pixel(m_projectedVertices[vid].x, m_projectedVertices[vid].y);
		int targetRed = targetColor.red();

		QColor targetColor_s = _subjectImage.pixel(m_projectedVertices[southInd].x, m_projectedVertices[southInd].y);
		int targetRed_s = targetColor_s.red();

		QColor targetColor_w = _subjectImage.pixel(m_projectedVertices[westInd].x, m_projectedVertices[westInd].y);
		int targetRed_w = targetColor_w.red();

		double d = (double)targetRed - std::min(colors[vid][0],255.0);
		double d_w = (double)targetRed_w - std::min(colors[westInd][0],255.0);
		double d_s = (double)targetRed_s - std::min(colors[southInd][0],255.0);
		
		testImage.setPixel(m_projectedVertices[vid].x, m_projectedVertices[vid].y, qRgb(std::min(colors[vid][0],255.0), std::min(colors[vid][1],255.0), std::min(colors[vid][2],255.0)));


		sumd += sqrt(d*d);

		double ux = m_FaceModel.shape[eastInd][0] - m_FaceModel.shape[vid][0];
		double wx = m_FaceModel.shape[northInd][0] - m_FaceModel.shape[vid][0];
		double uz = m_FaceModel.shape[eastInd][2] - m_FaceModel.shape[vid][2];
		double wz = m_FaceModel.shape[northInd][2] - m_FaceModel.shape[vid][2];

		double ux_s = m_FaceModel.shape[southeastInd][0] - m_FaceModel.shape[southInd][0];
		double uz_s = m_FaceModel.shape[southeastInd][2] - m_FaceModel.shape[southInd][2];

		double wx_w = m_FaceModel.shape[westnorthInd][0] - m_FaceModel.shape[westInd][0];
		double wz_w = m_FaceModel.shape[westnorthInd][2] - m_FaceModel.shape[westInd][2];

		double step1 = _texture[vid][0] * d * (l[0] * (uz- wz) + l[2] * (wx - ux))/length(_normal[vid]) + 
						_texture[southInd][0]*d_s*(l[0] * (-uz_s) + l[2] * (ux_s))/length(_normal[southInd]) + 
						_texture[westInd][0]*d_w*(l[0] * (wz_w) + l[2] * (-wx_w))/length(_normal[westInd]);
		
		double step2 = beta*(m_FaceModel.shape[vid][1] - _shape[vid][1]); 

		if (vid == 24832)
		{
			testImage.setPixel(m_projectedVertices[vid].x, m_projectedVertices[vid].y, qRgb(255, 0, 0));
			//qDebug() <<"coord:" << m_projectedVertices[vid].x << m_projectedVertices[vid].y;
			qDebug() << "target" << targetColor.red()<<targetColor.green()<<targetColor.blue();
			qDebug() << "colors" << std::min(colors[vid][0], 255.0) << std::min(colors[vid][1], 255.0) << std::min(colors[vid][2], 255.0);
			qDebug() << "d: " << d ;
		}
		/*if (i == 10)
		{
			qDebug() << step1 << step2 ;
		}*/
		
		m_FaceModel.shape[vid][1] -= (step1+step2)*delta;
		
	}
	m_FaceModel.updateNormal();
	glDeleteLists(face, 1);
	face = makeFace(this->m_FaceModel);
	updateGL();

	std::cout << "error: " << sumd/index.size() << std::endl;
	}
	return 0;
}


double GLWidget::updateModelZMethod2(const QImage& _subjectImage, QImage& testImage)
{
	testImage = QImage(_subjectImage.size(), QImage::Format_ARGB32_Premultiplied);
	std::vector<int> index = getSegmentIndex(2); //test update nose, vertex index starts from 0
	//std::vector<int> index = m_visibleVertexID;
	GLdouble tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixd(m_projectionMatrix);
	glMultMatrixd(m_modelviewMatrix);
	
	glGetDoublev(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	vec2d _shape = m_FaceModel.shape;
	QImage _renderedImage = grabFrameBuffer();

	std::vector<double> errors;

	//qDebug() << "here" ;
	projectVertices(index, m_projectedfeaturePointZ);
	double averageError = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedfeaturePointZ, errors);
	double delta = 10;
	double da = 0;
	if (index.size())
	{
		for (int j = 0; j < index.size(); j++)
		{

			m_FaceModel.shape[index[j]][1] += 0.1*errors[j];
			//testImage.setPixel(m_projectedfeaturePointZ[j].x, m_projectedfeaturePointZ[j].y, qRgb(255-errors[j],255-errors[j],255-errors[j]) );
			//qDebug() << errors[j];


		}
		
	}
	//WriteToFile("alpha.csv", m_FaceModel.shapeCoef);
	//m_FaceModel.generateShapeOnly();

	m_FaceModel.normal = m_FaceModel.CalculateNorm(m_FaceModel.shape, m_FaceModel.trilist);

    glDeleteLists(face, 1);
	face = makeFace(this->m_FaceModel);
	

	pasteTexture(_subjectImage);
	//_renderedImage = grabFrameBuffer();
	
	//averageError = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedfeaturePointZ, errors);
	

	return averageError;   


	/*QImage _renderedImage = grabFrameBuffer();

	std::vector<double> errors;
	projectVertices(m_featurePointsVertexID_z, m_projectedfeaturePointZ);
	double averageError = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedfeaturePointZ, errors);
	double delta = 0.01;
	double da = 0;
	if (m_featurePointsVertexID_z.size())
	{
		for (int i = 0; i < m_featurePointsVertexID_z.size(); i++)
		{
				
			double dz = errors[i];

			m_FaceModel.shape[m_featurePointsVertexID_z[i]-1][1] += dz*delta;

		}
			
	}

	pasteTexture(_subjectImage);
	static const GLfloat reflectance1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	face = makeFace(reflectance1, this->m_FaceModel);
	
	_renderedImage = grabFrameBuffer();
	
	averageError = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedfeaturePointZ, errors);
	

	return averageError; */
} 

void GLWidget::optimizeDirectionLight(QImage _subjectImage)
{
	std::cout<<"Optimizing Direction Light...";
	double minerror = 999999;
	int bestangleX, bestangleY;

	//search best x direction
	for (int i = 0; i < 180; i++)
	{
		setLightPositionX(i);
		QImage _renderedImage = grabFrameBuffer();
		projectEvaluationPoints();
		double error = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedEvaluationPoints);
		//std::cout << "error: " << error << std::endl;
		if (error < minerror)
		{
			minerror = error;
			bestangleX = i;
		}
		std::cout<<".";
		updateGL();
	}
	
	minerror = 999999;
	//search best y direction
	for (int i = -100; i < 101; i++)
	{
		setLightPositionY(i);
		QImage _renderedImage = grabFrameBuffer();
		projectEvaluationPoints();
		double error = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedEvaluationPoints);
		//std::cout << "error: " << error << std::endl;
		if (error < minerror)
		{
			minerror = error;
			bestangleY = i;
		}
		std::cout<<".";
		updateGL();
	}

	std::cout<< std::endl;
	std::cout << "best angleX: " << bestangleX << std::endl;
	std::cout << "best angleY: " << bestangleY << std::endl;
	setLightPositionX(bestangleX);
	setLightPositionY(bestangleY);
	std::cout << lightPos[0]<<" " << lightPos [1]<<" " << lightPos[2] << std::endl;
	updateGL();
}

void GLWidget::optimizeAmbLight(QImage _subjectImage)
{
	std::cout<<"Optimizing Ambiant Light";
	double minerror = 999999;
	int bestValue;

	for (int i = 1; i < 100; i++)
	{
		setAmbLightValue(i);
		QImage _renderedImage = grabFrameBuffer();
		projectEvaluationPoints();
		double error = DiffEvaluationPoints(_renderedImage, _subjectImage, m_projectedEvaluationPoints);
		//std::cout << "error: " << error << std::endl;
		if (error < minerror)
		{
			minerror = error;
			bestValue = i;
		}
		std::cout<<".";
	}
	std::cout<< std::endl;
	std::cout << "best ambiant light value: " << bestValue << std::endl;
	setAmbLightValue(bestValue);
	updateGL();
}

vec2d GLWidget::getVertexColor()
{
	vec2d colors;

	GLdouble tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0);
	glRotated(xRot, 1.0, 0.0, 0.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(zRot, 0.0, 0.0, 1.0);
	glScaled( 0.5, 0.5, 0.5);
	glTranslated( modelx, modely, modelz);
	glGetDoublev(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	//for (int i = 0; i < 16; i++)
	//{
	//	qDebug() << tempMatrix[i];
	//}
	//qDebug() << "//////////////" ;
	//for (int i = 0; i < 16; i++)
	//{
	//	qDebug() << m_modelviewMatrix[i];
	//}
	
	vec2d _shape = m_FaceModel.shape;
	vec2d _normal = m_FaceModel.normal;
	vec2d _texture = m_FaceModel.text;

	for(int i = 0; i < _shape.size(); i++)
	
	{
	vec1d vertex = _shape[i];
	vec1d normal = _normal[i];
	vec1d texture = _texture[i];
	//vec1d vertex_w; 
	
	vertex_w.push_back(vertex[0]*tempMatrix[0] + vertex[1]*tempMatrix[4] + vertex[2] * tempMatrix[8] + tempMatrix[12]);
	vertex_w.push_back(vertex[0]*tempMatrix[1] + vertex[1]*tempMatrix[5] + vertex[2] * tempMatrix[9] + tempMatrix[13]);
	vertex_w.push_back(vertex[0]*tempMatrix[2] + vertex[1]*tempMatrix[6] + vertex[2] * tempMatrix[10] + tempMatrix[14]);

	/*vec1d vertex_w1;

	vertex_w1.push_back(vertex[0]*tempMatrix[0] + vertex[1]*tempMatrix[4]+vertex[2] * tempMatrix[8] + tempMatrix[12]);
	vertex_w1.push_back(vertex[0]*tempMatrix[1] + vertex[1]*tempMatrix[5]+vertex[2] * tempMatrix[9] + tempMatrix[13]);
	vertex_w1.push_back(vertex[0]*tempMatrix[2] + vertex[1]*tempMatrix[6]+vertex[2] * tempMatrix[10] + tempMatrix[14]);*/
	
	vec1d light;
	light.push_back(lightPos[0]);
	light.push_back(lightPos[1]);
	light.push_back(lightPos[2]);

	/*vec1d lightw;
	lightw.push_back(light[0]*tempMatrix[0] + light[1]*tempMatrix[4] + light[2] * tempMatrix[8] + tempMatrix[12]);
	lightw.push_back(light[0]*tempMatrix[1] + light[1]*tempMatrix[5] + light[2] * tempMatrix[9] + tempMatrix[13]);
	lightw.push_back(light[0]*tempMatrix[2] + light[1]*tempMatrix[6] + light[2] * tempMatrix[10] + tempMatrix[14]);*/

	vec1d lightwn = normalize(light);

	//qDebug() << "light: " << lightwn[0] << " " << lightwn[1] << " " << lightwn[2] ;

	//vec1d vertextolight;
	//vertextolight.push_back(lightPos[0]-vertex_w[0]);
	//vertextolight.push_back(lightPos[1]-vertex_w[1]);
	//vertextolight.push_back(lightPos[2]-vertex_w[2]);

	//double vertextolight_length = qSqrt( vertextolight[0]*vertextolight[0] + vertextolight[1]*vertextolight[1] + vertextolight[2]*vertextolight[2]);

	/*vertextolight[0] = vertextolight[0]/vertextolight_length;
	vertextolight[1] = vertextolight[1]/vertextolight_length;
	vertextolight[2] = vertextolight[2]/vertextolight_length;*/

	vec1d normal_w;
	normal_w.push_back(normal[0]*tempMatrix[0] + normal[1]*tempMatrix[4]+ normal[2]*tempMatrix[8] + tempMatrix[12]);
	normal_w.push_back(normal[0]*tempMatrix[1] + normal[1]*tempMatrix[5]+ normal[2]*tempMatrix[9] + tempMatrix[13]);
	normal_w.push_back(normal[0]*tempMatrix[2] + normal[1]*tempMatrix[6]+ normal[2]*tempMatrix[10] + tempMatrix[14]);

	vec1d normalwn = normalize(normal_w);

	
	double diffusionvalue = (normalwn[0]*lightwn[0] + normalwn[1]*lightwn[1] + normalwn[2]*lightwn[2]);
	diffusionvalue = std::max(diffusionvalue, 0.0);

	//qDebug() << "diffuse value: " << diffusionvalue ;
	//qDebug() << "ambLightValue: " << ambLightValue;

	/*double r = texture[0]*(diffusionvalue + ambLightValue + 0.2);;
	double g = texture[1]*(diffusionvalue + ambLightValue + 0.2);
	double b = texture[2]*(diffusionvalue + ambLightValue + 0.2);*/

	/*double r = texture[0]*diffusionvalue + texture[0]*(ambLightValue + 0.1);
	double g = texture[1]*diffusionvalue + texture[1]*(ambLightValue + 0.1);
	double b = texture[2]*diffusionvalue + texture[2]*(ambLightValue + 0.1);*/
	vec1d color;

	double r = texture[0]*(0.1 + ambLightValue + diffusionvalue*0.5);
	double g = texture[1]*(0.1 + ambLightValue + diffusionvalue*0.5);
	double b = texture[2]*(0.1 + ambLightValue + diffusionvalue*0.5);

	color.push_back(std::min(r, 255.0));
	color.push_back(std::min(g, 255.0));
	color.push_back(std::min(b, 255.0));

	colors.push_back(color);
	//qDebug() << "rgb1: " << r<< g << b;
	//qDebug() << "rgb2: " << texture[0]  << texture[1]  << texture[2];

	/*std::vector<int> indices;
	indices.push_back(20000);
	
	std::vector<cv::Point> points;
	projectVertices(indices, points);

	QImage _renderedImage = grabFrameBuffer();

	QColor tcolor = _renderedImage.pixel(points[0].x, points[0].y);*/
	
	
	//qDebug() << "rgb3: " << tcolor.red()<< " " << tcolor.green()<< " " << tcolor.blue();
	//qDebug() << "light Direction: " << lightPos[0] << " " <<  lightPos[1] << " " <<  lightPos[2];
	//qDebug() << "normal: " << normalwn[0] << " " << normalwn[1] << " " << normalwn[2] ;

	//c = qRgb((int)r, (int)g, (int)b);
	}
	return colors;
}

void GLWidget::getVertexColorTest()
{
	vec2d colors;

	GLfloat tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0);
	glRotated(xRot, 1.0, 0.0, 0.0);
    glRotated(yRot, 0.0, 1.0, 0.0);
    glRotated(zRot, 0.0, 0.0, 1.0);
	glScaled( 0.5, 0.5, 0.5);
	glTranslated( modelx, modely, modelz);
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	cv::Mat matrix;
	openglMatrixToCvMat(tempMatrix, matrix);

	cv::Mat tmat = matrix.t();
	cv::Mat invmat =  tmat.inv();


	/*for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			std::cout << tempMatrix[i*4 + j]<< "  " ;
		}
		std::cout << std::endl;
	}*/
	
	vec2d _shape = m_FaceModel.shape;
	vec2d _normal = m_FaceModel.normal;
	vec2d _texture = m_FaceModel.text;


	for(int i = 30005; i < 30015; i++)
	{
	vec1d vertex = _shape[i];
	vec1d normal = _normal[i];
	vec1d texture = _texture[i];
	//vec1d vertex_w; 
	
		
	vec1d light;
	light.push_back(lightPos[0]*invmat.at<float>(0,0) + lightPos[1]*invmat.at<float>(0,1)+lightPos[2]*invmat.at<float>(0,2)+ invmat.at<float>(0,3));
	light.push_back(lightPos[0]*invmat.at<float>(1,0) + lightPos[1]*invmat.at<float>(1,1)+lightPos[2]*invmat.at<float>(1,2)+ invmat.at<float>(1,3));
	light.push_back(lightPos[0]*invmat.at<float>(2,0) + lightPos[1]*invmat.at<float>(2,1)+lightPos[2]*invmat.at<float>(2,2)+ invmat.at<float>(2,3));

	//lightw.push_back(light[0]*tempMatrix[0] + light[1]*tempMatrix[4] + light[2] * tempMatrix[8] + tempMatrix[12]);
	//lightw.push_back(light[0]*tempMatrix[1] + light[1]*tempMatrix[5] + light[2] * tempMatrix[9] + tempMatrix[13]);
	//lightw.push_back(light[0]*tempMatrix[2] + light[1]*tempMatrix[6] + light[2] * tempMatrix[10] + tempMatrix[14]);*/

    
	vec1d lightwn = normalize(light);

	vec1d normalwn = normalize(normal);

	
	double diffusionvalue = (normalwn[0]*lightwn[0] + normalwn[1]*lightwn[1] + normalwn[2]*lightwn[2]);
	diffusionvalue = std::max(diffusionvalue, 0.0);

	vec1d color;

	double r = texture[0]*(0.1 + ambLightValue + diffusionvalue*0.5);
	double g = texture[1]*(0.1 + ambLightValue + diffusionvalue*0.5);
	double b = texture[2]*(0.1 + ambLightValue + diffusionvalue*0.5);

	color.push_back(r);
	color.push_back(g);
	color.push_back(b);

	colors.push_back(color);
	qDebug() << "rgb1: " << r<< g << b;

	std::vector<int> indices;
	indices.push_back(i);
	
	std::vector<cv::Point> points;
	projectVertices(indices, points);

	QImage _renderedImage = grabFrameBuffer();

	QColor tcolor = _renderedImage.pixel(points[0].x, points[0].y);
	
	
	qDebug() << "rgb3: " << tcolor.red()<< " " << tcolor.green()<< " " << tcolor.blue();
	
	}
	
}


void GLWidget::testRendering(const QImage& _subjectImage, QImage& testImage)
{
	projectVertices();
	testImage = QImage(grabFrameBuffer());

	GLfloat tempMatrix[16];
	glPushMatrix();
	glLoadIdentity();
	//gluLookAt(camx, camy, camz, fx, fy, fz, 0.0, 1.0, 0.0);
	glRotated(xRot, 1.0, 0.0, 0.0);
	glRotated(yRot, 0.0, 1.0, 0.0);
	glRotated(zRot, 0.0, 0.0, 1.0);
	glScaled( 0.5, 0.5, 0.5);
	glTranslated( modelx, modely, modelz);
	glGetFloatv(GL_MODELVIEW_MATRIX, tempMatrix);
	glPopMatrix();

	cv::Mat matrix;
	openglMatrixToCvMat(tempMatrix, matrix);
	cv::Mat invmat = matrix.t().inv();  //get inverse matrix to tranfer light direction vector into face coordinate system
	
	vec1d light;
	light.push_back(lightPos[0]*invmat.at<float>(0,0) + lightPos[1]*invmat.at<float>(0,1)+lightPos[2]*invmat.at<float>(0,2)+ invmat.at<float>(0,3));
	light.push_back(lightPos[0]*invmat.at<float>(1,0) + lightPos[1]*invmat.at<float>(1,1)+lightPos[2]*invmat.at<float>(1,2)+ invmat.at<float>(1,3));
	light.push_back(lightPos[0]*invmat.at<float>(2,0) + lightPos[1]*invmat.at<float>(2,1)+lightPos[2]*invmat.at<float>(2,2)+ invmat.at<float>(2,3));

	vec1d lightwn = normalize(light);  //compute light direction vector under face coord system.

	
	qDebug()<<"light:" << lightPos[0] << lightPos [1] << lightPos[2] << lightPos[3]; 
	

	
	for (int i = 0; i < m_FaceModel.shape.size(); i++)
	{
		vec1d _normal = m_FaceModel.normal[i];
		vec1d _texture = m_FaceModel.text[i];
		_normal = normalize(_normal);

		double diffusionvalue = (_normal[0]*lightwn[0] + _normal[1]*lightwn[1] + _normal[2]*lightwn[2]);
		diffusionvalue = std::max(diffusionvalue, 0.0);

		

		double r = _texture[0]*(0.1 + ambLightValue + diffusionvalue*0.5);
		double g = _texture[1]*(0.1 + ambLightValue + diffusionvalue*0.5);
		double b = _texture[2]*(0.1 + ambLightValue + diffusionvalue*0.5);

		testImage.setPixel(m_projectedVertices[i].x, m_projectedVertices[i].y, qRgb(r, g, b));

		
	}
	

		
	
}


