/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
======================================================================-====*/
#ifndef _vvOverlayPanel_H
#define _vvOverlayPanel_H
#include <iostream>
#include <vector>

#include "ui_vvOverlayPanel.h"

//====================================================================
class vvOverlayPanel : public QWidget, private Ui::vvOverlayPanel
{

    Q_OBJECT

public:
    // constructor - destructor
    vvOverlayPanel(QWidget * parent=0);
    ~vvOverlayPanel() {}

    void getCurrentImageName(QString name);

    void getVFProperty(int subsampling, int scale, int log);
    void getVFName(QString name);

    void getOverlayProperty(int color);
    void getOverlayName(QString name);

    void getFusionProperty(int opacity, int colormap, double window, double level);
    void getFusionName(QString name);

    void getCurrentVectorInfo(int visibility, double x, double y, double z, double value);
    void getCurrentOverlayInfo(int visibility,double valueOver, double valueRef);
    void getCurrentFusionInfo(int visibility,double value);

public slots:
    void setVFProperty();
    void setOverlayProperty();
    void setFusionProperty();
    void VFColorChangeRequest();

signals:
    void VFPropertyUpdated(int subsampling, int scale, int log, int width, double r, double g, double b);
    void OverlayPropertyUpdated(int color);
    void FusionPropertyUpdated(int opacity, int colormap, double window, double level);
}; // end class vvOverlayPanel
//====================================================================

#endif /* end #define _vvOverlayPanel_H */

