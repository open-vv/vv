#ifndef vvConstants_h
#define vvConstants_h

//Constants used everywhere in the program

//Image types
typedef enum _IMAGETYPE {
IMAGE = 20,
DICOM,
MERGED,
IMAGEWITHTIME,
MERGEDWITHTIME,
VECTORFIELD,
UNDEFINEDIMAGETYPE
} LoadedImageType;

#endif
