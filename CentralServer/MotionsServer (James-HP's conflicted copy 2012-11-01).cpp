

#include "MotionsServer.h"
#define ANGAL_FOR_TURN 15

const double PI = 3.1415926;

extern ArPathPlanningTask* G_PathPlanning;
extern ArRobot robot;
extern ArSick sick;


extern ArVCC4* G_PTZHandler;		//from KeyPTZ.cpp
// extern ArAnalogGyro *G_gyro;

int goals[8] = {310,1000,  -800,1140,  -2000,1500,  -2000,-30};
int headings[4] = {0,-90,180,90};

int findNeighbor(int *goals);
int goalIndex = 0;
int headingIndex = 0;
int CameraMoveCount = 0;
void CoordinateCalculation(const double robotCurrentX, const double robotCurrentY, double* targetX, double* targetY, const double camAngle, const double robotHeading, const double movDistance);

/*************************************************************
**
**			Robot Turnning Routine
**			@Function: make a robot turn left or right according to the argument
**			@param the turnning angle ( -: left, +: right)
**
**************************************************************/
void basic_turn(int turnAngle)
{
	ArTime start;
	G_PTZHandler->reset();
	CameraMoveCount=0;
	robot.lock();

	robot.setHeading(robot.getTh() + turnAngle);
	robot.unlock();

	start.setToNow();
	while (1)
	{
		robot.lock();
		if (robot.isHeadingDone())
		{
			printf(" Finished turn\n");
			robot.unlock();
			ArUtil::sleep(50);
			cout << " current heading: " << "    " << robot.getTh()<<endl;
			break;
		}
		if (start.mSecSince() > 5000)
		{
			printf(" Turn timed out\n");
			robot.unlock();
			cout << " current heading: " << "    " << robot.getTh()<<endl;
			break;
		}
		robot.unlock();
		ArUtil::sleep(10);
	}  
}

/*************************************************************
**
**			Glasses Cancel Callback Routine
**			@Function: reset some para. 
**			@param
**
**************************************************************/
void S_GlassesCancel( ArServerClient *serverclient, ArNetPacket *socket)
{
	cout <<"Canceled from glasses, set back camera" <<endl;
	goalIndex = 0;
	headingIndex = 0;
	CameraMoveCount = 0;
	G_PTZHandler->reset();
}


/*************************************************************
**
**			Robot Motion Callback Routine
**			@Function: the robot goes to next node based on pathplanning
**			@param
**
**************************************************************/
void S_RobotMotion( ArServerClient *serverclient, ArNetPacket *socket)
{
	//-----------------------Formal Code for RobotMotion-------------------------------------------------  
	G_PTZHandler->reset();
	CameraMoveCount=0;
	if(goalIndex <= 7)
	{
		cout << robot.getPose().getX() << endl;
		cout << robot.getPose().getY() << endl;
		//  G_PathPlanning->pathPlanToPose(ArPose(300,0), true);
		;
		G_PathPlanning->pathPlanToPose(ArPose(	robot.getPose().getX() - 100, 
												robot.getPose().getY() - 100, 
												0),true );


		//G_PathPlanning->pathPlanToPose(ArPose(goals[goalIndex++],goals[goalIndex++],headings[headingIndex++]), true,true);
		cout << "RobotMotion is processing..." <<endl;

		while(G_PathPlanning->getState() != ArPathPlanningTask::REACHED_GOAL )
		{
			if (G_PathPlanning->getState() == ArPathPlanningTask::ABORTED_PATHPLAN)
			{ G_PathPlanning->cancelPathPlan();break;}
		}

		// 		goalIndex++;

	}
	else
	{
		goalIndex =0;
		headingIndex = 0;

	}   
	serverclient->sendPacketTcp(socket);
	//-------------------------------------------------------------------------------------------------------

}



/*************************************************************
**
**			Robot Camera Motion Callback Routine
**			@Function: Turn around robot camera for Obj Recognition
**			@param
**
**************************************************************/
void S_CameraMotion( ArServerClient *serverclient, ArNetPacket *socket)
{

	G_PTZHandler->panSlew(25);
	if(CameraMoveCount < 6)
	{	
		G_PTZHandler->panRel(-10);
	}
	else
	{
		if (CameraMoveCount==6) 
		{
			G_PTZHandler->reset();
			G_PTZHandler->panRel(10);
		}
		else 
			G_PTZHandler->panRel(10);
	}
	CameraMoveCount++;
	if (CameraMoveCount>12)
	{
		CameraMoveCount=0;G_PTZHandler->reset();
	}
	//------------------------------------------------------------------------------------------------------
	ArUtil::sleep(3500);
	//cout << timer.mSecSince() <<endl;
	cout << " Current pan = " << G_PTZHandler->getPan() << endl;
	serverclient->sendPacketTcp(socket);

}



void S_RobotTurnLeft( ArServerClient *serverclient, ArNetPacket *socket)
{
	cout << "turn left" <<endl;
	basic_turn(ANGAL_FOR_TURN);
	serverclient->sendPacketTcp(socket);
}


void S_RobotTurnRight( ArServerClient *serverclient, ArNetPacket *socket)
{
	cout << "turn right" <<endl;
	basic_turn(-ANGAL_FOR_TURN);
	serverclient->sendPacketTcp(socket);
}

/*************************************************************
**
**			Target Approach Callback Routine
**			@Function: find the object and approach based on pathplanning 
**			@param
**
**************************************************************/
void S_TargetApproach( ArServerClient *serverclient, ArNetPacket *socket)
{
	cout << "The last step: TargetApproach!" <<endl;


	double camAngle = 0.0;
	double robotHeading = 0.0;
	double robotCurrentX = 0.0;
	double robotCurrentY = 0.0;
	double angle = 0.0;
	double distance =0.0;

	double targetX, targetY;

	//test mode
	//     G_PTZHandler->panRel(-30);



	camAngle =  -1 * G_PTZHandler->getPan(); 
	robotHeading = robot.getPose().getTh();
	robotCurrentX =  robot.getPose().getX() ;
	robotCurrentY = robot.getPose().getY();



	//--------------- Set up the laser range device to read the distance from the target -----------------------------
	sick.lockDevice();

	distance = sick.currentReadingPolar(robotHeading+ camAngle -5, robotHeading+ camAngle +5, &angle) - 300;

	//   double distance = sick.currentReadingPolar(89, 90, &angle);

	cout << "The closest reading is " << distance << " at " << angle << " degree" << endl;

	sick.unlockDevice();
	//----------------------------------------------------------------------------------------------------------------

	//basic_turn(camAngle);

	cout << "Camera Angle is " << camAngle << endl;
	G_PTZHandler->reset();

	//cout << "before calculation, check originalX, originalY " << robotCurrentX << " " << robotCurrentY << " robotHeading is " << robotHeading << endl<<endl;  
	CoordinateCalculation(robotCurrentX,robotCurrentY,&targetX,&targetY,camAngle,robotHeading,distance);
	//
	//cout << "before movement, check targetX, target Y" << targetX << " " << targetY << "robotHeading is "<< robotHeading << endl << endl;

	//targetX = robotCurrentX + distance * sin(robotHeading) - 200;
	//targetY = robotCurrentY + distance * cos(robotHeading) - 200;
	cout << "targetX : " <<targetX << " targetY" <<targetY;
	G_PathPlanning->pathPlanToPose(ArPose(targetX,targetY,0),true,true);
	cout << "RobotMotion is processing..." <<endl;


	while(G_PathPlanning->getState() != ArPathPlanningTask::REACHED_GOAL )
	{
		//double currentCamAngle =-1* G_PTZHandler->getPan();
		//double currentHeading = robot.getPose().getTh();
		//if(currentCamAngle+currentHeading != camAngle+robotHeading)
		//{
		//  currentCamAngle = camAngle+robotHeading - currentHeading;
		//}
		//
		//G_PTZHandler->panRel(currentCamAngle);


		if (G_PathPlanning->getState() == ArPathPlanningTask::FAILED_MOVE)
		{
			G_PathPlanning->cancelPathPlan(); break;
		}
	}

	ArUtil::sleep(3000);

	serverclient->sendPacketTcp(socket);


	G_PathPlanning->pathPlanToPose(ArPose(-450,-1100,0),true,true);
	cout << "RobotMotion is heading home..." <<endl;
	while(G_PathPlanning->getState() != ArPathPlanningTask::REACHED_GOAL )
	{
		//cout << G_PathPlanning->getState() <<endl;
		if (G_PathPlanning->getState() == ArPathPlanningTask::FAILED_MOVE)
		{
			G_PathPlanning->cancelPathPlan(); break;
		}
	}

}



void CoordinateCalculation(const double robotCurrentX,const double robotCurrentY,double *outputX,double *outputY,const double camAngle, const double robotHeading,const double movDistance)
{
	double originalX = robotCurrentX;
	double originalY = robotCurrentY;

	double angleRobot = robotHeading;
	double angleCam = camAngle;
	double currentAngle = angleRobot+angleCam;


	double targetX,targetY;

	targetX = originalX;

	double length = 0;
	double targetLength = movDistance;

	if(currentAngle > 180)
	{
		currentAngle -= 360;
	}
	else if(currentAngle < -180)
	{
		currentAngle += 360;
	}

	targetY = tan(currentAngle*PI/180)*(targetX - originalX) + originalY;
	length = sqrt((targetY - originalY)*(targetY - originalY) +(targetX - originalX)*(targetX - originalX));


	while(length < targetLength)
	{
		if(currentAngle == 0)
		{
			targetX += 1;
			length = abs(targetX - originalX);
			continue;
		}
		else if(currentAngle == 180 || currentAngle == -180)
		{
			targetX -=1;
			length = abs(targetX - originalX);
			continue;
		}
		else if(currentAngle == 90)
		{
			targetY += 1;
			length = abs(targetY - originalY);
			continue;
		}
		else if(currentAngle == -90)
		{
			targetY -= 1;
			length = abs(targetY - originalY);
			continue;	
		}
		else if(currentAngle< 90 && currentAngle >-90)
		{
			targetX += 1;
		}
		else if((currentAngle > 90 &&currentAngle < 180)||(currentAngle > -180 && currentAngle < -90))
		{
			targetX -= 1;
		}
		targetY = tan(currentAngle*PI/180)*(targetX - originalX) + originalY;
		length = sqrt((targetY - originalY)*(targetY - originalY) +(targetX - originalX)*(targetX - originalX));
		// 		cout << "Currently, the length from the original point is " << length << endl;
	}

	if(targetX >= 2500)
	{
		targetX = 2200;
	}
	if(targetX <= -840)
	{
		targetX = -830;
	}
	if(targetY >= 1200)
	{
		targetY = 1000;
	}
	if(targetY <= -3200)
	{
		targetY = -2300;
	}

	cout << "targetX is " << targetX << "targetY is " << targetY << endl;

	*outputX = targetX;
	*outputY = targetY;

	return;
}







//-------------------------Legacy---------------------------------------
// int Turnangle=1;

void turn_func( ArServerClient *serverclient, ArNetPacket *socket)
{



	//   G_PathPlanning->pathPlanToPose(ArPose(500,-500), true);
	//   while(G_PathPlanning->getState() != ArPathPlanningTask::REACHED_GOAL );
	//   serverclient->sendPacketTcp(socket);


	//Path Planning Add by YMZ--------------------------------------------------------------


	//-----------------------------------------------------------------------------


	//   ptrPathPlanning->cancelPathPlan();
	//   if(ptrPathPlanning->pathPlanToPose(ArPose(0,0,0),1))
	//     printf("Heading to Home!\n"); 
	//     
	//   else printf("The Home location is not exist!");
	//   robot.lock();
	//   robot.setHeading(90*Turnangle);
	//   Turnangle++;
	//   robot.unlock();
	//   start.setToNow();
	// 	while (1)
	// 	{
	// 		robot.lock();
	// 		if (robot.isHeadingDone())
	// 		{
	// 			printf("directMotionExample: Finished turn\n");
	// 			robot.unlock();
	// 			ArUtil::sleep(50);
	// 			serverclient->sendPacketTcp(&pkg);
	// 			break;
	// 		}
	// 		if (start.mSecSince() > 5000)
	// 		{
	// 			printf("directMotionExample: Turn timed out\n");
	// 			robot.unlock();
	// 			serverclient->sendPacketTcp(&pkg);
	// 			break;
	// 		}
	// 		robot.unlock();
	// 		ArUtil::sleep(10);
	// 	}

}



// int findNeighbor(int *goal)
// {
//   
//   double currentX = robot.getPose().getX();
//   double currentY = robot.getPose().getY();
//   int index = 0;  
//   
//   for(int i = 0; i < sizeof(goal);)
//   {
// 
//     double min = abs(currentX - goal[i++]) + abs(currentY - goal[i++]);
//     double max = abs(currentX - goal[i++]) + abs(currentY - goal[i++]);
//     
//     if(min <= max)
//     {
//       index = i - 3;
//     }
//     else
//     {
//       index = i - 1;
//     }
//   }
//   
//   return index;
// }
