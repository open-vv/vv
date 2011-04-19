/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
===========================================================================**/

#ifndef VVTHREADEDFILTER_H
#define VVTHREADEDFILTER_H

// clitk
#include "clitkImageToImageGenericFilterBase.h"

// qt
#include <QThread>

//------------------------------------------------------------------------------
class vvThreadedFilter: public QThread
{
  Q_OBJECT
    public:
  vvThreadedFilter();
  ~vvThreadedFilter();

  // Called from the main thread, runs the reader and displays the progress bar
  void SetFilter(clitk::ImageToImageGenericFilterBase * f);
  void Update();

public slots:
  void reject();
  
 signals:
  void ThreadInterrupted();

protected:
  void run();
  clitk::ImageToImageGenericFilterBase * m_Filter;
  clitk::FilterBase * m_FilterBase;

}; // end class vvThreadedFilter
//------------------------------------------------------------------------------

#endif

