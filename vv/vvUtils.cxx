/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Joel Schaerer (joel.schaerer@insa-lyon.fr)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/

#include <fstream>
#include <algorithm>
#include <QDir>

#include "clitkCommon.h"
#include "vvUtils.h"

const std::string vv_user_file=".vv_settings.txt";
typedef std::list<std::string> FileListType;

///Returns the last images opened by the user
FileListType GetRecentlyOpenedImages()
{
    std::ifstream in((QDir::homePath().toStdString() + "/" + vv_user_file).c_str());
    std::string current_file;
    FileListType result;
    in >> current_file;
    while (in.good())
    {
        result.push_back(current_file);
        in >> current_file;
    }
    in.close();
    return result;
}

///Adds an image to the list of recently opened images
void AddToRecentlyOpenedImages(std::string filename)
{
    FileListType file_list = GetRecentlyOpenedImages();
    FileListType::iterator i = std::find(file_list.begin(),file_list.end(),filename);
    if (i != file_list.end()) // avoid dupes
        file_list.erase(i);
    while (file_list.size() >= 6) //keep list to a reasonable size
        file_list.pop_back();
    file_list.push_front(filename);
    std::ofstream out((QDir::homePath().toStdString() + "/" + vv_user_file).c_str(),std::ios_base::out | std::ios_base::trunc);
    for (FileListType::iterator j = file_list.begin() ; j != file_list.end() ; j++)
        out << (*j) << std::endl;
    out.close();
}
