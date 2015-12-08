/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/

#ifndef QTCVUTILITY_H
#define QTCVUTILITY_H

#include <string>
#include <QString>
#include <opencv2\core.hpp>
#include <qimage.h>

typedef std::vector<std::vector<double>> vec2d;
typedef std::vector<double> vec1d;
const double PI = std::acos(-1);



const char* QStringToChar(QString qs);
void QImageToCvMat(QImage& inImage, cv::Mat& outImage);
void CvMatToQImage(cv::Mat& inImage, QImage& outImage);
void QImageColorToGray(const QImage& inImage, QImage& outImage);

void WriteToFile(std::string filename, int data);
void WriteToFile(std::string filename, std::vector<int> data);
void WriteToFile(std::string filename, std::vector<float> data);
void WriteToFile(std::string filename, std::vector<std::vector<int>> data);
void WriteToCSVFile(std::string filename, std::vector<std::vector<int>> data);
void WriteToFile(std::string filename, vec1d data);
void WriteToFile(std::string filename, vec2d data);

int glhProjectf(double objx, double objy, double objz, double *modelview, double *projection, int *viewport, double& winx, double& winy);
void getComonSize(const QImage& im1, const QImage& im2, int& w, int& h);


cv::Mat DiffQImages(const QImage& inImage1, const QImage& inImage2, QImage& outImage);
double DiffEvaluationPoints(const QImage& inImage1, const QImage& inImage2, std::vector<cv::Point> _evaluationPoints);
double DiffEvaluationPoints(const QImage& inImage1, const QImage& inImage2, std::vector<cv::Point> _evaluationPoints, std::vector<double>& errors);

void HighLightPixels(QImage& image, std::vector<cv::Point> _coords); // highlight coordinates on image

double innerProduct(std::vector<double> vec1, std::vector<double> vec2);

void ReadFromFile(std::string filename, std::vector<int>& data);
int LoadCSVFile(std::string , vec2d& data); 
int LoadCSVFile(std::string , vec1d& data);

vec1d normalize(vec1d data);
double length(vec1d data);
double distance(vec1d vertex1, vec1d vertex2);
double dotProduct(vec1d vertex1, vec1d vertex2);
vec1d crossProduct(vec1d u, vec1d v);

void openglMatrixToCvMat(float * a, cv::Mat& m);


#endif