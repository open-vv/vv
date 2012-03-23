#include "vvAnimatedGIFWriter.h"
#include "clitkDD.h"

#include "ximagif.h"

#include <vtkImageData.h>
#include <vtkImageQuantizeRGBToIndex.h>
#include <vtkImageAppend.h>
#include <vtkImageCast.h>
#include <vtkObjectFactory.h>
#include <vtkLookupTable.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vvAnimatedGIFWriter);

//---------------------------------------------------------------------------
vvAnimatedGIFWriter::vvAnimatedGIFWriter()
{
  Rate = 5;
  Loops = 0;
}

//---------------------------------------------------------------------------
vvAnimatedGIFWriter::~vvAnimatedGIFWriter()
{
}

//---------------------------------------------------------------------------
void vvAnimatedGIFWriter::Start()
{
  // Create one volume with all slices
  RGBvolume = vtkSmartPointer<vtkImageAppend>::New();
  RGBvolume->SetAppendAxis(2);
  RGBslices.clear();
}

//---------------------------------------------------------------------------
void vvAnimatedGIFWriter::Write()
{
  // get the data
  this->GetInput()->UpdateInformation();
  int *wExtent = this->GetInput()->GetWholeExtent();
  this->GetInput()->SetUpdateExtent(wExtent);
  this->GetInput()->Update();

  RGBslices.push_back( vtkSmartPointer<vtkImageData>::New() );
  RGBslices.back()->ShallowCopy(this->GetInput());
  RGBvolume->AddInput(RGBslices.back());
}

//---------------------------------------------------------------------------
void vvAnimatedGIFWriter::End()
{
  RGBvolume->Update();

  // Quantize to 8 bit colors
  vtkSmartPointer<vtkImageQuantizeRGBToIndex> quant = vtkSmartPointer<vtkImageQuantizeRGBToIndex>::New();
  quant->SetNumberOfColors(256);
  quant->SetInput(RGBvolume->GetOutput());
  quant->Update();

  // Convert to 8 bit image
  vtkSmartPointer<vtkImageCast> cast =  vtkSmartPointer<vtkImageCast>::New();
  cast->SetInput( quant->GetOutput() );
  cast->SetOutputScalarTypeToUnsignedChar();
  cast->Update();

  // Create a stack of CxImages
  DWORD width = cast->GetOutput()->GetExtent()[1]-cast->GetOutput()->GetExtent()[0]+1;
  DWORD height = cast->GetOutput()->GetExtent()[3]-cast->GetOutput()->GetExtent()[2]+1;
  std::vector<CxImage*> cximages( RGBslices.size() );
  for(unsigned int i=0; i<RGBslices.size(); i++) {
    cximages[i] = new CxImage;
    cximages[i]->CreateFromArray((BYTE *)cast->GetOutput()->GetScalarPointer(0,0,i),
                                 width, height, 8, width, false);
    cximages[i]->SetFrameDelay(100/Rate);
    cximages[i]->SetPalette((RGBQUAD*)(quant->GetLookupTable()->GetPointer(0)));
  }

  // Create gif
  FILE * pFile;
  pFile = fopen (this->FileName, "wb");
  CxImageGIF cximagegif;
  cximagegif.SetLoops(Loops);
  bool result = cximagegif.Encode(pFile,&(cximages[0]), (int)RGBslices.size(), true);

  // Cleanup
  fclose(pFile);
  for(unsigned int i=0; i<RGBslices.size(); i++)
    delete cximages[i];
  if(!result) {
    vtkErrorMacro("Error in CxImage: " << cximagegif.GetLastError() );
  }
}

//---------------------------------------------------------------------------
void vvAnimatedGIFWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
