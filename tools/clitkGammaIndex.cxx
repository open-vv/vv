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
===========================================================================*/

#include "clitkGammaIndex_ggo.h"
#include "clitkIO.h"
#include "clitkDD.h"

#include <iostream>
#include <cmath>
#include <cassert>

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkMetaImageWriter.h>
#include <vtkPNGReader.h>
#include <vtkPolyData.h>
#include <vtkCellLocator.h>

#include <vvImage.h>
#include <vvImageReader.h>
#include <vvImageWriter.h>

#include <itkImage.h>
#include <itkImageRegionIterator.h>
typedef itk::Image<double> OutputImageType;
typedef itk::ImageRegionIterator<OutputImageType> OutputImageIterator;

using std::endl;
using std::cout;

vtkImageData* loadImage(const std::string& filename)
{
    vvImageReader::Pointer reader = vvImageReader::New();
    reader->SetInputFilename(filename);
    reader->Update();
    vvImage::Pointer vvimage = reader->GetOutput();
    if (!vvimage) { cerr << "can't load " << filename << endl; return NULL; }

    vtkImageData *image = vtkImageData::New();
    image->DeepCopy(vvimage->GetFirstVTKImageData());
    return image;
}

void saveImage(OutputImageType* image,const std::string &filename) {
    vvImage::Pointer vvimage = vvImage::New();
    vvimage->AddItkImage(image);

    vvImageWriter::Pointer writer = vvImageWriter::New();
    writer->SetOutputFileName(filename.c_str());
    writer->SetInput(vvimage);
    writer->Update();
}


void insertTriangles(vtkCellArray *cells, vtkPoints *points, const vtkIdType ids[4]) {
    double p0[3]; points->GetPoint(ids[0],p0);
    double p1[3]; points->GetPoint(ids[1],p1);
    double p2[3]; points->GetPoint(ids[2],p2);
    double p3[3]; points->GetPoint(ids[3],p3);
    //cout << "----------------------------------" << endl;
    //cout << "p0=[" << p0[0] << "," << p0[1] << "," << p0[2] << "]" << endl;
    //cout << "p1=[" << p1[0] << "," << p1[1] << "," << p1[2] << "]" << endl;
    //cout << "p2=[" << p2[0] << "," << p2[1] << "," << p2[2] << "]" << endl;
    //cout << "p3=[" << p3[0] << "," << p3[1] << "," << p3[2] << "]" << endl;

    double center[] = {0,0,0};
    for (int kk=0; kk<3; kk++) {
	center[kk] += p0[kk];
	center[kk] += p1[kk];
	center[kk] += p2[kk];
	center[kk] += p3[kk];
	center[kk] /= 4.;
    }

    vtkIdType center_id = points->InsertNextPoint(center);
    //cout << "center=[" << center[0] << "," << center[1] << "," << center[2] << "]" << endl;

    cells->InsertNextCell(3);
    cells->InsertCellPoint(ids[0]);
    cells->InsertCellPoint(ids[1]);
    cells->InsertCellPoint(center_id);

    cells->InsertNextCell(3);
    cells->InsertCellPoint(ids[1]);
    cells->InsertCellPoint(ids[3]);
    cells->InsertCellPoint(center_id);

    cells->InsertNextCell(3);
    cells->InsertCellPoint(ids[3]);
    cells->InsertCellPoint(ids[2]);
    cells->InsertCellPoint(center_id);

    cells->InsertNextCell(3);
    cells->InsertCellPoint(ids[2]);
    cells->InsertCellPoint(ids[0]);
    cells->InsertCellPoint(center_id);
}

void insertLine(vtkCellArray *cells, vtkPoints *points, const vtkIdType ids[2]) {
    cells->InsertNextCell(2);
    cells->InsertCellPoint(ids[0]);
    cells->InsertCellPoint(ids[1]);
}

double getMaximum(vtkImageData *image) {
    bool first = true;
    double maximum = 0;

    vtkPointData* point_data = image->GetPointData();
    assert(point_data);
    vtkDataArray* scalars = point_data->GetScalars();
    assert(scalars);

    for (int kk=0; kk<image->GetNumberOfPoints(); kk++) {
	double value = scalars->GetTuple1(kk);

	if (first) {
	    maximum = value;
	    first = false;
	    continue;
	}

	if (maximum<value) maximum = value;
    }

    return maximum;
}


vtkPolyData *buildPlane(vtkImageData *image,double spatial_margin,double dose_margin) {
    vtkPoints *points = vtkPoints::New();
    for (int kk=0; kk<image->GetNumberOfPoints(); kk++) {
	double *point = image->GetPoint(kk);
	double value = image->GetPointData()->GetScalars()->GetTuple1(kk);
	assert(value>=0);
	assert(point[2]==0);
	point[2] = value;

	point[0] /= spatial_margin;
	point[1] /= spatial_margin;
	point[2] /= dose_margin;

#ifndef NDEBUG
	vtkIdType point_id = points->InsertNextPoint(point);
	assert(kk==point_id);
#else
	points->InsertNextPoint(point);
#endif
    }

    vtkCellArray *cells = vtkCellArray::New();
    for (int kk=0; kk<image->GetNumberOfCells(); kk++) {
	vtkCell *cell = image->GetCell(kk);

	if (cell->GetNumberOfPoints()==4) {
	    insertTriangles(cells,points,cell->GetPointIds()->GetPointer(0));
	    continue;
	}

	if (cell->GetNumberOfPoints()==2) {
	    insertLine(cells,points,cell->GetPointIds()->GetPointer(0));
	    continue;
	}

	assert(false);
    }

    vtkPolyData *data = vtkPolyData::New();
    data->SetPoints(points);
    data->SetPolys(cells);
    points->Delete();
    cells->Delete();

    return data;
}

int main(int argc,char * argv[])
{
    clitk::RegisterClitkFactories();

    args_info_clitkGammaIndex args_info;

    if (cmdline_parser_clitkGammaIndex(argc, argv, &args_info) != 0)
	exit(1);

    if (!args_info.absolute_dose_margin_given && !args_info.relative_dose_margin_given) {
	std::cerr << "Specify either relative or absolute dose margin" << endl;
	exit(1);
    }

    bool verbose = args_info.verbose_flag;

    std::string reference_filename(args_info.reference_arg);
    std::string target_filename(args_info.target_arg);
    std::string gamma_filename(args_info.output_arg);
    double space_margin = args_info.spatial_margin_arg;
    double dose_rel_margin = args_info.relative_dose_margin_arg;
    double dose_margin = args_info.absolute_dose_margin_arg;
    bool use_dose_margin = args_info.absolute_dose_margin_given;

    if (verbose) {
	cout << "reference_filename=" << reference_filename << endl;
	cout << "target_filename=" << target_filename << endl;
	cout << "gamma_filename=" << gamma_filename << endl;
	cout << "space_margin=" << space_margin << endl;
	if (use_dose_margin) cout << "dose_margin=" << dose_margin << endl;
	else cout << "dose_rel_margin=" << dose_rel_margin << endl;
    }

    // load reference
    vtkImageData* reference = loadImage(reference_filename);
    assert(reference);

    // translate target with arguments values
    // reference is translated instead of target so that the output space stay the same as target
    {
	double reference_origin[3];
        reference->GetOrigin(reference_origin);
	reference_origin[0] -= args_info.translation_x_arg;
	reference_origin[1] -= args_info.translation_y_arg;
	reference_origin[2] -= args_info.translation_z_arg;
	reference->SetOrigin(reference_origin);
    }

    // intensity normalisation
    if (!use_dose_margin) {
	dose_margin = getMaximum(reference)*dose_rel_margin;
	if (verbose) cout << "dose_margin=" << dose_margin << endl;
    }

    // build surface
    vtkPolyData *data = buildPlane(reference,space_margin,dose_margin);
    reference->Delete();

    vtkAbstractCellLocator *locator = vtkCellLocator::New();
    locator->SetDataSet(data);
    DD("here");
    //    data->Delete();
    locator->CacheCellBoundsOn();
    locator->AutomaticOn();
    DD("BuildLocator");
    locator->BuildLocator();
    DD("end BuildLocator");

    // load target
    vtkImageData* target = loadImage(target_filename);
    assert(target);


    // allocate output
    OutputImageType::Pointer output = OutputImageType::New();
    {
	OutputImageType::SizeType::SizeValueType output_array_size[2];
	output_array_size[0] = target->GetDimensions()[0];
	output_array_size[1] = target->GetDimensions()[1];
	OutputImageType::SizeType output_size;
	output_size.SetSize(output_array_size);
	output->SetRegions(OutputImageType::RegionType(output_size));
	output->SetOrigin(target->GetOrigin());
	output->SetSpacing(target->GetSpacing());
	output->Allocate();
    }

    // fill output
    unsigned long  kk = 0;
    unsigned long over_one = 0;
    OutputImageIterator iter(output,output->GetLargestPossibleRegion());
    iter.GoToBegin();
    DD("loop");
    while (!iter.IsAtEnd()) {
	double *point = target->GetPoint(kk);
	double value = target->GetPointData()->GetScalars()->GetTuple1(kk);
	assert(value>=0);
	assert(point[2]==0);
	point[2] = value;

	point[0] /= space_margin;
	point[1] /= space_margin;
	point[2] /= dose_margin;

	double closest_point[3] = {0,0,0};
	vtkIdType cell_id = 0;
	int foo = 0;
	double squared_distance = 0;

	locator->FindClosestPoint(point,closest_point,cell_id,foo,squared_distance);
	double distance = sqrt(squared_distance);
	iter.Set(distance);

	if (distance>1) over_one++;
	kk++;
	++iter;
    }

    if (verbose) {
	cout << "total=" << kk << endl;
	cout << "over_one=" << over_one << endl;
	cout << "ratio=" << static_cast<float>(over_one)/kk << endl;
    }

    locator->Delete();
    target->Delete();

    saveImage(output,gamma_filename);

    return 0;
}
