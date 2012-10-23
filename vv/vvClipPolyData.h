#ifndef VVCLIPPOLYDATA_H
#define VVCLIPPOLYDATA_H

#include "vtkClipPolyData.h"

class vvClipPolyData : public vtkClipPolyData
{
public:
  vtkTypeMacro(vvClipPolyData,vtkClipPolyData);
  void PrintSelf(ostream& os, vtkIndent indent);

  static vvClipPolyData *New();

protected:
  vvClipPolyData();
  ~vvClipPolyData();

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vvClipPolyData(const vvClipPolyData&);  // Not implemented.
  void operator=(const vvClipPolyData&);  // Not implemented.
};

#endif // VVCLIPPOLYDATA_H
