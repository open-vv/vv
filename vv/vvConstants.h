/*=========================================================================

 Program:   vv
 Module:    $RCSfile: vvConstants.h,v $
 Language:  C++
 Date:      $Date: 2010/01/06 13:31:58 $
 Version:   $Revision: 1.1 $
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
#ifndef vvConstants_h
#define vvConstants_h

//Constants used everywhere in the program

//Image types
typedef enum _IMAGETYPE {
IMAGE = 20,
DICOM,
MERGED,
IMAGEWITHTIME,
MERGEDWITHTIME,
VECTORFIELD,
UNDEFINEDIMAGETYPE
} LoadedImageType;

#endif
