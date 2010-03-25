#include "vvImageMapToWLColors.h"
#include "clitkCommon.h"
#include "vtkDataArray.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkScalarsToColors.h"
#include "vtkPointData.h"
#include <vtkObjectFactory.h>

vtkStandardNewMacro(vvImageMapToWLColors);

vvImageMapToWLColors::vvImageMapToWLColors() :
    wl_mode(false)
{}

template <class T>
void vtkImageMapToWindowLevelClamps ( vtkImageData *data, double w, 
                                      double l, T& lower, T& upper, 
                                      unsigned char &lower_val, 
                                      unsigned char &upper_val)
{
  double f_lower, f_upper, f_lower_val, f_upper_val;
  double adjustedLower, adjustedUpper;
  double range[2];

  data->GetPointData()->GetScalars()->GetDataTypeRange( range );

  f_lower = l - fabs(w) / 2.0;
  f_upper = f_lower + fabs(w);

  // Set the correct lower value
  if ( f_lower <= range[1])
    {
    if (f_lower >= range[0])
      {
      lower = static_cast<T>(f_lower);
      adjustedLower = f_lower;
      }
    else
      {
      lower = static_cast<T>(range[0]);
      adjustedLower = range[0];
      }
    }
  else
    {
    lower = static_cast<T>(range[1]);
    adjustedLower = range[1];
    }
  
  // Set the correct upper value
  if ( f_upper >= range[0])
    {
    if (f_upper <= range[1])
      {
      upper = static_cast<T>(f_upper);
      adjustedUpper = f_upper;
      }
    else
      {
      upper = static_cast<T>(range[1]);
      adjustedUpper = range[1];
      }
    }
  else
    {
    upper = static_cast<T>(range[0]);
    adjustedUpper = range [0];
    }
  
  // now compute the lower and upper values
  if (w >= 0)
    {
    f_lower_val = 255.0*(adjustedLower - f_lower)/w;
    f_upper_val = 255.0*(adjustedUpper - f_lower)/w;
    }
  else
    {
    f_lower_val = 255.0 + 255.0*(adjustedLower - f_lower)/w;
    f_upper_val = 255.0 + 255.0*(adjustedUpper - f_lower)/w;
    }
  
  if (f_upper_val > 255) 
    {
    upper_val = 255;
    }
  else if (f_upper_val < 0)
    {
    upper_val = 0;
    }
  else
    {
    upper_val = static_cast<unsigned char>(f_upper_val);
    }
  
  if (f_lower_val > 255) 
    {
    lower_val = 255;
    }
  else if (f_lower_val < 0)
    {
    lower_val = 0;
    }
  else
    {
    lower_val = static_cast<unsigned char>(f_lower_val);
    }  
}

template <class T>
void vvImageMapToWindowLevelColorsExecute(
  vtkImageMapToWindowLevelColors *self, 
  vtkImageData *inData, T *inPtr,
  vtkImageData *outData, 
  unsigned char *outPtr,
  int outExt[6], int id, bool wl_mode)
{
    int idxX, idxY, idxZ;
    int extX, extY, extZ;
    vtkIdType inIncX, inIncY, inIncZ;
    vtkIdType outIncX, outIncY, outIncZ;
    unsigned long count = 0;
    unsigned long target;
    int dataType = inData->GetScalarType();
    int numberOfComponents,numberOfOutputComponents,outputFormat;
    int rowLength;
    vtkScalarsToColors *lookupTable = self->GetLookupTable();
    unsigned char *outPtr1;
    T *inPtr1;
    unsigned char *optr;
    T    *iptr;
    double shift =  self->GetWindow() / 2.0 - self->GetLevel();
    double scale = 255.0 / self->GetWindow();

    T   lower, upper;
    unsigned char lower_val, upper_val, result_val;
    vtkImageMapToWindowLevelClamps( inData, self->GetWindow(), 
            self->GetLevel(), 
            lower, upper, lower_val, upper_val );

    // find the region to loop over
    extX = outExt[1] - outExt[0] + 1;
    extY = outExt[3] - outExt[2] + 1; 
    extZ = outExt[5] - outExt[4] + 1;

    target = static_cast<unsigned long>(extZ*extY/50.0);
    target++;

    // Get increments to march through data 
    inData->GetContinuousIncrements(outExt, inIncX, inIncY, inIncZ);

    outData->GetContinuousIncrements(outExt, outIncX, outIncY, outIncZ);
    numberOfComponents = inData->GetNumberOfScalarComponents();
    numberOfOutputComponents = outData->GetNumberOfScalarComponents();
    outputFormat = self->GetOutputFormat();

    rowLength = extX*numberOfComponents;

    // Loop through output pixels
    outPtr1 = outPtr;
    inPtr1 = inPtr;
    for (idxZ = 0; idxZ < extZ; idxZ++)
    {
        for (idxY = 0; !self->AbortExecute && idxY < extY; idxY++)
        {
            if (!id) 
            {
                if (!(count%target))
                {
                    self->UpdateProgress(count/(50.0*target));
                }
                count++;
            }

            iptr = inPtr1;
            optr = outPtr1;

            if ( lookupTable )
            {
                lookupTable->MapScalarsThroughTable2(
                        inPtr1,
                        static_cast<unsigned char *>(outPtr1),
                        dataType,extX,numberOfComponents,
                        outputFormat);
                if (wl_mode)
                {
                    unsigned short ushort_val;
                    for (idxX = 0; idxX < extX; idxX++)
                    {
                        if (*iptr <= lower) 
                        {
                            ushort_val = lower_val;
                        }
                        else if (*iptr >= upper)
                        {
                            ushort_val = upper_val;
                        }
                        else
                        {
                            ushort_val = static_cast<unsigned char>((*iptr + shift)*scale);
                        }
                        *optr = static_cast<unsigned char>((*optr * ushort_val) >> 8);
                        switch (outputFormat)
                        {
                            case VTK_RGBA:
                                *(optr+1) = static_cast<unsigned char>(
                                        (*(optr+1) * ushort_val) >> 8);
                                *(optr+2) = static_cast<unsigned char>(
                                        (*(optr+2) * ushort_val) >> 8);
                                *(optr+3) = 255;
                                break;
                            case VTK_RGB:
                                *(optr+1) = static_cast<unsigned char>(
                                        (*(optr+1) * ushort_val) >> 8);
                                *(optr+2) = static_cast<unsigned char>(
                                        (*(optr+2) * ushort_val) >> 8);
                                break;
                            case VTK_LUMINANCE_ALPHA:
                                *(optr+1) = 255;
                                break;
                        }
                        iptr += numberOfComponents;
                        optr += numberOfOutputComponents;
                    }
                }
            }
            else
            {
                for (idxX = 0; idxX < extX; idxX++)
                {
                    if (*iptr <= lower) 
                    {
                        result_val = lower_val;
                    }
                    else if (*iptr >= upper)
                    {
                        result_val = upper_val;
                    }
                    else
                    {
                        result_val = static_cast<unsigned char>((*iptr + shift)*scale);
                    }
                    *optr = result_val;
                    switch (outputFormat)
                    {
                        case VTK_RGBA:
                            *(optr+1) = result_val;
                            *(optr+2) = result_val;            
                            *(optr+3) = 255;
                            break;
                        case VTK_RGB:
                            *(optr+1) = result_val;
                            *(optr+2) = result_val;            
                            break;
                        case VTK_LUMINANCE_ALPHA:
                            *(optr+1) = 255;
                            break;
                    }
                    iptr += numberOfComponents;
                    optr += numberOfOutputComponents;
                }
            }      
            outPtr1 += outIncY + extX*numberOfOutputComponents;
            inPtr1 += inIncY + rowLength;
        }
        outPtr1 += outIncZ;
        inPtr1 += inIncZ;
    }
}
    
    
void vvImageMapToWLColors::ThreadedRequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *vtkNotUsed(outputVector),
  vtkImageData ***inData,
  vtkImageData **outData,
  int outExt[6], int id)
{
    void *inPtr = inData[0][0]->GetScalarPointerForExtent(outExt);
    void *outPtr = outData[0]->GetScalarPointerForExtent(outExt);

    switch (inData[0][0]->GetScalarType())
    {
        vtkTemplateMacro(
                vvImageMapToWindowLevelColorsExecute(this, 
                    inData[0][0], 
                    static_cast<VTK_TT *>(inPtr), 
                    outData[0], 
                    static_cast<unsigned char *>(outPtr), 
                    outExt, 
                    id,wl_mode));
        default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        return;
    }
}


