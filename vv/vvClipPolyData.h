#ifndef VVCLIPPOLYDATA_H
#define VVCLIPPOLYDATA_H

#include "vtkClipPolyData.h"

class vvClipPolyData : public vtkClipPolyData
{
public:
  vtkTypeMacro(vvClipPolyData,vtkClipPolyData);
#if VTK_MAJOR_VERSION >= 8
  void PrintSelf(ostream& os, vtkIndent indent) override;
#else
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;
#endif

  static vvClipPolyData *New();

protected:
  vvClipPolyData();
  ~vvClipPolyData();

#if VTK_MAJOR_VERSION >= 8
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
#else
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
#endif

private:
  vvClipPolyData(const vvClipPolyData&);  // Not implemented.
  void operator=(const vvClipPolyData&);  // Not implemented.
};

#endif // VVCLIPPOLYDATA_H
