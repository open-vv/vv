#ifndef CLITKIO_H
#define CLITKIO_H
/**
   -------------------------------------------------------------------
   * @file   clitkIO.h
   * @author David Sarrut <David.Sarrut@creatis.insa-lyon.fr>
   * @date   17 May 2006 07:57:56

   * @brief  

   -------------------------------------------------------------------*/

//--------------------------------------------------------------------
// CLITK_INIT
#define CLITK_INIT clitk::RegisterClitkFactories();

namespace clitk {
  void RegisterClitkFactories();
}

#endif /* end #define CLITKIO_H */

