/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/


#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include "facePCA.h"

class GLParameters
{
public:
	GLParameters()
	{
		xRot = 90.0;
		yRot = 180.0;
		zRot = 0.0;

		lightPos[0] = -1.0f;
		lightPos[1] = 0.0f;
		lightPos[2] = 0.0f;
		lightPos[3] = 0.0f;

		ambLightValue = 0.0f;

		camx = 0.0;
		camy = 30.0;
		camz = 280.0;

		fx = 0.0;
		fy = 0.0;
		fz = 0.0;
	};

	inline void setLightParams(GLfloat _x, GLfloat _y, GLfloat _z, GLfloat _amblightvalue)
	{lightPos[0] = _x; lightPos[1] = _y; lightPos[2] = _z; lightPos[3] = 1.0f; ambLightValue = _amblightvalue;}; 
	inline void setCameraParams(double _camx, double _camy, double _camz, double _fx, double _fy, double _fz)
	{camx = _camx; camy = _camy; camz = _camz; fx = _fx; fy = _fy; fz = _fz;};
	inline void setModelParams(int _xRot, int _yRot, int _zRot)
	{xRot = _xRot; yRot = _yRot; zRot = _zRot;};

	void loadParameters();
	void writeParameters();
	//lighting
	GLfloat lightPos[4];
	GLfloat ambLightValue;
	
	//model rotation
    int xRot, yRot, zRot;


	//camera position
	double camx, camy, camz;
	double fx, fy, fz;

};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    double xRotation() const { return xRot; }
    double yRotation() const { return yRot; }
    double zRotation() const { return zRot; }

	void loadData();
	void loadGLParams(std::string filename);
	void writeGLParams(std::string filename); 
	void saveCurrentProject();
	void loadProject();
	//void loadImage(QString filename);
	void getCurrentMatrices();
	void setFeaturePoints(std::vector<cv::Point> fps){m_featurePoints = fps;}; // feature points image coordinates.
	std::vector<int> getFeaturePointTriangleIDs(); //feature points triangle id.
	void highlightTriangles(); // highlight feature points triangles.
	void highlightFeaturePoints(); // highlight feature points.
	void projectFeaturePoints(); // only project feature points to image plane. Entry point of projecting op.
	void projectEvaluationPoints();
	void myProjectFeaturePoints(); // testing my project function. test use only...

	void projectVertices(); // project all vertices. for texture pasting...
	void projectVertices(std::vector<int>& _vertexIndices, std::vector<cv::Point>& _projectedCoords);           //

	void optimizeDirectionLight(QImage _subjectImage);
	void optimizeAmbLight(QImage _subjectImage);

	void blendEar();

	double updateModel(const std::vector<cv::Point>& _targetFeaturePointsCoord, const std::vector<cv::Point>& _targetContourPointsCoord, const std::vector<cv::Point>& _targetProfilePointsCoord); //main face model fitting function...return distance of feature points.
	double updateModelZ(const QImage& _subjectImage, QImage& testImage); //step 2 fitting function for updating depth value.
	double updateModelZMethod2(const QImage& _subjectImage,  QImage& testImage);

	void testRendering(const QImage& _subjectImage,QImage& testImage);

	void pasteTexture(QImage inputImage); //paste texture from input image.

	void writeToPLYFile(QString filename, vec2d data);
	void writeToPLYFile(QString filename);

	void resetModel();

	std::vector<int> getVisibleIndex();
	std::vector<int> getSegmentIndex(int segment);

	vec2d getVertexColor();
	void getVertexColorTest();

	void hideTexture();
	void specularON();

	void saveFrontMatrix();
	void saveSideMatrix();
    void saveProfileMatrix();


public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

	void setXRotation(double angle);
    void setYRotation(double angle);
    void setZRotation(double angle);

	void setCamPosition(int z);

	void setLightPositionX(int angle);
	void setLightPositionY(int y);

	void setAmbLightValue(int x);


signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

	void camPosChanged(int z);
	void LightChanged();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent(QKeyEvent *event);
	//void paintEvent(QPaintEvent *event);

private slots:
    void upDateGL();

private:
    GLuint makeGear(const GLfloat *reflectance, GLdouble innerRadius,
                    GLdouble outerRadius, GLdouble thickness,
                    GLdouble toothSize, GLint toothCount);
    void drawGear(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                  GLdouble angle);

	GLuint makeFace(const CFacePCA& model);
	GLuint makeAverageFace(const GLfloat *reflectance, const CFacePCA& model);
    void drawFace(GLuint gear, GLdouble dx, GLdouble dy, GLdouble dz,
                  GLdouble angle);

	void drawAxis();

	void drawGround();

    void normalizeAngle(int *angle);
	void normalizeAngle(double *angle);

    GLuint gear1;
    GLuint gear2;
    GLuint gear3;

	GLuint face, aveface;

    double xRot;
    double yRot;
    double zRot;
    int gear1Rot;
    

	GLfloat lightPos[4];
	
	GLfloat ambLightValue;

	double camx, camy, camz;
	double fx, fy, fz;
	double nearplane, farplane;

	double modelx, modely, modelz;

    QPoint lastPos;  //mouse position

public:
	CFacePCA m_FaceModel;
	GLParameters *m_params;

	std::vector<cv::Point> m_featurePoints; //
	
	std::vector<cv::Point> m_projectedVertices; // image coordinates of vertices
	std::vector<cv::Point> m_myProjectedFeaturePoints; // for myprojection function test;
	
	std::vector<int> m_evaluationPointVertexID;
	std::vector<cv::Point> m_projectedEvaluationPoints;

	std::vector<int> m_featurePointsVertexID_z; //for updating depth value
	std::vector<cv::Point> m_projectedfeaturePointZ;

	std::vector<int> m_featurePointsTriangleID;
	std::vector<int> m_featurePointsVertexID;

	std::vector<int> m_visibleVertexID;
	std::vector<cv::Point> m_projectedFeaturePoints; //image coordinates of feature points

	std::vector<int> m_faceContourVertexID;   //45 degree side
	std::vector<int> m_faceProfileVertexID;   //90 degree side

	

	bool b_showAxis;
	bool b_showHighlight;
	bool b_needProject;
	bool b_isProjectionDone;
	bool b_showTexture;
	bool b_specluarON;

	vec2d newTexture;

	GLdouble m_modelviewMatrix[16];    //model view matrix
	GLdouble m_projectionMatrix[16];   //projection matrix
	GLint m_viewportMatrix[4];         //viewport matrix
	
	GLdouble m_frontMatrix[16];    //front matrix
	GLdouble m_sideMatrix[16];     //side matrix
	GLdouble m_profileMatrix[16];

	GLint m_frontViewportMatrix[4];
	GLint m_sideViewportMatrix[4];
	GLint m_profileViewportMatrix[4];

	vec1d vertex_w;
};

#endif
