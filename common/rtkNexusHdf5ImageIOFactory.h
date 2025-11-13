/*=========================================================================
 *
 *  Copyright RTK Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef __rtkNexusHdf5ImageIOFactory_h
#define __rtkNexusHdf5ImageIOFactory_h

#include "rtkNexusHdf5ImageIO.h"
#include <itkImageIOBase.h>
#include <itkObjectFactoryBase.h>
#include <itkVersion.h>

namespace rtk
{

/** \class NexusHdf5ImageIOFactory
 *
 * TODO
 *
 */
class NexusHdf5ImageIOFactory : public itk::ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef NexusHdf5ImageIOFactory           Self;
  typedef itk::ObjectFactoryBase        Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Class methods used to interface with the registered factories. */
  const char* GetITKSourceVersion(void) const ITK_OVERRIDE {
    return ITK_SOURCE_VERSION;
  }

  const char* GetDescription(void) const ITK_OVERRIDE {
    return "NexusHdf5 allows the loading of Nexus/Hdf5 images into insight";
  }

  /** Method for class instantiation. */
  itkFactorylessNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(NexusHdf5ImageIOFactory, ObjectFactoryBase);

  /** Register one factory of this type  */
  static void RegisterOneFactory(void) {
    ObjectFactoryBase::RegisterFactory( Self::New() );
  }

protected:
  NexusHdf5ImageIOFactory();
  ~NexusHdf5ImageIOFactory() {}
  typedef NexusHdf5ImageIOFactory myProductType;
  const myProductType* m_MyProduct;
private:
  NexusHdf5ImageIOFactory(const Self&); //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

};

} // end namespace

#endif