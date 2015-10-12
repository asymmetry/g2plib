///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaG2PRaster                                                              //
//                                                                           //
// Class for a beam raster device                                            //
// measuring two magnet currents of fastraster and slowraster                //
// which are proportional to the horizontal/vertical beam displacement       //
// the two planes are assumed to be decoupled                                //
// there is no phase shift between the current and the actual beam position  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaG2PRaster.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "VarDef.h"
#include "VarType.h"
#include "TMath.h"
#include <iostream>

using namespace std;

THaG2PRaster::THaG2PRaster(const char *name, const char *description, THaApparatus *apparatus) : THaBeamDet(name, description, apparatus), fSLRawPos(2), fRawPos(2), fRasterFreq(2), fSLRasterFreq(2), fSlopePedestal(2), fRasterPedestal(2), fSLRasterPedestal(2)
{
    // Constructor

    fRaw2Pos[0].ResizeTo(3, 2);
    fRaw2Pos[1].ResizeTo(3, 2);
    //  fRaw2Pos[2].ResizeTo(3,2);
    fSLRaw2Pos[0].ResizeTo(3, 2);
    fSLRaw2Pos[1].ResizeTo(3, 2);
    //  fSLRaw2Pos[2].ResizeTo(3,2);
}

// ReadDatabase:  if detectors cant be added to detmap
//                or entry for bpms is missing           -> kInitError
//                otherwise                              -> kOk
//                CAUTION: i do not check for incomplete or
//                         inconsistent data
//

Int_t THaG2PRaster::ReadDatabase(const TDatime &date)
{
    static const char *const here = "ReadDatabase()";

    const int LEN = 100;
    char buf[LEN];
    char *filestatus;
    char keyword[LEN];

    FILE *fi = OpenFile(date);

    if (!fi) return kFileError;

    // okay, this needs to be changed, but since i dont want to re- or pre-invent
    // the wheel, i will keep it ugly and read in my own configuration file with
    // a very fixed syntax:

    // Seek our detmap section (e.g. "Raster_detmap")
    sprintf(keyword, "[%s_detmap]", GetName());
    Int_t n = strlen(keyword);

    do {
        filestatus = fgets(buf, LEN, fi);
    } while ((filestatus != NULL) && (strncmp(buf, keyword, n) != 0));

    if (filestatus == NULL) {
        Error(Here("ReadDataBase()"), "Unexpected end of raster configuration file");
        fclose(fi);
        return kInitError;
    }

    // again that is not really nice, but since it will be changed anyhow:
    // i dont check each time for end of file, needs to be improved

    fDetMap->Clear();
    int first_chan, crate, dummy, slot, first, last, modulid;

    do {
        fgets(buf, LEN, fi);
        sscanf(buf, "%d %d %d %d %d %d %d", &first_chan, &crate, &dummy, &slot, &first, &last, &modulid);

        if (first_chan >= 0) {
            if (fDetMap->AddModule(crate, slot, first, last, first_chan) < 0) {
                Error(Here(here), "Couldnt add Raster to DetMap. Good bye, blue sky, good bye!");
                fclose(fi);
                return kInitError;
            }
        }
    } while (first_chan >= 0);

    fclose(fi);
    return kOK;
}

Int_t THaG2PRaster::DefineVariables(EMode mode)
{
    // Initialize global variables and lookup table for decoder

    if (mode == kDefine && fIsSetup) return kOK;

    fIsSetup = (mode == kDefine);

    // Register variables in global list

    RVarDef vars[] = {
        { "rawcur.x", "current in horizontal raster", "GetRawPosX()"},
        { "rawcur.y", "current in vertical raster", "GetRawPosY()"},
        { "rawcurSL.x", "derivative of current in horizontal raster", "GetRawPosSLX()"},
        { "rawcurSL.y", "derivative of current in vertical raster", "GetRawPosSLY()"},
        { 0}
    };

    return DefineVarsFromList(vars, mode);
}

THaG2PRaster::~THaG2PRaster()
{
    // Destructor. Remove variables from global list.

    if (fIsSetup)
        RemoveVariables();
}

inline void THaG2PRaster::ClearEvent()
{
    // Reset per-event data.

    fRawPos(0) = -1;
    fRawPos(1) = -1;
    fSLRawPos(0) = -1;
    fSLRawPos(1) = -1;
    fPosition[0].SetXYZ(0., 0., -10000.);
    fPosition[1].SetXYZ(0., 0., -10000.);
    //  fPosition[2].SetXYZ(0.,0.,-10000.);
    fNfired = 0;
}

Int_t THaG2PRaster::Decode(const THaEvData &evdata)
{
    // clears the event structure
    // loops over all modules defined in the detector map
    // copies raw data into local variables
    // pedestal subtraction is not foreseen for the raster

    ClearEvent();

    UInt_t chancnt = 0;

    for (Int_t i = 0; i < fDetMap->GetSize(); i++) {
        THaDetMap::Module *d = fDetMap->GetModule(i);

        for (Int_t j = 0; j < evdata.GetNumChan(d->crate, d->slot); j++) {
            Int_t chan = evdata.GetNextChan(d->crate, d->slot, j);

            if ((chan >= d->lo) && (chan <= d->hi)) {
                Int_t data = evdata.GetData(d->crate, d->slot, chan, 0);
                Int_t k = chancnt + d->first + chan - d->lo - 1;

                if (k < 2) {
                    fRawPos(k) = data;
                    fNfired++;
                } else if (k < 4) {
                    fSLRawPos(k - 2) = data;
                    fNfired++;
                } else
                    Warning(Here("Decode()"), "Illegal detector channel: %d", k);
            }
        }

        chancnt += d->hi - d->lo + 1;
    }

    if (fNfired != 4)
        Warning(Here("Decode()"), "Number of fired Channels out of range. Setting beam position to nominal values");

    return 0;
}

Int_t THaG2PRaster::Process()
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
ClassImp(THaG2PRaster)
