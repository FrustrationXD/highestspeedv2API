#include "Delta.h"


SESSION_ID	 Delta::Sid=0;
BUFLIST_ID   Delta::Bid = 0;
INTERFACE_ID Delta::Iid = 0;
Int8*        Delta::ImaqBuffer=NULL;    // acquisiton buffer
Int32        Delta::AcqWinWidth;
Int32        Delta::AcqWinHeight;
BOOL		 Delta::StopGrab ;

const int img_Width = 2040;
const int img_Height = 1086;
const int scale = 2;

void Delta::test(void* ptr)
{
;
}
int Delta::recvString()
{
	char tt[1000];
	int val=recv(m_socket,tt,strlen(tt),0);

	int nRecvBuf=1024*1024;//設置為1024K
	setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	if(val)
		strcpy(recvbuf,tt);

	return val;
}

void Delta::sendString(char* str)
{
	send( m_socket, str, strlen(str), 0 );
}

void Delta::moveL(float toX,float toY,float toZ ,float R)
{
	if(useSocket)
		return;
	useSocket=true;
	//sprintf_s(sendbuf,"%s%f %f %f 0\n WAITMOTION\n","MOVE L ",toX,toY,toZ);
	sprintf_s(sendbuf,"%s%f %f %f %f\n ","MOVE L ",toX,toY,toZ,R);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	useSocket=false;
}

void Delta::moveL(cv::Point2f toPoint)
{
	if(useSocket)
		return;
	useSocket=true;
	sprintf_s(sendbuf,"%s%f %f -275.4 0\n ","MOVE L ",toPoint.x,toPoint.y);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	useSocket=false;
	
}

void Delta::moveL_wait(float toX,float toY,float toZ )
{
	if(useSocket)
		return;
	useSocket=true;
	sprintf_s(sendbuf,"%s%f %f %f 0\n WAITMOTION\n","MOVE L ",toX,toY,toZ);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	useSocket=false;
}

void Delta::moveL_wait(float toX,float toY,float toZ,float R )
{
	if(useSocket)
		return;
	useSocket=true;
	sprintf_s(sendbuf,"%s%f %f %f %f \n WAITMOTION\n","MOVE L ",toX,toY,toZ,R);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	useSocket=false;
}

void Delta::moveL_wait(cv::Point2f toPoint)
{
	if(useSocket)
		return;
	useSocket=true;
	sprintf_s(sendbuf,"%s%f %f -275.4 0\n WAITMOTION\n","MOVE L ",toPoint.x,toPoint.y);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	useSocket=false;
}

void Delta::findCenter(IplImage *img_in)
{
	img = img_in;
	long x=0,y=0,count=0;
	cv::Mat temp;

	//IplImage* imgSmall = cvCreateImage(cvSize(img_Width/scale,img_Height/scale),IPL_DEPTH_8U,1);
	cv::resize(cv::Mat(img),temp,cvSize(img_Width/scale,img_Height/scale));
	IplImage *imgSmall = &IplImage(temp);
	
	#pragma omp parallel for

	for(int width=0;width<2040/scale;width++)
		for(int length=0;length<1086/scale;length++)
			if(cvGet2D(imgSmall,length,width).val[0]<50)
			{
				count+=1;
				x+=width;
				y+=length;
			
			}
	if(count)
	{
		x=x/count;
		y=y/count;
		x=x*scale;
		y=y*scale;
	}
	cv::imshow("workDetec",cv::Mat(imgSmall));
	cv::waitKey(1);
	if(count>1000/scale){
		x=1020-x;
		y=543-y;
		
		moveToPoint.x=a*x+b*y+nowPosition.x;
		moveToPoint.y=c*x+d*y+nowPosition.y;
		center=moveToPoint;
	}
}

void Delta::grabFrame()
{
	
	cap>>oimg;
	cv::waitKey(30);	
	img = &IplImage(oimg);

	/*
	ImaqBuffer = NULL;
	StopGrab = FALSE;
	imgGrab (Sid, (void**)&ImaqBuffer, TRUE);
	if(ImaqBuffer!= NULL)
		cvSetData(img,ImaqBuffer,AcqWinWidth);*/
}

void Delta::grabToHand()
{
	
	cap>>oimg;
	cv::waitKey(10);	
	img_ToHand =new IplImage(oimg);
	cv::waitKey(10);	
	/*
	ImaqBuffer = NULL;
	StopGrab = FALSE;
	imgGrab (Sid, (void**)&ImaqBuffer, TRUE);
	if(ImaqBuffer!= NULL)
		cvSetData(img,ImaqBuffer,AcqWinWidth);*/
}

void Delta::grabInHand()
{
	ImaqBuffer = NULL;
	StopGrab = FALSE;
	imgGrab (Sid, (void**)&ImaqBuffer, TRUE);
	if(ImaqBuffer!= NULL)
		cvSetData(img_InHand,ImaqBuffer,AcqWinWidth);
}

void Delta::getCurPose()
{

		sendString("NETGETCURPOS");
		//sendString("NETGETCURPOS");
		Sleep(25);
		if(recvString()>0)
		{
			char *x,*y,*z;
			char separator[]=",";
			x=strtok(recvbuf,separator);
			y=strtok(NULL,separator);
			z=strtok(NULL,separator);

			std::cout<<x<<","<<y<<","<<z<<std::endl;
			setNowPosition(std::atof(x),std::atof(y));

	//std::cout<<x<<std::endl;
	//std::cout<<y<<std::endl;
		}
	
}

void Delta::setNowPosition(cv::Point2f position)
{
	setNowPosition(position.x,position.y);
}

void Delta::setNowPosition(float nowX,float nowY)
{
	nowPosition.x = nowX;
	nowPosition.y = nowY;
}

void Delta::setSpeed(float speed)
{
	sprintf_s(sendbuf,"SETLINESPEED %f\n",speed);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	Sleep(100);
}

void Delta::setACCTime(float time)
{
	sprintf_s(sendbuf,"SETACCTIME %f\n",time);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	Sleep(100);
}

void Delta::setDECTime(float time)
{
	sprintf_s(sendbuf,"SETDECTIME %f\n",time);
	send( m_socket, sendbuf, strlen(sendbuf), 0 );
	Sleep(100);
}

void Delta::iniPathParameter()
{
	pixel2mm = 327.0/430;
	original.y = 225 + 327;
	original.x=202.0 * pixel2mm;
	 a=0.024,b=0.032,c=0.032,d=-0.024;
	 moveToPoint = cv::Point2f(0,0);
	 nowPosition = cv::Point2f(0,0);
	 useSocket=false;
	 timeFlag = 0;
}

void Delta::iniSocket()
{
	//---------------socket------------------


	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR )
	printf("Error at WSAStartup()n");
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	if ( m_socket == INVALID_SOCKET ) {
	printf( "Error at socket(): %ldn", WSAGetLastError() );
	WSACleanup();
	//return 0;
	}

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr( "192.168.0.10" );
	service.sin_port = htons( 27015 );

	if ( bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
	printf( "bind() failed" );
	closesocket(m_socket);
	//return 0;
	}

	if ( listen( m_socket, 1 ) == SOCKET_ERROR )
	printf( "Error listening on socket");

	printf( "Waiting for a client to connect...\n" );
	while (1) {
	AcceptSocket = SOCKET_ERROR;
	while ( AcceptSocket == SOCKET_ERROR ) {
	AcceptSocket = accept( m_socket, NULL, NULL );
	}
	printf( "Client Connected\n");
	m_socket = AcceptSocket; 
	//設置超時
	int nNetTimeout=10;
	//setsockopt(m_socket, SOL_SOCKET,SO_RCVBUF,(char*)&optVal,optLen );
	setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&nNetTimeout,sizeof(int));  //timeOut
	//設置緩衝區大小
	int nRecvBuf=1*256;//設置為1024K
	setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

	int _on = 0;
	int _rc = setsockopt( m_socket, IPPROTO_TCP,TCP_NODELAY, (char*)&_on, sizeof(_on));
	break;
	}
}

void Delta::iniCamera()
{
	cap.open(1);
	
	
	StopGrab = FALSE;  //FALSE
	Sid=0;
	Bid=0;
	Iid=0;
	ImaqBuffer=NULL;

	imgInterfaceOpen ("img0", &Iid);
	imgSessionOpen (Iid, &Sid);
	
	imgGetAttribute (Sid, IMG_ATTR_ROI_WIDTH, &AcqWinWidth);
	imgGetAttribute (Sid, IMG_ATTR_ROI_HEIGHT, &AcqWinHeight);
	imgGrabSetup (Sid, TRUE);

	img_InHand = cvCreateImage(cvSize(AcqWinWidth,AcqWinHeight),IPL_DEPTH_8U,1);
}

Delta::Delta()
{

	iniPathParameter();
	iniSocket();
	iniCamera();
	setSpeed(5000);
	setACCTime(50);
	setDECTime(50);
	
}

void Delta::setROI(int dtop,int dbutton,int dleft,int dright)
{
	oimg_roi = oimg.adjustROI(dtop,dbutton,dleft,dright);
}

void Delta::find1114(IplImage *img_in)
{
	float factor=0.25;
	detected = false;
	cv::Mat tmpMat = cv::Mat(img_in);
	cv::Mat showMat = cv::Mat(img_in).clone();
	Sleep(100);
	//imshow("f2",tmpMat);
	cv::threshold(tmpMat,tmpMat,10,255,CV_THRESH_BINARY);
	resize(tmpMat,tmpMat,cv::Size(),factor,factor);
	cv::dilate(tmpMat,tmpMat,cv::Mat());
	
	resize(tmpMat,tmpMat,cv::Size(),1/factor,1/factor);
	cv::Mat tmpMat2(tmpMat.size(),CV_8U,cv::Scalar(0)); 

	std::vector< std::vector<cv::Point> > contours;
	cv::findContours(tmpMat,contours,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
	int i=0;
	int sumX=0,sumY=0;
	for(auto contour =  contours.begin();contour != contours.end();contour++)		//iterator style
  //for(auto contour : contours)			
	{
		if(contour->size() > 3000 && contour->size()<6000){
			sumX=0;
			sumY=0;
			
			//imshow("line",tmpMat);
			cv::drawContours(showMat,contours,i,cv::Scalar(255,0,0),8);
			cv::drawContours(tmpMat2,contours,i,cv::Scalar(255,255,255),8);
			//resize(tmpMat2,tmpMat2,cv::Size(),factor,factor);

			for(auto point = contour->begin();point!=contour->end();point++)
			{
				sumX += point->x;
				sumY += point->y;
			}
			sumX = sumX/contour->size();
			sumY = sumY/contour->size();
			
			cv::circle(showMat,cv::Point(1020,543),3,cv::Scalar(255,255,255),8);
			detected = true;

			//cv::Canny(tmpMat2,tmpMat2,50, 200, 3);
			std::vector<cv::Vec4i> lines;
			//cv::dilate(tmpMat2,tmpMat2,cv::Mat());
			cv::HoughLinesP(tmpMat2, lines, 1, CV_PI/180, 5, 600, 150 );
			if(!lines.empty())
			{
				cv::Vec4i l =lines[0] ;

				double nx = (double)(l[0]+l[2])/2;
				double ny = (double)(l[1]+l[3])/2;
				double m = -(double)(l[3]-l[1]) / (l[2]-l[0]);

				m=-1.0/m;
				angle = 180.0*atan( m )/3.1415926;
				double shiftR = 120;
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
					angleD = false;
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
					angleD = true;
				}


				//sumX += (nx-sumX)/2;
				//sumY += (ny-sumY)/2;
				cv::circle(showMat,cv::Point(sumX,sumY),3,cv::Scalar(255,255,255),2);

			
				for( size_t i = 0; i < lines.size(); i++ )
				 {
					 l = lines[i];
					 cv::line( showMat, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,255,255),8, CV_AA);
				 }
				angle = 180.0*atan( -(double)(l[3]-l[1]) / (l[2]-l[0]) )/3.1415926;
			}
		}
		i++;
	}
	//cv::waitKey(100);
	//imshow("f",showMat);

	sumX =1020-sumX;
	sumY=543-sumY;//+2430;
		
	moveToPoint.x=a*sumX+b*sumY+nowPosition.x;
	moveToPoint.y=c*sumX+d*sumY+nowPosition.y;
	center=moveToPoint;

}

void Delta::fetch(float toX,float toY, float toZ )
{
	float R = -40+angle+11;

	if(toX>0)//toX>0
		toX += 69 + 5*toX/100;
	else
		toX+=69;  //69.05  83.5689  78.49  83

	if(toX>0)
		toY -= 53.876 + 4*toX/100;// 54.065 -53.876 -58
	else
		toY-=53.876;  // 54.065 -53.876 -58

	//if(toX>0)
		//toZ -= 0.5*toX/100;

	setSpeed(1000);
	Sleep(100);
	moveL(toX,toY,-275.4,R);
	Sleep(200);
	moveL_wait(toX,toY,toZ,R);
	Sleep(500);
	sendString("OUTPUT ON 42\n");
	Sleep(800);
	moveL(toX,toY,-300,R);
	Sleep(50);
	if(angleD)
		moveL_wait(-164,84.46,-300,11);//-165.46
	else
		moveL_wait(-164,84.46,-300,-169);
	Sleep(100);
	//moveL_wait(toX,toY,toZ,R);
	//Sleep(100);
	sendString("OUTPUT OFF 42\n");
	Sleep(800);
	//moveL(-165.46,84.46,-275.4,R);
	//Sleep(100);
	//moveL_wait(0,0,-275.4,R);
	//Sleep(100);
	setSpeed(5000);
}

void Delta::waitToFetch()
{
	int y = -90;
	double waitTime = 5500;
	if(!fetchQueue.empty())
	{
		if((clock() - fetchQueue.front().time) < waitTime)
		{
			detected = false;
			setSpeed(1000);
			Sleep(100);
			y+= 30*(clock() - fetchQueue.front().time)/1000;
			moveL(cv::Point2d(fetchQueue.front().fetchPointX,y ));//+ (clock() - 10*fetchQueue.front().time)/1000
			Sleep(100);
			setSpeed(5000);
			setNowPosition(fetchQueue.front().fetchPointX,y);
			while(!detected)
			{	
				grabInHand();
				//cv::waitKey(100);
				find1114(img_InHand);
				if(clock() - fetchQueue.front().time > waitTime)
					break;
				if(detected)
				{
					//delta.fetch(delta.center.x,delta.center.y);
					//delta.setSpeed(90);
					moveL_wait(cv::Point2d(center.x,center.y+40));
					setSpeed(100);//73  57
					moveL_wait(cv::Point2d(center.x,y+80));//80 100
					//Sleep(2000);

					//grabInHand();
					//cv::waitKey(100);
					//find1114(img_InHand);
					fetch(center.x,y+108.5);//92  140 less

				}
			}
			fetchQueue.pop();
		}
		else
			fetchQueue.pop();
	}
}

void Delta::toHandFind()
{
	float factor = 0.25;
	bool detect = false;
	int sumY = 0,sumX = 0;
	cv::Mat tmpMat2(oimg.size(),CV_8U,cv::Scalar(0)); 
	xAndTime xTime;

	while(!detect)
	{
		grabToHand();
		Sleep(30);
		grabToHand();
		cv::Mat tmpMat = oimg.clone();
		setROI(-50,0,-2,-130); //-80 -170
		oimg.adjustROI(roiTop,roiButton-250,roiLeft,roiRight);
		cv::cvtColor(oimg,tmpMat,CV_RGB2GRAY);
		cv::threshold(tmpMat,tmpMat,70,255,CV_THRESH_BINARY);
		//tmpMat.adjustROI(roiTop,roiButton,roiLeft,roiRight);
		//cv::imshow("t2t",tmpMat);
		//resize(tmpMat,tmpMat,cv::Size(),factor,factor);
		cv::dilate(tmpMat,tmpMat,cv::Mat());
		//resize(tmpMat,tmpMat,cv::Size(),1/factor,1/factor);
		
		std::vector< std::vector<cv::Point> > contours;
		int i=0;
		cv::findContours(tmpMat,contours,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
		for(auto contour =  contours.begin();contour != contours.end();contour++)
		{
			if(contour->size() > 100 && contour->size()<200)
			{
				cv::drawContours(oimg,contours,i,cv::Scalar(255,0,0),2);
				cv::drawContours(tmpMat2,contours,i,cv::Scalar(255,255,255),2);

				for(auto point = contour->begin();point!=contour->end();point++)
				{
					sumY += point->y;
					sumX += point->x;
				}
				sumY = sumY/contour->size();
				sumX = sumX/contour->size();
				

				std::vector<cv::Vec4i> lines;
				//cv::dilate(tmpMat2,tmpMat2,cv::Mat());
				//cv::imshow("t32t",tmpMat2);
				cv::HoughLinesP(tmpMat2, lines, 1, CV_PI/180, 15, 30, 5 );
				if(!lines.empty())
				{
					cv::Vec4i l =lines[0] ;

					double nx = (double)(l[0]+l[2])/2;
					double ny = (double)(l[1]+l[3])/2;
					double m = -(double)(l[3]-l[1]) / (l[2]-l[0]);

					m=-1.0/m;
					angle = 180.0*atan( m )/3.1415926;
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
						angleD = false;
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
						angleD = true;
					}
					cv::circle(oimg,cv::Point(sumX,sumY),3,cv::Scalar(255,255,255),1);
					/*for( size_t i = 0; i < lines.size(); i++ )
					 {
						 l = lines[i];
						 cv::line( oimg, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(255,255,255),2, CV_AA);
					 }*/
					angle = 180.0*atan( -(double)(l[3]-l[1]) / (l[2]-l[0]) )/3.1415926;

					//****fetchPt***
					if(sumX > 175)
						fetchPointX = (float)(sumX-175)*(-0.8);
					else
						fetchPointX = (float)(175-sumX)*0.75; 

					//-*************
					detect = true;
				}
				
			}
			i++;
		}
		imshow("tt",oimg);
	}
	xTime.fetchPointX = fetchPointX;
	
	if( clock() - timeFlag  > 2500)
	{
		xTime.time = clock();
		timeFlag = xTime.time;
		fetchQueue.push(xTime);
	}
}

void Delta::findConveyorAngle()
{
	std::vector<float> X,Y;
	int sumY = 0,sumX = 0;
	grabToHand();
	setROI(-50,0,-2,-130); //-80 -170
	//cv::imshow("f2",oimg);
	cv::Mat tmpMat = oimg.clone();
	cv::inRange(oimg,cv::Scalar(0,0,200) , cv::Scalar(150,150,255), tmpMat);
	std::vector< std::vector<cv::Point> > contours;
		
	int i=0;
	cv::findContours(tmpMat,contours,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
	for(auto contour =  contours.begin();contour != contours.end();contour++)
	{
		if(contour->size() > 50 && contour->size()<200)
		{
			cv::drawContours(oimg,contours,i,cv::Scalar(255,0,0),2);
			for(auto point = contour->begin();point!=contour->end();point++)
			{
				sumY += point->y;
				sumX += point->x;
			}
			sumY = sumY/contour->size();
			sumX = sumX/contour->size();
			Y.push_back(sumY);
			X.push_back(sumX);
		}
		i++;
	}
	if(Y.size() == 2)
	{
		conveyorAngle = 180*atan(-(Y[0]-Y[1]) / (X[0]-X[1]))/3.1415926;
	
		roiTop = -50-20;
		roiButton = 0;
		if(X[1]<X[0])
		{
			roiLeft = -X[1]-10;
			roiRight = -oimg.cols+X[0]-30;
		}
		else
		{
			roiLeft = -X[0]-10;
			roiRight = -oimg.cols+X[1]-30;
		}
		std::cout<<conveyorAngle<<std::endl;
		setROI(roiTop,roiButton,roiLeft,roiRight); //-80 -170
		cv::imshow("f",oimg);
	}
}

void Delta::findContour()
{
	validContourCenters.clear();
	cv::Mat tmpMat = oimg.clone();
	cv::cvtColor(tmpMat,tmpMat,cv::COLOR_RGB2GRAY);
	cv::threshold(tmpMat,tmpMat,110,255,CV_THRESH_BINARY);
	pre_oimg_roi = tmpMat.clone();
	imshow("binary",pre_oimg_roi);
	cv::waitKey(30);
	
	
  std::vector< std::vector<cv::Point> > contours;
  cv::findContours(tmpMat,contours,cv::RETR_LIST,cv::CHAIN_APPROX_NONE);
   int i=0;
  //for(int i=0;i<contours.size();i++)			//old style
  for(auto contour =  contours.begin();contour != contours.end();contour++)		//iterator style
  //for(auto contour : contours)			
  {
	  
	  if(contour->size() > 200 && contour->size()<600){
		int sumX=0,sumY=0;

		cv::drawContours(oimg,contours,i,cv::Scalar(255,0,0),2);
		//for(auto point : contour)
		for(auto point = contour->begin();point!=contour->end();point++)
		{
			sumX += point->x;
			sumY += point->y;
		}
		
		
		cv::circle(oimg,cv::Point(sumX/contour->size(),sumY/contour->size()),3,cv::Scalar(0,0,255),2);
		validContourCenters.push_back(cv::Point(sumX/contour->size(),sumY/contour->size()));
		//std::string s =""+sumY/contour.size();
		std::stringstream ss("1");
		int t=sumY/contour->size();
		ss<<t;
		//std::string s("tt");
		
		//cv::addText(oimg, s,cv::Point(1,2),CvFont());
		
	  }
	  i++;
  }

  std::sort(validContourCenters.begin(),validContourCenters.end(),[](cv::Point pointl,cv::Point pointr)->bool{return pointl.y>pointr.y;});
  i=0;
  std::stringstream ss;
  for(auto point = validContourCenters.begin();point != validContourCenters.end();point++){
	  	  i++;
	  ss.str("");
	  ss<<i;//point->x<<","<<point->y;
	  cv::putText(oimg,ss.str(),*point,0,1,cv::Scalar(100,0,255));
	   velocity = point->y;
  }
 
  
}

void Delta::getShift()
{
	int waitTime = 1000;
	grabFrame();
	setROI(-50,0,-130,-105);
	cv::Mat tmpMatPre = oimg.clone();
	Sleep(waitTime);
	grabFrame();
	setROI(-50,0,-130,-105);
	cv::Mat tmpMatNow = oimg.clone();
	cv::Mat result;
	cv::cvtColor(tmpMatNow,tmpMatNow,cv::COLOR_RGB2GRAY);
	cv::threshold(tmpMatNow,tmpMatNow,120,255,CV_THRESH_BINARY);
	cv::cvtColor(tmpMatPre,tmpMatPre,cv::COLOR_RGB2GRAY);
	cv::threshold(tmpMatPre,tmpMatPre,120,255,CV_THRESH_BINARY);
	
	int scale = 1; 
	cv::resize(tmpMatNow,tmpMatNow,cv::Size(tmpMatNow.cols/scale,tmpMatNow.rows/scale));
	cv::resize(tmpMatPre,tmpMatPre,cv::Size(tmpMatPre.cols/scale,tmpMatPre.rows/scale));

	int min = 999999;
	double v;
	double index=-1;
	for(int offset=0;offset<100;offset++)
	{
	tmpMatNow.adjustROI(-1,0,0,0);
	tmpMatPre.adjustROI(0,-1,0,0);
	cv::subtract(tmpMatPre,tmpMatNow,result);
	if(cv::sum(result).val[0] < min)
	{
		min = cv::sum(result).val[0];
		index =  offset*scale;
	}
	//imshow("sub",result);
	//cv::waitKey(30);
	}
	v=index;
	velocity = v;
	std::cout<<v<<" "<<min<<std::endl;
}

cv::Point2d Delta::pixel2Delta(cv::Point point)
{
	double deltaX = 0,deltaY = 0;
	deltaY = pixel2mm * point.y - original.y;
	deltaX = original.x - pixel2mm * point.x; 
	return cv::Point2d(deltaX,deltaY);
}