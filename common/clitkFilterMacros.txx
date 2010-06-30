/*=========================================================================
  Program:   vv                     http://www.creatis.insa-lyon.fr/rio/vv

  Authors belong to: 
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://oncora1.lyon.fnclcc.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ======================================================================-====*/

#define GGO_DefineOption_Vector(OPTION, FUNC, TYPE, NB, BOOL)           \
  template<class ArgsInfoType>                                          \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {                           \
    if (mArgsInfo.OPTION##_given == NB) {                               \
      TYPE aaa;                                                         \
      for(unsigned int i=0; i<NB; i++) {                                \
        aaa[i] = mArgsInfo.OPTION##_arg[i];                             \
      }                                                                 \
      VerboseOption(#FUNC, mArgsInfo.OPTION##_given, mArgsInfo.OPTION##_arg); \
      FUNC(aaa);                                                        \
    }                                                                   \
    else {                                                              \
      if (BOOL && mArgsInfo.OPTION##_given != 0) {                      \
        std::cerr << "Sorry, you give only " << mArgsInfo.OPTION##_given << " while " \
                  << NB << " values a required for option '" << #OPTION << "'" << std::endl; \
        exit(0);                                                        \
      }                                                                 \
    }                                                                   \
  }

#define GGO_DefineOption_Multiple(OPTION, FUNC, TYPE)                   \
  template<class ArgsInfoType>                                          \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {                           \
    VerboseOption(#OPTION, mArgsInfo.OPTION##_given, mArgsInfo.OPTION##_arg); \
    for(unsigned int i=0; i<mArgsInfo.OPTION##_given; i++) {            \
      FUNC(mArgsInfo.OPTION##_arg[i]);                                  \
    }                                                                   \
  }

#define GGO_DefineOption(OPTION, FUNC, TYPE)            \
  template<class ArgsInfoType>                          \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {           \
    VerboseOption(#OPTION, mArgsInfo.OPTION##_arg);     \
    FUNC(static_cast<TYPE>(mArgsInfo.OPTION##_arg));    \
  }

#define GGO_DefineOption_Flag(OPTION, FUNC)              \
  template<class ArgsInfoType>                           \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {            \
    VerboseOption(#OPTION, mArgsInfo.OPTION##_flag);     \
    FUNC(mArgsInfo.OPTION##_flag);    \
  }

#define GGO_DefineOption_WithTest(OPTION, FUNC, TYPE, TEST)             \
  template<class ArgsInfoType>                                          \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {                           \
    VerboseOption(#OPTION, mArgsInfo.OPTION##_given, mArgsInfo.OPTION##_arg); \
    if (mArgsInfo.OPTION##_given) {                                     \
      VerboseOption(#OPTION, mArgsInfo.OPTION##_arg);                   \
      FUNC(static_cast<TYPE>(mArgsInfo.OPTION##_arg));                  \
      TEST##On();                                                       \
    }                                                                   \
    else TEST##Off();                                                   \
  }

#define GGO_DefineOption_LabelParam(OPTION, FUNC, TYPE)                 \
  template<class ArgsInfoType>                                          \
  void FUNC##_GGO(ArgsInfoType & mArgsInfo) {                           \
    TYPE * param = new TYPE;                                            \
    param->SetFirstKeep(mArgsInfo.firstKeep##OPTION##_arg);             \
    for(unsigned int i=0; i<mArgsInfo.remove##OPTION##_given; i++)      \
      param->AddLabelToRemove(mArgsInfo.remove##OPTION##_arg[i]);       \
    if (mArgsInfo.lastKeep##OPTION##_given) {                           \
      param->SetLastKeep(mArgsInfo.lastKeep##OPTION##_arg);             \
      param->UseLastKeepOn();                                           \
    }                                                                   \
    else param->UseLastKeepOff();                                       \
    VerboseOption("firstKeep"#OPTION, mArgsInfo.firstKeep##OPTION##_arg); \
    VerboseOption("lastKeep"#OPTION, mArgsInfo.lastKeep##OPTION##_given, mArgsInfo.lastKeep##OPTION##_arg ); \
    VerboseOptionV("remove"#OPTION, mArgsInfo.remove##OPTION##_given, mArgsInfo.remove##OPTION##_arg); \
    FUNC(param);                                                        \
  }

