#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "CommReceiver.h"
#include "RobotVideoReceiver.h"
#include "glassesVideo.h"
#include "GlassesMotion\CircularBuffer.h"


// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <SDKDDKVer.h>


#include <sapi.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>


#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
extern CircularBuffer CB;
extern CComModule _Module;
#include <atlcom.h>


extern char* server_ip;
extern ArMutex mutex_robotVideo;
extern Mat robot_img;
extern int G_Target;
extern ArClientBase *client;

extern int G_Search_Step;
bool isDoneRobot =false;
extern GLASSESMODE G_glassesMode;
extern ArMutex GlassesModeMutex;
time_t HelpStartTime, HelpEndTime, elapseTime;



/**
* ATTENTION: The prototype for speech, users should not use it anymore.

* Use this function to speak Object's name.
*
* @param index is the number of object that between 0 and 119 for Oct.14 version.
* @return true if worked perfectlly, otherwise false.
*/
int robotSpeakObjName(int index){
	
	if(index > 119 || index < 0){
		std::cout << "robotSpeekObjName: The index exceed normal value." << std::endl;
		exit(1);
		return false;
	}
	int choice = index / 10;
	ISpVoice * pVoice = NULL;
	LPCWSTR objectName[12] = {L"Wireless Router", L"Small Car", L"Coffee Cup", L"Paper Box", L"Tea Container",
							  L"Mr Potato Head", L"Toy Train", L"Big Bottle", L"Pencil Case", L"Small Bottle",
							  L"Tooth Paste", L"Tooth Paste"};


    if (FAILED(::CoInitialize(NULL)))
        return false;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice );
    if( SUCCEEDED( hr ) )
    {
        hr = pVoice->Speak(objectName[choice], 0, NULL);
        pVoice->Release();
        pVoice = NULL;
    }
	
    ::CoUninitialize();
    return true;
}

/**
* Use this function to speak Object's name.
*
* @param: index is the number of object that between 0 and 119 for Oct.14 version.
* @return: if worked perfectlly the function will return 1, otherwise 0.
*/

int robotSpeak(const int index, const char *option)
{

	ISpVoice * pVoice = NULL;
	LPCWSTR objectName[3] = {L"Truck", L"Toy", L"flower pot "};


    if (FAILED(::CoInitialize(NULL)))
        return false;

    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice );
    if( SUCCEEDED( hr ) )
    {
			if(!strcmp(option, "name"))
			{
				if(index > 15 || index < 0)
				{
				std::cout << "robotSpeekObjName: The index exceed normal value." << std::endl;
				exit(1);
				return false;
				}
				int choice = index / 5;
        hr = pVoice->Speak(objectName[choice], 0, NULL);
			}

			if(!strcmp(option, "help"))
				hr = pVoice->Speak(L"I Need Help", 0, NULL);
			if(!strcmp(option, "idle"))
				hr = pVoice->Speak(L"back to idle", 0, NULL);
			if(!strcmp(option,  "cancel"))
				hr = pVoice->Speak(L"canceled   go back to idle", 0, NULL);
			if(!strcmp(option,  "OR_Entry"))
				hr = pVoice->Speak(L"Object Recognition mode", 0, NULL);
			if(!strcmp(option,  "complete"))
				hr = pVoice->Speak(L"Mission is completed go back home", 0, NULL);
			if(!strcmp(option,  "tagetApproach"))
				hr = pVoice->Speak(L"heading to taget", 0, NULL);
      pVoice->Release();
      pVoice = NULL;
    }

    ::CoUninitialize();

    return true;
}

void asking_for_help()
{
	
	robotSpeak(0,"help");
	
	cout << "the robot needs help!" << endl;
	CB.clear();
	G_glassesMode=glassesControl;
  ISpVoice * pVoice = NULL;

}


void RobotCommand(int robotMoveComm )
{
	/*robotMoveComm = 255;*/
	if (client->getRunningWithLock())
	{
		switch(robotMoveComm)
		{
		case 0:
			//client.requestOnce("search_mode");
			break;
		case 1: 
			client->requestOnce("RobotMotion");
			cout << "RobotMotion has been sent out!" << endl;
			break;
		case 2:
			client->requestOnce("CameraMotion");
			cout << "CameraMotion has been sent out!" << endl;
			break;
		case 3:
			client->requestOnce("RobotTurnLeft");
			elapseTime=0;
			cout << "RobotTurnLeft has been sent out!" << endl;
			break;
		case 4:
			client->requestOnce("RobotTurnRight");
			elapseTime=0;

			cout << "RobotTurnRight has been sent out!" << endl;
			break;
		case 5:
			robotSpeak(0,"tagetApproach");
			client->requestOnce("TargetApproach");
			cout << "TargetApproach has been sent out!" << endl;
			break;
		case 255: //for testing
			cout << "You are calling the Legacy function!" <<endl;
			client->requestOnce("turn");
			cout << "TURN is sent out!" << endl; 
			break;
		}	
	}
}

void C_RobotTurnLeft(ArNetPacket * pack)
{
	cout << "Turn left is finished!" << endl; 
}

void C_RobotTurnRight(ArNetPacket * pack)
{
	cout << "Turn right is finished!" << endl; 
}


void C_TargetApproach(ArNetPacket * pack)
{
	cout << "Final step completed: Arrive the object!!!" << endl; 
	robotSpeak(0,"complete");
	GlassesModeMutex.lock();
	G_glassesMode = idle;
	robotSpeak(0,"idle");
	GlassesModeMutex.unlock();
}


void C_RobotMotion(ArNetPacket * pack)
{
	
	cout << "RobotMotion has been finished!" << endl;
	G_Search_Step++;
	isDoneRobot = true;
	
	//RobotCommand(2); //CameraMotion
}


void C_CameraMotion(ArNetPacket * pack)
{
	cout << "Camera motion is back!" <<endl;
	G_Search_Step++;
	isDoneRobot = true;
	
	
}

void* RobotSearch::runThread(void*)
{
	int robot_object[3]={255,255,255};
	Mat robot_img_backup;
	ObjectRecognition robotOR("robot_3_db.yml.gz");
	


	// Approach test-------------------------------------------------------

	//RobotCommand(robotSearch);


	//------------------------------------------------------------------------


	while(1) 
  {
		if(G_glassesMode == robotSearch)
		{
						for(int i=0;i<3;i++)
						{
							mutex_robotVideo.lock();
							robot_img.copyTo(robot_img_backup);
							mutex_robotVideo.unlock();
							robot_object[i]=255;
							cout << "robot OR -- " <<endl;
							robot_object[i] = robotOR.find(robot_img_backup);
							if (robot_object[i]!=255)
							{
								robot_object[i] /= 5;
							}
						}

			if ((robot_object[0] == G_Target || robot_object[1] == G_Target || robot_object[2] == G_Target) && G_Target!=255 && G_glassesMode == robotSearch) //object is detected
			{
				G_glassesMode = targetApproach;
				RobotCommand(TargetApproach);
			}
			else 
			{
				if(isDoneRobot && (G_glassesMode == robotSearch))
				{
					if(G_Search_Step<13)
					{

						//the object is detected, enter the serach mode. callback: S_RobotMotion
						RobotCommand(CameraMotion); //cameraMotion
						
						isDoneRobot = false;
					}
					if(G_Search_Step>=13)
					{
						RobotCommand(RobotMotion);
						cout << "RobotMotion  " <<endl;
						G_Search_Step = 0;
						isDoneRobot = false;
					}

				}//end of isDoneRobot
			}

			HelpEndTime = time(NULL);
			elapseTime = HelpEndTime - HelpStartTime;
		}
		GlassesModeMutex.lock();
		if(elapseTime >= 120 /*1min*/ && ( G_glassesMode==robotSearch/*||G_glassesMode==glassesControl*/) )
		{
				asking_for_help();
		}
		GlassesModeMutex.unlock();
		
  }
}


//--------------Legacy---------------
//void CommReceiver(ArNetPacket * pack)
//{
//    ObjectRecognition robot_or;
////     cout << "Next command is computing." << endl;
//    //mutex_robotVideo.lock();
//    //Mat robot_img_copy = robot_img.clone();
//    //mutex_robotVideo.unlock();
//    
//    //object detection from robot view.
//    int robotObjResult = 255;
//    Sleep(3);		//3s
//    //robotObjResult = robot_or.find(robot_img_copy);
//    
//    
//	
//    if (255 != robotObjResult )
//    {
//      //object has been detected, go forward!
//      RobotCommand(2);
//    }
//    else
//    {
//      RobotCommand(1);
//    }
//
//  
//}


//--------------------------Legacy-------------------------------------
//       for the database of 120 small objects 
//---------------------------------------------------------------------

//int robotSpeakObjName(int index){
//	
//	if(index > 119 || index < 0){
//		std::cout << "robotSpeekObjName: The index exceed normal value." << std::endl;
//		exit(1);
//		return false;
//	}
//	int choice = index / 10;
//	ISpVoice * pVoice = NULL;
//	LPCWSTR objectName[12] = {L"Wireless Router", L"Small Car", L"Coffee Cup", L"Paper Box", L"Tea Container",
//							  L"Mr Potato Head", L"Toy Train", L"Big Bottle", L"Pencil Case", L"Small Bottle",
//							  L"Tooth Paste", L"Tooth Paste"};
//
//
//    if (FAILED(::CoInitialize(NULL)))
//        return false;
//
//    HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice );
//    if( SUCCEEDED( hr ) )
//    {
//        hr = pVoice->Speak(objectName[choice], 0, NULL);
//        pVoice->Release();
//        pVoice = NULL;
//    }
//	
//    ::CoUninitialize();
//    return true;
//}