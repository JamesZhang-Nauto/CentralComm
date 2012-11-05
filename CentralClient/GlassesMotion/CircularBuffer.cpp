/*
 * CircularBuffer.cpp
 *
 *  Created on: Sep 4, 2012
 *      Author: zhlshuo
 */

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() {
	maxSize = 60;
	size = 0;
	startPoint = 0;
	endPoint = maxSize - 1;

	for(int i = 0; i < maxSize; ++i){
		pair<int, int> p(0, 0);
		v.push_back(p);
	}

	for(int i = 0; i < 56; ++i){
		float f = 0;
		_56vector.push_back(f);
	}
}

CircularBuffer::CircularBuffer(int nBuffer) {
	maxSize = nBuffer;
	size = 0;
	startPoint = 0;
	endPoint = maxSize - 1;

	for(int i = 0; i < maxSize; ++i){
		pair<int, int> p(0, 0);
		v.push_back(p);
	}

	for(int i = 0; i < 56; ++i){
		float f = 0;
		_56vector.push_back(f);
	}
}

int CircularBuffer::addElement(pair<int, int> p) {
	++endPoint;
	++size;

	if(endPoint >= maxSize)
		endPoint = 0;

	if(size > maxSize){
		++startPoint;
		size = maxSize;
	}


	if(startPoint > maxSize - 1)
		startPoint = 0;

	v[endPoint] = p;

	return startPoint;
}

pair<int, int> CircularBuffer::getElement() {
	if(size == 0)
		return make_pair(0, 0);

	pair<int, int> tempPair = v[startPoint];
	++startPoint;
	if(startPoint >= maxSize)
		startPoint = 0;
	--size;

	return tempPair;
}

void CircularBuffer::showData(){
	pair<int, int> tempPair;
	for(int i = 0; i < maxSize; ++i){
		tempPair = getElement();
		cout << tempPair.first << " " << tempPair.second << ",";
	}
}

bool CircularBuffer::zero(pair<int, int> p){
	return (p.first == 0 && p.second == 0);
}

float CircularBuffer::getMod(pair<int, int> p){
	return sqrt((double)(p.first * p.first + p.second * p.second));
}

vector<float> CircularBuffer::creat56Vector(){
		for(double j = 0; j < 3; ++j){
		vector<float> his = creatHistogram(pow(2, j));
		for(int i = 8 * (pow(2, j) - 1); i < 8 * (pow(2, j + 1) - 1); ++i){
			_56vector[i] = his[i - 8 * (pow(2, j) - 1)];
		}

	}

	return _56vector;
}

vector<float> CircularBuffer::creatHistogram(int partitionN){
	vector<float> histogram(8 * partitionN);
	float angle;
	float mod;

	int frameN = maxSize/partitionN;

	for(int j = 0; j < partitionN; ++j){
		for(int i = j * frameN; i < (j + 1) * frameN; ++i){
			int pos = startPoint + i;
			if(pos >= maxSize)
				pos = pos - maxSize;

			if(!zero(v[pos])){
				mod = getMod(v[pos]);

				if(v[pos].first == 0 && v[pos].second > 0){//positive y direction
					histogram[2 + 8 * j] += mod;
					continue;
				}else if(v[pos].first == 0 && v[pos].second < 0){//negative y direction
					histogram[6 + 8 * j] += mod;
					continue;
				}else if(v[pos].second == 0 && v[pos].first > 0){//positive x direction
					histogram[0 + 8 * j] += mod;
					continue;
				}else{//negative x direction
					histogram[4 + 8 * j] += mod;
					continue;
				}

				angle = atan((v[pos].second*1.0)/v[pos].first);

				if(v[pos].first > 0 && v[pos].second > 0){//positive x and positive y region
					if(angle <= 3.1415926/4){
						histogram[0 + 8 * j] += mod;
						continue;
					}
					if(angle > 3.1415926/4 && angle < 3.1415926/2){
						histogram[1 + 8 * j] += mod;
						continue;
					}
				}else if(v[pos].first > 0 && v[pos].second < 0){//positive x and negative y region
					if(angle > 3.1415926/4){
						histogram[7 + 8 * j] += mod;
						continue;
					}
					if(angle > -3.1415926/2 && angle < -3.1415926/4){
						histogram[6 + 8 * j] += mod;
						continue;
					}
				}else if(v[pos].first < 0 && v[pos].second > 0){//negative x and positive y region
					if(angle > -3.1415926/4){
						histogram[3 + 8 * j] += mod;
						continue;
					}
					if(angle > -3.1415926/2 && angle < -3.1415926/4){
						histogram[2 + 8 * j] += mod;
						continue;
					}
				}else if(v[pos].first < 0 && v[pos].second < 0){//negative x and negative y region
					if(angle <= 3.1415926/4){
						histogram[4 + 8 * j] += mod;
						continue;
					}
					if(angle > 3.1415926/4 && angle < 3.1415926/2){
						histogram[5 + 8 * j] += mod;
						continue;
					}
				}
			}//end of if
		}//end of for
	}

	for(int i = 0; i < 8 * partitionN; ++i){
		histogram[i] /= frameN * 1.0;
	}

	return histogram;
}

void CircularBuffer::clear(){
	for(int i = 0; i < maxSize; ++i){
		v[i].first = 0;
		v[i].second = 0;
	}
}

vector<pair<int, int> > CircularBuffer::getLatestBuffer(int n){
	vector<pair<int, int> > nLatestBuffers;
	int pos;
	for(int i = 0; i < n; ++i){
		if(endPoint - i < 0)
			pos = 60 + endPoint - i;
		else
			pos = endPoint - i;
		nLatestBuffers.push_back(v[pos]);
	}

	return nLatestBuffers;
}

CircularBuffer::~CircularBuffer() {
	// TODO Auto-generated destructor stub
}
