#ifndef KEYPTU_H
#define KEYPTU_H


#include "Aria.h"
#include "ArVCC4.h"
#include <vector>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

class KeyPTU
{
public:
  // constructor
  KeyPTU(ArRobot *robot);
  // destructor, its just empty
  ~KeyPTU(void) {}
  
  void up(void);
  void down(void);
  void left(void);
  void right(void);

  void plus(void);
  void minus(void);
  void greater(void);
  void less(void);
  void question(void);
  void status(void);

  void c(void);
  void h(void);
  void i(void);
  void p(void);
  void x(void);
  void z(void);

  void r(void);
  // the callback function
  void drive(void);

protected:

  ArTime myExerciseTime;

  int myPanSlew;
  int myTiltSlew;
  int myPosIncrement;
  int mySlewIncrement;
  int myZoomIncrement;

  bool myMonitor;
  bool myInit;
  bool myAbsolute;
  bool myExercise;

  void exercise(void) { myExercise = !myExercise; }
  void autoupdate(void);

  ArFunctorC<KeyPTU> myUpCB;
  ArFunctorC<KeyPTU> myDownCB;
  ArFunctorC<KeyPTU> myLeftCB;
  ArFunctorC<KeyPTU> myRightCB;

  ArFunctorC<KeyPTU> myPlusCB;
  ArFunctorC<KeyPTU> myMinusCB;
  ArFunctorC<KeyPTU> myGreaterCB;
  ArFunctorC<KeyPTU> myLessCB;
  ArFunctorC<KeyPTU> myQuestionCB;
  ArFunctorC<KeyPTU> mySCB;

  ArFunctorC<KeyPTU> myECB;
  ArFunctorC<KeyPTU> myACB;

  ArFunctorC<KeyPTU> myCCB;
  ArFunctorC<KeyPTU> myHCB;
  ArFunctorC<KeyPTU> myICB;
  ArFunctorC<KeyPTU> myPCB;
  ArFunctorC<KeyPTU> myXCB;
  ArFunctorC<KeyPTU> myZCB;

  ArFunctorC<KeyPTU> myRCB;

  // the PTU
  ArVCC4 myPTU;

  // the serial connection, in case we are connected to a computer
  // serial port
  ArSerialConnection myCon;

  // whether or not we've requested that the Camera initialize itself
  bool myPTUInitRequested;
  // pointer to the robot
  ArRobot *myRobot;
  // callback for the drive function
  ArFunctorC<KeyPTU> myDriveCB;
};


#endif