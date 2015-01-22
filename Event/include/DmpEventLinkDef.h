/*
 *  $Id: DmpEventLinkDef.h, 2015-01-23 02:48:47 DAMPE $
 *  Author(s):
 *    Chi WANG (chiwang@mail.ustc.edu.cn) 13/12/2013
*/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all namespaces;
//#pragma link C++ nestedtypedef;

#pragma link C++ namespace DmpEDetectorID;
#pragma link C++ class DmpEvtBgoHits+;
#pragma link C++ typedef DmpEvtPsdHits;
#pragma link C++ class DmpEvtNudHits+;
#pragma link C++ class Cluster+;
#pragma link C++ class RHClass+;
#pragma link C++ class Event+;
#pragma link C++ class DmpEvtBTAnc+;
//#pragma link C++ typedef DmpTriggerStatus;
//#pragma link C++ typedef DmpSubDetStatus;
#pragma link C++ class DmpEvtHeader+;
#pragma link C++ class DmpFeeNavig+;
#pragma link C++ class DmpEvtBgoRaw+;
#pragma link C++ typedef DmpEvtPsdRaw;
#pragma link C++ class DmpEvtNudRaw+;
#pragma link C++ class DmpEvtStkRaw+;
//#pragma link C++ class DmpEvtRec0+;

// common
/*
#pragma link C++ class DmpEvtHeader+;
#pragma link C++ class DmpEvtMCPrimaryParticle+;
// Psd
#pragma link C++ class DmpEvtRdcPsdStrip+;
#pragma link C++ class DmpEvtMCPsdStrip+;
// Stk
#pragma link C++ class DmpEvtRdcStkLadder+;
#pragma link C++ class DmpEvtMCStkStrip+;
// Bgo
#pragma link C++ struct _DmpBgoSignal+;
#pragma link C++ class DmpEvtMCBgoBar+;
#pragma link C++ class DmpEvtBgoShower+;
#pragma link C++ class DmpDataBgoBarPed+;
#pragma link C++ class DmpCalDataBgoMSDMip+;
#pragma link C++ class DmpCalDataBgoMSDRel+;
// Nud
#pragma link C++ class DmpEvtRdcNudBlock+;
#pragma link C++ class DmpEvtMCNudBlock+;
*/

#endif
