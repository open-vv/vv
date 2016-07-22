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


#include "itkMatrix.h"
#include "itkArray.h"
//#include "itkPoint.h"
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
                imagePoint[0] = x;
                is >> imagePoint[1];


                is >> trackerPoint[0];
                is >> trackerPoint[1];

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



              // Write result
             if (args_info.error_arg != 0)
             {
                std::ofstream error;
                clitk::openFileForWriting(error, args_info.error_arg);


              //imagePoints
              int nImagePoints = imageLandmarks.size();
              double  imagePoints[3][nImagePoints];
              double  trackerPoints[3][nImagePoints];
              double  imagePointstoTrackerCoordinate[3][nImagePoints];
              double FRE = 0;
              double PointEuclideanDistance = 0;
              double SumofPointsEuclideanDistance = 0;


              for(int n= 0; n < imageLandmarks.size(); n++)
              {
                  for(int j= 0; j < 2; j++)
                  {
                      imagePoints[j][n] = imageLandmarks[n][j];
                      trackerPoints[j][n] = trackerLandmarks[n][j];




//                      imagePoints[j][n] = trackerLandmarks[n][j];
//                      trackerPoints[j][n] = imageLandmarks[n][j];
                      imagePointstoTrackerCoordinate[j][n] = 0;

                  }
                  imagePoints[2][n] = 1.0;
                  trackerPoints[2][n] = 1.0;
                  imagePointstoTrackerCoordinate[2][n] = 0;
              }


              //transformation matrix
              double RigidTransformationImagetoTracker[4][4];


              for(int i = 0; i < 2; i++)
                for(int j = 0; j < 2; j++)
                    RigidTransformationImagetoTracker[i][j] = matrix[i][j];


              for(int i = 0; i < 2; i++)
                  RigidTransformationImagetoTracker[i][2] = offset[i];

              RigidTransformationImagetoTracker[2][0] = 0.0;
              RigidTransformationImagetoTracker[2][1] = 0.0;
              RigidTransformationImagetoTracker[2][2] = 1.0;


              //writing results
              error << "ImagePoints:    CorrespondingTrackerPoints:     ImagePointstoTrackerCoordinateSystem:  PointsDistanceafterRigidRegistration:" << std::endl;





              //Calculate FRE

              for(int n= 0; n < imageLandmarks.size(); n++)
              {


                   //Calculate imagePointstoTrackerCoordinate
                   for(int i = 0; i < 3; i++)
                   {

                       for(int k = 0; k < 3; k++)
                       {
                            imagePointstoTrackerCoordinate[i][n] = imagePointstoTrackerCoordinate[i][n] + RigidTransformationImagetoTracker[i][k]*imagePoints[k][n];

                       }

                   }


                   //writing results

                   for(int i = 0; i < 2; i++)
                            error << imagePoints[i][n] << ' ';

                   error << "    ";

                   for(int i = 0; i < 2; i++)
                            error << trackerPoints[i][n] << ' ';

                   error << "   ";

                   for(int i = 0; i < 2; i++)
                            error << imagePointstoTrackerCoordinate[i][n] << ' ';



                   //calculate PointEuclideanDistance
                   PointEuclideanDistance = 0;
                   for(int i=0; i < 2;i++)
                   {
                       PointEuclideanDistance = PointEuclideanDistance + std::pow((trackerPoints[i][n] - imagePointstoTrackerCoordinate[i][n]),2);
                   }
                   PointEuclideanDistance = std::sqrt(PointEuclideanDistance);
                   error << "   " <<   PointEuclideanDistance   << "   ";

                   SumofPointsEuclideanDistance = SumofPointsEuclideanDistance +  std::pow(PointEuclideanDistance,2);






                   error << std::endl;





              } //end loop via points

              error << std::endl;
              SumofPointsEuclideanDistance = std::sqrt(SumofPointsEuclideanDistance/nImagePoints);
              error << "Fiducial Registration Error:   " <<  SumofPointsEuclideanDistance;
              error.close();

            } //end if (args_info.error_arg != null)
        } //end Dimension == 2 case
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
            LandmarkContainerType imageLandmarksTotrackerCoordinate;


            LandmarkPointType imagePoint;
            LandmarkPointType trackerPoint;

            is >> x;

            while (is && !is.eof()) {
                imagePoint[0] = x;
                is >> imagePoint[1];
                is >> imagePoint[2];

                is >> trackerPoint[0];
                is >> trackerPoint[1];
                is >> trackerPoint[2];

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


              if (args_info.error_arg != 0)
              {
              // Write result
                std::ofstream error;
                clitk::openFileForWriting(error, args_info.error_arg);


              //imagePoints
              int nImagePoints = imageLandmarks.size();
              double  imagePoints[4][nImagePoints];
              double  trackerPoints[4][nImagePoints];
              double  imagePointstoTrackerCoordinate[4][nImagePoints];
              double FRE = 0;
              double PointEuclideanDistance = 0;
              double SumofPointsEuclideanDistance = 0;


              for(int n= 0; n < imageLandmarks.size(); n++)
              {
                  for(int j= 0; j < 3; j++)
                  {
                      imagePoints[j][n] = imageLandmarks[n][j];
                      trackerPoints[j][n] = trackerLandmarks[n][j];




//                      imagePoints[j][n] = trackerLandmarks[n][j];
//                      trackerPoints[j][n] = imageLandmarks[n][j];
                      imagePointstoTrackerCoordinate[j][n] = 0;

                  }
                  imagePoints[3][n] = 1.0;
                  trackerPoints[3][n] = 1.0;
                  imagePointstoTrackerCoordinate[3][n] = 0;
              }


              //transformation matrix
              double RigidTransformationImagetoTracker[4][4];


              for(int i = 0; i < 3; i++)
                for(int j = 0; j < 3; j++)
                    RigidTransformationImagetoTracker[i][j] = matrix[i][j];


              for(int i = 0; i < 3; i++)
                  RigidTransformationImagetoTracker[i][3] = offset[i];

              RigidTransformationImagetoTracker[3][0] = 0.0;
              RigidTransformationImagetoTracker[3][1] = 0.0;
              RigidTransformationImagetoTracker[3][2] = 0.0;
              RigidTransformationImagetoTracker[3][3] = 1.0;

              //writing results
              error << "ImagePoints:    CorrespondingTrackerPoints:     ImagePointstoTrackerCoordinateSystem:  PointsDistanceafterRigidRegistration:" << std::endl;





              //Calculate FRE

              for(int n= 0; n < imageLandmarks.size(); n++)
              {


                   //Calculate imagePointstoTrackerCoordinate
                   for(int i = 0; i < 4; i++)
                   {

                       for(int k = 0; k < 4; k++)
                       {
                            imagePointstoTrackerCoordinate[i][n] = imagePointstoTrackerCoordinate[i][n] + RigidTransformationImagetoTracker[i][k]*imagePoints[k][n];

                       }

                   }


                   //writing results

                   for(int i = 0; i < 3; i++)
                            error << imagePoints[i][n] << ' ';

                   error << "    ";

                   for(int i = 0; i < 3; i++)
                            error << trackerPoints[i][n] << ' ';

                   error << "   ";

                   for(int i = 0; i < 3; i++)
                            error << imagePointstoTrackerCoordinate[i][n] << ' ';



                   //calculate PointEuclideanDistance
                   PointEuclideanDistance = 0;
                   for(int i=0; i < 3;i++)
                   {
                       PointEuclideanDistance = PointEuclideanDistance + std::pow((trackerPoints[i][n] - imagePointstoTrackerCoordinate[i][n]),2);
                   }
                   PointEuclideanDistance = std::sqrt(PointEuclideanDistance);
                   error << "   " <<   PointEuclideanDistance   << "   ";

                   SumofPointsEuclideanDistance = SumofPointsEuclideanDistance +  std::pow(PointEuclideanDistance,2);






                   error << std::endl;





              } //end loop via points

              error << std::endl;
              SumofPointsEuclideanDistance = std::sqrt(SumofPointsEuclideanDistance/nImagePoints);
              error << "Fiducial Registration Error:   " <<  SumofPointsEuclideanDistance;
              error.close();


              } //end if (args_info.error_arg != null)
          } //end Dimension == 3 case
        else
        {
              is.close();
              return EXIT_FAILURE;

        }







  return EXIT_SUCCESS;
}// end main
//--------------------------------------------------------------------
