/**
 *  \file       example/cc/Filter1.cc
 *  \brief      \$Id: Filter1.cc 507 2009-11-28 15:18:46Z dev1usr $
 *  
 *  (C) 2009 - NHI - #1 - Project - Group
 *  
 *  \version    \$Rev: 507 $
 *  \author     EMail: aotto1968 at users.sourceforge.net
 *  \attention  this software has GPL permissions to copy
 *              please contact AUTHORS for additional information
 */

#include <vector>
#include <string>
#include "ccmsgque.h"
#include "debug.h"

using namespace std;
using namespace ccmsgque;

class Filter1 : public MqC, public IFilterFTR, public IFilterEOF {

    typedef vector<MQ_CST> MQ_ROW;
    typedef vector<MQ_ROW> MQ_TAB;
    MQ_TAB data;

    // service definition
    void fFTR () {
      MQ_ROW d;
      MQ_BUF buf = GetTempBuffer();
      while (ReadItemExists()) {
	d.push_back(mq_strdup(MqBufferSetV (buf, "<%s>", ReadC())->cur.C));
      }
      data.push_back(d);
    }

    // service definition
    void fEOF () {
      MQ_TAB::iterator row;
      MQ_ROW::iterator itm;
      for (row = data.begin(); row < data.end(); row++ ) {
	SendSTART();
	for (itm = (*row).begin(); itm < (*row).end(); itm++) {
	  SendC(*itm);
	  free((void*)*itm);
	}
	SendFTR();
      }
    }
};

/*****************************************************************************/
/*                                                                           */
/*                               M A I N                                     */
/*                                                                           */
/*****************************************************************************/

int MQ_CDECL main (int argc, MQ_CST argv[])
{
  Filter1 filter;
  try {
    filter.LinkCreateVC (argc, argv);
    filter.ProcessEvent ();
  } catch (const exception& e) {
    filter.ErrorSet(e);
  }
  filter.Exit ();
}
