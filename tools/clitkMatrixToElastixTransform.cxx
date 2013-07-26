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

// clitk
#include "clitkMatrixToElastixTransform_ggo.h"
#include "clitkTransformUtilities.h"
#include "clitkIO.h"

// itk
#include <itkEuler3DTransform.h>

//--------------------------------------------------------------------
int main(int argc, char * argv[])
{
  // Init command line
  GGO(clitkMatrixToElastixTransform, args_info);
  CLITK_INIT;

  // Read matrix
  itk::Matrix<double, 4, 4> matrix;
  try {
    matrix = clitk::ReadMatrix3D(args_info.input_arg);
  }
  catch (itk::ExceptionObject & err) {
    std::cerr << "Error reading " << args_info.input_arg << std::endl;
    std::cerr << err.GetDescription() << std::endl;
    exit(-1);
  }

  // Compute parameters from transfer using itk Euler transform
  itk::Euler3DTransform<double>::CenterType center;
  center.Fill(0.);
  if(args_info.center_given==3) {
    center[0] = args_info.center_arg[0];
    center[1] = args_info.center_arg[1];
    center[2] = args_info.center_arg[2];
  }
  itk::Euler3DTransform<double>::MatrixType rotMat;
  itk::Euler3DTransform<double>::OutputVectorType transVec;
  for(int i=0; i<3; i++) {
    transVec[i] = matrix[i][3];
    for(int j=0; j<3; j++)
      rotMat[i][j] = matrix[i][j];
  }
  itk::Euler3DTransform<double>::Pointer euler;
  euler = itk::Euler3DTransform<double>::New();
  euler->SetCenter(center);
  euler->SetOffset(transVec);
  euler->SetComputeZYX(false);
  try {
    euler->SetMatrix(rotMat);
  }
  catch (itk::ExceptionObject & err) {
    std::cerr << "Error reading " << args_info.input_arg << std::endl;
    std::cerr << err.GetDescription() << std::endl;
    exit(-1);
  }

  // Write result
  std::ofstream out;
  clitk::openFileForWriting(out, args_info.output_arg);
  out << "(Transform \"EulerTransform\")" << std::endl;
  out << "(NumberOfParameters 6)" << std::endl;
  out << "(TransformParameters ";
  for(unsigned int i=0; i<6; i++)
    out << euler->GetParameters()[i] << ' ';
  out << ')' << std::endl;
  out << "(InitialTransformParametersFileName \"NoInitialTransform\")" << std::endl;
  out << "(HowToCombineTransforms \"Compose\")" << std::endl;

  out << "// EulerTransform specific" << std::endl;
  out << "(CenterOfRotationPoint "<< center[0] << ' ' << center[1] << ' ' << center[2] << ')' << std::endl;
  out << "(ComputeZYX \"false\")" << std::endl;

  // The rest is commented, up to the user to define it manually
  out << "// Image specific" << std::endl;
  out << "// (FixedImageDimension 3)" << std::endl;
  out << "// (MovingImageDimension 3)" << std::endl;
  out << "// (FixedInternalImagePixelType \"float\")" << std::endl;
  out << "// (MovingInternalImagePixelType \"float\")" << std::endl;
  out << "// (Size 1 1 1)" << std::endl;
  out << "// (Index 0 0 0)" << std::endl;
  out << "// (Spacing 1 1 1)" << std::endl;
  out << "// (Origin -0.5 -0.5 -0.5)" << std::endl;
  out << "// (Direction 1 0 0 0 1 0 0 0 1)" << std::endl;
  out << "// (UseDirectionCosines \"true\")" << std::endl << std::endl;

  out << "// ResampleInterpolator specific" << std::endl;
  out << "// (ResampleInterpolator \"FinalBSplineInterpolator\")" << std::endl;
  out << "// (FinalBSplineInterpolationOrder 3)" << std::endl << std::endl;

  out << "// Resampler specific" << std::endl;
  out << "// (Resampler \"DefaultResampler\")" << std::endl;
  out << "// (DefaultPixelValue 0.000000)" << std::endl;
  out << "// (ResultImageFormat \"mhd\")" << std::endl;
  out << "// (ResultImagePixelType \"short\")" << std::endl;
  out << "// (CompressResultImage \"false\")" << std::endl;
  out.close();

  return EXIT_SUCCESS;
}// end main

//--------------------------------------------------------------------
