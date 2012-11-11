/**
 * File: Matches.cpp
 * Project: DVision library
 * Author: Dorian Galvez-Lopez
 * Date: October 4, 2010
 * Description: Function to manage correspondences
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "Matches.h"

// ---------------------------------------------------------------------------

void DVision::Matches::Save(const std::string &filename,
    const std::vector<int> &c1, const std::vector<int> &c2)
{
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);  
  DVision::Matches::save(fs, c1, c2);
}

// ---------------------------------------------------------------------------

void DVision::Matches::save(cv::FileStorage &fs, const std::vector<int> &c0, 
    const std::vector<int> &c1)
{
  if(!c0.empty())
    fs << "c0" << "[" << c0 << "]";
  else
    fs << "c0" << "[" << "]";

  if(!c1.empty())
    fs << "c1" << "[" << c1 << "]";
  else
    fs << "c1" << "[" << "]";
}

// ---------------------------------------------------------------------------

void DVision::Matches::Load(const std::string &filename,
    std::vector<int> &c1, std::vector<int> &c2)
{
  cv::FileStorage fs(filename, cv::FileStorage::READ);
  DVision::Matches::load(fs, c1, c2);
}

// ---------------------------------------------------------------------------

void DVision::Matches::load(cv::FileStorage &fs, std::vector<int> &c0, 
    std::vector<int> &c1)
{
  fs["c0"] >> c0;
  fs["c1"] >> c1; 
}

// ---------------------------------------------------------------------------

void DVision::Matches::Save(const std::string &filename,
    const SurfSet &s1, const SurfSet &s2,
    const std::vector<int> &c1, const std::vector<int> &c2)
{
  cv::FileStorage fs(filename, cv::FileStorage::WRITE);
  s1.save(fs, 0);
  s2.save(fs, 1);
  DVision::Matches::save(fs, c1, c2);
}

// ---------------------------------------------------------------------------

void DVision::Matches::Load(const std::string &filename,
    SurfSet &s1, SurfSet &s2,
    std::vector<int> &c1, std::vector<int> &c2)
{
  cv::FileStorage fs(filename, cv::FileStorage::READ); 
  s1.load(fs, 0);
  s2.load(fs, 1);
  DVision::Matches::load(fs, c1, c2);
}

// ---------------------------------------------------------------------------


