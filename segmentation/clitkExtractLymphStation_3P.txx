
//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_SetDefaultValues()
{
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class TImageType>
void 
clitk::ExtractLymphStationsFilter<TImageType>::
ExtractStation_3P()
{
  if (!CheckForStation("3P")) return;

  StartNewStep("Station 3P");
  StartSubStep();  

  // Get the current support 
  StartNewStep("[Station 3P] Get the current 3P suppport");
  m_Working_Support = m_ListOfSupports["S3P"];
  m_ListOfStations["3P"] = m_Working_Support;
  StopCurrentStep<MaskImageType>(m_Working_Support);

  // Generic RelativePosition processes
  m_ListOfStations["3P"] = this->ApplyRelativePositionList("Station_3P", m_ListOfStations["3P"]);
  m_Working_Support = m_ListOfStations["3P"];
  ExtractStation_8_Single_CCL_Limits(); // YES 8 !
  ExtractStation_3P_Remove_Structures(); // after CCL
  m_ListOfStations["3P"] = m_Working_Support;

  // Old stuff
  // LR limits superiorly => not here for the moment because not clear in the def
  // ExtractStation_3P_LR_sup_Limits_2(); //TODO
  // ExtractStation_3P_LR_sup_Limits();   // old version to change
  // ExtractStation_3P_LR_inf_Limits();  // <-- done in RelPosList 

  // Store image filenames into AFDB 
  writeImage<MaskImageType>(m_ListOfStations["3P"], "seg/Station3P.mhd");
  GetAFDB()->SetImageFilename("Station3P", "seg/Station3P.mhd"); 
  WriteAFDB();   
  StopSubStep();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_Remove_Structures() 
{
  StartNewStep("[Station 3P] Remove some structures.");
  Remove_Structures("3P", "Esophagus");
  Remove_Structures("3P", "Thyroid");
  Remove_Structures("3P", "VertebralArtery"); // (inside the station)
  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_LR_sup_Limits() 
{
  /*
    "On the right side, the limit is deﬁned by the air–soft-tissue
    interface. On the left side, it is deﬁned by the air–tissue
    interface superiorly (Fig. 2A–C) and the aorta inferiorly
    (Figs. 2D–I and 3A–C)."

    sup : 
    Resizelike support : Trachea, SubclavianArtery
    Trachea, slice by slice, get centroid trachea
    SubclavianArtery, slice by slice, CCL
    prendre la 1ère à L et R, not at Left
    
  */
  StartNewStep("[Station 3P] Left/Right limits (superior part) ");

  // Load structures
  MaskImagePointer Trachea = GetAFDB()->template GetImage<MaskImageType>("Trachea");
  MaskImagePointer SubclavianArtery = GetAFDB()->template GetImage<MaskImageType>("SubclavianArtery");
  
  // Crop like current support
  Trachea = clitk::ResizeImageLike<MaskImageType>(Trachea, m_Working_Support, GetBackgroundValue());
  SubclavianArtery = clitk::ResizeImageLike<MaskImageType>(SubclavianArtery, m_Working_Support, GetBackgroundValue());
  
  // writeImage<MaskImageType>(Trachea, "tr.mhd");
  // writeImage<MaskImageType>(SubclavianArtery, "sca.mhd");
  
  // Get list of slices
  std::vector<MaskSlicePointer> slices_support;
  std::vector<MaskSlicePointer> slices_trachea;
  std::vector<MaskSlicePointer> slices_subclavianartery;
  clitk::ExtractSlices<MaskImageType>(m_Working_Support, 2, slices_support);
  clitk::ExtractSlices<MaskImageType>(Trachea, 2, slices_trachea);
  clitk::ExtractSlices<MaskImageType>(SubclavianArtery, 2, slices_subclavianartery);

  // Loop on slices
  std::vector<MaskImagePointType> points;
  MaskImagePointType p;
  for(uint i=0; i<slices_support.size(); i++) {
    // Get Trachea centroid
    std::vector<typename MaskSliceType::PointType> centroids;
    typename MaskSliceType::PointType c;
    ComputeCentroids<MaskSliceType>(slices_trachea[i], GetBackgroundValue(), centroids);
    c = centroids[1];

    // [debug] Store point
    clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[1], Trachea, i, p);
    points.push_back(p);
    
    // Get Right and Left CCL in SubclavianArtery
    slices_subclavianartery[i] = Labelize<MaskSliceType>(slices_subclavianartery[i], 0, true, 10);
    ComputeCentroids<MaskSliceType>(slices_subclavianartery[i], GetBackgroundValue(), centroids);

    if (centroids.size() > 1) {
      // Determine the one at Right/Left -> first after Trachea
      // centroid 
      typename MaskSliceType::PointType right;
      typename MaskSliceType::PointType left;
      int label_right=-1;
      int label_left=-1;
      right[0] = c[0]-100;
      left[0] = c[0]+100;
      for(uint j=1; j<centroids.size(); j++) {
        if (centroids[j][0] < c[0]) { // At Right of Trachea centroid
          if (centroids[j][0] >= right[0]) {
            right = centroids[j];
            label_right = j;
          }
        }
        if (centroids[j][0] > c[0]) { // At Left of Trachea centroid
          if (centroids[j][0] <= left[0]) {
            left = centroids[j];
            label_left = j;
          }
        }
      }

      if (label_right != -1) {
    
        // Debug points
        clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[label_right], SubclavianArtery, i, p);
        points.push_back(p);

        // Set Background and ForegroundValue according to label_right
        MaskSlicePointer object = 
          clitk::Binarize<MaskSliceType>(slices_subclavianartery[i], label_right, label_right, 
                                         GetBackgroundValue(), GetForegroundValue());
      
        // Relative Position : not at Right
        typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskSliceType> RelPosFilterType;
        typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("RightTo");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

        // Relative Position : not Anterior
        relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("AntTo");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

      } // End RelativePosition for Right


      if (label_left != -1) {
    
        // Debug points
        clitk::PointsUtils<MaskImageType>::Convert2DTo3D(centroids[label_left], SubclavianArtery, i, p);
        points.push_back(p);

        // Set Background and ForegroundValue according to label_right
        MaskSlicePointer object = 
          clitk::Binarize<MaskSliceType>(slices_subclavianartery[i], label_left, label_left, 
                                         GetBackgroundValue(), GetForegroundValue());
      
        // Relative Position : not at Right
        typedef clitk::AddRelativePositionConstraintToLabelImageFilter<MaskSliceType> RelPosFilterType;
        typename RelPosFilterType::Pointer relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("L");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

        // Relative Position : not Anterior
        relPosFilter = RelPosFilterType::New();
        relPosFilter->VerboseStepFlagOff();
        relPosFilter->WriteStepFlagOff();
        relPosFilter->SetBackgroundValue(GetBackgroundValue());
        relPosFilter->SetInput(slices_support[i]); 
        relPosFilter->SetInputObject(object); 
        relPosFilter->AddOrientationTypeString("A");
        relPosFilter->SetInverseOrientationFlag(true);
        //      relPosFilter->SetIntermediateSpacing(3);
        relPosFilter->SetIntermediateSpacingFlag(false);
        relPosFilter->SetFuzzyThreshold(0.7);
        relPosFilter->AutoCropFlagOff(); // important ! because we join the slices after this loop
        relPosFilter->Update();
        slices_support[i] = relPosFilter->GetOutput();

      }

    
    } // if centroids.size > 1
  } // End loop slices

  // Joint slices
  m_Working_Support = clitk::JoinSlices<MaskImageType>(slices_support, m_Working_Support, 2);

  // Save list points
  clitk::WriteListOfLandmarks<MaskImageType>(points, "subcl-lr.txt");


  StopCurrentStep<MaskImageType>(m_Working_Support);
  m_ListOfStations["3P"] = m_Working_Support;
}
//--------------------------------------------------------------------

//--------------------------------------------------------------------
template <class ImageType>
void 
clitk::ExtractLymphStationsFilter<ImageType>::
ExtractStation_3P_LR_sup_Limits_2() 
{
  /*
    "On the right side, the limit is deﬁned by the air–soft-tissue
    interface. On the left side, it is deﬁned by the air–tissue
    interface superiorly (Fig. 2A–C) and the aorta inferiorly
    (Figs. 2D–I and 3A–C)."

    sup : 
    Resizelike support : Trachea, SubclavianArtery
    Trachea, slice by slice, get centroid trachea
    SubclavianArtery, slice by slice, CCL
    prendre la 1ère à L et R, not at Left
    
  */
  //  StartNewStep("[Station 3P] Left/Right limits (superior part) ");
  

}
//--------------------------------------------------------------------

