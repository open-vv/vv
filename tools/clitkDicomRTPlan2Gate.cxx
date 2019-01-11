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
#ifndef CLITKDICOMRTPLAN2GATE_CXX
#define CLITKDICOMRTPLAN2GATE_CXX

#include "clitkDicomRTPlan2Gate_ggo.h"

#include "clitkIO.h"
#include "clitkCommon.h"

#include "gdcmFile.h"
#include "gdcmReader.h"
#include "gdcmAttribute.h"

int main(int argc, char * argv[])
{

  // init command line
  GGO(clitkDicomRTPlan2Gate, args_info);
  CLITK_INIT;
  char* input_file = args_info.input_arg;
  char* output_file = args_info.output_arg;

  gdcm::Reader reader;
  reader.SetFileName(input_file);
  if( !reader.Read() )
  {
    std::cerr << "Failed to read: " << input_file << std::endl;
    return 1;
  }

  const gdcm::DataSet & ds = reader.GetFile().GetDataSet();

  //Check file type
  gdcm::MediaStorage ms;
  ms.SetFromFile(reader.GetFile());
  if( ms != gdcm::MediaStorage::RTIonPlanStorage )
  {
    std::cerr << "The file <" << input_file << "> is not a RT Ion Plan." << std::endl;
    exit(0);
  }
  
  std::ofstream output;
  output.open(output_file);
  output << "#TREATMENT-PLAN-DESCRIPTION" << "\n";
  
  gdcm::Attribute<0x300a,0x2> label;
  label.SetFromDataSet(ds);
  gdcm::Attribute<0x300a,0x3> planName;
  planName.SetFromDataSet(ds);
  
  output << "#PlanName\n"<< planName.GetValue() << "\n";
  
  gdcm::Tag tFractionGroupSQ(0x300a,0x70);
  gdcm::Attribute<0x300a,0x71> fractionID;
  gdcm::Attribute<0x300a,0x78> numberOfFractions;
  gdcm::Attribute<0x300a,0x80> numberOfBeams;
  
  gdcm::Tag tReferencedBeamSQ(0x300c,0x4);
  gdcm::Attribute<0x300a,0x86> meterSet;
  gdcm::Attribute<0x300c,0x6> beamID;
  
  const gdcm::DataElement &FractionGroupSQ = ds.GetDataElement( tFractionGroupSQ );
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqf = FractionGroupSQ.GetValueAsSQ();
  gdcm::Item &ReferencedBeamItem = sqf->GetItem(1);
  gdcm::DataSet &ReferencedBeamData = ReferencedBeamItem.GetNestedDataSet();
  numberOfFractions.SetFromDataSet(ReferencedBeamData);
  
  output << "#NumberOfFractions\n"<< numberOfFractions.GetValue() << "\n";
  
  for( unsigned int fidx = 1; fidx <= sqf->GetNumberOfItems(); ++fidx )
  {
    gdcm::Item &ReferencedBeamItem = sqf->GetItem(fidx);
    gdcm::DataSet &ReferencedBeamData = ReferencedBeamItem.GetNestedDataSet();
    
    fractionID.SetFromDataSet(ReferencedBeamData);
    numberOfFractions.SetFromDataSet(ReferencedBeamData);
    numberOfBeams.SetFromDataSet(ReferencedBeamData);
    output << "##FractionID\n"<<  fractionID.GetValue() << "\n";
    output << "##NumberOfFields\n"<<  numberOfBeams.GetValue() << "\n";
        
    const gdcm::DataElement &ReferencedBeamSQ = ReferencedBeamData.GetDataElement(tReferencedBeamSQ);
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqb = ReferencedBeamSQ.GetValueAsSQ();   
    for( unsigned int bidx = 1; bidx <= sqb->GetNumberOfItems(); ++bidx )
    {
      gdcm::Item &ReferencedBeamGroupItem = sqb->GetItem(bidx);
      gdcm::DataSet &ReferencedBeamGroupData = ReferencedBeamGroupItem.GetNestedDataSet();
      
      meterSet.SetFromDataSet(ReferencedBeamGroupData);
      beamID.SetFromDataSet(ReferencedBeamGroupData);
      output << "###FieldsID\n"<<  beamID.GetValue() << "\n";
    }
  }
  
  gdcm::Tag tPatientSetupSQ(0x300a,0x180);
  gdcm::Attribute<0x0018,0x5100> position;
  gdcm::Attribute<0x300a,0x1b0> setupTechnique;
  gdcm::Attribute<0x300a,0x182> setupID;
  
  const gdcm::DataElement &PatientSetupSQ = ds.GetDataElement( tPatientSetupSQ );
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqs = PatientSetupSQ.GetValueAsSQ();
  for( unsigned int sidx = 1; sidx <= sqs->GetNumberOfItems(); ++sidx )
  {
    gdcm::Item &PatientSetupItem = sqs->GetItem(sidx);
    gdcm::DataSet &PatientSetupData = PatientSetupItem.GetNestedDataSet();
    
    position.SetFromDataSet(PatientSetupData);
    setupTechnique.SetFromDataSet(PatientSetupData);
    setupID.SetFromDataSet(PatientSetupData);
  }
  
  gdcm::Tag tIonBeamSQ(0x300a,0x3a2);
  gdcm::Attribute<0x300a,0xb2> machine;
  gdcm::Attribute<0x300a,0xc6> radiationType;
  gdcm::Attribute<0x300a,0xce> treatmentType;
  gdcm::Attribute<0x300a,0xc0> ionBeamID;
  gdcm::Attribute<0x300a,0x10e> finalCumulativeMeterSetWeight;
  gdcm::Attribute<0x300a,0x110> numberOfControlPoints;
  gdcm::Attribute<0x300c,0x6a> patientSetupID;
  gdcm::Attribute<0x300a,0xd0> numberOfWedges;
  gdcm::Attribute<0x300a,0xe0> numberOfCompensators;
  gdcm::Attribute<0x300a,0xed> numberOfBoli;
  gdcm::Attribute<0x300a,0xf0> numberOfBlocks;
  gdcm::Attribute<0x300a,0x312> numberOfRangeShifters;
  gdcm::Attribute<0x300a,0x330> numberOfLateralSpreadingDevices;
  gdcm::Attribute<0x300a,0x340> numberOfRangeModulators;
  
  //gdcm::Tag tSnoutSQ(0x300a,0x30c);
  //gdcm::Attribute<0x300a,0x30f> snoutID;
  
  gdcm::Tag tControlPointSQ(0x300a,0x3a8);
  gdcm::Attribute<0x300a,0x15> energyUnit;
  gdcm::Attribute<0x300a,0x114> energyValue;
  gdcm::Attribute<0x300a,0x112> controlPointIndex;
  gdcm::Attribute<0x300a,0x134> cumulativeMetersetWeight;
  gdcm::Attribute<0x300a,0x392> numberOfScannedSpots;
  gdcm::Attribute<0x300a,0x390> spotTunnedID;
  
  gdcm::Tag tSpotPositionsData(0x300a,0x394);
  gdcm::Attribute<0x300a,0x394> spotPositions;
  gdcm::Tag tSpotWeightsData(0x300a,0x396);
  gdcm::Attribute<0x300a,0x396> spotWeights;
  
  gdcm::Attribute<0x300a,0x11e> gantryAngle;
  gdcm::Attribute<0x300a,0x122> patientSupportAngle;
  gdcm::Attribute<0x300a,0x12c> isocenterPosition;
  
  long totalCumMeterSet = 0;
  
  const gdcm::DataElement &IonBeamSQ = ds.GetDataElement( tIonBeamSQ );
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = IonBeamSQ.GetValueAsSQ();
  
  for( unsigned int iidx = 1; iidx <= sqi->GetNumberOfItems(); ++iidx )
  {
    gdcm::Item &IonBeamItem = sqi->GetItem(iidx);
    gdcm::DataSet &IonBeamData = IonBeamItem.GetNestedDataSet();
    finalCumulativeMeterSetWeight.SetFromDataSet(IonBeamData);
    totalCumMeterSet += finalCumulativeMeterSetWeight.GetValue();
  }
  
  output << "#TotalMetersetWeightOfAllFields\n" <<  totalCumMeterSet << "\n";
  
  for( unsigned int iidx = 1; iidx <= sqi->GetNumberOfItems(); ++iidx )
  {
    gdcm::Item &IonBeamItem = sqi->GetItem(iidx);
    gdcm::DataSet &IonBeamData = IonBeamItem.GetNestedDataSet();
    
    machine.SetFromDataSet(IonBeamData);
    radiationType.SetFromDataSet(IonBeamData);
    treatmentType.SetFromDataSet(IonBeamData);
    ionBeamID.SetFromDataSet(IonBeamData);
    finalCumulativeMeterSetWeight.SetFromDataSet(IonBeamData);
    numberOfControlPoints.SetFromDataSet(IonBeamData);
    patientSetupID.SetFromDataSet(IonBeamData);
    numberOfWedges.SetFromDataSet(IonBeamData);
    numberOfCompensators.SetFromDataSet(IonBeamData);
    numberOfBoli.SetFromDataSet(IonBeamData);
    numberOfBlocks.SetFromDataSet(IonBeamData);
    numberOfRangeShifters.SetFromDataSet(IonBeamData);
    numberOfLateralSpreadingDevices.SetFromDataSet(IonBeamData);
    numberOfRangeModulators.SetFromDataSet(IonBeamData);
    
    /*
    const gdcm::DataElement &SnoutSQ = IonBeamData.GetDataElement(tSnoutSQ);
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqsn = SnoutSQ.GetValueAsSQ();
    for( unsigned int snidx = 1; snidx <= sqsn->GetNumberOfItems(); ++snidx )
    {
      gdcm::Item &SnoutItem = sqsn->GetItem(snidx);
      gdcm::DataSet &SnoutData = SnoutItem.GetNestedDataSet();
      
      snoutID.SetFromDataSet(SnoutData);
    }
    */
    
    const gdcm::DataElement &ControlPointSQ = IonBeamData.GetDataElement(tControlPointSQ);
    gdcm::SmartPointer<gdcm::SequenceOfItems> sqcp = ControlPointSQ.GetValueAsSQ();
    gdcm::Item &ControlPointItem = sqcp->GetItem(1);
    gdcm::DataSet &ControlPointData = ControlPointItem.GetNestedDataSet();
    
    gantryAngle.SetFromDataSet(ControlPointData);
    patientSupportAngle.SetFromDataSet(ControlPointData);
    isocenterPosition.SetFromDataSet(ControlPointData);
    
    output << "\n#FIELD-DESCRIPTION" << "\n";
    output << "###FieldID\n"<< ionBeamID.GetValue() << "\n";
    output << "###FinalCumulativeMeterSetWeight\n"<<  finalCumulativeMeterSetWeight.GetValue() << "\n";
    output << "###GantryAngle\n" << gantryAngle.GetValue() << "\n";
    output << "###PatientSupportAngle\n" << patientSupportAngle.GetValue() << "\n";
    output << "###IsocenterPosition\n" << isocenterPosition.GetValue(0);
    output << " " << isocenterPosition.GetValue(1);
    output << " " << isocenterPosition.GetValue(2) << "\n";
    output << "###NumberOfControlPoints\n" << numberOfControlPoints.GetValue() << "\n\n";
    output << "#SPOTS-DESCRIPTION" << "\n";
    
    for( unsigned int cpidx = 1; cpidx <= sqcp->GetNumberOfItems(); ++cpidx )
    {
      gdcm::Item &ControlPointItem = sqcp->GetItem(cpidx);
      gdcm::DataSet &ControlPointData = ControlPointItem.GetNestedDataSet();
      
      if ( cpidx == 1 )
      {
	gantryAngle.SetFromDataSet(ControlPointData);
	patientSupportAngle.SetFromDataSet(ControlPointData);
	isocenterPosition.SetFromDataSet(ControlPointData);
      }
      
      if ( cpidx < sqcp->GetNumberOfItems() )
      {
	energyUnit.SetFromDataSet(ControlPointData);
	energyValue.SetFromDataSet(ControlPointData);
	controlPointIndex.SetFromDataSet(ControlPointData);
	cumulativeMetersetWeight.SetFromDataSet(ControlPointData);
	numberOfScannedSpots.SetFromDataSet(ControlPointData);
	spotTunnedID.SetFromDataSet(ControlPointData);
	
	const gdcm::DataElement &spotPositionsData = ControlPointData.GetDataElement(tSpotPositionsData);
	spotPositions.SetFromDataElement(spotPositionsData);
	const gdcm::DataElement &spotWeightsData = ControlPointData.GetDataElement(tSpotWeightsData);
	spotWeights.SetFromDataElement(spotWeightsData);
      }
      
      if ( cpidx == sqcp->GetNumberOfItems() )
      {
 	energyUnit.SetValue( "0" );
 	energyValue.SetValue( 0 );
	controlPointIndex.SetFromDataSet(ControlPointData);
	cumulativeMetersetWeight.SetFromDataSet(ControlPointData);
 	numberOfScannedSpots.SetValue( 0 );
 	spotTunnedID.SetValue( "0" );
      }
      
      output << "####ControlPointIndex\n" << controlPointIndex.GetValue() << "\n";
      output << "####SpotTunnedID\n" << spotTunnedID.GetValue() << "\n";
      output << "####CumulativeMetersetWeight\n" << cumulativeMetersetWeight.GetValue() << "\n";
      output << "####Energy (MeV)\n"<< energyValue.GetValue() << "\n";
      output << "####NbOfScannedSpots\n" << numberOfScannedSpots.GetValue() << "\n";
      output << "####X Y Weight\n";
      for ( unsigned int sp=0; sp < numberOfScannedSpots.GetValue(); ++sp)
      {
	output << spotPositions.GetValue(2*sp) << " " << spotPositions.GetValue(2*sp+1) << " " << spotWeights.GetValue(sp) << "\n";
      }
      
    }
    
  }
    
  return 0;
}

#endif
