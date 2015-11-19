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
#include "vvGlyphSource.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkMath.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vvGlyphSource);


//----------------------------------------------------------------------------
int vvGlyphSource::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the ouptut
  vtkPolyData *output = vtkPolyData::SafeDownCast(
                          outInfo->Get(vtkDataObject::DATA_OBJECT()));

  //Allocate storage
  vtkPoints *pts = vtkPoints::New();
  pts->Allocate(6,6);
  vtkCellArray *verts = vtkCellArray::New();
  verts->Allocate(verts->EstimateSize(1,1),1);
  vtkCellArray *lines = vtkCellArray::New();
  lines->Allocate(lines->EstimateSize(4,2),2);
  vtkCellArray *polys = vtkCellArray::New();
  polys->Allocate(polys->EstimateSize(1,4),4);
  vtkUnsignedCharArray *colors = vtkUnsignedCharArray::New();
  colors->SetNumberOfComponents(3);
  colors->Allocate(2,2);

  this->ConvertColor();

  //Special options
  if ( this->Dash ) {
    int filled = this->Filled;
    this->Filled = 0;
    this->CreateDash(pts,lines,polys,colors,this->Scale2);
    this->Filled = filled;
  }
  if ( this->Cross ) {
    int filled = this->Filled;
    this->Filled = 0;
    this->CreateCross(pts,lines,polys,colors,this->Scale2);
    this->Filled = filled;
  }

  //Call the right function
  switch (this->GlyphType) {
  case VTK_NO_GLYPH:
    break;
  case VTK_VERTEX_GLYPH:
    this->CreateVertex(pts,verts,colors);
    break;
  case VTK_DASH_GLYPH:
    this->CreateDash(pts,lines,polys,colors,this->Scale);
    break;
  case VTK_CROSS_GLYPH:
    this->CreateCross(pts,lines,polys,colors,this->Scale);
    break;
  case VTK_THICKCROSS_GLYPH:
    this->CreateThickCross(pts,lines,polys,colors);
    break;
  case VTK_TRIANGLE_GLYPH:
    this->CreateTriangle(pts,lines,polys,colors);
    break;
  case VTK_SQUARE_GLYPH:
    this->CreateSquare(pts,lines,polys,colors);
    break;
  case VTK_CIRCLE_GLYPH:
    this->CreateCircle(pts,lines,polys,colors);
    break;
  case VTK_DIAMOND_GLYPH:
    this->CreateDiamond(pts,lines,polys,colors);
    break;
  case VTK_ARROW_GLYPH:
    this->CreateArrow(pts,lines,polys,colors);
    break;
  case VTK_THICKARROW_GLYPH:
    this->CreateThickArrow(pts,lines,polys,colors);
    break;
  case VTK_HOOKEDARROW_GLYPH:
    this->CreateHookedArrow(pts,lines,polys,colors);
    break;
  case VTK_EDGEARROW_GLYPH:
    this->CreateEdgeArrow(pts,lines,polys,colors);
    break;
  case VTK_SPECIFICARROW_GLYPH:
    this->CreateSpecificArrow(pts,lines,polys,colors);
    break;
  }

  this->TransformGlyph(pts);

  //Clean up
  output->SetPoints(pts);
  pts->Delete();

  output->SetVerts(verts);
  verts->Delete();

  output->SetLines(lines);
  lines->Delete();

  output->SetPolys(polys);
  polys->Delete();

  output->GetCellData()->SetScalars(colors);
  colors->Delete();

  return 1;
}

void vvGlyphSource::CreateSpecificArrow(vtkPoints *pts, vtkCellArray *lines,
                                        vtkCellArray *polys, vtkUnsignedCharArray *colors)
{
  //stem
  vtkIdType ptIds[3];
  ptIds[0] = pts->InsertNextPoint( 0.0, 0.0, 0.0);
  ptIds[1] = pts->InsertNextPoint(  1.0, 0.0, 0.0);
  lines->InsertNextCell(2,ptIds);
  colors->InsertNextValue(0);
  colors->InsertNextValue(0);
  colors->InsertNextValue(1);

  //arrow head
  ptIds[0] = pts->InsertNextPoint( 0.7, -0.1, 0.0);
  ptIds[1] = pts->InsertNextPoint( 1.0,  0.0, 0.0);
  ptIds[2] = pts->InsertNextPoint( 0.7,  0.1, 0.0);
  lines->InsertNextCell(3,ptIds);
  colors->InsertNextValue(0);
  colors->InsertNextValue(1);
  colors->InsertNextValue(0);
}

void vvGlyphSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
