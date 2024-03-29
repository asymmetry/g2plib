#ifndef ROOT_THaG2PBPM
#define ROOT_THaG2PBPM

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaG2PBPM                                                                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaBeamDet.h"
#include "TVector.h"

class THaG2PBPM : public THaBeamDet
{
public:
    THaG2PBPM(const char *name, const char *description = "", THaApparatus *a = NULL);
    virtual ~THaG2PBPM();

    virtual Int_t Decode(const THaEvData &);
    virtual Int_t Process();

    virtual TVector3 GetPosition() const
    {
        return fPosition;
    }

    virtual TVector3 GetDirection() const
    {
        return fDirection;
    }

    Double_t GetRawSignal0()
    {
        return fRawSignal(0);
    }

    Double_t GetRawSignal1()
    {
        return fRawSignal(1);
    }

    Double_t GetRawSignal2()
    {
        return fRawSignal(2);
    }

    Double_t GetRawSignal3()
    {
        return fRawSignal(3);
    }

    Double_t GetRotPosX()
    {
        return fRotPos(0);
    }

    Double_t GetRotPosY()
    {
        return fRotPos(1);
    }

protected:
    void ClearEvent();
    virtual Int_t ReadDatabase(const TDatime &date);
    virtual Int_t DefineVariables(EMode mode = kDefine);

    //  THaG2PBPM() {}
    //  THaG2PBPM( const THaG2PBPM& ) {}

    THaG2PBPM &operator=(const THaG2PBPM &)
    {
        return *this;
    }

    TVector fRawSignal; // induced signal of the antennas
    TVector fPedestals;
    TVector fCorSignal; // pedestal subtracted signal

    TVector fRotPos; // position in the BPM system, arbitrary units

    TMatrix fRot2HCSPos;

    TVector3 fOffset;
    TVector3 fPosition; // Beam position at the BPM (meters)
    TVector3 fDirection; // Beam direction at the BPM
    // always points along z-axis

    Int_t fNfired;
    Double_t fCalibRot;

private:
    ClassDef(THaG2PBPM, 1) // Generic BPM class
};

///////////////////////////////////////////////////////////////////////////////

#endif
