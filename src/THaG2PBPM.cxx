///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THaG2PBPM                                                                 //
//                                                                           //
// Class for a BPM                                                           //
// measuring signals from four antennas                                      //
// and  the position at the BPM (without phase correction)                   //
// works with standard ADCs (lecroy like types)                              //
// is good for unrastered beam, or to get average positions                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THaG2PBPM.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "VarDef.h"
#include "VarType.h"
#include "TMath.h"

ClassImp(THaG2PBPM)

THaG2PBPM::THaG2PBPM(const char* name, const char* description, THaApparatus* apparatus) : THaBeamDet(name, description, apparatus), fRawSignal(4), fPedestals(4), fCorSignal(4), fRotPos(2), fRot2HCSPos(2, 2)
{
    // Constructor
}

//
// ReadDatabase:  if detectors cant be added to detmap
//                or entry for bpms is missing           -> kInitError
//                otherwise                              -> kOk
//                CAUTION: i do not check for incomplete or
//                         inconsistent data
//

Int_t THaG2PBPM::ReadDatabase(const TDatime& date)
{
    static const char* const here = "ReadDatabase()";

    const int LEN = 100;
    char buf[LEN];
    char *filestatus;
    char keyword[LEN];

    FILE* fi = OpenFile(date);
    if (!fi) return kInitError;

    // okay, this needs to be changed, but since i dont want to re- or pre-invent
    // the wheel, i will keep it ugly and read in my own configuration file with
    // a very fixed syntax:
    sprintf(keyword, "[%s_detmap]", GetName());
    Int_t n = strlen(keyword);
    do {
        filestatus = fgets(buf, LEN, fi);
    } while ((filestatus != NULL)&&(strncmp(buf, keyword, n) != 0));
    if (filestatus == NULL) {
        Error(Here(here), "Unexpected end of BPM configuration file");
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
                Error(Here(here), "Couldnt add BPM to DetMap. Good bye, blue sky, good bye!");
                fclose(fi);
                return kInitError;
            }
        }
    } while (first_chan >= 0);

    fclose(fi);
    return kOK;
}

Int_t THaG2PBPM::DefineVariables(EMode mode)
{
    // Initialize global variables and lookup table for decoder

    if (mode == kDefine && fIsSetup) return kOK;
    fIsSetup = (mode == kDefine);

    // Register variables in global list
    RVarDef vars[] = {
        { "rawcur.1", "current in antenna 1", "GetRawSignal0()"},
        { "rawcur.2", "current in antenna 2", "GetRawSignal1()"},
        { "rawcur.3", "current in antenna 3", "GetRawSignal2()"},
        { "rawcur.4", "current in antenna 4", "GetRawSignal3()"},
        { 0}
    };

    return DefineVarsFromList(vars, mode);
}

THaG2PBPM::~THaG2PBPM()
{
    // Destructor. Remove variables from global list.

    if (fIsSetup)
        RemoveVariables();
}

inline void THaG2PBPM::ClearEvent()
{
    // Reset per-event data

    fPosition.SetXYZ(0., 0., -10000.);
    fDirection.SetXYZ(0., 0., 1.);
    fNfired = 0;
    fRawSignal(0) = -1;
    fRawSignal(1) = -1;
    fRawSignal(2) = -1;
    fRawSignal(3) = -1;
    fCorSignal(0) = -1;
    fCorSignal(1) = -1;
    fCorSignal(2) = -1;
    fCorSignal(3) = -1;


}

Int_t THaG2PBPM::Decode(const THaEvData& evdata)
{
    // clears the event structure
    // loops over all modules defined in the detector map
    // copies raw data into local variables
    // performs pedestal subtraction

    ClearEvent();
    for (Int_t i = 0; i < fDetMap->GetSize(); i++) {
        THaDetMap::Module* d = fDetMap->GetModule(i);
        for (Int_t j = 0; j < evdata.GetNumChan(d->crate, d->slot); j++) {
            Int_t chan = evdata.GetNextChan(d->crate, d->slot, j);
            if ((chan >= d->lo)&&(chan <= d->hi)) {
                Int_t data = evdata.GetData(d->crate, d->slot, chan, 0);
                Int_t k = d->first + chan - d->lo - 1;
                if ((k < 4)&&(fRawSignal(k) == -1)) {
                    fRawSignal(k) = data;
                    fNfired++;
                } else {
                    Warning(Here("Decode()"), "Illegal detector channel: %d", k);
                }
            }
        }
    }

    if (fNfired != 4) {
        Warning(Here("Decode()"), "Number of fired Channels out of range. Setting beam position to nominal values");
    } else {
        fCorSignal = fRawSignal;
        fCorSignal -= fPedestals;
    }

    return 0;
}

Int_t THaG2PBPM::Process()
{
    // called by the beam apparaturs
    // uses the pedestal substracted signals from the antennas
    // to get the position in the bpm coordinate system
    // and uses the transformation matrix defined in the database
    // to transform it into the HCS
    // directions are not calculated, they are always set parallel to z
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
