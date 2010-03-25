#ifndef vvXXX_h
#define vvXXX_h
#include <list>
#include <string>

///Returns the last images opened by the user
std::list<std::string> GetRecentlyOpenedImages();

///Adds an image to the list of recently opened images
void AddToRecentlyOpenedImages(std::string filename);

#endif
