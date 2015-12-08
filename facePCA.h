/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/

#ifndef FACEPCA_H
#define FACEPCA_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <time.h>
#include <math.h>
#include "qtcvutility.h"



class CFacePCA
{
public:
	CFacePCA();
	~CFacePCA();

	void loadPCAData();
	void loadCoeffs(std::string filename);
	void generateRandomCoef();
	void generateZeroCoef();
	void generateModel(); 
	void generateShapeOnly();
	void updateNormal();
	void generateAverageFace();
	int getSize();
    
	void updateAlbedo(std::vector<cv::Point> projectedVertices, const cv::Mat& im);

	vec2d cylindarCoordinate();
	std::vector<std::vector<int>> findAdjacentVertex();
	std::vector<int> findTriangles(int vertxIndex);
	//vec1d updateCoef(std::vector<cv::Point> _modelFeaturePointsImageCoord, std::vector<cv::Point> _targetFeaturePointsImageCoord); //model feature points image coord, target feature point image coord

public:
	vec2d ShapeFromCoef(vec1d coef, vec2d PC, vec1d MU);
	vec1d RandomCoef(int length, vec1d ev);
	vec1d ZeroCoef(int length);
	
	vec2d CalculateNorm (const vec2d& vertex, const vec2d& triList);
	vec2d CalculateNormSH (const vec2d& vertex, const vec2d& triList);
    
	//vec2d cylindarCoordinate(const vec2d& vertex);

	vec2d ReshapeData(vec1d indata);

public:
	vec2d shapePC;
	vec1d shapeMU;
	vec1d shapeEV;

	vec2d textPC;
	vec1d textMU;
	vec1d textEV;

	vec1d shapeCoef;
	vec1d textCoef;

	vec2d shape;
	vec2d text;
	vec2d normal;

	vec2d trilist;   //vertex index starts from 1
	vec2d adjacentVertices;    //vertex index starts from 0

	vec2d ave_shape;
	vec2d ave_text;
	vec2d ave_normal;

	vec1d albedo;

	std::vector<int> segmentMask;
};


#endif