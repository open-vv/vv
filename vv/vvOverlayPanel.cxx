#ifndef _vvOverlayPanel_CXX
#define _vvOverlayPanel_CXX

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

#include "vvOverlayPanel.h"

#include <QtGui>
#include <Qt>
#include "QTreePushButton.h"

#include <vtksys/SystemTools.hxx>

//====================================================================
vvOverlayPanel::vvOverlayPanel(QWidget * parent):QWidget(parent)
{
    setupUi(this);

    vFFrame->hide();
    compareFrame->hide();
    fusionFrame->hide();
    subSamplingSpinBox->setEnabled(0);
    scaleSpinBox->setEnabled(0);
    lutCheckBox->hide();
    lutCheckBox->setEnabled(0);
    connect(subSamplingSpinBox,SIGNAL(editingFinished()),this,SLOT(setVFProperty()));
    connect(scaleSpinBox,SIGNAL(editingFinished()),this,SLOT(setVFProperty()));
    connect(lutCheckBox,SIGNAL(clicked()),this,SLOT(setVFProperty()));
    connect(colorHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(setOverlayProperty()));
    connect(opacityHorizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(setFusionProperty()));
    connect(fusionColorMapComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setFusionProperty()));
    connect(windowSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setFusionProperty()));
    connect(levelSpinBox,SIGNAL(valueChanged(double)),this,SLOT(setFusionProperty()));
}

void vvOverlayPanel::getCurrentImageName(QString name)
{
    QString filename = "<b>Current image : </b>";
    filename += vtksys::SystemTools::GetFilenameWithoutExtension(name.toStdString()).c_str();
    currentImageLabel->setText(filename.toStdString().c_str());
}

void vvOverlayPanel::getVFProperty(int subsampling, int scale, int log)
{
    if (subsampling != -1)
    {
        vFFrame->show();
        vFFrame->setEnabled(1);
        subSamplingSpinBox->setEnabled(1);
        subSamplingSpinBox->setValue(subsampling);
        scaleSpinBox->setEnabled(1);
        scaleSpinBox->setValue(scale);
        lutCheckBox->setEnabled(1);
        if (log > 0)
            lutCheckBox->setCheckState(Qt::Checked);
        else
            lutCheckBox->setCheckState(Qt::Unchecked);
    }
    else
    {
        vFFrame->hide();
        vFFrame->setEnabled(0);
        subSamplingSpinBox->setEnabled(0);
        subSamplingSpinBox->setValue(0);
        scaleSpinBox->setEnabled(0);
        scaleSpinBox->setValue(0);
        lutCheckBox->setEnabled(0);
        lutCheckBox->setCheckState(Qt::Unchecked);
    }
}

void vvOverlayPanel::getVFName(QString name)
{
    QString filename = "<b>Deformation Field : </b>";
    filename += vtksys::SystemTools::GetFilenameWithoutExtension(name.toStdString()).c_str();
    vectorFieldNameLabel->setText(filename.toStdString().c_str());
}

void vvOverlayPanel::setVFProperty()
{
    emit VFPropertyUpdated(subSamplingSpinBox->value(),
                           scaleSpinBox->value(),
                           lutCheckBox->checkState());
}

void vvOverlayPanel::getCurrentVectorInfo(int visibility, double x,double y,double z, double value)
{
    QString motion = "<b>Displacement : </b>";
    QString motionValue = "<b>Length : </b>";
    if (visibility)
    {
        motion += QString::number(x,'f',1) + " ";
        motion += QString::number(y,'f',1) + " ";
        motion += QString::number(z,'f',1) + " ";

        motionValue += QString::number(value,'f',1);
    }
    coordinatesLabel->setText(motion);
    normLabel->setText(motionValue);
}

void vvOverlayPanel::getOverlayName(QString name)
{
    QString filename = "<b>Compare with : </b>";
    filename += vtksys::SystemTools::GetFilenameWithoutExtension(name.toStdString()).c_str();
    imageComparedLabel->setText(filename.toStdString().c_str());
}

void vvOverlayPanel::getOverlayProperty(int value)
{
    if (value > -1)
    {
        compareFrame->show();
        compareFrame->setEnabled(1);
        colorHorizontalSlider->setEnabled(1);
        colorHorizontalSlider->setValue(value);
    }
    else
    {
        compareFrame->hide();
        compareFrame->setEnabled(0);
        colorHorizontalSlider->setEnabled(0);
        colorHorizontalSlider->setValue(0);
    }
}

void vvOverlayPanel::setOverlayProperty()
{
    emit OverlayPropertyUpdated(colorHorizontalSlider->value());
}

void vvOverlayPanel::getCurrentOverlayInfo(int visibility,double valueOver, double valueRef)
{
    QString refValue = "<b>Pixel value in image 1 : </b>";
    QString overlayValue = "<b>Pixel value in image 2 : </b>";
    QString diffValue = "<b>Pixel difference : </b>";
    if (visibility)
    {
        refValue += QString::number(valueRef);
        overlayValue += QString::number(valueOver,'f',1);
        diffValue += QString::number(valueRef - valueOver,'f',1);
    }
    refValueLabel->setText(refValue);
    valueLabel->setText(overlayValue);
    diffValueLabel->setText(diffValue);
}
void vvOverlayPanel::getFusionName(QString name)
{
    QString filename = "<b>Fusion with : </b>";
    filename += vtksys::SystemTools::GetFilenameWithoutExtension(name.toStdString()).c_str();
    dataFusionnedLabel->setText(filename.toStdString().c_str());
}

void vvOverlayPanel::getFusionProperty(int opacity, int colormap, double window, double level)
{
    if (opacity > -1)
    {
        fusionFrame->show();
        fusionFrame->setEnabled(1);
        opacityHorizontalSlider->setEnabled(1);
        opacityHorizontalSlider->setValue(opacity);
        fusionColorMapComboBox->setEnabled(1);
        fusionColorMapComboBox->setCurrentIndex(colormap);
        windowSpinBox->setEnabled(1);
        levelSpinBox->setEnabled(1);
        windowSpinBox->setValue(window);
        levelSpinBox->setValue(level);
    }
    else
    {
        fusionFrame->hide();
        fusionFrame->setEnabled(0);
        opacityHorizontalSlider->setEnabled(0);
        opacityHorizontalSlider->setValue(0);
        fusionColorMapComboBox->setEnabled(0);
        fusionColorMapComboBox->setCurrentIndex(-1);
        windowSpinBox->setEnabled(0);
        levelSpinBox->setEnabled(0);
    }
}

void vvOverlayPanel::setFusionProperty()
{
    emit FusionPropertyUpdated(opacityHorizontalSlider->value(), fusionColorMapComboBox->currentIndex(),
                               windowSpinBox->value(), levelSpinBox->value());
}

void vvOverlayPanel::getCurrentFusionInfo(int visibility,double value)
{
    QString fusionValue = "<b>Pixel value in image 2 : </b>";
    if (visibility)
    {
        fusionValue += QString::number(value,'f',1);
    }
    valueFusionnedLabel->setText(fusionValue);
}

#endif /* end #define _vvOverlayPanel_CXX */

