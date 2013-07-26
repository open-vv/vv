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

#ifndef clitkElastix_h
#define clitkElastix_h

#include <itkEuler3DTransform.h>

//--------------------------------------------------------------------
namespace clitk {

//-------------------------------------------------------------------
bool
GetElastixValueFromTag(std::ifstream & is,
                       std::string tag,
                       std::string & value)
{
  std::string line;
  is.seekg (0, is.beg);
  while(std::getline(is, line))   {
    unsigned pos = line.find(tag);
    if (pos<line.size()) {
      value=line.substr(pos+tag.size(),line.size()-2);// remove last ')'
      value.erase (std::remove (value.begin(), value.end(), '"'), value.end());
      value.erase (std::remove (value.begin(), value.end(), ')'), value.end());
      return true;
    }
  }
  return false;
}
//-------------------------------------------------------------------


//-------------------------------------------------------------------
void
GetValuesFromValue(const std::string & s,
                   std::vector<std::string> & values)
{
  std::stringstream strstr(s);
  std::istream_iterator<std::string> it(strstr);
  std::istream_iterator<std::string> end;
  std::vector<std::string> results(it, end);
  values.clear();
  values.resize(results.size());
  for(uint i=0; i<results.size(); i++) values[i] = results[i];
}
//-------------------------------------------------------------------


//-------------------------------------------------------------------
template<unsigned int Dimension>
typename itk::Matrix<double, Dimension+1, Dimension+1>
createMatrixFromElastixFile(std::vector<std::string> & filename, bool verbose=true) {
  if (Dimension != 3) {
    FATAL("Only 3D yet" << std::endl);
  }
  typename itk::Matrix<double, Dimension+1, Dimension+1> matrix;

  itk::Euler3DTransform<double>::Pointer mat = itk::Euler3DTransform<double>::New();
  itk::Euler3DTransform<double>::Pointer previous;
  for(uint j=0; j<filename.size(); j++) {

    // Open file
    if (verbose) std::cout << "Read elastix parameters in " << filename[j] << std::endl;
    std::ifstream is;
    clitk::openFileForReading(is, filename[j]);

    // Check Transform
    std::string s;
    bool b = GetElastixValueFromTag(is, "Transform ", s);
    if (!b) {
      FATAL("Error must read 'Transform' in " << filename[j] << std::endl);
    }
    if (s != "EulerTransform") {
      FATAL("Sorry only 'EulerTransform'" << std::endl);
    }

    // FIXME check
    //    (InitialTransformParametersFilename[j] "NoInitialTransform")

    // Get CenterOfRotationPoint
    GetElastixValueFromTag(is, "CenterOfRotationPoint ", s); // space is needed
    if (!b) {
      FATAL("Error must read 'CenterOfRotationPoint' in " << filename[j] << std::endl);
    }
    std::vector<std::string> cor;
    GetValuesFromValue(s, cor);
    itk::Euler3DTransform<double>::CenterType c;
    for(uint i=0; i<3; i++)
      c[i] = atof(cor[i].c_str());
    mat->SetCenter(c);

    // Get Transformparameters
    GetElastixValueFromTag(is, "ComputeZYX ", s); // space is needed
    mat->SetComputeZYX( s==std::string("true") );

    // Get Transformparameters
    GetElastixValueFromTag(is, "TransformParameters ", s); // space is needed
    if (!b) {
      FATAL("Error must read 'TransformParameters' in " << filename[j] << std::endl);
    }
    std::vector<std::string> results;
    GetValuesFromValue(s, results);

    // construct a stream from the string
    itk::Euler3DTransform<double>::ParametersType p;
    p.SetSize(6);
    for(uint i=0; i<3; i++)
      p[i] = atof(results[i].c_str()); // Rotation
    for(uint i=0; i<3; i++)
      p[i+3] = atof(results[i+3].c_str()); // Translation
    mat->SetParameters(p);

    if (verbose) {
      std::cout << "Rotation      (deg) : " << rad2deg(p[0]) << " " << rad2deg(p[1]) << " " << rad2deg(p[2]) << std::endl;
      std::cout << "Center of rot (phy) : " << c[0] << " " << c[1] << " " << c[2] << std::endl;
      std::cout << "Translation   (phy) : " << p[3] << " " << p[4] << " " << p[5] << std::endl;
    }

    // Compose with previous if needed
    if (j!=0) {
      mat->Compose(previous);
      if (verbose) {
        std::cout << "Composed rotation      (deg) : " << rad2deg(mat->GetAngleX()) << " " << rad2deg(mat->GetAngleY()) << " " << rad2deg(mat->GetAngleZ()) << std::endl;
        std::cout << "Composed center of rot (phy) : " << mat->GetCenter() << std::endl;
        std::cout << "Compsoed translation   (phy) : " << mat->GetTranslation() << std::endl;
      }
    }
    // previous = mat->Clone(); // ITK4
    previous = itk::Euler3DTransform<double>::New();
    previous->SetParameters(mat->GetParameters());
    previous->SetCenter(c);
    previous->SetComputeZYX(mat->GetComputeZYX());
  }

  mat = previous;
  for(uint i=0; i<3; i++)
    for(uint j=0; j<3; j++)
      matrix[i][j] = mat->GetMatrix()[i][j];
  // Offset is -Rc + t + c
  matrix[0][3] = mat->GetOffset()[0];
  matrix[1][3] = mat->GetOffset()[1];
  matrix[2][3] = mat->GetOffset()[2];
  matrix[3][3] = 1;

  return matrix;
}
}
//-------------------------------------------------------------------

#endif
