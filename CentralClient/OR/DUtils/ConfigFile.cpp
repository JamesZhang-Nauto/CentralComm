/*	
 * File: ConfigFile.cpp
 * Project: DUtils library
 * Author: Dorian Galvez-Lopez
 * Date: February 15, 2011
 * Description: simple text file for human-machine storage
 *
 * Note: the text files managed are simple files with one entry per line, with
 * entries such as "item = value", ignoring blank spaces and tabs, and
 * with # for comments (note that this implementation does not support
 * escaping the #)
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

#include "DException.h"
#include "FileModes.h"
#include "LineFile.h"
#include "ConfigFile.h"
#include "StringFunctions.h"
#include <vector>
#include <fstream>
#include <map>
using namespace std;
using namespace DUtils;

// ----------------------------------------------------------------------------

ConfigFile::ConfigFile()
{
}

// ----------------------------------------------------------------------------

ConfigFile::~ConfigFile()
{
  Close();
}

// ----------------------------------------------------------------------------

ConfigFile::ConfigFile(const char *filename, const FILE_MODES mode)
{
  Init(filename, mode);
}

// ----------------------------------------------------------------------------

ConfigFile::ConfigFile(const string &filename, const FILE_MODES mode)
{
  Init(filename.c_str(), mode);
}

// ----------------------------------------------------------------------------

void ConfigFile::Init(const char *filename, const FILE_MODES mode)
{
  if(mode & READ){
		OpenForReading(filename);
	}else if((mode & WRITE) && (mode & APPEND)){
		OpenForAppending(filename);
	}else if(mode & WRITE){
		OpenForWriting(filename);
	}else{
		throw DException("Wrong access mode");
	}
}

// ----------------------------------------------------------------------------

void ConfigFile::OpenForReading(const char *filename)
{
  m_file.OpenForReading(filename);
  readContent();
}

// ----------------------------------------------------------------------------

void ConfigFile::OpenForWriting(const char *filename)
{
  m_file.OpenForWriting(filename);
}

// ----------------------------------------------------------------------------

void ConfigFile::OpenForAppending(const char *filename)
{
  m_file.OpenForAppending(filename);
}

// ----------------------------------------------------------------------------

void ConfigFile::Close()
{
  if(m_file.GetOpenMode() & APPEND || m_file.GetOpenMode() & WRITE)
  {
    writeContent();
  }

  m_file.Close();
}

// ----------------------------------------------------------------------------

void ConfigFile::writeContent()
{
  std::map<std::string, std::string>::const_iterator mit;
  
  for(mit = m_data.begin(); mit != m_data.end(); ++mit)
  {
    string s = mit->first + " = " + mit->second;
    m_file << s;
  }
}

// ----------------------------------------------------------------------------

void ConfigFile::readContent()
{
  m_data.clear();
  string s;
  while(!m_file.Eof())
  {
    m_file >> s;
    
    StringFunctions::removeFrom(s, '#');
    StringFunctions::trim(s);
    
    vector<std::string> tokens;
    StringFunctions::split(s, tokens, "=");
    
    if(tokens.size() >= 2)
    {
      StringFunctions::trim(tokens[0]);
      StringFunctions::trim(tokens[1]);
      
      // if value is between quotes, remove them because it is a literal string
      if(tokens[1].size() > 1)
      {
        if(tokens[1][0] == '"' && tokens[1][tokens[1].size()-1] == '"')
        {
          tokens[1].erase(tokens[1].begin() + tokens[1].size() - 1);
          tokens[1].erase(tokens[1].begin());
        }
      }
      
      put(tokens[0], tokens[1]);
    }
  }
}

// ----------------------------------------------------------------------------

