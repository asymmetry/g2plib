#ifndef ROOT_THaG2PRasteredBeam
#define ROOT_THaG2PRasteredBeam

//////////////////////////////////////////////////////////////////////////
//
// THaG2PRasteredBeam
//
//////////////////////////////////////////////////////////////////////////

#include "THaBeam.h"

class THaG2PRasteredBeam : public THaBeam
{
public:
    THaG2PRasteredBeam(const char *name, const char *description);

    virtual ~THaG2PRasteredBeam()
    {
    }

    virtual Int_t Reconstruct();

private:
    ClassDef(THaG2PRasteredBeam, 1) // A beam with rastered beam, analyzed event by event using raster currents
};

#endif
