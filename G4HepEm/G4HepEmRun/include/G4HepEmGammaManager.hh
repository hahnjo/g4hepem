
#ifndef G4HepEmGammaManager_HH
#define G4HepEmGammaManager_HH

#include "G4HepEmMacros.hh"

struct G4HepEmData;
struct G4HepEmParameters;
struct G4HepEmGammaData;

class  G4HepEmTLData;
class  G4HepEmGammaTrack;
class  G4HepEmTrack;

/**
 * @file    G4HepEmGammaManager.hh
 * @struct  G4HepEmGammaManager
 * @author  M. Novak
 * @date    2021
 *
 * @brief The top level run-time manager for \f$\gamma\f$ transport simulations.
 *
 * It will be the same for \f$\gamma\f$ as the G4HepEmElectronManager for e-/e+.
 */

class G4HepEmGammaManager {
private:
  G4HepEmGammaManager() = delete;

public:
  // step length
  static void HowFar(struct G4HepEmData* /*hepEmData*/, struct G4HepEmParameters* /*hepEmPars*/, G4HepEmTLData* /*tlData*/);

  G4HepEmHostDevice
  static void HowFar(struct G4HepEmData* hepEmData, struct G4HepEmParameters* hepEmPars, G4HepEmGammaTrack* theTrack);


  // interactions
  static void Perform(struct G4HepEmData* /*hepEmData*/, struct G4HepEmParameters* /*hepEmPars*/, G4HepEmTLData* /*tlData*/);

  G4HepEmHostDevice
  static void UpdateNumIALeft(G4HepEmTrack* theTrack);

  G4HepEmHostDevice
  static double
  GetTotalMacXSec(const struct G4HepEmData* hepEmData, G4HepEmGammaTrack* theGammaTrack);

  G4HepEmHostDevice
  static double GetMacXSecPE(const struct G4HepEmData* hepEmData, const int imat, const double ekin);

  static void SelectInteraction(const struct G4HepEmData* hepEmData, G4HepEmTLData* tlData);

  G4HepEmHostDevice
  static void
  SampleInteraction(const struct G4HepEmData* hepEmData, G4HepEmGammaTrack* theGammaTrack, const double urnd);

};

#endif // G4HepEmGammaManager_HH
