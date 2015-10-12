//*-- Author :    Bodo Reitz April 2003

//////////////////////////////////////////////////////////////////////////
//
// THaG2PUnRasteredBeam
//
// Apparatus describing an unrastered beam.
//      can also be used if one only is interested
//      in average beam position for a rastered beam
//      but in that latter case the event by event ext-target
//      corrections need to be skipped
//     (e.g. using an idealbeam for the hrs reconstruction and
//      unrastered beam to fill global variables of the beamline)
//
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "THaG2PUnRasteredBeam.h"
#include "THaG2PBPM.h"
#include "TMath.h"
#include "TDatime.h"
#include "VarDef.h"
#include "TVector.h"
#include "TList.h"

using namespace std;

ClassImp(THaG2PUnRasteredBeam)

THaG2PUnRasteredBeam::THaG2PUnRasteredBeam(const char* name, const char* description, Int_t runningsum_depth) : THaBeam(name, description), fRunningSumDepth(runningsum_depth)
{
    AddDetector(new THaG2PBPM("BPMA", "1st bpm", this));
    AddDetector(new THaG2PBPM("BPMB", "2nd bpm", this));
}

Int_t THaG2PUnRasteredBeam::Reconstruct()
{
    return 0;
}

void THaG2PUnRasteredBeam::ClearRunningSum()
{
    fRunningSumNext = 0;
    fRunningSumWrap = false;
}
