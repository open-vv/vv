#ifndef _vvOverlayPanel_H
#define _vvOverlayPanel_H

/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 200COLUMN_IMAGE_NAME
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

signals:
    void VFPropertyUpdated(int subsampling, int scale, int log);
    void OverlayPropertyUpdated(int color);
    void FusionPropertyUpdated(int opacity, int colormap, double window, double level);

}; // end class vvOverlayPanel
//====================================================================

#endif /* end #define _vvOverlayPanel_H */

