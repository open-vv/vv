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
#ifndef VVIMAGEREADER_CXX
#define VVIMAGEREADER_CXX

#include <itkImageFileReader.h>
#include "vvImageReader.h"
#include "vvImageReader.txx"
#include "clitkTransformUtilities.h"

//------------------------------------------------------------------------------
vvImageReader::vvImageReader()
{
  mImage = NULL;
  mInputFilenames.resize(0);
  mLastError = "";
  mType = UNDEFINEDIMAGETYPE;
  mSlice = 0;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
vvImageReader::~vvImageReader() { }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageReader::Update()
{
  Update(mType);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageReader::Update(LoadedImageType type)
{
  itk::ImageIOBase::Pointer reader = itk::ImageIOFactory::CreateImageIO(mInputFilenames[0].c_str(), itk::ImageIOFactory::ReadMode);
  if (!reader) {
    mLastError="Unable to read file.";
  } else {
    reader->SetFileName(mInputFilenames[0]);
    reader->ReadImageInformation();
    if (mInputFilenames.size() > 1)
      Update(reader->GetNumberOfDimensions()+1,reader->GetComponentTypeAsString(reader->GetComponentType()),type);
    else
      Update(reader->GetNumberOfDimensions(),reader->GetComponentTypeAsString(reader->GetComponentType()),type);
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageReader::Update(int dim,std::string inputPixelType, LoadedImageType type)
{
  //CALL_FOR_ALL_DIMS(dim,UpdateWithDim,inputPixelType);
  mType = type;
  mDim = dim;
  mInputPixelType=inputPixelType;

  switch(mDim)     {
  case 2:
    UpdateWithDim<2>(mInputPixelType);
    break;
  case 3:
    UpdateWithDim<3>(mInputPixelType);
    break;
  case 4:
    UpdateWithDim<4>(mInputPixelType);
    break;
  default:
    std::cerr << "dimension unknown in Update ! " << std::endl;
  }
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageReader::SetInputFilename(const std::string & filename)
{
  mInputFilenames.resize(0);
  mInputFilenames.push_back(filename);
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void vvImageReader::SetInputFilenames(const std::vector<std::string> & filenames)
{
  mInputFilenames = filenames;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//Read transformation in NKI format (Xdr, transposed, cm)
//void vvImageReader::ReadNkiImageTransform()
//{
//  bool bRead=false;
//  std::string filename = mInputFilenames[0]+".MACHINEORIENTATION";
//  if(itksys::SystemTools::FileExists(filename.c_str())){
//    typedef itk::ImageFileReader< itk::Image< double, 2 > > MatrixReaderType;
//    MatrixReaderType::Pointer readerTransfo = MatrixReaderType::New();
//    readerTransfo->SetFileName(filename);
//    try {
//      bRead = true;
//      readerTransfo->Update();
//    } catch( itk::ExceptionObject & err ) {
//      bRead=false;
//      std::cerr << "Cannot read " << filename << std::endl
//                << "The error is: " << err << std::endl;
//    }

//    if (bRead) {
//      //Transpose matrix (NKI format)
//      for(int j=0; j<4; j++)
//        for(int i=0; i<4; i++)
//          mImage->GetTransform()->GetMatrix()->SetElement(j,i,readerTransfo->GetOutput()->GetBufferPointer()[4*i+j]);

//      //From cm to mm
//      for(int i=0; i<3; i++)
//        mImage->GetTransform()->GetMatrix()->SetElement(i,3,10*mImage->GetTransform()->GetMatrix()->GetElement(i,3));

//      mImage->GetTransform()->Inverse();
//    }
//  }
//}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//Read transformation in ASCII format
void vvImageReader::ReadMatImageTransform()
{
  std::string filename(mInputFilenames[0]);
  std::string ext(itksys::SystemTools::GetFilenameLastExtension(filename));
  if (ext.length() > 0) {
    size_t pos = filename.rfind(ext);
    filename.replace(pos, ext.length(), ".mat");
  }
  else
    filename += ".mat";
    
  std::ifstream f(filename.c_str());
  if(f.is_open()) {
    f.close();

    itk::Matrix<double, 4, 4> itkMat;
    itkMat.SetIdentity();
    try {
      itkMat = clitk::ReadMatrix3D(filename);
    }
    catch (itk::ExceptionObject & err) {
      itkWarningMacro(<< "Found " << filename
                      << " but this is not a 4x4 matrix so it is ignored.");
    }

    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    matrix->Identity();
    for(int j=0; j<4; j++)
      for(int i=0; i<4; i++)
        matrix->SetElement(j,i,itkMat[j][i]);

    // RP: 14/03/2011
    //  For some reason, the transformation matrix given in the MHD
    // file is inverted wrt the transformation given in the MAT file.
    // We don't really know where the inversion takes place inside
    // VTK or ITK. For what we could see in VV, the transformation
    // given in the MHD file seems "more correct" than that given in
    // the MAT file. For this reason, we invert the matrix read from
    // the MAT file before concatenating it to the current transformation.
    // Still, it would be nice to find out what happens exactly between
    // VTK and ITK...
    //
    // SR: 23/06/2011
    // We actually use vtkImageReslice which takes the inverse transform
    // as input. So it seems more correct to inverse the matrix given by
    // itk::ImageBase< VImageDimension >::GetDirection() than the matrix
    // in .mat which is indeed the inverse optimized by a typical
    // affine registration algorithm.
    // Validated using clitkAffineTransform --transform_grid
    if (matrix->Determinant() == 0)
    {
      vtkGenericWarningMacro("Matrix in " << filename.c_str() << " cannot be inverted (determinant = 0)");
    }

    // TODO SR and BP: check on the list of transforms and not the first only
    mImage->GetTransform()[0]->PreMultiply();
    mImage->GetTransform()[0]->Concatenate(matrix);
    mImage->GetTransform()[0]->Update();

    //for image sequences, apply the transform to each images of the sequence
    if (mImage->IsTimeSequence())
    {
    	for (unsigned i = 1 ; i<mImage->GetTransform().size() ; i++)
        {
            mImage->GetTransform()[i]->PreMultiply();
            mImage->GetTransform()[i]->Concatenate(matrix);
            mImage->GetTransform()[i]->Update();
        }
    }

  }
}
//------------------------------------------------------------------------------
#endif

