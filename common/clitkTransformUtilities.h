#ifndef CLITKTRANSFORMUTILITIES_H
#define CLITKTRANSFORMUTILITIES_H
 
#include "itkMatrix.h"
#include "itkArray.h"
#include "itkPoint.h"
#include "clitkImageCommon.h"
#include "clitkCommon.h"
 
 
namespace clitk
{
  //============================================================================
  //Declarations
  //============================================================================
  itk::Matrix<double, 3, 3> GetForwardAffineMatrix2D(itk::Array<double> transformParameters);
  itk::Matrix<double, 3, 3> GetBackwardAffineMatrix2D(itk::Array<double> transformParameters);
  itk::Matrix<double, 4, 4> GetForwardAffineMatrix3D(itk::Array<double> transformParameters);
  itk::Matrix<double, 4, 4> GetBackwardAffineMatrix3D(itk::Array<double> transformParameters);
  itk::Matrix<double, 3, 3> GetRotationMatrix3D(itk::Array<double> rotationParameters);
  itk::Point<double, 3> GetRotatedPoint3D(itk::Array<double> rotationParameters, itk::Point<double, 3> input);
  itk::Matrix<double, 4, 4> GetCenteredRotationMatrix3D(itk::Array<double> rotationParameters,itk::Point<double,3> centerOfRotation);
  //   itk::Matrix<double, 4, 4> GetComposedMatrix3D(itk::Matrix<double, 4, 4> firstTransform, itk::Matrix<double, 4, 4> secondTransform);
  
  itk::Matrix<double, 5, 5> ReadMatrix4D(std::string fileName);
  itk::Matrix<double, 4, 4> ReadMatrix3D(std::string fileName);
  itk::Matrix<double, 3, 3> ReadMatrix2D(std::string fileName);
  template <unsigned int Dimension > itk::Matrix<double, Dimension+1 , Dimension+1> ReadMatrix(std::string fileName);
   
  itk::Matrix<double, 3, 3> GetRotationalPartMatrix3D(itk::Matrix<double, 4, 4> input);
  itk::Matrix<double, 3, 3> GetRotationalPartMatrix(itk::Matrix<double, 4, 4> input);
  itk::Matrix<double, 2, 2> GetRotationalPartMatrix2D(itk::Matrix<double, 3, 3> input);
  itk::Matrix<double, 2, 2> GetRotationalPartMatrix(itk::Matrix<double, 3, 3> input);
     
  itk::Vector<double,3> GetTranslationPartMatrix3D(itk::Matrix<double, 4, 4> input);
  itk::Vector<double,3> GetTranslationPartMatrix(itk::Matrix<double, 4, 4> input);
  itk::Vector<double,2> GetTranslationPartMatrix2D(itk::Matrix<double, 3, 3> input);
  itk::Vector<double,2> GetTranslationPartMatrix(itk::Matrix<double, 3, 3> input);
    
 
  //============================================================================
  //Inline functions definition in header file, otherwise linker errors
  //============================================================================
 
  //========================================================================================
  inline  itk::Matrix<double, 3, 3> GetForwardAffineMatrix2D(itk::Array<double> transformParameters)
  {
    itk::Matrix<double, 3, 3> matrix;
    //rotation part
    matrix[0][0]=cos(transformParameters[0]);
    matrix[0][1]=-sin(transformParameters[0]);
    matrix[1][0]=sin(transformParameters[0]);
    matrix[1][1]=cos(transformParameters[0]);
    //translation part
    matrix[0][2]=transformParameters[1];
    matrix[1][2]=transformParameters[2];
    //homogenize
    matrix[2][0]=0.;
    matrix[2][1]=0.;
    matrix[2][2]=1.;
    return matrix;
  }
  
  inline  itk::Matrix<double, 3, 3> GetBackwardAffineMatrix2D(itk::Array<double> transformParameters)
  {
    itk::Matrix<double, 3, 3> matrix;
    //rotation part
    matrix[0][0]=cos(transformParameters[0]);
    matrix[0][1]=sin(transformParameters[0]);
    matrix[1][0]=-sin(transformParameters[0]);
    matrix[1][1]=cos(transformParameters[0]);
    //translation part
    matrix[0][2]=transformParameters[1];
    matrix[1][2]=transformParameters[2];
    //homogenize
    matrix[2][0]=0.;
    matrix[2][1]=0.;
    matrix[2][2]=1.;
    return matrix;
  }
 
 
  inline  itk::Matrix<double, 4, 4> GetForwardAffineMatrix3D(itk::Array<double> transformParameters)
  {
    itk::Matrix<double, 4, 4> matrix;
    //rotational part
    matrix[0][0]= cos(transformParameters[1])*cos(transformParameters[2]);
    matrix[0][1]= sin(transformParameters[0])*sin(transformParameters[1])*cos(transformParameters[2])+ sin(transformParameters[2])*cos(transformParameters[0]);
    matrix[0][2]= -cos(transformParameters[0])*sin(transformParameters[1])*cos(transformParameters[2])+sin(transformParameters[0])*sin(transformParameters[2]);
    matrix[1][0]= -cos(transformParameters[1])*sin(transformParameters[2]);
    matrix[1][1]= -sin(transformParameters[0])*sin(transformParameters[1])*sin(transformParameters[2])+cos(transformParameters[0])*cos(transformParameters[2]);
    matrix[1][2]= cos(transformParameters[0])*sin(transformParameters[1])*sin(transformParameters[2])+sin(transformParameters[0])*cos(transformParameters[2]);
    matrix[2][0]= sin(transformParameters[1]);
    matrix[2][1]= -sin(transformParameters[0])*cos(transformParameters[1]);
    matrix[2][2]= cos(transformParameters[0])*cos(transformParameters[1]);
    //translational part
    matrix[0][3]=transformParameters[3];
    matrix[1][3]=transformParameters[4];
    matrix[2][3]=transformParameters[5];
    //homogenize
    matrix[3][0]=0.;
    matrix[3][1]=0.;
    matrix[3][2]=0.;  
    matrix[3][3]=1.;
    return matrix;
  }
 
 
  inline  itk::Matrix<double, 4, 4> GetBackwardAffineMatrix3D(itk::Array<double> transformParameters)
  {
    itk::Matrix<double, 4, 4> matrix;
    //rotational part
    matrix[0][0]= cos(transformParameters[1])*cos(transformParameters[2]);
    matrix[0][1]= sin(transformParameters[0])*sin(transformParameters[1])*cos(transformParameters[2])- sin(transformParameters[2])*cos(transformParameters[0]);
    matrix[0][2]= cos(transformParameters[0])*sin(transformParameters[1])*cos(transformParameters[2])+sin(transformParameters[0])*sin(transformParameters[2]);
    matrix[1][0]= cos(transformParameters[1])*sin(transformParameters[2]);
    matrix[1][1]= sin(transformParameters[0])*sin(transformParameters[1])*sin(transformParameters[2])+cos(transformParameters[0])*cos(transformParameters[2]);
    matrix[1][2]= cos(transformParameters[0])*sin(transformParameters[1])*sin(transformParameters[2])-sin(transformParameters[0])*cos(transformParameters[2]);
    matrix[2][0]= -sin(transformParameters[1]);
    matrix[2][1]= sin(transformParameters[0])*cos(transformParameters[1]);
    matrix[2][2]= cos(transformParameters[0])*cos(transformParameters[1]);
    //translational part
    matrix[0][3]=transformParameters[3];
    matrix[1][3]=transformParameters[4];
    matrix[2][3]=transformParameters[5];
    //homogenize
    matrix[3][0]=0.;
    matrix[3][1]=0.;
    matrix[3][2]=0.;  
    matrix[3][3]=1.;
    return matrix;
  }
  
  inline itk::Matrix<double, 3, 3> GetRotationMatrix3D(itk::Array<double> rotationParameters)
  {
    itk::Matrix<double, 3, 3> matrix;
    //rotational part
    matrix[0][0]= cos(rotationParameters[1])*cos(rotationParameters[2]);
    matrix[0][1]= sin(rotationParameters[0])*sin(rotationParameters[1])*cos(rotationParameters[2])+ sin(rotationParameters[2])*cos(rotationParameters[0]);
    matrix[0][2]= -cos(rotationParameters[0])*sin(rotationParameters[1])*cos(rotationParameters[2])+sin(rotationParameters[0])*sin(rotationParameters[2]);
    matrix[1][0]= -cos(rotationParameters[1])*sin(rotationParameters[2]);
    matrix[1][1]= -sin(rotationParameters[0])*sin(rotationParameters[1])*sin(rotationParameters[2])+cos(rotationParameters[0])*cos(rotationParameters[2]);
    matrix[1][2]= cos(rotationParameters[0])*sin(rotationParameters[1])*sin(rotationParameters[2])+sin(rotationParameters[0])*cos(rotationParameters[2]);
    matrix[2][0]= sin(rotationParameters[1]);
    matrix[2][1]= -sin(rotationParameters[0])*cos(rotationParameters[1]);
    matrix[2][2]= cos(rotationParameters[0])*cos(rotationParameters[1]);
    return matrix;
  }
 
 
 
  //========================================================================================
  inline  itk::Point<double, 3> GetRotatedPoint3D(itk::Array<double> rotationParameters, itk::Point<double, 3> input)
  {
    itk::Matrix<double, 3, 3> matrix = GetRotationMatrix3D(rotationParameters);
    itk::Point<double, 3> output;
    for (unsigned int i=0;i<3;i++)
      {
 	output[i]=0.0;
 	for (unsigned int j=0;j<3;j++)
 	  output[i]+=matrix(i,j)*input[j];
      }
    return output;
  }
   
   
  inline itk::Matrix<double, 4, 4> GetCenteredRotationMatrix3D(itk::Array<double> rotationParameters,itk::Point<double,3> centerOfRotation )
  {
    //rotational part is identical as affine matrix, translations change
    itk::Array<double> parameters(6);
    for(unsigned int i=0; i<3;i++) parameters[i]=rotationParameters[i];
    for(unsigned int i=3; i<6;i++) parameters[i]=centerOfRotation[i-3];
    itk::Matrix<double, 4, 4> matrix=GetForwardAffineMatrix3D(parameters);
     
    //Get the rotation of the centerOfRotation
    itk::Matrix<double,3,3> rotation = GetRotationalPartMatrix3D(matrix);
    itk::Point<double,3> rotatedCenter=rotation*centerOfRotation; //GetRotatedPoint3D(rotationParameters, centerOfRotation);
     
    //Substract this point to the translational part 
    matrix(0,3)-=rotatedCenter[0];
    matrix(1,3)-=rotatedCenter[1];
    matrix(2,3)-=rotatedCenter[2];
    return matrix;
  }
 
    
  //   inline  itk::Matrix<double, 4, 4> GetComposedMatrix3D(itk::Matrix<double, 4, 4> firstAppliedTransform, itk::Matrix<double, 4, 4> secondAppliedTransform)
  //   {
  //     itk::Matrix<double, 4, 4> matrix;
  //     for (unsigned int i=0;i<4;i++)
  //       for (unsigned int j=0;j<4;j++)
  // 	{
  // 	  matrix[i][j]=0.0;
  // 	  for (unsigned int k=0;k<4;k++)
  // 	    matrix[i][j]+=firstAppliedTransform[i][k]*secondAppliedTransform[k][j];
  // 	}
  //     return matrix;
  //   }
 
 
  //========================================================================================
  inline itk::Matrix<double, 5, 5> ReadMatrix4D(std::string fileName)
  {
    // read input matrix
    std::ifstream is;
    openFileForReading(is, fileName);
    std::vector<double> nb;
    double x;
    skipComment(is);
    is >> x;
    while (!is.eof()) {
      nb.push_back(x);
      skipComment(is);
      is >> x;  
    }
    
    //copy it to the matrix
    itk::Matrix<double, 5, 5> matrix;
    unsigned int index=0;
    for (unsigned int i=0;i<5;i++)
      for (unsigned int j=0;j<5;j++)
	matrix[i][j]=nb[index++];
    return matrix; 
  }
   
  inline itk::Matrix<double, 4, 4> ReadMatrix3D(std::string fileName)
  {
    // read input matrix
    std::ifstream is;
    openFileForReading(is, fileName);
    std::vector<double> nb;
    double x;
    skipComment(is);
    is >> x;
    while (!is.eof()) {
      nb.push_back(x);
      skipComment(is);
      is >> x;  
    }
     
    //copy it to the matrix
    itk::Matrix<double, 4, 4> matrix;
    unsigned int index=0;
    for (unsigned int i=0;i<4;i++)
      for (unsigned int j=0;j<4;j++)
 	matrix[i][j]=nb[index++];
    return matrix; 
  }
 
  inline itk::Matrix<double, 3, 3> ReadMatrix2D(std::string fileName)
  {
    // read input matrix
    std::ifstream is;
    openFileForReading(is, fileName);
    std::vector<double> nb;
    double x;
    skipComment(is);
    is >> x;
    while (!is.eof()) {
      nb.push_back(x);
      skipComment(is);
      is >> x;  
    }
     
    //copy it to the matrix
    itk::Matrix<double, 3, 3> matrix;
    unsigned int index=0;
    for (unsigned int i=0;i<3;i++)
      for (unsigned int j=0;j<3;j++)
 	matrix[i][j]=nb[index++];
    return matrix; 
  }
   
  template <unsigned int Dimension > inline itk::Matrix<double, Dimension+1 , Dimension+1> ReadMatrix(std::string fileName)
  {

    // read input matrix
    std::ifstream is;
    openFileForReading(is, fileName);
    std::vector<double> nb;
    double x;
    skipComment(is);
    is >> x;
    while (!is.eof()) {
      nb.push_back(x);
      skipComment(is);
      is >> x;  
    }
     
    //copy it to the matrix
    itk::Matrix<double, Dimension+1, Dimension+1> matrix;
    unsigned int index=0;
    for (unsigned int i=0;i<Dimension+1;i++)
      for (unsigned int j=0;j<Dimension+1;j++)
	matrix[i][j]=nb[index++];
    return matrix; 
  }
   
   
  //    template<> inline itk::Matrix<double, 3, 3>  ReadMatrix<2> (std::string fileName)
  //    {
  //      return  ReadMatrix2D(fileName);
  //    }
  //    template<> inline itk::Matrix<double, 4, 4>  ReadMatrix<3> (std::string fileName)
  //    {
  //      return  ReadMatrix3D(fileName);
  //    }
  //  template<> inline itk::Matrix<double, 5, 5>  ReadMatrix<4> (std::string fileName)
  //    {
  //      return  ReadMatrix4D(fileName);
  //    }


  //========================================================================================
  inline  itk::Matrix<double, 4, 4> GetRotationalPartMatrix4D(itk::Matrix<double, 5, 5> input)
  {
    itk::Matrix<double,4,4> matrix;
    matrix[0][0]= input[0][0];
    matrix[0][1]= input[0][1];
    matrix[0][2]= input[0][2];
    matrix[0][3]= input[0][3];
    matrix[1][0]= input[1][0];
    matrix[1][1]= input[1][1];
    matrix[1][2]= input[1][2];
    matrix[1][3]= input[1][3];
    matrix[2][0]= input[2][0];
    matrix[2][1]= input[2][1];
    matrix[2][2]= input[2][2];
    matrix[2][2]= input[2][2];
    matrix[2][3]= input[2][3];
    matrix[3][0]= input[3][0];
    matrix[3][1]= input[3][1];
    matrix[3][2]= input[3][2];
    matrix[3][2]= input[3][2];
    matrix[3][3]= input[3][3];
 
    return matrix;
  }
 
 
  inline  itk::Matrix<double, 3, 3> GetRotationalPartMatrix3D(itk::Matrix<double, 4, 4> input)
  {
    itk::Matrix<double,3,3> matrix;
    matrix[0][0]= input[0][0];
    matrix[0][1]= input[0][1];
    matrix[0][2]= input[0][2];
    matrix[1][0]= input[1][0];
    matrix[1][1]= input[1][1];
    matrix[1][2]= input[1][2];
    matrix[2][0]= input[2][0];
    matrix[2][1]= input[2][1];
    matrix[2][2]= input[2][2];
    return matrix;
  }
   
  inline  itk::Matrix<double, 2, 2> GetRotationalPartMatrix2D(itk::Matrix<double, 3, 3> input)
  {
    itk::Matrix<double,2,2> matrix;
    matrix[0][0]= input[0][0];
    matrix[0][1]= input[0][1];
    matrix[0][2]= input[0][2];
    matrix[1][0]= input[1][0];
    matrix[1][1]= input[1][1];
    return matrix;
  }
   
  inline itk::Matrix<double, 4, 4> GetRotationalPartMatrix(itk::Matrix<double, 5, 5> input)
  {
    return GetRotationalPartMatrix4D(input);
  }
 
  inline itk::Matrix<double, 3, 3> GetRotationalPartMatrix(itk::Matrix<double, 4, 4> input)
  {
    return GetRotationalPartMatrix3D(input);
  }
 
  inline itk::Matrix<double, 2, 2> GetRotationalPartMatrix(itk::Matrix<double, 3, 3> input)
  {
    return GetRotationalPartMatrix2D(input);
  }
 
 
  //========================================================================================
  inline  itk::Vector<double,4> GetTranslationPartMatrix4D(itk::Matrix<double, 5, 5> input)
  {
    itk::Vector<double,4> vec;
    vec[0]= input[0][4];
    vec[1]= input[1][4];
    vec[2]= input[2][4];
    vec[3]= input[3][4];
    return vec;
 
  }
 
  inline  itk::Vector<double,3> GetTranslationPartMatrix3D(itk::Matrix<double, 4, 4> input)
  {
    itk::Vector<double,3> vec;
    vec[0]= input[0][3];
    vec[1]= input[1][3];
    vec[2]= input[2][3];
    return vec;
 
  }
  inline  itk::Vector<double,2> GetTranslationPartMatrix2D(itk::Matrix<double, 3, 3> input)
  {
    itk::Vector<double,2> vec;
    vec[0]= input[0][2];
    vec[1]= input[1][2];
    return vec;
 
  }
 
  inline itk::Vector<double,4> GetTranslationPartMatrix(itk::Matrix<double, 5, 5> input)
  {
 
    return GetTranslationPartMatrix4D(input);
  }
   
  inline itk::Vector<double,3> GetTranslationPartMatrix(itk::Matrix<double, 4, 4> input)
  {
 
    return GetTranslationPartMatrix3D(input);
  }
   
  inline itk::Vector<double,2> GetTranslationPartMatrix(itk::Matrix<double, 3, 3> input)
  {
 
    return GetTranslationPartMatrix2D(input);
  }
}
 
#endif //#define CLITKTRANSFORMUTILITIES_H
