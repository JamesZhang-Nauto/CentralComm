/*	
 * File: Profiler.h
 * Project: DUtils library
 * Author: Dorian Galvez-Lopez
 * Date: September 14, 2010
 * Description: class for profiling code
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

#pragma once
#ifndef __D_PROFILER__
#define __D_PROFILER__

#include <map>
#include <vector>
#include <string>

#include "Timestamp.h"

namespace DUtils {

/**
 * Measures the execution time of the given command and prints it with
 * a title
 * @param cmd command
 * @param s title
 */
#define PROFILE_S(cmd, s) \
  { DUtils::Timestamp t_begin, t_end; \
    t_begin.setToCurrentTime(); \
    cmd; \
    t_end.setToCurrentTime(); \
    std::cout << s << " - elapsed time: " \
      << DUtils::Timestamp::Format(t_end - t_begin) \
      << std::endl; \
  }

/**
 * Measures the execution time of the given command and prints it
 * @param cmd command
 */
#define PROFILE(cmd) PROFILE_S(cmd, "")

/** 
 * Profiles a command with a profiler
 * @param P Profiler object
 * @param name name of profiling item
 * @param scale scaling factor of execution time
 * @param cmd command
 */
#define PROFILE_CODE(P, name, scale, cmd) \
  { (P).profile(name); \
    cmd; \
    (P).stopAndScale((scale), (name)); \
  }

/// Measures execution time of code
class Profiler
{
public:

  Profiler(): m_last_profile(""){}
  virtual ~Profiler(){}
  
  /**
   * Starts profiling the given item. If it was already been profiled, 
   * the last call to profile with that item is ignored
   * @param name name of item to profile
   */
  void profile(const std::string &name = "");
  
  /**
   * Does the same as Profiler::stop, but multiplies the elapsed time by
   * the given scale factor
   * @param scale stored_duration = actual_duration (s) * scale
   * @param name item name
   * @note use scale 1e3 to store the time in milliseconds
   */
  void stopAndScale(double scale, const std::string &name = "");
  
  /**
   * Stops profiling the given item or the last one if this is not provided.
   * Adds the elapsed time to the sum of this item profile time
   * @param name item name
   */
  inline void stop(const std::string &name = "")
  {
    stopAndScale(1.0, name);
  }

  /**
   * Adds a value to the given entry
   * @param v value (already scaled) to add
   * @param name entry name
   */
  void add(double v, const std::string &name = "");

  /**
   * Returns the last measured time for an item
   * @param name item name
   */
  double back(const std::string &name = "") const;
 
  /**
   * Removes all the measurements of the given item
   */
  void reset(const std::string &name = "");
  
  /**
   * Removes the measurements of all the items
   */
  inline void resetAll()
  {
    m_profiles.clear();
  }

  /**
   * Returns the mean of the time of the given entry
   * @param name entry name
   */
  double getMeanTime(const std::string &name = "") const ;
  
  /**
   * Returns the standard deviation of the time of the given entry
   * @param name entry name
   */
  double getStdevTime(const std::string &name = "") const ;
  
  /**
   * Returns the min time of the given entry
   * @param name entry name
   */
  double getMinTime(const std::string &name = "") const ;
  
  /**
   * Returns the max time of the given entry
   * @param name entry name
   */
  double getMaxTime(const std::string &name = "") const ;
  
  /**
   * Returns all the time measurements of the given entry
   * @param time (out) measurements
   * @param name entry name
   */
  void getTime(std::vector<double> &time, const std::string &name = "") const;
  
  /**
   * Returns all the statistics of the given entry
   * @param mean (out) mean
   * @param stdev (out) standard deviation
   * @param min (out) min value
   * @param max (out) max value
   * @param name entry name
   */
  void getStatistics(double &mean, double &stdev,
    double &min, double &max, const std::string &name = "") const;

protected:
  
  /// Profile data
  std::map<std::string, std::vector<double> > m_profiles;
  
  /// Starting points
  std::map<std::string, Timestamp> m_start_points;
  
  /// Last used entry
  std::string m_last_profile;
  
};

}

#endif
