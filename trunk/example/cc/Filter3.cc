/**
 *  \file       example/cc/Filter3.cc
 *  \brief      \$Id: Filter3.cc 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <vector>
#include <stdexcept>
#include "ccmsgque.h"

using namespace std;
using namespace ccmsgque;

class Filter3 : public MqC, public IFilterEOF {
    void fEOF () {
      throw std::runtime_error("my C++ error");
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Filter3 filter;
  try {
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent ();
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit();
}
