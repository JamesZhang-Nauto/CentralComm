/*
 * CircularBuffer.h
 *
 *  Created on: Sep 4, 2012
 *      Author: zhlshuo
 */

#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class CircularBuffer {
private:
	vector<pair<int, int> > v;
	int maxSize;
	int size;
	int startPoint;
	int endPoint;
	bool zero(pair<int, int>);
	float getMod(pair<int, int>);
	vector<float> _56vector;
public:
	CircularBuffer();
	CircularBuffer(int);
	int addElement(pair<int, int>);
	void showData();
	vector<float> creatHistogram(int partitionN);
	vector<float> creat56Vector();
	pair<int, int> getElement();
	void clear();
	vector<pair<int, int> > getLatestBuffer(int);
	virtual ~CircularBuffer();
};

#endif /* CIRCULARBUFFER_H_ */
