///////////////////////////////////////////////////////////////////////
//
// THaG2PHappex
// By Pengjia Zhu,pzhu@jlab.org
//
////////////////////////////////////////////////////////////////////////

#include "THaG2PHappex.h"
#include "THaEvData.h"
#include "TMath.h"
#include "TError.h"
#include "VarDef.h"
#include "THaAnalysisObject.h"   // For LoadDB
#include <iostream>

using namespace std;

THaG2PHappex::THaG2PHappex(const char* name, const char* description, THaApparatus* apparatus) :
THaDetector(name, description, apparatus),
fEvtype(0), fEvtNum(0), fHAPPEX(kFALSE), fIHAPPEX(0), fIHAPPEX_read(0), fNumADCHAPPEX(0), fNumADCHAPPEX_word(0), fHAPPEXTREE(kFALSE), fHappexTree(NULL)
{
    // Constructor
    //cout<<"happex class constructing..."<<endl;
}

THaG2PHappex::~THaG2PHappex()
{
    //cout << "happex class destructing..." << endl;
    // Destructor

    for (Int_t i = 0; i < kMAXADC * 4; i++) {
        delete[] fADCHAPPEX[i];
        fADCHAPPEX[i] = NULL;
    }

    delete[] fQRTHAPPEX;
    fQRTHAPPEX = NULL;
    delete[] fHelicityHAPPEX;
    fHelicityHAPPEX = NULL;
    delete[] fPairHAPPEX;
    fPairHAPPEX = NULL;
    delete[] fbpma_rawcur_1;
    fbpma_rawcur_1 = NULL;
    delete[] fbpma_rawcur_2;
    fbpma_rawcur_2 = NULL;
    delete[] fbpma_rawcur_3;
    fbpma_rawcur_3 = NULL;
    delete[] fbpma_rawcur_4;
    fbpma_rawcur_4 = NULL;
    delete[] fbpmb_rawcur_1;
    fbpmb_rawcur_1 = NULL;
    delete[] fbpmb_rawcur_2;
    fbpmb_rawcur_2 = NULL;
    delete[] fbpmb_rawcur_3;
    fbpmb_rawcur_3 = NULL;
    delete[] fbpmb_rawcur_4;
    fbpmb_rawcur_4 = NULL;
    delete[] fraster_rawcur_x;
    fraster_rawcur_x = NULL;
    delete[] fraster_rawcur_y;
    fraster_rawcur_y = NULL;
    delete[] fslraster_rawcur_x;
    fslraster_rawcur_x = NULL;
    delete[] fslraster_rawcur_y;
    fslraster_rawcur_y = NULL;
    delete[] fbcmup;
    fbcmup = NULL;
    delete[] fbcmdown;
    fbcmdown = NULL;
    delete[] fadchelicity;
    fadchelicity = NULL;

    //for happex tree
    if (fHappexTree) {
        fHappexTree->Write("", TObject::kOverwrite);
        delete fHappexTree;
    }
}

void THaG2PHappex::Clear(Option_t* opt)
{
    // cout<<"happex clearing..."<<endl;
    THaDetector::Clear(opt);
    fIHAPPEX = 0;
    for (Int_t i = 0; i < kMAXADC * 4; i++)
        for (Int_t j = 0; j < fHAPPEXDepth; j++)
            fADCHAPPEX[i][j] = 0;
    for (Int_t i = 0; i < fHAPPEXDepth; i++) {
        fHelicityHAPPEX[i] = 0;
        fQRTHAPPEX[i] = 0;
        fPairHAPPEX[i] = 0;
        fbpma_rawcur_1[i] = 0;
        fbpma_rawcur_2[i] = 0;
        fbpma_rawcur_3[i] = 0;
        fbpma_rawcur_4[i] = 0;
        fbpmb_rawcur_1[i] = 0;
        fbpmb_rawcur_2[i] = 0;
        fbpmb_rawcur_3[i] = 0;
        fbpmb_rawcur_4[i] = 0;
        fraster_rawcur_x[i] = 0;
        fraster_rawcur_y[i] = 0;
        fslraster_rawcur_x[i] = 0;
        fslraster_rawcur_y[i] = 0;
        fbcmup[i] = 0;
        fbcmdown[i] = 0;
    }
}

Int_t THaG2PHappex::DefineVariables(EMode mode)
{
    //cout<<"happex defining variables..."<<endl;
    // Initialize global variables

    if (mode == kDefine && fIsSetup) return kOK;
    fIsSetup = (mode == kDefine);

    VarDef var[] = {
        { "event", "event number", kInt, 1, &fEvtNum},
        { "ringentry", "entry in happex ring buffer", kInt, 1, &ringentry},
        { "numring", "event amount in HAPPEX DAQ ring buffer", kInt, 1, &fIHAPPEX},
        { "qrt", "qrt recorded in HAPPEX FLEXIO", kInt, 50, fQRTHAPPEX, &fIHAPPEX},
        { "helicity", "Helicity recorded in HAPPEX FLEXIO", kInt, 50, fHelicityHAPPEX, &fIHAPPEX},
        { "pair", "pair sync recorded in HAPPEX FLEXIO", kInt, 50, fPairHAPPEX, &fIHAPPEX},
        { "BPMA.rawcur.1", "bpma rawcur 1 recorded in HAPPEX ADC", kInt, 50, fbpma_rawcur_1, &fIHAPPEX},
        { "BPMA.rawcur.2", "bpma rawcur 2 recorded in HAPPEX ADC", kInt, 50, fbpma_rawcur_2, &fIHAPPEX},
        { "BPMA.rawcur.3", "bpma rawcur 3 recorded in HAPPEX ADC", kInt, 50, fbpma_rawcur_3, &fIHAPPEX},
        { "BPMA.rawcur.4", "bpma rawcur 4 recorded in HAPPEX ADC", kInt, 50, fbpma_rawcur_4, &fIHAPPEX},
        { "BPMB.rawcur.1", "bpmb rawcur 1 recorded in HAPPEX ADC", kInt, 50, fbpmb_rawcur_1, &fIHAPPEX},
        { "BPMB.rawcur.2", "bpmb rawcur 2 recorded in HAPPEX ADC", kInt, 50, fbpmb_rawcur_2, &fIHAPPEX},
        { "BPMB.rawcur.3", "bpmb rawcur 3 recorded in HAPPEX ADC", kInt, 50, fbpmb_rawcur_3, &fIHAPPEX},
        { "BPMB.rawcur.4", "bpmb rawcur 4 recorded in HAPPEX ADC", kInt, 50, fbpmb_rawcur_4, &fIHAPPEX},
        { "Raster.rawcur.x", "rasterx recorded in HAPPEX ADC", kInt, 50, fraster_rawcur_x, &fIHAPPEX},
        { "Raster.rawcur.y", "rastery recorded in HAPPEX ADC", kInt, 50, fraster_rawcur_y, &fIHAPPEX},
        { "Raster.rawcurSL.x", "slrasterx recorded in HAPPEX ADC", kInt, 50, fslraster_rawcur_x, &fIHAPPEX},
        { "Raster.rawcurSL.y", "slrastery recorded in HAPPEX ADC", kInt, 50, fslraster_rawcur_y, &fIHAPPEX},
        { "bcm_up", "upstream bcm recorded in HAPPEX ADC", kInt, 50, fbcmup, &fIHAPPEX},
        { "bcm_down", "downstream bcm recorded in HAPPEX ADC", kInt, 50, fbcmdown, &fIHAPPEX},
        { "adc_helicity", "helicity recorded in HAPPEX ADC", kInt, 50, fadchelicity, &fIHAPPEX},
        { "bcm_upc", "accumulated upstream bcm recorded in HAPPEX ADC", kLong, 1, &fbcmupc,},
        { "bcm_downc", "accumulated downstream bcm recorded in HAPPEX ADC", kLong, 1, &fbcmdownc},
        { 0}
    };

    //for happex tree
    if (fHAPPEXTREE) {
        fHappexTree = new TTree("hapring", "g2p_happex");
        fHappexTree->Branch("event", &fEvtNum, "event/I");
        fHappexTree->Branch("ringentry", &ringentry, "ringentry/I");
        fHappexTree->Branch("qrt", &hQRTHAPPEX, "qrt/I");
        fHappexTree->Branch("helicity", &hHelicityHAPPEX, "helicity/I");
        fHappexTree->Branch("pair", &hPairHAPPEX, "pair/I");
        fHappexTree->Branch("BPMA.rawcur.1", &hbpma_rawcur_1, "BPMA.rawcur.1/I");
        fHappexTree->Branch("BPMA.rawcur.2", &hbpma_rawcur_2, "BPMA.rawcur.2/I");
        fHappexTree->Branch("BPMA.rawcur.3", &hbpma_rawcur_3, "BPMA.rawcur.3/I");
        fHappexTree->Branch("BPMA.rawcur.4", &hbpma_rawcur_4, "BPMA.rawcur.4/I");
        fHappexTree->Branch("BPMB.rawcur.1", &hbpmb_rawcur_1, "BPMB.rawcur.1/I");
        fHappexTree->Branch("BPMB.rawcur.2", &hbpmb_rawcur_2, "BPMB.rawcur.2/I");
        fHappexTree->Branch("BPMB.rawcur.3", &hbpmb_rawcur_3, "BPMB.rawcur.3/I");
        fHappexTree->Branch("BPMB.rawcur.4", &hbpmb_rawcur_4, "BPMB.rawcur.4/I");
        fHappexTree->Branch("Raster.rawcur.x", &hraster_rawcur_x, "Raster.rawcur.x/I");
        fHappexTree->Branch("Raster.rawcur.y", &hraster_rawcur_y, "Raster.rawcur.y/I");
        fHappexTree->Branch("Raster.rawcurSL.x", &hslraster_rawcur_x, "Raster.rawcurSL.x/I");
        fHappexTree->Branch("Raster.rawcurSL.y", &hslraster_rawcur_y, "Raster.rawcurSL.y/I");
        fHappexTree->Branch("bcm_up", &hbcmup, "bcm_up/I");
        fHappexTree->Branch("bcm_down", &hbcmdown, "bcm_down/I");
        fHappexTree->Branch("adc_helicity", &hadchelicity, "adc_helicity/I");
        fHappexTree->Branch("bcm_upc", &fbcmupc, "bcm_upc/L");
        fHappexTree->Branch("bcm_downc", &fbcmdownc, "bcm_downc/L");
    }

    return DefineVarsFromList(var, mode);
}

const char* THaG2PHappex::GetDBFileName() const
{
    //cout<<"happex getting dbfilename..."<<endl;
    // All happex detectors read from db_happex.dat by default. Database
    // keys should (but do not have to) be prefixed by each detector's prefix,
    // e.g. L.adchel.detmap
    return "happex.";
}

void THaG2PHappex::MakePrefix()
{
    //cout<<"happex making prefix..."<<endl;
    // All happex detectors use only their name, not apparatus.name, as
    // prefix.
    THaDetectorBase::MakePrefix(0);
}

Int_t THaG2PHappex::ReadDatabase(const TDatime& date)
{
    //cout<<"happex reading database..."<<endl;
    static const char* const here = "THaG2PHappex::ReadDatabase";
    FILE* file = THaAnalysisObject::OpenFile(GetDBFileName(), date, here, "r", fG2PDebug);
    if (!file)return THaAnalysisObject::kFileError;
    Int_t rocaddr;
    Int_t happexdepth = 0;
    Int_t buildtree = 0;
    DBRequest req[] = {
        { "happexroc", &rocaddr, kInt, 0, 1, -2},
        { "happexdepth", &happexdepth, kInt, 0, 1, -2},
        { "buildtree", &buildtree, kInt, 0, 1, -2},
        { "happex_bcmchan", &fBCMChan, kIntV, 0, 1, -2},
        { "happex_bpmchan", &fBPMChan, kIntV, 0, 1, -2},
        { "happex_rasterchan", &fRasterChan, kIntV, 0, 1, -2},
        { 0}
    };
    Int_t st = THaAnalysisObject::LoadDB(file, date, req, GetPrefix());
    fclose(file);
    if (st)return THaAnalysisObject::kInitError;

    fROCinfoh.roc = rocaddr;
    fROCinfoh.header = 0xbf998300;
    fROCinfoh.index = 0;

    fHAPPEXDepth = happexdepth;
    fHAPPEX = kFALSE;
    fHAPPEXTREE = kFALSE;
    if (fROCinfoh.roc > 0) {
        fHAPPEX = kTRUE;
        fHAPPEXTREE = buildtree > 0;
    }
    //     cout<<fROCinfoh.roc<<endl;
    //     cout<<fBCMChan[0]<<" "<<fBCMChan[1]<<endl;
    //     cout<<fBPMChan[0]<<" "<<fBPMChan[1]<<" "<<fBPMChan[2]<<" "<<fBPMChan[3]<<" "<<fBPMChan[4]<<" "<<fBPMChan[5]<<" "<<fBPMChan[6]<<" "<<fBPMChan[7]<<endl;
    //     cout<<fRasterChan[0]<<" "<<fRasterChan[1]<<" "<<fRasterChan[2]<<" "<<fRasterChan[3]<<endl;
    for (Int_t i = 0; i < kMAXADC * 4; i++) fADCHAPPEX[i] = new Int_t[fHAPPEXDepth];
    fQRTHAPPEX = new Int_t[fHAPPEXDepth];
    fHelicityHAPPEX = new Int_t[fHAPPEXDepth];
    fPairHAPPEX = new Int_t[fHAPPEXDepth];
    fbpma_rawcur_1 = new Int_t[fHAPPEXDepth];
    fbpma_rawcur_2 = new Int_t[fHAPPEXDepth];
    fbpma_rawcur_3 = new Int_t[fHAPPEXDepth];
    fbpma_rawcur_4 = new Int_t[fHAPPEXDepth];
    fbpmb_rawcur_1 = new Int_t[fHAPPEXDepth];
    fbpmb_rawcur_2 = new Int_t[fHAPPEXDepth];
    fbpmb_rawcur_3 = new Int_t[fHAPPEXDepth];
    fbpmb_rawcur_4 = new Int_t[fHAPPEXDepth];
    fraster_rawcur_x = new Int_t[fHAPPEXDepth];
    fraster_rawcur_y = new Int_t[fHAPPEXDepth];
    fslraster_rawcur_x = new Int_t[fHAPPEXDepth];
    fslraster_rawcur_y = new Int_t[fHAPPEXDepth];
    fbcmup = new Int_t[fHAPPEXDepth];
    fbcmdown = new Int_t[fHAPPEXDepth];
    fadchelicity = new Int_t[fHAPPEXDepth];
    fbcmupc = 0;
    fbcmdownc = 0;
    ringentry = 0;

    return THaAnalysisObject::kOK;
}

Int_t THaG2PHappex::FindWord(const THaEvData& evdata, const ROCinfo& info)
{
    // find the index of the word we are looking for given a header
    // or simply return the index already stored in ROC info (in that case
    // the header stored in ROC info needs to be 0
    // cout<<"happex finding word"<<endl;
    Int_t len = evdata.GetRocLength(info.roc);
    if (len <= 4)
        return -1;

    Int_t i;
    if (info.header == 0)
        i = info.index;
    else {
        for (i = 0; i < len && ((evdata.GetRawData(info.roc, i) & info.header) != info.header); ++i);
        i += info.index;
        i++;
    }

    return (i < len) ? i : -1;
}

Int_t THaG2PHappex::Decode(const THaEvData& evdata)
{
    //cout<<"happex decoding..."<<endl;
    static const char* here = "THaG2PHappex::Decode";

    if (!fHAPPEX) {
        ::Error(here, "ROC data (detector map) not properly set up ");
        return -1;
    }
    fEvtype = evdata.GetEvType();
    fEvtNum = evdata.GetEvNum();

    Int_t index = 0;

    if (fHAPPEX)
        index = FindWord(evdata, fROCinfoh);
    else
        index = -1;

    if (index >= 0) {
        index = FindWord(evdata, fROCinfoh);
        Int_t happexroc = fROCinfoh.roc;
        Int_t hlen = evdata.GetRocLength(happexroc);
        Int_t hdata, hdata1;
        hdata = evdata.GetRawData(happexroc, index++);
        if (((hdata & 0xbf1ff000) == 0xbf1ff000)&&((hdata & 0xffffffff) != 0xffffffff)) {
            fIHAPPEX = hdata & 0xfff;
        } else {
            fIHAPPEX = 0;
        }
        for (Int_t i = index; i < hlen; i++) {
            hdata = evdata.GetRawData(happexroc, i);
            if ((hdata & 0xbfead000) == 0xbfead000 && ((hdata & 0xffffffff) != 0xffffffff)) {
                fIHAPPEX_read = hdata & 0xfff;
            }
            if ((hdata & 0xbffec000) == 0xbffec000 && ((hdata & 0xffffffff) != 0xffffffff)) {
                hdata1 = evdata.GetRawData(happexroc, i + 1);
                fPairHAPPEX[fIHAPPEX_read] = hdata1 & 1;
                fHelicityHAPPEX[fIHAPPEX_read] = (hdata1 & 2) >> 1;
                fQRTHAPPEX[fIHAPPEX_read] = (hdata1 & 4) >> 2;
            }
            if ((hdata & 0xbfadc000) == 0xbfadc000 && ((hdata & 0xffffffff) != 0xffffffff)) {
                fNumADCHAPPEX = hdata & 0xf;
                fNumADCHAPPEX_word = (hdata & 0xf0) >> 4;
                for (Int_t j = 0; j < fNumADCHAPPEX; j++) {
                    for (Int_t k = i + 2 + j * fNumADCHAPPEX_word; k < i + 2 + (j + 1) * fNumADCHAPPEX_word; k++) {
                        hdata1 = evdata.GetRawData(happexroc, k);
                        adc18_decode_data((UInt_t) hdata1, j);
                    }
                }
            }
        }
    }

    for (Int_t i = 0; i < fIHAPPEX; i++) {
        fbpma_rawcur_1[i] = fADCHAPPEX[fBPMChan[0]][i];
        fbpma_rawcur_2[i] = fADCHAPPEX[fBPMChan[1]][i];
        fbpma_rawcur_3[i] = fADCHAPPEX[fBPMChan[2]][i];
        fbpma_rawcur_4[i] = fADCHAPPEX[fBPMChan[3]][i];
        fbpmb_rawcur_1[i] = fADCHAPPEX[fBPMChan[4]][i];
        fbpmb_rawcur_2[i] = fADCHAPPEX[fBPMChan[5]][i];
        fbpmb_rawcur_3[i] = fADCHAPPEX[fBPMChan[6]][i];
        fbpmb_rawcur_4[i] = fADCHAPPEX[fBPMChan[7]][i];
        fraster_rawcur_x[i] = fADCHAPPEX[fRasterChan[0]][i];
        fraster_rawcur_y[i] = fADCHAPPEX[fRasterChan[1]][i];
        fslraster_rawcur_x[i] = fADCHAPPEX[fRasterChan[2]][i];
        fslraster_rawcur_y[i] = fADCHAPPEX[fRasterChan[3]][i];
        fbcmup[i] = fADCHAPPEX[fBCMChan[0]][i];
        fbcmdown[i] = fADCHAPPEX[fBCMChan[1]][i];
        fadchelicity[i] = fADCHAPPEX[fBCMChan[2]][i];
        fbcmupc += fADCHAPPEX[fBCMChan[0]][i];
        fbcmdownc += fADCHAPPEX[fBCMChan[1]][i];
        //for happex tree
        if (fHAPPEXTREE) {
            hQRTHAPPEX = fQRTHAPPEX[i];
            hHelicityHAPPEX = fHelicityHAPPEX[i];
            hPairHAPPEX = fPairHAPPEX[i];
            hbpma_rawcur_1 = fbpma_rawcur_1[i];
            hbpma_rawcur_2 = fbpma_rawcur_2[i];
            hbpma_rawcur_3 = fbpma_rawcur_3[i];
            hbpma_rawcur_4 = fbpma_rawcur_4[i];
            hbpmb_rawcur_1 = fbpmb_rawcur_1[i];
            hbpmb_rawcur_2 = fbpmb_rawcur_2[i];
            hbpmb_rawcur_3 = fbpmb_rawcur_3[i];
            hbpmb_rawcur_4 = fbpmb_rawcur_4[i];
            hraster_rawcur_x = fraster_rawcur_x[i];
            hraster_rawcur_y = fraster_rawcur_y[i];
            hslraster_rawcur_x = fslraster_rawcur_x[i];
            hslraster_rawcur_y = fslraster_rawcur_y[i];
            hbcmup = fbcmup[i];
            hbcmdown = fbcmdown[i];
            hadchelicity = fadchelicity[i];
            fHappexTree->Fill();
        }
        ringentry++;
    }

    return 0;
}

void THaG2PHappex::adc18_decode_data(UInt_t data, UInt_t adcnum)
{
    UInt_t module_id, event_number;
    UInt_t ch_number, divider, div_n, data_type;
    UInt_t diff_value;
    Int_t sign, difference;
    Int_t ii;
    Double_t diff_avg;

    if (data & 0x80000000) { //header
        module_id = (0x1F)&(data >> 26);
        event_number = data & 0x3FFFFFF;
    } else { //data
        ch_number = (0x3)&(data >> 29);
        div_n = ((0x3)&(data >> 25));
        divider = 1;
        for (ii = 0; ii < (Int_t) div_n; ii++)
            divider = divider * 2;
        data_type = (0x7)&(data >> 22);
        if (data_type == 0) {
            diff_value = (0x1FFFFF) & data;
            if (data & 0x200000) {
                sign = -1;
                difference = sign * ((~diff_value & 0x1FFFFF) + 1); // 2's complement
            } else {
                sign = 1;
                difference = diff_value;
            }
            diff_avg = ((Float_t) difference) / ((Float_t) divider);
            if (ch_number >= 0 && ch_number < kNADCCHAN)
                fADCHAPPEX[ch_number + adcnum * 4][fIHAPPEX_read] = Int_t(diff_avg);
        }
    }
}

ClassImp(THaG2PHappex)
