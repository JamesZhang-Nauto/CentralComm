/*
 * File: String.cpp
 * Author: Dorian Galvez-Lopez
 * Date: December 2010
 * Description: string functions
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

#include <string>
#include <vector>
#include "StringFunctions.h"

using namespace std;
using namespace DUtils;

// ---------------------------------------------------------------------------

void StringFunctions::split(const std::string &s, 
  std::vector<std::string> &tokens, const std::string &delims)
{
  tokens.resize(0);
  
  string::size_type first, last;
  first = 0;
  
  bool goon = true;
  while(goon)
  {
    last = s.find_first_of(delims, first);
    if(last == string::npos)
    {
      goon = false;
      last = s.length();
    }
    
    if(last > first)
    {    
      // add from [first-last)
      tokens.push_back( s.substr(first, last-first) );
    }
    
    first = last + 1;
  }
  
}

// ---------------------------------------------------------------------------

void StringFunctions::removeFrom(std::string &s, const char c)
{
  string::size_type n = s.find(c);
  if(n != string::npos)
  {
    s.resize(n);
  }
}

// ---------------------------------------------------------------------------

void StringFunctions::trim(std::string &s)
{
  #define notvalid(s) ((s) == ' ' || (s) == '\r' || (s) == '\n' || (s) == '\t')
  
  // trim spaces at the end
  int n = s.length()-1;
  while(n >= 0 && notvalid(s[n]) ) n--;
  s = s.substr(0, n+1);

  // trim spaces in the beginning
  n = 0;
  while(n < (int)s.length() && notvalid(s[n]) ) n++;
  if(n > 0) s = s.substr(n);
  
  #undef notvalid
}

// ---------------------------------------------------------------------------

void StringFunctions::replace(std::string &s, 
  const std::vector<std::pair<std::string, std::string> > &map)
{
  std::vector<std::pair<std::string, std::string> >::const_iterator mit;
  for(mit = map.begin(); mit != map.end(); ++mit)
  {
    const string &search = mit->first;
    const string &rep = mit->second;
    
    size_t n = 0;
    while((n = s.find(search, n)) != string::npos)
    {
      s.replace(n, search.size(), rep);
      n = n + search.size();
    }
  }
}  

// ---------------------------------------------------------------------------

void StringFunctions::replace(std::string &s, const std::string &search,
  const std::string &rep)
{
  std::vector<std::pair<std::string, std::string> > map;
  map.push_back(make_pair(search, rep));
  
  replace(s, map);
}

// ---------------------------------------------------------------------------

