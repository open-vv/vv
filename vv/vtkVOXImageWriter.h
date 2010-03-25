#ifndef __vtkVoxImageWriter_h
#define __vtkVOXImageWriter_h
#include "vtkImageWriter.h"
#include <string>


class vtkVOXImageWriter : public vtkImageWriter
{
public:
    vtkTypeRevisionMacro(vtkVOXImageWriter,vtkImageWriter);
    void PrintSelf(ostream& os, vtkIndent indent);

    // Description:
    static vtkVOXImageWriter *New();

    // Description:
    // Specify file name of meta file
    vtkSetStringMacro(FileName);

    // This is called by the superclass.
    // This is the method you should override.
    virtual void Write();

protected:
    vtkVOXImageWriter();
    ~vtkVOXImageWriter();

private:
    vtkVOXImageWriter(const vtkVOXImageWriter&);  // Not implemented.
    void operator=(const vtkVOXImageWriter&);  // Not implemented.


};

#endif
