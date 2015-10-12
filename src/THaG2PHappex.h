#ifndef ROOT_THaG2PHappex
#define ROOT_THaG2PHappex

//////////////////////////////////////////////////////////////////////////
//
// THaG2PHappex
// This class is used to decode happex crate information for g2p experiment
// The happex crate for g2p experiment records bpm,bcm,raster,lumi signal,
// triggered by MPS signal from helicity control board
//
// By Pengjia Zhu,pzhu@jlab.org
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "TH1F.h"
#include "THaDetector.h"
#include "TTree.h"
#include "TObject.h"
#include <string>

class THaG2PHappex : public THaDetector {
public:
    THaG2PHappex(const char* name, const char* description, THaApparatus* apparatus = NULL);
    virtual ~THaG2PHappex();
    virtual const char* GetDBFileName() const;
    virtual void MakePrefix();

    virtual Int_t Decode(const THaEvData& evdata);
    virtual void Clear(Option_t* opt = "");

protected:
    Int_t fEvtype; // Current CODA event type
    Int_t fEvtNum;
    virtual Int_t DefineVariables(EMode mode = kDefine);
    virtual Int_t ReadDatabase(const TDatime& date);

    // Used by ReadDatabase()

    struct ROCinfo {
        Int_t roc; // ROC to read out
        UInt_t header; // Headers to search for (0 = ignore)
        Int_t index; // Index into buffer
    } fROCinfoh;
    Int_t fG2PDebug; // Debug level

    //for happex ring buffer

    enum {
        kMAXADC = 10
    };

    enum {
        kNADCCHAN = 4
    };
    Bool_t fHAPPEX;
    Int_t fHAPPEXDepth;
    Int_t fIHAPPEX, fIHAPPEX_read;
    Int_t fNumADCHAPPEX, fNumADCHAPPEX_word;
    std::vector<Int_t> fBCMChan, fBPMChan, fRasterChan;
    Int_t *fADCHAPPEX[kMAXADC * 4];
    Int_t* fQRTHAPPEX, *fHelicityHAPPEX, *fPairHAPPEX;
    Int_t *fbpma_rawcur_1, *fbpma_rawcur_2, *fbpma_rawcur_3, *fbpma_rawcur_4;
    Int_t *fbpmb_rawcur_1, *fbpmb_rawcur_2, *fbpmb_rawcur_3, *fbpmb_rawcur_4;
    Int_t *fraster_rawcur_x, *fraster_rawcur_y, *fslraster_rawcur_x, *fslraster_rawcur_y;
    Int_t* fbcmup, *fbcmdown, *fadchelicity;
    Long_t fbcmupc, fbcmdownc;

    //for happex tree
    Bool_t fHAPPEXTREE;
    TTree *fHappexTree;
    Int_t ringentry;
    Int_t hQRTHAPPEX, hHelicityHAPPEX, hPairHAPPEX;
    Int_t hbpma_rawcur_1, hbpma_rawcur_2, hbpma_rawcur_3, hbpma_rawcur_4;
    Int_t hbpmb_rawcur_1, hbpmb_rawcur_2, hbpmb_rawcur_3, hbpmb_rawcur_4;
    Int_t hraster_rawcur_x, hraster_rawcur_y, hslraster_rawcur_x, hslraster_rawcur_y;
    Int_t hbcmup, hbcmdown, hadchelicity;


private:
    static Int_t FindWord(const THaEvData& evdata, const ROCinfo& info);
    void adc18_decode_data(UInt_t data, UInt_t adcnum);

    ClassDef(THaG2PHappex, 0)
};

#endif