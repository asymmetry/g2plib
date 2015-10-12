//////////////////////////////////////////////////////////////////////////
//
// THaG2PClockDecode
// By Pengjia Zhu,pzhu@jlab.org
//
//////////////////////////////////////////////////////////////////////////

#include "THaG2PClockDecode.h"
#include "THaEvData.h"
#include "TMath.h"
#include "TError.h"
#include "VarDef.h"
#include "THaAnalysisObject.h"   // For LoadDB
#include <iostream>
#include <string>
#define isDigital(x) (((x)>='0') && ((x) <= '9'))

using namespace std;

THaG2PClockDecode::THaG2PClockDecode(const char* name, const char* description, THaApparatus* apparatus) : THaDetector(name, description, apparatus), fEvtype(0), fEvtNum(0)
{
    //Constructor

    Nfastbus = 0;
    char* s = Form("%s", description);
    char *ssp = strtok(s, " ,");
    while (ssp != NULL) {
        if (isDigital(string(ssp))) {
            fFastbusRocAddr[Nfastbus] = atoi(ssp);
            Nfastbus++;
        }
        ssp = strtok(NULL, " ,");
    }
    fastbusclk = new Int_t[Nfastbus];
}

THaG2PClockDecode::~THaG2PClockDecode()
{
    // Destructor
    delete[] fastbusclk;
}

void THaG2PClockDecode::Clear(Option_t* opt)
{
    fastclk = 0;
    fastclk2 = 0;
    slowclk = 0;
    for (Int_t i = 0; i < Nfastbus; i++) fastbusclk[i] = 0;
}

Int_t THaG2PClockDecode::DefineVariables(EMode mode)
{
    if (mode == kDefine && fIsSetup) return kOK;
    fIsSetup = (mode == kDefine);
    VarDef var[] = {
        {"fastclk", "fastclk from sis3800", kInt, 1, &fastclk},
        {"fastclk2", "fastclk from sis3800,should be same as fastclk", kInt, 1, &fastclk2},
        {"slowclk", "slowclk from sis3800", kInt, 1, &slowclk},
        {"Nfastbusclk", "fastbus crate amount", kInt, 1, &Nfastbus},
        {"fastbusclk", "clock signal in fastbus(vxTicks)", kInt, 10, fastbusclk, &Nfastbus},
        { 0}
    };
    return DefineVarsFromList(var, mode);
}

Int_t THaG2PClockDecode::ReadDatabase(const TDatime& date)
{
    //static const char* const here = "THaG2PClockDecode::ReadDatabase";
    char arm = GetPrefix()[0];
    if (arm == "L"[0]) fRocAddr = 11;
    else fRocAddr = 10;
    return THaAnalysisObject::kOK;
}

void THaG2PClockDecode::MakePrefix()
{
    THaDetectorBase::MakePrefix(0);
}

Int_t THaG2PClockDecode::Decode(const THaEvData& evdata)
{
    //static const char* here = "THaG2PClockDecode::Decode";
    fEvtype = evdata.GetEvType();
    fEvtNum = evdata.GetEvNum();
    //clock in scaler
    Int_t len = evdata.GetRocLength(fRocAddr);
    if (len < 6) return 0;
    fastclk = (Long_t) evdata.GetRawData(fRocAddr, 4);
    Int_t data1 = 0;
    Int_t data2 = 0;
    Int_t data3 = 0;

    for (Int_t i = 5; i < len; i++) {
        data3 = data2;
        data2 = data1;
        data1 = evdata.GetRawData(fRocAddr, i);
        if ((data1 & 0xcafe0000) == 0xcafe0000 && (data1 & 0xffffffff) != 0xffffffff) {
            fastclk2 = data2;
            slowclk = data3;
            break;
        }
    }

    //clock in fastbus
    for (Int_t i = 0; i < Nfastbus; i++) {
        len = evdata.GetRocLength(fFastbusRocAddr[i]);
        for (Int_t j = 1; j < len; j++) {
            data1 = evdata.GetRawData(fFastbusRocAddr[i], j);
            if ((data1 & 0xfabc0000) == 0xfabc0000 && (data1 & 0xffffffff) != 0xffffffff) {
                fastbusclk[i] = evdata.GetRawData(fFastbusRocAddr[i], j + 8);
                break;
            }
        }
    }
    //     cout<<hex<<fastclk<<" "<<fastclk2<<" "<<slowclk<<" ";
    //     for(Int_t i=0;i<Nfastbus;i++) cout<<hex<<fastbusclk[i]<<" ";
    //     cout<<endl;
    return 0;
}

ClassImp(THaG2PClockDecode)