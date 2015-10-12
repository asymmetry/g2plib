#ifndef ROOT_THaG2PRaster
#define ROOT_THaG2PRaster

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaG2PRaster                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaBeamDet.h"
#include "TVector.h"

class THaG2PRaster : public THaBeamDet
{
public:
    THaG2PRaster(const char *name, const char *description = "", THaApparatus *a = NULL);
    virtual ~THaG2PRaster();

    virtual Int_t Decode(const THaEvData &);
    virtual Int_t Process();

    virtual TVector3 GetPosition() const
    {
        return fPosition[0];
    }

    virtual TVector3 GetDirection() const
    {
        return fPosition[1];
    }
    // as soon as someone finds a better solution, he should change the following
    // lines, it is kind of ridiculus to have nine methodes to get the the components
    // of the beam position at various locations, but i do not know how else to
    // get them into histograms, besides i would write my own event loop

    Double_t GetRawPosX()
    {
        return fRawPos(0);
    }

    Double_t GetRawPosY()
    {
        return fRawPos(1);
    }

    Double_t GetRawPosSLX()
    {
        return fSLRawPos(0);
    }

    Double_t GetRawPosSLY()
    {
        return fSLRawPos(1);
    }

    Double_t GetPosBPMAX()
    {
        return fPosition[0](0);
    }

    Double_t GetPosBPMAY()
    {
        return fPosition[0](1);
    }

    Double_t GetPosBPMBX()
    {
        return fPosition[1](0);
    }

    Double_t GetPosBPMBY()
    {
        return fPosition[1](1);
    }

protected:
    void ClearEvent();
    virtual Int_t ReadDatabase(const TDatime &date);
    virtual Int_t DefineVariables(EMode mode = kDefine);

    THaG2PRaster &operator=(const THaG2PRaster &)
    {
        return *this;
    }

    TVector fSLRawPos; // current in Fast Raster ADCs for position
    TVector fRawPos; // current in Slow Raster ADCs for position

    TVector3 fPosition[2]; // Beam position at 1st, 2nd BPM or at the target (meters)
    TVector3 fDirection; // Beam angle at the target (meters)

    TMatrix fRaw2Pos[2];
    TMatrix fSLRaw2Pos[2];
    TVector3 fPosOff[2];
    TVector3 fSLPosOff[2];

    TVector fRasterFreq;
    TVector fSLRasterFreq;
    TVector fSlopePedestal;
    TVector fRasterPedestal;
    TVector fSLRasterPedestal;

    Int_t fNfired;
    Int_t fRasterLinear;
    Int_t fSLRasterLinear;

private:
    ClassDef(THaG2PRaster, 1) // Generic Raster class
};

////////////////////////////////////////////////////////////////////////////////

#endif
