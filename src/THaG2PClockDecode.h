#ifndef ROOT_THaG2PClockDecode
#define ROOT_THaG2PClockDecode

//////////////////////////////////////////////////////////////////////////
//
// THaG2PClockDecode
// This class is used to decode clock info event by event
// By Pengjia Zhu,pzhu@jlab.org
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TH1F.h"
#include "THaDetector.h"
#include "TTree.h"
#include "TObject.h"
#include <string>

class THaG2PClockDecode : public THaDetector
{
public:
    THaG2PClockDecode(const char *name, const char *description, THaApparatus *apparatus = NULL);
    virtual ~THaG2PClockDecode();
    virtual Int_t Decode(const THaEvData &evdata);
    virtual void Clear(Option_t *opt = "");
    virtual void MakePrefix();

protected:
    Int_t fEvtype; // Current CODA event type
    Int_t fEvtNum;
    Int_t fRocAddr; //scaler roc address
    Int_t fastclk, fastclk2, slowclk;
    Int_t *fastbusclk; //clock info in fastbus
    Int_t Nfastbus; //how many fastbus crates
    Int_t fFastbusRocAddr[10];
    virtual Int_t DefineVariables(EMode mode = kDefine);
    virtual Int_t ReadDatabase(const TDatime &date);

private:
    ClassDef(THaG2PClockDecode, 1)
};

#endif
