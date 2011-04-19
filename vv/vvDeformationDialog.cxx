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
#include <vector>
#include <QComboBox>
#include <QThread>
#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileInfo>

#include <itkImage.h>
#include <itkWarpImageFilter.h>
#include <itkJoinSeriesImageFilter.h>
#include <itkImageFileReader.h>
#include <itkSubtractImageFilter.h>
#include <itkDisplacementFieldJacobianDeterminantFilter.h>


#include <vtksys/SystemTools.hxx>
#include <itksys/SystemTools.hxx>

#include "clitkCommon.h"

#include "vvDeformationDialog.h"
#include "vvDeformableRegistration.h"
#include "vvSlicer.h"
#include "vvToITK.h"
#include "vvFromITK.h"
#include "vvSlicerManager.h"

vvSlicerManager * vvDeformationDialog::GetSelectedSlicer() const
{
  return mSlicerManagers[inputSequenceBox->currentIndex()];
}

int vvDeformationDialog::GetReferenceFrameIndex() const
{
  return refImageSlider->value();
}

vvDeformationDialog::vvDeformationDialog(int initialSlicer,const std::vector<vvSlicerManager*>& slicerManagers)
  : mSlicerManagers(slicerManagers)
{
  setupUi(this);
  connect(this,SIGNAL(accepted()),this,SLOT(computeDeformationField()));
  for (unsigned int i=0; i<slicerManagers.size(); i++)
    inputSequenceBox->addItem(vtksys::SystemTools::GetFilenameName(slicerManagers[i]->GetFileName()).c_str());
  connect(inputSequenceBox,SIGNAL(currentIndexChanged(int)),this,SLOT(resetSlider(int)));
  connect(refImageSlider,SIGNAL(valueChanged(int)),this,SLOT(updateSliderLabel(int)));
  connect(outputPushButton, SIGNAL(clicked()), this, SLOT(selectOutputFile()));
  inputSequenceBox->setCurrentIndex(initialSlicer);
  resetSlider(initialSlicer);

  //Compute ideal number of threads and update dialog accordingly
  int best_thread=QThread::idealThreadCount();
  threadSpin->setValue(best_thread);

}

void vvDeformationDialog::selectOutputFile()
{
  QString Extensions = "MHD Images( *.mhd);;";
  QString fileName = QFileDialog::getSaveFileName(this,
                     tr("Save As"),
                     itksys::SystemTools::GetFilenamePath(
                       mSlicerManagers[inputSequenceBox->currentIndex()]->GetFileName()).c_str(),
                     Extensions);
  outputLineEdit->setText(fileName);
}

void vvDeformationDialog::updateSliderLabel(int refimage)
{
  QString count;
  count.setNum(refimage); //Normal people start counting at 1...
  QString newlabel="Reference Image (";
  newlabel+=count;
  newlabel+=")";
  refImageLabel->setText(newlabel);
}

void vvDeformationDialog::resetSlider(int slicer_index)
{
  refImageSlider->setRange(0,mSlicerManagers[slicer_index]->GetSlicer(0)->GetImage()->GetSize()[3]-1);
  int refimage=mSlicerManagers[slicer_index]->GetSlicer(0)->GetTSlice();
  refImageSlider->setSliderPosition(refimage);
  updateSliderLabel(refimage);
}

void vvDeformationDialog::computeDeformationField()
{
  vvImage::Pointer sequence=mSlicerManagers[inputSequenceBox->currentIndex()]->GetSlicer(0)->GetImage();
  vtkImageData * first_image = sequence->GetVTKImages()[0];
  if (!sequence->IsTimeSequence()) {
    this->setResult(QDialog::Rejected);
    QMessageBox::warning(this,tr("Image type error"), tr("Deformable image registration only makes sense on time sequences."));
  } else if ((first_image->GetSpacing()[0] != first_image->GetSpacing()[1]) || (first_image->GetSpacing()[0] != first_image->GetSpacing()[2])) {
    this->setResult(QDialog::Rejected);
    QMessageBox::warning(this,tr("Image type error"), tr("Deformable registration only works well with isotropic voxels. Please resample the image."));
    return;
  } else {
    bool aborted=false;
    QProgressDialog progress(this);
    QProgressDialog cancel(this);
    cancel.setLabelText("Canceling, please wait...");
    cancel.setCancelButtonText(0);
    cancel.hide();
    //1 step per registration plus one for each of the image conversions
    progress.setMaximum(mSlicerManagers[inputSequenceBox->currentIndex()]
                        ->GetSlicer(0)->GetImage()->GetSize()[3]+2);
    progress.setLabelText("Computing deformation model...");
    progress.setMinimumDuration(0);
    progress.setWindowModality(Qt::WindowModal);
    progress.setCancelButtonText("Cancel");
    qApp->processEvents();
    QFileInfo info(outputLineEdit->text().toStdString().c_str());
    if (info.isRelative()) { //this is a bit hackish, but should work
      QFileInfo im_info(mSlicerManagers[inputSequenceBox->currentIndex()]->GetFileName().c_str());
      outputLineEdit->setText((im_info.path().toStdString() + "/" + outputLineEdit->text().toStdString()).c_str());
    }
    vvDeformableRegistration registrator(sequence,refImageSlider->value(), iterSpin->value(),threadSpin->value(), alphaSpin->value(), sigmaSpin->value(),outputLineEdit->text().toStdString(),stopSpin->value());
    registrator.start();
    while (!registrator.isFinished()) {
      if (progress.wasCanceled() && !aborted) {
        this->setResult(QDialog::Rejected);
        registrator.abort();
        aborted=true;
        progress.hide();
        cancel.show();
      }
      if (!aborted)
        progress.setValue(registrator.getProgress());
      qApp->processEvents();
      registrator.wait(50);
    }
    if (!aborted) {
      mOutput=registrator.getOutput();
    }
  }
}
