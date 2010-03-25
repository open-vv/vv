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
