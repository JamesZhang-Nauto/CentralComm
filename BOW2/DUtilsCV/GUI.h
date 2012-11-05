/**
 * File: GUI.h
 * Project: DUtilsCV library
 * Author: Dorian Galvez
 * Date: September 24, 2010
 * Description: OpenCV-related GUI functions
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

#ifndef __D_CV_GUI__
#define __D_CV_GUI__

#include <vector>
#include <opencv/cv.h>
#include <string>

namespace DUtilsCV
{

/// Window management for image visualization
class GUI
{
public:
  
  /// Key codes
  enum tKey
  {
    NO_KEY = 0, // special value
    BACKSPACE = 8,
    ESC = 27,
    ENTER = 10,
    SPACEBAR = 32,
    LEFT_ARROW = 81,
    UP_ARROW = 82,
    RIGHT_ARROW = 83,
    DOWN_ARROW = 84
  };
  
  /// Handler for windows
  typedef std::string tWinHandler;
  
public:
  
  /**
   * Creates a windows showing the given image and waits untils some key
   * is pressed
   * @param image
   * @param autosize param used the first time the window is created
   * @param hwnd (in/out) if given, the image is shown in the given window. 
   *   If not, a new window is created and its name is returned here
   * @param timeout time to wait for user input (in ms). If 0 or not given,
   *   there is no time limit. In case of timeout, NO_KEY is returned.
   * @returns the pressed key by the user, or NO_KEY in case of timeout
   */
  static int showImage(const cv::Mat &image, bool autosize = true,
    tWinHandler *hwnd = NULL, int timeout = 0);
  
  /**
   * Saves the image in a temporary file to visualize it with a system
   * application
   * @param image
   * @param tmp_file file where the image is stored
   * @param app application invoked to visualize the image
   * @return true iff success
   */
  static bool showSystemImage(const cv::Mat &image,
    const std::string &tmp_file = "tmp.png",
    const std::string &app = "eog");

};

}

#endif
