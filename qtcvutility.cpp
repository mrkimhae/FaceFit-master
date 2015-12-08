/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/

#include "qtcvutility.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

const char* QStringToChar(QString qs)
{
	QByteArray ba = qs.toLatin1();
	const char *fn = ba.data(); 
	return fn;
}

void getComonSize(const QImage& im1, const QImage& im2, int& w, int& h)
{
	int width1, width2, height1, height2;
	width1 = im1.width();
	height1 = im1.height();
	width2 = im2.width();
	height2 = im2.height();

	if (width1>width2)
		w = width2;
	else
		w = width1;
	if (height1>height2)
		h = height2;
	else
		h = height1;
}

void QImageToCvMat(QImage& inImage, cv::Mat& outImage)
{
	outImage = cv::Mat(inImage.height(), inImage.width() ,CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());
}

void CvMatToQImage(cv::Mat& inImage, QImage& outImage)
{
	outImage = QImage((uchar*)inImage.data, inImage.cols, inImage.rows, QImage::Format_ARGB32);

}

void QImageColorToGray(const QImage& inImage, QImage& outImage)
{
	cv::Mat tmpImage = cv::Mat(inImage.height(), inImage.width() ,CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine());
	cv::Mat greyImage;
	cv::cvtColor(tmpImage, greyImage, cv::COLOR_BGR2GRAY);
	
	static QVector<QRgb>  sColorTable;
 
    if ( sColorTable.isEmpty() )
    {
        for ( int i = 0; i < 256; ++i )
            sColorTable.push_back( qRgb( i, i, i ) );
    }
 
	outImage = QImage( greyImage.data, greyImage.cols, greyImage.rows, greyImage.step, QImage::Format_Indexed8 );
 
    outImage.setColorTable( sColorTable );

}

cv::Mat DiffQImages(const QImage& inImage1, const QImage& inImage2, QImage& outImage)
{
	int w, h;
	getComonSize(inImage1, inImage2, w, h);



	cv::Mat tmpImage1 = cv::Mat(inImage1.height(), inImage1.width() ,CV_8UC4, const_cast<uchar*>(inImage1.bits()), inImage1.bytesPerLine());
	cv::Mat greyImage1;
	cv::Mat tmpImage2 = cv::Mat(inImage2.height(), inImage2.width() ,CV_8UC4, const_cast<uchar*>(inImage2.bits()), inImage2.bytesPerLine());
	cv::Mat greyImage2;
	cv::cvtColor(tmpImage1, greyImage1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(tmpImage2, greyImage2, cv::COLOR_BGR2GRAY);

	cv::Mat ggray1 = greyImage1(cv::Rect(0,0,w,h));
	cv::Mat ggray2 = greyImage2(cv::Rect(0,0,w,h));

	cv::Mat diffImage = ggray2 - ggray1;

	//cv::imshow("diffImage", diffImage);

	static QVector<QRgb>  sColorTable;
 
    if ( sColorTable.isEmpty() )
    {
        for ( int i = 0; i < 256; ++i )
            sColorTable.push_back( qRgb( i, i, i ) );
    }
 
	outImage = QImage( diffImage.data, diffImage.cols, diffImage.rows, diffImage.step, QImage::Format_Indexed8 );
 
    outImage.setColorTable( sColorTable );

	return diffImage;
}


double DiffEvaluationPoints(const QImage& inImage1, const QImage& inImage2, std::vector<cv::Point> _evaluationPoints) //deprecated
{
	cv::Mat tmpImage1 = cv::Mat(inImage1.height(), inImage1.width() ,CV_8UC4, const_cast<uchar*>(inImage1.bits()), inImage1.bytesPerLine());
	cv::Mat greyImage1;
	cv::Mat tmpImage2 = cv::Mat(inImage2.height(), inImage2.width() ,CV_8UC4, const_cast<uchar*>(inImage2.bits()), inImage2.bytesPerLine());
	cv::Mat greyImage2;
	cv::cvtColor(tmpImage1, greyImage1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(tmpImage2, greyImage2, cv::COLOR_BGR2GRAY);

	double error = 0;
	uchar g1, g2;
	for (int i = 0; i < _evaluationPoints.size(); i++)
	{
		g1 = greyImage1.at<uchar>(_evaluationPoints[i]);
		g2 = greyImage2.at<uchar>(_evaluationPoints[i]);
		error += ((int)g1 - (int)g2)*((int)g1 - (int)g2);
		//std::cout << (int)g1 << " " << (int)g2 << std::endl;
	}
	error = error/_evaluationPoints.size();
	return error;
}


double DiffEvaluationPoints(const QImage& inImage1, const QImage& inImage2, std::vector<cv::Point> _evaluationPoints, std::vector<double>& errors)
{
	cv::Mat tmpImage1 = cv::Mat(inImage1.height(), inImage1.width() ,CV_8UC4, const_cast<uchar*>(inImage1.bits()), inImage1.bytesPerLine());
	cv::Mat greyImage1;
	cv::Mat tmpImage2 = cv::Mat(inImage2.height(), inImage2.width() ,CV_8UC4, const_cast<uchar*>(inImage2.bits()), inImage2.bytesPerLine());
	cv::Mat greyImage2;
	cv::cvtColor(tmpImage1, greyImage1, cv::COLOR_BGR2GRAY);
	cv::cvtColor(tmpImage2, greyImage2, cv::COLOR_BGR2GRAY);

	int w, h;
	getComonSize(inImage1, inImage2, w, h);

	cv::Mat ggray1 = greyImage1(cv::Rect(0,0,w,h));
	cv::Mat ggray2 = greyImage2(cv::Rect(0,0,w,h));

	cv::Mat diffImage = ggray2 - ggray1;

	errors.clear();

	uchar g1, g2;
	for (int i = 0; i < _evaluationPoints.size(); i++)
	{
		//g1 = greyImage1.at<uchar>(_evaluationPoints[i]);
		//g2 = greyImage2.at<uchar>(_evaluationPoints[i]);
		int error = diffImage.at<uchar>(_evaluationPoints[i]);
		errors.push_back(error);
	}
	double errorsum = 0;
	for (int i = 0; i < errors.size(); i++)
	{
		errorsum += errors[i];
	}
	
	return errorsum/errors.size();
}

void HighLightPixels(QImage& image, std::vector<cv::Point> _coords) 
{
	cv::Mat cvImage;
	QImageToCvMat(image, cvImage);
	for (int i = 0; i < _coords.size(); i++)
	{
		cv::circle(cvImage, _coords[i], 2, cv::Scalar(0, 255, 255) );
	}

}


void WriteToFile(std::string filename, std::vector<int> data)
{
	std::cout << "Writing to file: " << filename << "...";
	std::ofstream outfile;

	outfile.open(filename);
	for (int i = 0 ; i < data.size(); i ++)
	{
		if (i < data.size()-1)
			outfile << data[i] << std::endl;
		else
			outfile << data[i] ;
	}
	outfile.close();
	std::cout << "Done" << std::endl;
}

void WriteToFile(std::string filename, std::vector<float> data)
{
	std::ofstream outfile;

	outfile.open(filename, std::ostream::out|std::ostream::app);
	for (int i = 0 ; i < data.size(); i ++)
	{
		outfile << data[i] << ",";
	}
	outfile << std::endl;
	outfile.close();

}

void WriteToFile(std::string filename, int data)
{
	std::ofstream outfile;

	outfile.open(filename, std::ostream::out|std::ostream::app);
	
	outfile << data << std::endl;
	outfile.close();

}

void ReadFromFile(std::string filename, std::vector<int>& data)
{
	std::cout << "Reading from file: " << filename << "...";
	std::ifstream infile;
	int tmpData;
	infile.open(filename);
	while (!infile.eof())
	{
		infile >> tmpData;
		data.push_back(tmpData);
	}
	std::cout << "Done" << std::endl;
}

int glhProjectf(double objx, double objy, double objz, double *modelview, double *projection, int *viewport, double& winx, double& winy)
{
    //Transformation vectors
    double fTempo[8];
    //Modelview transform
    fTempo[0]=modelview[0]*objx+modelview[4]*objy+modelview[8]*objz+modelview[12];  //w is always 1
    fTempo[1]=modelview[1]*objx+modelview[5]*objy+modelview[9]*objz+modelview[13];
    fTempo[2]=modelview[2]*objx+modelview[6]*objy+modelview[10]*objz+modelview[14];
    fTempo[3]=modelview[3]*objx+modelview[7]*objy+modelview[11]*objz+modelview[15];
    //Projection transform, the final row of projection matrix is always [0 0 -1 0]
    //so we optimize for that.
    fTempo[4]=projection[0]*fTempo[0]+projection[4]*fTempo[1]+projection[8]*fTempo[2]+projection[12]*fTempo[3];
    fTempo[5]=projection[1]*fTempo[0]+projection[5]*fTempo[1]+projection[9]*fTempo[2]+projection[13]*fTempo[3];
    fTempo[6]=projection[2]*fTempo[0]+projection[6]*fTempo[1]+projection[10]*fTempo[2]+projection[14]*fTempo[3];
    fTempo[7]=-fTempo[2];
    //The result normalizes between -1 and 1
    if(fTempo[7]==0.0)	//The w value
        return 0;
    fTempo[7]=1.0/fTempo[7];
    //Perspective division
    fTempo[4]*=fTempo[7];
    fTempo[5]*=fTempo[7];
    fTempo[6]*=fTempo[7];
    //Window coordinates
    //Map x, y to range 0-1
    winx=(fTempo[4]*0.5+0.5)*viewport[2]+viewport[0];
    winy=(fTempo[5]*0.5+0.5)*viewport[3]+viewport[1];
    //This is only correct when glDepthRange(0.0, 1.0)
    //windowCoordinate[2]=(1.0+fTempo[6])*0.5;	//Between 0 and 1
    return 1;
}



double innerProduct(std::vector<double> vec1, std::vector<double> vec2)
{
	double ans = 0;
	for (int i = 0; i < vec1.size(); i++)
	{
		ans = ans + vec1[i]*vec2[i];
	}
	return ans;
}


int LoadCSVFile(std::string filename , vec2d& data)
{
  std::cout<< "Loading " << filename << "....";
  std::ifstream infile( filename );

  while (infile)
  {
    std::string s;
    if (!getline( infile, s )) break;

    std::istringstream ss( s );
    vec1d record;

    while (ss)
    {
      std::string s;
      if (!getline( ss, s, ',' )) break;
	  double val = atof(s.c_str());
      record.push_back( val);
    }

    data.push_back( record );
  }
  std::cout<< "Done" << std::endl;
   return 1; 
}

int LoadCSVFile(std::string filename , vec1d & data)
{
  std::cout<< "Loading " << filename << "....";
  std::ifstream infile( filename );
  data.clear();
  while (infile)
  {
    std::string s;
    if (!getline( infile, s )) break;
	double val = atof(s.c_str());
     

    data.push_back( val );
  }
   std::cout<< "Done" << std::endl;
   return 1; 
}


void WriteToFile(std::string fn,vec1d data)
{
	std::ofstream outfile;

	outfile.open(fn);
	for (int i = 0 ; i < data.size(); i ++)
	{
		outfile << data[i] << std::endl;
	}
	outfile.close();

}

void WriteToFile(std::string filename,vec2d data)
{
	std::ofstream outfile;

	outfile.open(filename);
	for (int i = 0 ; i < data.size(); i ++)
	{
		for(int j =0; j < data[i].size(); j++)
			outfile << data[i][j] << " " ;
		outfile << std::endl;
	}
	outfile.close();

}

void WriteToFile(std::string filename, std::vector<std::vector<int>> data)
{
	std::ofstream outfile;

	outfile.open(filename);
	for (int i = 0 ; i < data.size(); i ++)
	{
		for(int j =0; j < data[i].size(); j++)
			outfile << data[i][j] << " " ;
		outfile << std::endl;
	}
	outfile.close();
}

void WriteToCSVFile(std::string filename, std::vector<std::vector<int>> data)
{
	std::ofstream outfile;

	outfile.open(filename);
	for (int i = 0 ; i < data.size(); i ++)
	{
		for(int j =0; j < data[i].size(); j++)
			outfile << data[i][j] << "," ;
		outfile << std::endl;
	}
	outfile.close();
}

vec1d normalize(vec1d data)
{
	vec1d output;
	double l = sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
	output.push_back(data[0]/l);
	output.push_back(data[1]/l);
	output.push_back(data[2]/l);
	return output;
}

double length(vec1d data)
{
	return std::sqrt(data[0]*data[0] + data[1]*data[1] + data[2]*data[2]);
}

double distance(vec1d vertex1, vec1d vertex2)
{
	if (vertex1.size() == vertex2.size())
		return std::sqrt((vertex1[0]-vertex2[0])*(vertex1[0]-vertex2[0]) + (vertex1[1]-vertex2[1])*(vertex1[1]-vertex2[1]) + (vertex1[2]-vertex2[2])*(vertex1[2]-vertex2[2]));
	else
	{
		return -1;
		//throw  "size of two vectors are different.";
	}
}

double dotProduct(vec1d vertex1, vec1d vertex2)
{
	if (vertex1.size() == vertex2.size())
		return vertex1[0]*vertex2[0] + vertex1[1]* vertex2[1] + vertex1[2]*vertex2[2];
	else
	{
		return 0;
		//throw  "size of two vectors are different.";
	}
}

vec1d crossProduct(vec1d u, vec1d v)
{
	
	if (u.size() == 3 && v.size() == 3 )
	{
		vec1d product;
		product.push_back(u[1]*v[2] - u[2]*v[1]);
		product.push_back(u[2]*v[0] - u[0]*v[2]);
		product.push_back(u[0]*v[1] - u[1]*v[0]);
		return product;
	}

	else
	{
		vec1d product(3, 0);
		return product;
		//throw  "size of two vectors are different.";
	}
	
}


void openglMatrixToCvMat(float * a, cv::Mat& m)
{
	cv::Mat tm = cv::Mat(4, 4, CV_32F, a);
	//m = tm.t();
	m = tm;
}

