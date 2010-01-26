/*=========================================================================

 Program:   vv
 Language:  C++
 Author :   Pierre Seroul (pierre.seroul@gmail.com)

Copyright (C) 2008
Léon Bérard cancer center http://oncora1.lyon.fnclcc.fr
CREATIS-LRMN http://www.creatis.insa-lyon.fr

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/
#include "vvGlyph2D.h"

#include "vtkCell.h"
#include "vtkDataSet.h"
#include "vtkFloatArray.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTransform.h"
#include "vtkUnsignedCharArray.h"

vtkCxxRevisionMacro(vvGlyph2D, "DummyRevision");
vtkStandardNewMacro(vvGlyph2D);

vvGlyph2D::vvGlyph2D()
{
    mOrientation[0] = 1;
    mOrientation[1] = 1;
    mOrientation[2] = 1;
    mUseLog = 0;
}

//----------------------------------------------------------------------------
int vvGlyph2D::RequestData(
    vtkInformation *vtkNotUsed(request),
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and ouptut
    vtkDataSet *input = vtkDataSet::SafeDownCast(
                            inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkPolyData *output = vtkPolyData::SafeDownCast(
                              outInfo->Get(vtkDataObject::DATA_OBJECT()));

    vtkPointData *pd;
    vtkDataArray *inSScalars; // Scalars for Scaling
    vtkDataArray *inCScalars; // Scalars for Coloring
    vtkDataArray *inVectors;
    int requestedGhostLevel;
    unsigned char* inGhostLevels=0;
    vtkDataArray *inNormals, *sourceNormals = NULL;
    vtkDataArray *sourceTCoords = NULL;
    vtkIdType numPts, numSourcePts, numSourceCells, inPtId, i;
    int index;
    vtkPoints *sourcePts = NULL;
    vtkPoints *newPts;
    vtkDataArray *newScalars=NULL;
    vtkDataArray *newVectors=NULL;
    vtkDataArray *newNormals=NULL;
    vtkDataArray *newTCoords = NULL;
    double x[3], v[3], vNew[3], s = 0.0, vMag = 0.0, value, tc[3];
    vtkTransform *trans = vtkTransform::New();
    vtkCell *cell;
    vtkIdList *cellPts;
    int npts;
    vtkIdList *pts;
    vtkIdType ptIncr, cellId;
    int haveVectors, haveNormals, haveTCoords = 0;
    double scalex,scaley,scalez, den;
    vtkPointData *outputPD = output->GetPointData();
    int numberOfSources = this->GetNumberOfInputConnections(1);
    vtkPolyData *defaultSource = NULL;
    vtkIdTypeArray *pointIds=0;
    vtkPolyData *source = 0;

    vtkDebugMacro(<<"Generating glyphs");

    pts = vtkIdList::New();
    pts->Allocate(VTK_CELL_SIZE);

    pd = input->GetPointData();
    inSScalars = this->GetInputArrayToProcess(0,inputVector);
    inVectors = this->GetInputArrayToProcess(1,inputVector);
    inNormals = this->GetInputArrayToProcess(2,inputVector);
    inCScalars = this->GetInputArrayToProcess(3,inputVector);
    if (inCScalars == NULL)
    {
        inCScalars = inSScalars;
    }

    vtkDataArray* temp = 0;
    if (pd)
    {
        temp = pd->GetArray("vtkGhostLevels");
    }
    if ( (!temp) || (temp->GetDataType() != VTK_UNSIGNED_CHAR)
            || (temp->GetNumberOfComponents() != 1))
    {
        vtkDebugMacro("No appropriate ghost levels field available.");
    }
    else
    {
        inGhostLevels = ((vtkUnsignedCharArray*)temp)->GetPointer(0);
    }

    requestedGhostLevel =
        outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_GHOST_LEVELS());

    numPts = input->GetNumberOfPoints();
    if (numPts < 1)
    {
        vtkDebugMacro(<<"No points to glyph!");
        pts->Delete();
        trans->Delete();
        return 1;
    }

    // Check input for consistency
    //
    if ( (den = this->Range[1] - this->Range[0]) == 0.0 )
    {
        den = 1.0;
    }
    if ( this->VectorMode != VTK_VECTOR_ROTATION_OFF &&
            ((this->VectorMode == VTK_USE_VECTOR && inVectors != NULL) ||
             (this->VectorMode == VTK_USE_NORMAL && inNormals != NULL)) )
    {
        haveVectors = 1;
    }
    else
    {
        haveVectors = 0;
    }

    if ( (this->IndexMode == VTK_INDEXING_BY_SCALAR && !inSScalars) ||
            (this->IndexMode == VTK_INDEXING_BY_VECTOR &&
             ((!inVectors && this->VectorMode == VTK_USE_VECTOR) ||
              (!inNormals && this->VectorMode == VTK_USE_NORMAL))) )
    {
        if ( this->GetSource(0, inputVector[1]) == NULL )
        {
            vtkErrorMacro(<<"Indexing on but don't have data to index with");
            pts->Delete();
            trans->Delete();
            return 1;
        }
        else
        {
            vtkWarningMacro(<<"Turning indexing off: no data to index with");
            this->IndexMode = VTK_INDEXING_OFF;
        }
    }

    // Allocate storage for output PolyData
    //
    outputPD->CopyVectorsOff();
    outputPD->CopyNormalsOff();
    outputPD->CopyTCoordsOff();

    if (!this->GetSource(0, inputVector[1]))
    {
        defaultSource = vtkPolyData::New();
        defaultSource->Allocate();
        vtkPoints *defaultPoints = vtkPoints::New();
        defaultPoints->Allocate(6);
        defaultPoints->InsertNextPoint(0, 0, 0);
        defaultPoints->InsertNextPoint(1, 0, 0);
        vtkIdType defaultPointIds[2];
        defaultPointIds[0] = 0;
        defaultPointIds[1] = 1;
        defaultSource->SetPoints(defaultPoints);
        defaultSource->InsertNextCell(VTK_LINE, 2, defaultPointIds);
        defaultSource->SetUpdateExtent(0, 1, 0);
        this->SetSource(defaultSource);
        defaultSource->Delete();
        defaultSource = NULL;
        defaultPoints->Delete();
        defaultPoints = NULL;
    }

    if ( this->IndexMode != VTK_INDEXING_OFF )
    {
        pd = NULL;
        haveNormals = 1;
        for (numSourcePts=numSourceCells=i=0; i < numberOfSources; i++)
        {
            source = this->GetSource(i, inputVector[1]);
            if ( source != NULL )
            {
                if (source->GetNumberOfPoints() > numSourcePts)
                {
                    numSourcePts = source->GetNumberOfPoints();
                }
                if (source->GetNumberOfCells() > numSourceCells)
                {
                    numSourceCells = source->GetNumberOfCells();
                }
                if ( !(sourceNormals = source->GetPointData()->GetNormals()) )
                {
                    haveNormals = 0;
                }
            }
        }
    }
    else
    {
        source = this->GetSource(0, inputVector[1]);
        sourcePts = source->GetPoints();
        numSourcePts = sourcePts->GetNumberOfPoints();
        numSourceCells = source->GetNumberOfCells();

        sourceNormals = source->GetPointData()->GetNormals();
        if ( sourceNormals )
        {
            haveNormals = 1;
        }
        else
        {
            haveNormals = 0;
        }

        sourceTCoords = source->GetPointData()->GetTCoords();
        if (sourceTCoords)
        {
            haveTCoords = 1;
        }
        else
        {
            haveTCoords = 0;
        }

        // Prepare to copy output.
        pd = input->GetPointData();
        outputPD->CopyAllocate(pd,numPts*numSourcePts);
    }

    newPts = vtkPoints::New();
    newPts->Allocate(numPts*numSourcePts);
    if ( this->GeneratePointIds )
    {
        pointIds = vtkIdTypeArray::New();
        pointIds->SetName(this->PointIdsName);
        pointIds->Allocate(numPts*numSourcePts);
        outputPD->AddArray(pointIds);
        pointIds->Delete();
    }
    if ( this->ColorMode == VTK_COLOR_BY_SCALAR && inCScalars )
    {
        newScalars = inCScalars->NewInstance();
        newScalars->SetNumberOfComponents(inCScalars->GetNumberOfComponents());
        newScalars->Allocate(inCScalars->GetNumberOfComponents()*numPts*numSourcePts);
        newScalars->SetName(inCScalars->GetName());
    }
    else if ( (this->ColorMode == VTK_COLOR_BY_SCALE) && inSScalars)
    {
        newScalars = vtkFloatArray::New();
        newScalars->Allocate(numPts*numSourcePts);
        newScalars->SetName("GlyphScale");
        if (this->ScaleMode == VTK_SCALE_BY_SCALAR)
        {
            newScalars->SetName(inSScalars->GetName());
        }
    }
    else if ( (this->ColorMode == VTK_COLOR_BY_VECTOR) && haveVectors)
    {
        newScalars = vtkFloatArray::New();
        newScalars->Allocate(numPts*numSourcePts);
        newScalars->SetName("VectorMagnitude");
    }
    if ( haveVectors )
    {
        newVectors = vtkFloatArray::New();
        newVectors->SetNumberOfComponents(3);
        newVectors->Allocate(3*numPts*numSourcePts);
        newVectors->SetName("GlyphVector");
    }
    if ( haveNormals )
    {
        newNormals = vtkFloatArray::New();
        newNormals->SetNumberOfComponents(3);
        newNormals->Allocate(3*numPts*numSourcePts);
        newNormals->SetName("Normals");
    }
    if (haveTCoords)
    {
        newTCoords = vtkFloatArray::New();
        int numComps = sourceTCoords->GetNumberOfComponents();
        newTCoords->SetNumberOfComponents(numComps);
        newTCoords->Allocate(numComps*numPts*numSourcePts);
        newTCoords->SetName("TCoords");
    }

    // Setting up for calls to PolyData::InsertNextCell()
    if (this->IndexMode != VTK_INDEXING_OFF )
    {
        output->Allocate(3*numPts*numSourceCells,numPts*numSourceCells);
    }
    else
    {
        output->Allocate(this->GetSource(0, inputVector[1]),
                         3*numPts*numSourceCells, numPts*numSourceCells);
    }

    // Traverse all Input points, transforming Source points and copying
    // point attributes.
    //
    ptIncr=0;
    for (inPtId=0; inPtId < numPts; inPtId++)
    {
        scalex = scaley = scalez = 1.0;
        if ( ! (inPtId % 10000) )
        {
            this->UpdateProgress ((double)inPtId/numPts);
            if (this->GetAbortExecute())
            {
                break;
            }
        }

        // Get the scalar and vector data
        if ( inSScalars )
        {
            s = inSScalars->GetComponent(inPtId, 0);
            if ( this->ScaleMode == VTK_SCALE_BY_SCALAR ||
                    this->ScaleMode == VTK_DATA_SCALING_OFF )
            {
                scalex = scaley = scalez = s;
            }
        }

        if ( haveVectors )
        {
            if ( this->VectorMode == VTK_USE_NORMAL )
            {
                inNormals->GetTuple(inPtId, v);
            }
            else
            {
                inVectors->GetTuple(inPtId, v);
            }

            vMag = vtkMath::Norm(v);
            if ( this->ScaleMode == VTK_SCALE_BY_VECTORCOMPONENTS )
            {
                scalex = v[0];
                scaley = v[1];
                scalez = v[2];
            }
            else if ( this->ScaleMode == VTK_SCALE_BY_VECTOR )
            {
                scalex = scaley = scalez = vMag;
            }
        }

        // Clamp data scale if enabled
        if ( this->Clamping )
        {
            scalex = (scalex < this->Range[0] ? this->Range[0] :
                      (scalex > this->Range[1] ? this->Range[1] : scalex));
            scalex = (scalex - this->Range[0]) / den;
            scaley = (scaley < this->Range[0] ? this->Range[0] :
                      (scaley > this->Range[1] ? this->Range[1] : scaley));
            scaley = (scaley - this->Range[0]) / den;
            scalez = (scalez < this->Range[0] ? this->Range[0] :
                      (scalez > this->Range[1] ? this->Range[1] : scalez));
            scalez = (scalez - this->Range[0]) / den;
        }

        // Compute index into table of glyphs
        if ( this->IndexMode == VTK_INDEXING_OFF )
        {
            index = 0;
        }
        else
        {
            if ( this->IndexMode == VTK_INDEXING_BY_SCALAR )
            {
                value = s;
            }
            else
            {
                value = vMag;
            }

            index = (int) ((double)(value - this->Range[0]) * numberOfSources / den);
            index = (index < 0 ? 0 :
                     (index >= numberOfSources ? (numberOfSources-1) : index));

            source = this->GetSource(index, inputVector[1]);
            if ( source != NULL )
            {
                sourcePts = source->GetPoints();
                sourceNormals = source->GetPointData()->GetNormals();
                numSourcePts = sourcePts->GetNumberOfPoints();
                numSourceCells = source->GetNumberOfCells();
            }
        }

        // Make sure we're not indexing into empty glyph
        if ( this->GetSource(index, inputVector[1]) == NULL )
        {
            continue;
        }

        // Check ghost points.
        // If we are processing a piece, we do not want to duplicate
        // glyphs on the borders.  The corrct check here is:
        // ghostLevel > 0.  I am leaving this over glyphing here because
        // it make a nice example (sphereGhost.tcl) to show the
        // point ghost levels with the glyph filter.  I am not certain
        // of the usefullness of point ghost levels over 1, but I will have
        // to think about it.
        if (inGhostLevels && inGhostLevels[inPtId] > requestedGhostLevel)
        {
            continue;
        }

        if (!this->IsPointVisible(input, inPtId))
        {
            continue;
        }

        // Now begin copying/transforming glyph
        trans->Identity();

        // Copy all topology (transformation independent)
        for (cellId=0; cellId < numSourceCells; cellId++)
        {
            cell = this->GetSource(index, inputVector[1])->GetCell(cellId);
            cellPts = cell->GetPointIds();
            npts = cellPts->GetNumberOfIds();
            for (pts->Reset(), i=0; i < npts; i++)
            {
                pts->InsertId(i,cellPts->GetId(i) + ptIncr);
            }
            output->InsertNextCell(cell->GetCellType(),pts);
        }

        // translate Source to Input point
        input->GetPoint(inPtId, x);

        //projection on the plane orthogonale to the camera
        trans->Scale(mOrientation[0],mOrientation[1],mOrientation[2]);

        trans->Translate(x[0], x[1], x[2]);

        if ( haveVectors )
        {
            // Copy Input vector
            for (i=0; i < numSourcePts; i++)
            {
                newVectors->InsertTuple(i+ptIncr, v);
            }
            if (this->Orient && (vMag > 0.0))
            {
                // if there is no y or z component
                if ( v[1] == 0.0 && v[2] == 0.0 )
                {
                    if (v[0] < 0) //just flip x if we need to
                    {
                        trans->RotateWXYZ(180.0,0,1,0);
                    }
                }
                else
                {
                    vNew[0] = (v[0]+vMag) / 2.0;
                    vNew[1] = v[1] / 2.0;
                    vNew[2] = v[2] / 2.0;
                    trans->RotateWXYZ((double)180.0,vNew[0],vNew[1],vNew[2]);
                }
            }
        }

        if (haveTCoords)
        {
            for (i = 0; i < numSourcePts; i++)
            {
                sourceTCoords->GetTuple(i, tc);
                newTCoords->InsertTuple(i+ptIncr, tc);
            }
        }

        // determine scale factor from scalars if appropriate
        // Copy scalar value
        if (inSScalars && (this->ColorMode == VTK_COLOR_BY_SCALE))
        {
            for (i=0; i < numSourcePts; i++)
            {
                newScalars->InsertTuple(i+ptIncr, &scalex); // = scaley = scalez
            }
        }
        else if (inCScalars && (this->ColorMode == VTK_COLOR_BY_SCALAR))
        {
            for (i=0; i < numSourcePts; i++)
            {
                outputPD->CopyTuple(inCScalars, newScalars, inPtId, ptIncr+i);
            }
        }
        if (haveVectors && this->ColorMode == VTK_COLOR_BY_VECTOR)
        {
            double color = 1;
            for (i=0; i < numSourcePts; i++)
            {
                newScalars->InsertTuple(i+ptIncr, &color);
            }
        }

        // scale data if appropriate
        if ( this->Scaling )
        {
            if ( this->ScaleMode == VTK_DATA_SCALING_OFF )
            {
                scalex = scaley = scalez = this->ScaleFactor;
            }
            else
            {
                scalex *= this->ScaleFactor;
                scaley *= this->ScaleFactor;
                scalez *= this->ScaleFactor;
            }

            if ( scalex == 0.0 )
            {
                scalex = 1.0e-10;
            }
            if ( scaley == 0.0 )
            {
                scaley = 1.0e-10;
            }
            if ( scalez == 0.0 )
            {
                scalez = 1.0e-10;
            }
            trans->Scale(scalex,scaley,scalez);
        }
        // multiply points and normals by resulting matrix
        trans->TransformPoints(sourcePts,newPts);

        if ( haveNormals )
        {
            trans->TransformNormals(sourceNormals,newNormals);
        }

        // Copy point data from source (if possible)
        if ( pd )
        {
            for (i=0; i < numSourcePts; i++)
            {
                outputPD->CopyData(pd,inPtId,ptIncr+i);
            }
        }

        // If point ids are to be generated, do it here
        if ( this->GeneratePointIds )
        {
            for (i=0; i < numSourcePts; i++)
            {
                pointIds->InsertNextValue(inPtId);
            }
        }

        ptIncr += numSourcePts;
    }

    // Update ourselves and release memory
    //
    output->SetPoints(newPts);
    newPts->Delete();

    if (newScalars)
    {
        int idx = outputPD->AddArray(newScalars);
        outputPD->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
        newScalars->Delete();
    }

    if (newVectors)
    {
        outputPD->SetVectors(newVectors);
        newVectors->Delete();
    }

    if (newNormals)
    {
        outputPD->SetNormals(newNormals);
        newNormals->Delete();
    }

    if (newTCoords)
    {
        outputPD->SetTCoords(newTCoords);
        newTCoords->Delete();
    }

    output->Squeeze();
    trans->Delete();
    pts->Delete();

    return 1;
}

void vvGlyph2D::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);
}

void vvGlyph2D::SetOrientation(int x, int y, int z)
{
    if (x == 0)
        mOrientation[0] = 1.0e-10;
    else
        mOrientation[0] = 1.0;
    if (y == 0)
        mOrientation[1] = 1.0e-10;
    else
        mOrientation[1] = 1.0;
    if (z == 0)
        mOrientation[2] = 1.0e-10;
    else
        mOrientation[2] = 1.0;
}
