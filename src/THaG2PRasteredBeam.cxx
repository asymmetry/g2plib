//*-- Author :    Bodo Reitz April 2003

//////////////////////////////////////////////////////////////////////////
//
// THaG2PRasteredBeam
//
// Apparatus describing an rastered beam.
//
//////////////////////////////////////////////////////////////////////////

#include "THaG2PRasteredBeam.h"
#include "THaG2PRaster.h"
#include "TMath.h"
#include "TDatime.h"
#include "TList.h"

#include "VarDef.h"

ClassImp(THaG2PRasteredBeam)

THaG2PRasteredBeam::THaG2PRasteredBeam(const char *name, const char *description) : THaBeam(name, description)
{
    AddDetector(new THaG2PRaster("Raster", "raster", this));
}

Int_t THaG2PRasteredBeam::Reconstruct()
{
    return 0;
}
