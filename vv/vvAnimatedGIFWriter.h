#ifndef __vvAnimatedGIFWriter_h
#define __vvAnimatedGIFWriter_h

#include <vector>

#include <vtkGenericMovieWriter.h>
#include <vtkSmartPointer.h>

class vtkImageAppend;

class vvAnimatedGIFWriter : public vtkGenericMovieWriter //test this if link error...
{
public:
  static vvAnimatedGIFWriter *New();
  vtkTypeMacro(vvAnimatedGIFWriter,vtkGenericMovieWriter);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Description:
  // These methods start writing an Movie file, write a frame to the file
  // and then end the writing process.
  void Start() VTK_OVERRIDE;
  void Write() VTK_OVERRIDE;
  void End() VTK_OVERRIDE;

  // Description:
  // Set/Get the frame rate, in frame/s.
  vtkSetClampMacro(Rate, int, 1, 5000);
  vtkGetMacro(Rate, int);

  // Description:
  // Set/Get the number of loops,  0 means infinite
  vtkSetClampMacro(Loops, int, 0, 5000);
  vtkGetMacro(Loops, int);

  // Description:
  // Set/Get the dithering
  vtkSetMacro(Dither, bool);
  vtkGetMacro(Dither, bool);

protected:
  vvAnimatedGIFWriter();
  ~vvAnimatedGIFWriter();

  int Rate;
  int Loops;
  bool Dither;

  vtkSmartPointer<vtkImageAppend> RGBvolume;
  std::vector< vtkSmartPointer<vtkImageData> > RGBslices;

private:
  vvAnimatedGIFWriter(const vvAnimatedGIFWriter&); // Not implemented
  void operator=(const vvAnimatedGIFWriter&); // Not implemented
};

#endif
