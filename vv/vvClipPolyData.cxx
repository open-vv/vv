#include "vvClipPolyData.h"

#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataObject.h"
#include "vtkSmartPointer.h"
#include "vtkImplicitFunction.h"
#include "vtkStringArray.h"
#include "vtkPointData.h"


vtkStandardNewMacro(vvClipPolyData);

vvClipPolyData::vvClipPolyData()
{
  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);
}

vvClipPolyData::~vvClipPolyData()
{

}

int vvClipPolyData::RequestData(vtkInformation *vtkNotUsed(request),
					     vtkInformationVector **inputVector,
					     vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);
    // get the input and ouptut
    vtkPolyData *input = vtkPolyData::SafeDownCast(
			     inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkStringArray* inputLabels = vtkStringArray::SafeDownCast(input->GetPointData()->GetAbstractArray("labels"));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
			    outInfo->Get(vtkDataObject::DATA_OBJECT()));
    //
    vtkImplicitFunction* currentImpliciteFunction = this->GetClipFunction();
    int insideOutValue = this->GetInsideOut();
    //if insideOutValue=0; we want to retrieve ouside points
    //if insideOutValue=1; we want to retrieve inside points
    vtkSmartPointer<vtkPoints> outputPoints = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkStringArray> outputStrings = vtkSmartPointer<vtkStringArray>::New();
    outputStrings->SetName("labels");
    for(vtkIdType i=0;i<input->GetNumberOfPoints();i++) {
	//
	double* currentPoint = input->GetPoint(i);
	double currentIFvalue = currentImpliciteFunction->FunctionValue(currentPoint);
	//if currentIFvalue>0, current point is outside the clip
	if (currentIFvalue>0 && insideOutValue==0) {
	    outputPoints->InsertNextPoint(currentPoint);
	    vtkStdString label = inputLabels->GetValue(i);
	    outputStrings->InsertNextValue(label);
	}
	//currentIFvalue<=0, current point is inside the clip
	else if (currentIFvalue<=0 && insideOutValue==1) {
	    outputPoints->InsertNextPoint(currentPoint);
	    vtkStdString label = inputLabels->GetValue(i);
	    outputStrings->InsertNextValue(label);
	}
	else {
	    //vtkErrorMacro("vvClipPolyData - NOT IMPLEMENTED");
	}
    }
    //
    output->ShallowCopy(input);
    output->SetPoints(outputPoints);
    output->GetPointData()->AddArray(outputStrings);
    return 1;
}

//----------------------------------------------------------------------------

void vvClipPolyData::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}
