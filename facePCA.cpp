/****************************************************************************
*                 FaceFit
* Copyright (c) 2015 by Hai Jin, all rights reserved.
* Author:      	Hai Jin
* 
****************************************************************************/


#include "facePCA.h"
#include <fstream>
CFacePCA::CFacePCA(void)
{
}
CFacePCA::~CFacePCA(void)
{
}



void CFacePCA::loadPCAData()
{
	LoadCSVFile("../../data/shapePC.csv", this->shapePC);

	LoadCSVFile("../../data/textPC.csv", this->textPC);

	LoadCSVFile("../../data/shapeMU.csv", this->shapeMU);

	LoadCSVFile("../../data/textMU.csv", this->textMU);

	LoadCSVFile("../../data/shapeEV.csv", this->shapeEV);

	LoadCSVFile("../../data/textEV.csv", this->textEV);

	LoadCSVFile("../../data/triangulation.csv", this->trilist);

	LoadCSVFile("../../data/adjacentVertices.csv", this->adjacentVertices);

	ReadFromFile("../../data/segmentMask.txt", this->segmentMask);
}

void CFacePCA::loadCoeffs(std::string fn)
{
	LoadCSVFile(fn, shapeCoef);
}

vec1d CFacePCA::RandomCoef(int length, vec1d ev)
{
	vec1d randomCoef;
	srand (time(NULL));
	for(int i = 0; i < length; i++)
	{
		double r = (std::rand() % 20000) * ev[i];
		randomCoef.push_back((r/10000- 1));

	}
	return randomCoef;
}

vec1d CFacePCA::ZeroCoef(int length)
{
	vec1d randomCoef;
	for(int i = 0; i < length; i++)		
		randomCoef.push_back(0);
	return randomCoef;
}

void CFacePCA::generateRandomCoef()
{
	this->shapeCoef = this->RandomCoef(119, this->shapeEV);
	this->textCoef = this->RandomCoef(119, this->textEV);

}

void CFacePCA::generateZeroCoef()
{
	this->shapeCoef = this->ZeroCoef(119);
	this->textCoef = this->ZeroCoef(119);

}

vec2d CFacePCA::ShapeFromCoef(vec1d coef, vec2d PC, vec1d MU)
{
	vec1d result;
	
	for (int j = 0; j < PC.size(); j++)
	{
		double sum = 0;
		for (int k = 0; k< coef.size(); k++)
			sum += PC[j][k]*coef[k];
		
		result.push_back(sum+MU[j]);
	
	}
	//this->WriteToFile(result);

	vec2d shape = ReshapeData(result);
	return shape;
}

void CFacePCA::generateModel()
{

	this->shape.clear();
	this->text.clear();
	this->normal.clear();
	this->shape = ShapeFromCoef(shapeCoef, shapePC, shapeMU);
	this->text = ShapeFromCoef(textCoef, textPC, textMU);
	this->normal = CalculateNorm(shape, this->trilist);
	//this->normal = CalculateNormSH(shape, this->trilist);
}

void CFacePCA::generateShapeOnly()
{
	this->shape.clear();
	this->normal.clear();
	this->shape = ShapeFromCoef(shapeCoef, shapePC, shapeMU);
	//this->normal = CalculateNormSH(shape, this->trilist);
	this->normal = CalculateNorm(shape, this->trilist);
}

void CFacePCA::updateNormal()
{
	this->normal.clear();
	this->normal = CalculateNormSH(shape, this->trilist);
}

int CFacePCA::getSize()
{
	return this->shape.size();
}

vec2d CFacePCA::ReshapeData(vec1d indata)
{
	//std::cout << "Reshaping...";
	vec2d outdata;
	for (int i = 0; i < indata.size()/3; i++)
	{
		vec1d xyz;
		for (int j = 0; j <3; j++)
		{
			xyz.push_back(indata[3*i + j]); 
		}
		outdata.push_back(xyz);
	}
	//std::cout << "Done"<< std::endl;
	return outdata;
}

void CFacePCA::generateAverageFace()
{
	ave_shape = ReshapeData(shapeMU);
	ave_text = ReshapeData(textMU);
	ave_normal = CalculateNormSH(ave_shape, this->trilist);

}

vec2d CFacePCA::CalculateNorm (const vec2d& v, const vec2d& tl)
{   
	//std::cout << "Computing Noramls...";
	float nf0, nf1, nf2;
	int num_vertices = v.size();
	int num_triangles = tl.size();
	vec2d tmpnorm(num_vertices,vec1d(3));
	for (int i = 0; i < num_triangles; i++)
	{
		
		float u1 = v[tl[i][0]-1][0] - v[tl[i][1]-1][0] ;  // a.x - b.x
		float u2 = v[tl[i][0]-1][1] - v[tl[i][1]-1][1] ;  // a.y - b.y
		float u3 = v[tl[i][0]-1][2] - v[tl[i][1]-1][2] ;  // a.z - b.z
		float w1 = v[tl[i][0]-1][0] - v[tl[i][2]-1][0] ;  // a.x - c.x
		float w2 = v[tl[i][0]-1][1] - v[tl[i][2]-1][1] ;  // a.y - c.y
		float w3 = v[tl[i][0]-1][2] - v[tl[i][2]-1][2] ;  // a.z - c.z
		nf0 = u2*w3 - u3*w2;
		nf1 = u3*w1 - u1*w3;
		nf2 = u1*w2 - u2*w1;
		tmpnorm[tl[i][0]-1][0] += nf0;
		tmpnorm[tl[i][0]-1][1] += nf1;
		tmpnorm[tl[i][0]-1][2] += nf2;
		tmpnorm[tl[i][1]-1][0] += nf0;
		tmpnorm[tl[i][1]-1][1] += nf1;
		tmpnorm[tl[i][1]-1][2] += nf2;
		tmpnorm[tl[i][2]-1][0] += nf0;
		tmpnorm[tl[i][2]-1][1] += nf1;
		tmpnorm[tl[i][2]-1][2] += nf2;
	}

	//WriteToFile(tmpnorm);
	//std::cout << "normal " << tmpnorm.size() << " " << tmpnorm[0].size() << std::endl;
	//std::cout<< "Done" << std::endl;
	return tmpnorm;
};
vec2d CFacePCA::CalculateNormSH (const vec2d& v, const vec2d& tl)
{
	//std::cout << "Computing SH Noramls...";
	//std::vector<std::vector<int>> closestVertex = findAdjacentVertex();


	float nf0, nf1, nf2;
	int num_vertices = v.size();
	int num_triangles = tl.size();
	vec2d tmpnorm;
	for (int i = 0; i < num_vertices; i++)
	{
		int eastId = adjacentVertices[i][0];
		int northId = adjacentVertices[i][1];
		vec1d eastv,northv, normalv;

		if (eastId!=-1 && northId != -1 && eastId != northId)
		{
			eastv.push_back( v[eastId][0] - v[i][0] );  
			eastv.push_back( v[eastId][1] - v[i][1] );  
			eastv.push_back( v[eastId][2] - v[i][2] );  
			northv.push_back( v[northId][0] - v[i][0] );  
			northv.push_back( v[northId][1] - v[i][1] );  
			northv.push_back( v[northId][2] - v[i][2] );  
		
			normalv = crossProduct(eastv,northv);
			//normalv = normalize(normalv);
		}
		else
		{
			for (int i = 0; i < 3; i++)
			{
				normalv.push_back(0);
			}
			
		}
		
		tmpnorm.push_back(normalv);
	}

	//WriteToFile(tmpnorm);
	//std::cout << "normal " << tmpnorm.size() << " " << tmpnorm[0].size() << std::endl;
	//std::cout<< "Done" << std::endl;
	return tmpnorm;

}

vec2d CFacePCA::cylindarCoordinate()
{
	
	vec2d cycos;
	double rad, angle;
	for (int i = 0; i < shape.size(); i++)
	{
		vec1d cyco;
		rad = std::sqrt( shape[i][0] * shape[i][0] + shape[i][1] * shape[i][1] );
		angle = std::atan2(shape[i][1], shape[i][0]);

		if (angle < -PI/2)
		{
			angle = angle + 2*PI;
		}

		cyco.push_back(0);
		cyco.push_back(angle*100);
		cyco.push_back(shape[i][2]);
		cycos.push_back(cyco);
	}
	return cycos;
}

std::vector<std::vector<int>> CFacePCA::findAdjacentVertex()
{
	vec2d cycoords = cylindarCoordinate();
	//WriteToFile("cyco.txt", cycoords);
	//vec2d closestVertices; //[0] east, [1] north , if boundary, vertex index is -1;
	std::vector<std::vector<int>> closestVertexVector;
	std::vector<std::vector<int>> trianglevector;
	std::vector<int> triangles;
	for (int i = 0; i < shape.size(); i++)								//for each vertex in shape
	{
		double minangleX = 45;
		double minangleY = 45;
		int eastVertexID = -1;
		int northVertexID = -1;

		std::vector<int> closestVertices;

		triangles.clear();
		triangles = findTriangles(i+1);
		trianglevector.push_back(triangles);
		
		for (int j = 0; j < triangles.size(); j++)						// for each triangle with the vertex
		{
			int tid = triangles[j];
			for (int k = 0; k < 3; k++)									// for each vertex in the triangle
			{
				int vid = trilist[tid][k]-1;
				if (vid != i )											//for the other vertices
				{
					//check relative location of vid and i
					//compute distance
					double x = cycoords[vid][1] - cycoords[i][1];
					double y = cycoords[vid][2] - cycoords[i][2];
					
					double anglex = std::abs(atan2(y, x) * 180 / PI - 0);
					double angley = std::abs(atan2(y, x) * 180 / PI - 90);

					if (anglex < minangleX)
					{
						eastVertexID = vid;
						minangleX = anglex;
					}
					if (angley < minangleY)
					{
						northVertexID = vid;
						minangleY = angley;
					}
				}
			}
			
		}
		closestVertices.push_back(eastVertexID);
		closestVertices.push_back(northVertexID);
		closestVertexVector.push_back(closestVertices);

		
	}


	for (int i = 0; i < shape.size(); i++)
	{
		int westID = -1;
		int southID = -1;
		for (int j = 0; j < shape.size(); j++)
		{
			if (closestVertexVector[j][0] == i)
			{
				westID = j;
			}
			if (closestVertexVector[j][1] == i)
			{
				southID = j;
			}
		}
		
		closestVertexVector[i].push_back(westID);
		closestVertexVector[i].push_back(southID);

	}
	//std::cout << trianglevector[100].size() << std::endl;;

	WriteToCSVFile("adjacentVertices.csv", closestVertexVector);
	return closestVertexVector;

}

std::vector<int> CFacePCA::findTriangles(int vertexIndex)
{
	std::vector<int> triangles;
	for (int i = 0; i < trilist.size(); i++)
	{
		if (trilist[i][0] == vertexIndex||trilist[i][1] == vertexIndex||trilist[i][2] == vertexIndex)
		{
			triangles.push_back(i);
		}

	}
	return triangles;
}

void CFacePCA::updateAlbedo(std::vector<cv::Point> projectedVertices, const cv::Mat& im)
{
	albedo.clear();
	//cv::imshow("test2", im);
	for (int i = 0; i < projectedVertices.size(); i++)
	{
		
		albedo.push_back(im.at<uchar>(projectedVertices[i].y, projectedVertices[i].x));
		//if (i == 25306)
		//{
		//	//std::cout<< projectedVertices[i].x <<" "<< projectedVertices[i].y << std::endl;
		//	std::cout<< (int)im.at<uchar>(projectedVertices[i].x, projectedVertices[i].y) << std::endl;
		//}
	}
	std::cout << "Detail Update Done... \n";
}