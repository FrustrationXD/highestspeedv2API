#include <WINSOCK2.H>
#include <stdio.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <queue>
#include <vector>
#pragma comment(lib,"ws2_32.lib")

//#include <opencv\cv.h>
//#include <opencv\highgui.h>
//#include <opencv\cxcore.h>

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>

#include <windows.h>
#include <mmsystem.h>
#include "HLGrab.h"
#define _NIWIN  
#include "niimaq.h"
#include<process.h>
#include<time.h>

struct xAndTime
{
	float fetchPointX;
	double time;
};

class Delta 
{
private:
	static HANDLE HThread;
	static HWND    	    ImaqSmplHwnd;
	
	static SESSION_ID   Sid ;
	static BUFLIST_ID   Bid ;
	static INTERFACE_ID Iid ;
	static Int32        AcqWinWidth;
	static Int32        AcqWinHeight;
	static BOOL			StopGrab ;
	
	WSADATA wsaData;
	SOCKET m_socket;
	sockaddr_in service;
	SOCKET AcceptSocket;

public:
	bool detected;
	static Int8*        ImaqBuffer;    // acquisiton buffer
	bool useSocket;
	char sendbuf[100];
	char recvbuf[50000];
	float a,b,c,d;						//�ഫ�x�}
	float angle;
	bool  angleD;
	double pixel2mm;
	double timeFlag;
	int velocity;
	cv::Point2d original;
	cv::Point2f moveToPoint;					//�n�쪺�I
	cv::Point2f nowPosition;					//End-Effect�{�b���I
	cv::Point2f center;
	cv::VideoCapture cap;
	cv::Mat oimg;						//eye-to-hand���v��
	cv::Mat oimg_roi;					//roi���v��
	cv::Mat pre_oimg;
	cv::Mat pre_oimg_roi;
	IplImage *img;						//eye-in-hand��쪺�v���b�o
	IplImage *img_ToHand;
	IplImage *img_InHand;
	std::vector< cv::Point > validContourCenters;
	std::queue<xAndTime> fetchQueue;

	float conveyorAngle;
	float fetchPointX;
	float fetchPointY;
	int roiLeft;
	int roiRight;
	int roiTop;
	int roiButton;

	Delta();
	void iniSocket();
	void iniCamera();
	void iniPathParameter();
	void setSpeed(float speed);
	void setACCTime(float time);
	void setDECTime(float time);
	void setNowPosition(cv::Point2f position);
	void setNowPosition(float nowX,float nowY);
	void getCurPose();

	void moveL(float toX,float toY,float toZ= -275.4,float R=0);	//���ʨ�x y z���tsleep�b�� z�w�]275.4
	void moveL(cv::Point2f toPoint);	//���ʨ� toPoint
	void moveL_wait(float toX,float toY,float toZ= -275.4);	//���ʨ�x y z���tsleep�b�� z�w�]275.4
	void moveL_wait(float toX,float toY,float toZ= -275.4,float R=0);	//���ʨ�x y z���tsleep�b�� z�w�]275.4
	void moveL_wait(cv::Point2f toPoint);	//���ʨ� toPoint
	void fetch(float toX,float toY, float toZ =-334);

	void grabFrame();					//��simg oimg���Ϥ�
	void grabToHand();
	void grabInHand();

	void findCenter(IplImage*);					//��s�e�����l�I���I�y��  centerX centerY 
	void findCenterOfContours();
	void sendString(char* str);		//��r��
	int recvString();				//���r�� �s��recvbuf   return -1 �N��������

	void find1114(IplImage *);
	void waitToFetch();
	void toHandFind();
	void findConveyorAngle();
	
	void test(void* ptr);

	void setROI(int dtop,int dbutton,int dleft,int dright);
	void findContour();
	void getShift();
	cv::Point2d pixel2Delta(cv::Point );
};



