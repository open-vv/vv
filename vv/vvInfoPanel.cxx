#include <QtGui>
#include <Qt>

#include "vvInfoPanel.h"

void vvInfoPanel::setFileName(QString text)
{
    if (text.size() > 30)
        imageLabel->setText("..." + text.right(27));
    else
        imageLabel->setText(text);
}

void vvInfoPanel::setDimension(QString text)
{
    dimensionLabel->setText(text);
}

void vvInfoPanel::setSizeMM(QString text)
{
    sizeMMLabel->setText(text);
}

void vvInfoPanel::setNPixel(QString text)
{
    nPixelLabel->setText(text);
}

void vvInfoPanel::setSizePixel(QString text)
{
    sizePixelLabel->setText(text);
}

void vvInfoPanel::setOrigin(QString text)
{
    originLabel->setText(text);
}

void vvInfoPanel::setSpacing(QString text)
{
    spacingLabel->setText(text);
}

void vvInfoPanel::setCurrentInfo(int visibility, double x, double y, double z, double X, double Y, double Z, double value)
{
    QString world = "";
    QString mouse = "";
    QString val = "";
    if (visibility)
    {
        world += QString::number(x,'f',1) + " ";
        world += QString::number(y,'f',1) + " ";
        world += QString::number(z,'f',1) + " ";

        mouse += QString::number(X,'f',1) + " ";
        mouse += QString::number(Y,'f',1) + " ";
        mouse += QString::number(Z,'f',1) + " ";

        val += QString::number(value);
    }
    worldPosLabel->setText(world);
    pixelPosLabel->setText(mouse);
    valueLabel->setText(val);
}


void vvInfoPanel::setViews(int window, int view, int slice)
{
    QString viewString;
    switch (view)
    {
    case 0:
    {
        viewString = "Sagital,  ";
        break;
    }
    case 1:
    {
        viewString = "Coronal, ";
        break;
    }
    case 2:
    {
        viewString = "Axial,   ";
        break;
    }
    }

    QString text = viewString;
    if (view != -1)
    {
        text += "current slice : ";
        text += QString::number(slice);
    }
    else
    {
        text = "Disable";
    }

    switch (window)
    {
    case 0:
    {
        ULLabel->setText(text);
        break;
    }
    case 1:
    {
        URLabel->setText(text);
        break;
    }
    case 2:
    {
        DLLabel->setText(text);
        break;
    }
    case 3:
    {
        DRLabel->setText(text);
        break;
    }
    }
}
