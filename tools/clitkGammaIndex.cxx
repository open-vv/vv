#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkMetaImageWriter.h>
#include <vtkPNGReader.h>
#include <vtkPolyData.h>
#include <vtkCellLocator.h>
#include <iostream>
#include <cmath>
#include <cassert>
using std::endl;
using std::cout;

#include "clitkGammaIndex_ggo.h"

vtkImageData *loadImage(const std::string &filename) {
  vtkImageReader2 *reader = vtkMetaImageReader::New();
  //vtkImageReader2 *reader = vtkPNGReader::New();
  reader->SetFileName(filename.c_str());
  reader->Update();

  vtkImageData *image = reader->GetOutput();
  image->Register(NULL);
  reader->Delete();

  return image;
}

void saveImage(vtkImageData *image,const std::string &filename) {
  cout << "saving " << filename << endl;
  vtkImageWriter *writer = vtkMetaImageWriter::New();
  writer->SetFileName(filename.c_str());
  writer->SetInput(image);
  writer->Write();
  writer->Delete();
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

double getMaximum(vtkImageData *image) {
  bool first = true;
  double maximum = 0;

  for (int kk=0; kk<image->GetNumberOfPoints(); kk++) {
    double value = image->GetPointData()->GetScalars()->GetTuple1(kk);

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
    assert(cell->GetNumberOfPoints()==4);
    insertTriangles(cells,points,cell->GetPointIds()->GetPointer(0));
  }

  vtkPolyData *data = vtkPolyData::New();
  data->SetPoints(points);
  data->SetPolys(cells);
  points->Delete();
  cells->Delete();

  return data;
}

void assert2D(vtkImageData *image) {
#ifndef NDEBUG
  int *extent = image->GetWholeExtent();
  assert(extent[4]==0);
  assert(extent[5]==0);
#endif
}

int main(int argc,char * argv[])
{
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
  vtkImageData *reference = loadImage(reference_filename);
  assert2D(reference);

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
  data->Delete();
  locator->CacheCellBoundsOn();
  locator->AutomaticOn();
  locator->BuildLocator();

  // load target
  vtkImageData *target = loadImage(target_filename);
  assert2D(target);

  // allocate output
  vtkImageData *output = vtkImageData::New();
  output->SetExtent(target->GetWholeExtent());
  output->SetOrigin(target->GetOrigin());
  output->SetSpacing(target->GetSpacing());
  output->SetScalarTypeToFloat();
  output->AllocateScalars();

  // fill output
  unsigned long total = 0;
  unsigned long over_one = 0;
  for (int kk=0; kk<target->GetNumberOfPoints(); kk++) {
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
    output->GetPointData()->GetScalars()->SetTuple1(kk,distance);

    if (value>1) over_one++;
    total++;

  }

  if (verbose) {
    cout << "total=" << total << endl;
    cout << "over_one=" << over_one << endl;
    cout << "ratio=" << static_cast<float>(over_one)/total << endl;
  }

  locator->Delete();
  target->Delete();

  saveImage(output,gamma_filename);
  output->Delete();

  return 0;
}

