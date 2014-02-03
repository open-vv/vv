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

/* =================================================
 * @file   clitkPointRigidRegistrationGenericFilter.txx
 * @author xxx <xxx@creatis.insa-lyon.fr>
 * @date   29 June 2029
 *
 * @brief PointRigidRegistration an image
 *
 ===================================================*/

// clitk
#include "clitkPointRigidRegistration_ggo.h"
#include "clitkPointRigidRegistrationGenericFilter.h"

//paste from RigidRegistration
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkVector.h"
#include "itkResampleImageFilter.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkRigid2DTransform.h"
#include "itkVersorRigid3DTransform.h"
#include <iostream>

//paste from /home/dspinczyk/dev/clitk_superbuild_Agata/Source/clitk/common/clitkTransformUtilities.h
#include "itkMatrix.h"
#include "itkArray.h"
#include "itkPoint.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
//#define VTK_EXCLUDE_STRSTREAM_HEADERS
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

//for open file for reading
#include "clitkIO.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"


//--------------------------------------------------------------------
int main(int argc, char * argv[])
{

  // Init command line
  GGO(clitkPointRigidRegistration, args_info);
  CLITK_INIT;




  // Iinit itk
  // read input file
  //open stream to reading
  std::ifstream is;
  clitk::openFileForReading(is, args_info.input_arg);



  //reading first line of input file to chck thw dimension of data
  double x = 0;
  //clitk::skipComment(is);
  is >> x;

    typedef   unsigned char  PixelType;





    unsigned int Dimension_temp = (unsigned int)x;



        if (Dimension_temp==2)
        {
            const     unsigned int   Dimension = 2;
            typedef   itk::Image< PixelType, Dimension > ImageType;
            typedef   float          VectorComponentType;
            typedef   itk::Vector< VectorComponentType, Dimension >    VectorType;
            //Typ LandmarkBasedTransormInitializer
            typedef itk::Rigid2DTransform< double > Rigid2DTransformType;
            typedef itk::LandmarkBasedTransformInitializer< Rigid2DTransformType,ImageType, ImageType>
                LandmarkBasedTransformInitializerType;

            LandmarkBasedTransformInitializerType::Pointer landmarkBasedTransformInitializer = LandmarkBasedTransformInitializerType::New();

            //  Create source and target landmarks.
            typedef LandmarkBasedTransformInitializerType::LandmarkPointContainer     LandmarkContainerType;
            typedef LandmarkBasedTransformInitializerType::LandmarkPointType          LandmarkPointType;

            LandmarkContainerType imageLandmarks;
            LandmarkContainerType trackerLandmarks;

            LandmarkPointType imagePoint;
            LandmarkPointType trackerPoint;

            is >> x;


            while (is && !is.eof()) {
                trackerPoint[0] = x;
                is >> trackerPoint[1];


                is >> imagePoint[0];
                is >> imagePoint[1];

                imageLandmarks.push_back(imagePoint );
                trackerLandmarks.push_back(trackerPoint );


              is >> x;
            }

            is.close();

            landmarkBasedTransformInitializer->SetFixedLandmarks( imageLandmarks);
            landmarkBasedTransformInitializer->SetMovingLandmarks( trackerLandmarks);

            Rigid2DTransformType::Pointer transform = Rigid2DTransformType::New();

            transform->SetIdentity();

            landmarkBasedTransformInitializer->SetTransform(transform);
            landmarkBasedTransformInitializer->InitializeTransform();

            Rigid2DTransformType::MatrixType matrix = transform->GetMatrix();
            Rigid2DTransformType::OffsetType offset = transform->GetOffset();

            // Write result
              std::ofstream out;
              clitk::openFileForWriting(out, args_info.output_arg);

              out << matrix[0][0] << ' ' << matrix[0][1] << ' ' << offset[0] << std::endl;
              out << matrix[1][0] << ' ' << matrix[1][1] << ' ' << offset[1] << std::endl;
              out << 0.0  << ' ' << 0.0 << ' ' << 1.0;
              out.close();


        }
        else if (Dimension_temp==3)
        {

            const     unsigned int   Dimension = 3;
            typedef   itk::Image< PixelType, Dimension > ImageType;
            typedef   float          VectorComponentType;
            typedef   itk::Vector< VectorComponentType, Dimension >    VectorType;
            //Typ LandmarkBasedTransormInitializer
            typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;
            typedef itk::LandmarkBasedTransformInitializer< Rigid3DTransformType,ImageType, ImageType>
                LandmarkBasedTransformInitializerType;

            LandmarkBasedTransformInitializerType::Pointer landmarkBasedTransformInitializer = LandmarkBasedTransformInitializerType::New();

            //  Create source and target landmarks.
            typedef LandmarkBasedTransformInitializerType::LandmarkPointContainer     LandmarkContainerType;
            typedef LandmarkBasedTransformInitializerType::LandmarkPointType          LandmarkPointType;

            LandmarkContainerType imageLandmarks;
            LandmarkContainerType trackerLandmarks;

            LandmarkPointType imagePoint;
            LandmarkPointType trackerPoint;

            is >> x;

            while (is && !is.eof()) {
                trackerPoint[0] = x;
                is >> trackerPoint[1];
                is >> trackerPoint[2];

                is >> imagePoint[0];
                is >> imagePoint[1];
                is >> imagePoint[2];

                imageLandmarks.push_back(imagePoint );
                trackerLandmarks.push_back(trackerPoint );

                is >> x;
            }

            is.close();

            landmarkBasedTransformInitializer->SetFixedLandmarks( imageLandmarks);
            landmarkBasedTransformInitializer->SetMovingLandmarks( trackerLandmarks);

            Rigid3DTransformType::Pointer transform = Rigid3DTransformType::New();

            transform->SetIdentity();

            landmarkBasedTransformInitializer->SetTransform(transform);
            landmarkBasedTransformInitializer->InitializeTransform();

            Rigid3DTransformType::MatrixType matrix = transform->GetMatrix();
            Rigid3DTransformType::OffsetType offset = transform->GetOffset();

            // Write result
              std::ofstream out;
              clitk::openFileForWriting(out, args_info.output_arg);

              out << matrix[0][0] << ' ' << matrix[0][1] << ' ' << matrix[0][2] << ' '<< offset[0] << std::endl;
              out << matrix[1][0] << ' ' << matrix[1][1] << ' ' << matrix[1][2] << ' '<< offset[1] << std::endl;
              out << matrix[2][0] << ' ' << matrix[2][1] << ' ' << matrix[2][2] << ' '<< offset[2] << std::endl;
              out << 0.0  << ' ' << 0.0 << ' ' << 0.0 << ' ' << 1.0;
              out.close();



        }
        else
        {
              is.close();
              return EXIT_FAILURE;

        }





  return EXIT_SUCCESS;
}// end main
//--------------------------------------------------------------------
