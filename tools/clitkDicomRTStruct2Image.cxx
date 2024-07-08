/*=========================================================================
  Program:         vv http://www.creatis.insa-lyon.fr/rio/vv
  Main authors :   XX XX XX
  Extended by  :   Petros K. Iosifidis (ipetroskon@cgfl.fr)

  Authors belongs to:
  - University of LYON           http://www.universite-lyon.fr/
  - Léon Bérard cancer center    http://www.centreleonberard.fr
  - CREATIS CNRS laboratory      http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence
  - BSD       http://www.opensource.org/licenses/bsd-license.php
  - CeCILL-B  http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  =========================================================================*/

/* --- LIBS --- */
#include "clitkDicomRTStruct2ImageFilter.h"
#include "clitkDicomRT_StructureSet.h"
#include "clitkDicomRTStruct2Image_ggo.h"
#include "clitkIO.h"

// VTK
#include <vtkAppendPolyData.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkSmartPointer.h>

// Qt 
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

/* --- FUNCTION DECLARATIONS --- */
std::string JsonOutputName(std::string, int, const args_info_clitkDicomRTStruct2Image &);
std::string outputFileName(clitk::DicomRT_ROI::Pointer, const args_info_clitkDicomRTStruct2Image &);

/* --- MAIN --- */
int main(int argc, char* argv[]) {
    GGO(clitkDicomRTStruct2Image, args_info);
    CLITK_INIT;
    
    //operational mode check
    bool mode;
    std::string inputName = args_info.input_arg;
    size_t dotPosition = inputName.find_last_of(".");
    if (dotPosition != std::string::npos) {
        std::string fileExtension = inputName.substr(dotPosition + 1);
        if (fileExtension == "json")
            mode = true;
        else if (fileExtension == "dcm")
            mode = false;
        else {
            std::cerr << "Error, unidentified extension of input: " << fileExtension << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    if (mode) { //operating for json files 
        // vars 
        int slice = 0, tissue = 0, tar_qt = 0, idx = 0;
        std::vector<std::string> regions;
        vtkSmartPointer<vtkPolyData> json_data;
        vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
        vtkSmartPointer<vtkPolyData> mMesh;
        vtkSmartPointer<vtkPoints> mPoints;
        vtkIdType ids[2];
        
        // Qt JSON grabber 
        QFile file(QString::fromStdString(args_info.input_arg));
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "Failed to open file." << std::endl;
            return EXIT_FAILURE;
        }
        QByteArray fileData = file.readAll();
        QJsonDocument jsonDoc(QJsonDocument::fromJson(fileData));
        QJsonObject data = jsonDoc.object();

        // Reference image grabber
        typedef unsigned char PixelType;
        const unsigned int Dimension = 3;
        typedef itk::Image<PixelType, Dimension> ImageType;
        typename ImageType::Pointer imagej = clitk::readImage<ImageType>(args_info.image_arg);
        typename ImageType::IndexType index = { 0,0,0 };
        typename ImageType::PointType physPoint;
        
        // JSON iterators & mesh grabber
        for (QJsonObject::const_iterator it = data.begin(); it != data.end(); ++it) { // each tissue 
            slice = it.value().toObject().length();               // grab total spatial slices
            regions.push_back(it.key().toStdString());         // save tissue name
            json_data = vtkSmartPointer<vtkPolyData>::New();   // create tissue specific storage
            /*
            if (args_info.roi_given && args_info.roi_arg != tissue++)     // specific region kill constraint 
                continue;
            // TODO >> Make sure multilevel contours are actually implemented properly by killing the stencil op
            // EXAMPLE: for MYO you'll also need the LV contour. 
            // So the kill switch needs to be implemented on the F_OUT of LV in case MYO is on ops 
            // This is the case for any multi contour that's using sublevels. 
            //*/
            for (const QJsonValue pk : it.value().toObject()) {          // for each spatial slice
                //initializers 
                mMesh = vtkSmartPointer<vtkPolyData>::New();
                mMesh->Allocate();
                mPoints = vtkSmartPointer<vtkPoints>::New();
                mMesh->SetPoints(mPoints);

                idx = 0;
                for (const QJsonValue xypair : pk.toArray()) {   // for each x,y pair 
                    // updade index
                    index[0] = xypair[0].toDouble()-0.5;
                    index[1] = xypair[1].toDouble()-0.5;
                    index[2] = float(slice);

                    // transform point 
                    imagej->TransformIndexToPhysicalPoint(index, physPoint);

                    // update current 2D mesh
                    mMesh->GetPoints()->InsertNextPoint(physPoint[0], physPoint[1], physPoint[2]);
                    ids[0] = idx++;
                    ids[1] = (ids[0] + 1) % pk.toArray().size(); // increment pairs & the (n-1 -> 0) pair
                    mMesh->GetLines()->InsertNextCell(2, ids);   // link points
                }
                slice--; // spatial step
                append->AddInputData(mMesh);
            }
            append->Update();
            json_data->DeepCopy(append->GetOutput());

            // invoke filter
            clitk::DicomRTStruct2ImageFilter filter;
            filter.SetMode(mode);
            filter.SetCropMaskEnabled(args_info.crop_flag);
            filter.SetImageFilename(args_info.image_arg);
            if (args_info.vtk_flag) {
                filter.SetWriteMesh(true);
            }
            std::string name = JsonOutputName(it.key().toStdString(), tar_qt++, args_info);
            filter.SetOutputImageFilename(name);
            filter.SetVerbose(args_info.verbose_flag);
            filter.SetInputData(json_data);
            filter.Update();
        }
        
    }
    else { //operating for dcm files
        // Read and display information
        clitk::DicomRT_StructureSet::Pointer s = clitk::DicomRT_StructureSet::New();
        s->Read(args_info.input_arg, args_info.tolerance_arg);
        if (args_info.verboseFile_flag) {
            s->Print(std::cout);
        }
        if (args_info.roiName_given || (args_info.roi_given && args_info.roi_arg != -1)) {
            clitk::DicomRT_ROI::Pointer roi;
            if (args_info.roiName_given) {
                roi = s->GetROIFromROIName(args_info.roiName_arg);
            }
            else if (args_info.roi_given && args_info.roi_arg != -1) {
                roi = s->GetROIFromROINumber(args_info.roi_arg);
            }
            if (roi) {
                // New filter to convert to binary image
                clitk::DicomRTStruct2ImageFilter filter;
                filter.SetMode(mode);
                filter.SetCropMaskEnabled(args_info.crop_flag);
                filter.SetImageFilename(args_info.image_arg);  // Used to get spacing + origin
                if (args_info.vtk_flag) {
                    filter.SetWriteMesh(true);
                }
                filter.SetROI(roi);
                filter.SetOutputImageFilename(outputFileName(roi, args_info));
                filter.SetVerbose(args_info.verbose_flag);
                filter.Update();
            }
            else {
                std::cerr << "No ROI with this name/id" << std::endl;
                return EXIT_FAILURE;
            }
        }
        else {
            clitk::DicomRT_StructureSet::ROIMapContainer* rois;
            if (args_info.roiNameSubstr_given)
                rois = s->GetROIsFromROINameSubstr(args_info.roiNameSubstr_arg);
            else
                rois = &s->GetROIs();

            clitk::DicomRT_StructureSet::ROIConstIteratorType iter;
            if (rois) {
                for (iter = rois->begin(); iter != rois->end(); iter++) {
                    clitk::DicomRT_ROI::Pointer roi = iter->second;
                    clitk::DicomRTStruct2ImageFilter filter;
                    filter.SetMode(mode);
                    filter.SetCropMaskEnabled(args_info.crop_flag);
                    filter.SetImageFilename(args_info.image_arg);  // Used to get spacing + origin
                    if (args_info.vtk_flag) {
                        filter.SetWriteMesh(true);
                    }
                    filter.SetROI(roi);
                    filter.SetOutputImageFilename(outputFileName(roi, args_info));
                    filter.SetVerbose(args_info.verbose_flag);
                    filter.Update();
                }
            }
            else {
                std::cerr << "No ROIs with this substring of ROI name" << std::endl;
                return EXIT_FAILURE;
            }
        }
    }

    return EXIT_SUCCESS;
}


//--------------------------------------------------------------------
std::string outputFileName(clitk::DicomRT_ROI::Pointer roi, const args_info_clitkDicomRTStruct2Image& args_info)
{
    std::string name = roi->GetName();
    int num = roi->GetROINumber();
    name.erase(remove_if(name.begin(), name.end(), isspace), name.end());
    std::string n;
    n = std::string(args_info.output_arg).append(clitk::toString(num)).append("_").append(name);
    if (args_info.mha_flag) {
        n = n.append(".mha");
    }
    else if (args_info.nii_flag) {
        n = n.append(".nii");
    }
    else if (args_info.niigz_flag) {
        n = n.append(".nii.gz");
    }
    else {
        n = n.append(".mhd");
    }
    if (args_info.verbose_flag) {
        std::cout << num << " " << roi->GetName() << " num=" << num << " : " << n << std::endl;
    }
    return n;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
std::string JsonOutputName(std::string name, int num, const args_info_clitkDicomRTStruct2Image& args_info)
{
    name.erase(remove_if(name.begin(), name.end(), isspace), name.end());
    std::string n;
    n = std::string(args_info.output_arg).append("_").append(std::to_string(num)).append("_").append(name);
    if (args_info.mha_flag) {
        n = n.append(".mha");
    }
    else if (args_info.nii_flag) {
        n = n.append(".nii");
    }
    else if (args_info.niigz_flag) {
        n = n.append(".nii.gz");
    }
    else {
        n = n.append(".mhd");
    }
    if (args_info.verbose_flag) {
        std::cout << "name=" << name << " num=" << num << " >>> " << n << std::endl;
    }
    return n;
}
//--------------------------------------------------------------------
