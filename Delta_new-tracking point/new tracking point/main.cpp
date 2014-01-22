#include "time.h" 
#include"Delta.h"
#include <omp.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdio.h>


using namespace cv;
using namespace std;

void toHandFind(void* ptr);
Delta delta;

	clock_t start, finish; 
	double   duration;  


int main()
{
	//------------------initial-----------
	
	delta.grabFrame();

	

	IplImage *img ;
	img=delta.img;

	namedWindow( "workDetec",1 );
	//------------------------------------
	delta.findConveyorAngle();
	delta.findConveyorAngle();
	_beginthread(toHandFind,0,NULL);
	while(1)
	{
		//delta.toHandFind();
		//delta.findConveyorAngle();
		delta.waitToFetch();
	}
	
	char saveName[100];

	return 0;

}



void toHandFind(void* ptr)
{
	float factor = 0.25;
	float angle;
	float fetchPointX;
	bool detect = false;
	int sumY = 0,sumX = 0;
	cv::Mat tmpMat2(delta.oimg.size(),CV_8U,cv::Scalar(0)); 
	xAndTime xTime;

	while(1)
	{
		delta.grabToHand();
		Sleep(30);
		delta.grabToHand();

		//*****Preprocess******
		cv::Mat tmpMat = delta.oimg.clone();
		delta.setROI(-50,0,-2,-130); //-80 -170
		delta.oimg.adjustROI(delta.roiTop,delta.roiButton-250,delta.roiLeft,delta.roiRight);
		cv::cvtColor(delta.oimg,tmpMat,CV_RGB2GRAY);
		cv::threshold(tmpMat,tmpMat,70,255,CV_THRESH_BINARY);
		cv::dilate(tmpMat,tmpMat,cv::Mat());
		//*********************
		
		std::vector< std::vector<cv::Point> > contours;
		int i=0;
		cv::findContours(tmpMat,contours,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
		for(auto contour =  contours.begin();contour != contours.end();contour++)
		{
			if(contour->size() > 100 && contour->size()<200)
			{
				cv::drawContours(delta.oimg,contours,i,cv::Scalar(255,0,0),2);
				cv::drawContours(tmpMat2,contours,i,cv::Scalar(255,255,255),2);

				for(auto point = contour->begin();point!=contour->end();point++)
				{
					sumY += point->y;
					sumX += point->x;
				}
				sumY = sumY/contour->size();			//momemton
				sumX = sumX/contour->size();
				

				std::vector<cv::Vec4i> lines;
				cv::HoughLinesP(tmpMat2, lines, 1, CV_PI/180, 15, 30, 5 );
				if(!lines.empty())
				{
					cv::Vec4i l =lines[0] ;

					double nx = (double)(l[0]+l[2])/2;
					double ny = (double)(l[1]+l[3])/2;
					double m = -(double)(l[3]-l[1]) / (l[2]-l[0]);

					m=-1.0/m;
					angle = 180.0*atan( m )/3.1415926;   //slope of piece

					//********align fetch point*************
					double shiftR = 8;
					if(ny<sumY)
					{
						if(angle<0)
						{
							sumX-=shiftR*cos(angle*3.14159/180);
							sumY+=shiftR*sin(angle*3.14159/180);
						}
						else
						{
							sumX+=shiftR*cos(angle*3.14159/180);
							sumY-=shiftR*sin(angle*3.14159/180);
						}
					}
					else
					{
						if(angle<0)
						{
							sumX+=shiftR*cos(angle*3.14159/180);
							sumY-=shiftR*sin(angle*3.14159/180);
						}
						else
						{
							sumX-=shiftR*cos(angle*3.14159/180);
							sumY+=shiftR*sin(angle*3.14159/180);
						}
					}
					cv::circle(delta.oimg,cv::Point(sumX,sumY),3,cv::Scalar(255,255,255),1);
					//********align fetch point*************

					angle = 180.0*atan( -(double)(l[3]-l[1]) / (l[2]-l[0]) )/3.1415926;

					//****fetchPoint***
					if(sumX > 185)
						fetchPointX = (float)(sumX-175)*(-0.95);
					else if(sumX > 175)
						fetchPointX = (float)(sumX-175)*(-0.9);
					else
						fetchPointX = (float)(175-sumX)*0.55; 
					//-*************
					detect = true;

					xTime.fetchPointX = fetchPointX;
	
					if( clock() - delta.timeFlag  > 500)//2500  reject too close piece
					{
						xTime.time = clock();
						delta.timeFlag = xTime.time;
						delta.fetchQueue.push(xTime);
					}

				}
				
			}
			i++;
		}
		imshow("tt",delta.oimg);
	}
	_endthread();
}