#ifndef ROOT_THaG2PUnRasteredBeam
#define ROOT_THaG2PUnRasteredBeam

//////////////////////////////////////////////////////////////////////////
//
// THaG2PUnRasteredBeam
//
//////////////////////////////////////////////////////////////////////////

#include "THaBeam.h"
#include <vector>

class THaG2PUnRasteredBeam : public THaBeam {
public:
    THaG2PUnRasteredBeam(const char* name, const char* description, Int_t runningsum_depth = 0);

    virtual ~THaG2PUnRasteredBeam()
    {
    }

    virtual Int_t Reconstruct();

    void ClearRunningSum();

    Int_t fRunningSumDepth;

protected:
    Bool_t fRunningSumWrap;
    Int_t fRunningSumNext;

private:
    ClassDef(THaG2PUnRasteredBeam, 0) // A beam with unrastered beam, analyzed event by event
};

#endif

