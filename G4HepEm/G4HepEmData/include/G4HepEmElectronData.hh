
#ifndef G4HepEmElectronData_HH
#define G4HepEmElectronData_HH

/**
 * @file    G4HepEmElectronData.hh
 * @struct  G4HepEmElectronData
 * @author  M. Novak
 * @date    2020
 *
 * @brief All energy loss process related data used for \f$e^-/e+\f$ simulations by `G4HepEm`.
 *
 * Energy loss processes are the *Ionisation* and *Bremsstrahlung* interactions
 * described by using the **condensed history** approach. It means, that *sub-threshold*
 * *interactions* are modelled as **continous energy losses** along the particle steps
 * while *super-threshold interactions*, i.e. **generation of secondary** \f$e^-/\gamma\f$
 * *particles* in case of ionisation/bremsstrahlung with intitial energy above the
 * secondary production threshold, are modelled explicitly as point like, **discrete**
 * **interactions**. This data structure contains all the data required to account these
 * interactions at run-time.
 *
 * The **continous energy loss** is characterised by the **restricted stopping power**, which
 * is the mean value of the energy losses due to the sub-threshold interactions
 * along a unit step lenght, and other related quantities such as the corresponding
 * **restricted range** or **restricted inverse range** values. The
 * *restricted* (ionisation - electronic)/(bremsstrahlung - radiative) *stopping power*
 * depend on the primary particle type, kinetic energy, target material and secondary
 * \f$e^-\f$(electronic)/\f$\gamma\f$(radiative) production threshold values. Therefore,
 * such tables are built separately \f$e^-\f$ and \f$e^+\f$, over a wide enough range of
 * primary particle kinetic energy (100 [eV] - 100 [TeV] by default) for all
 * material and secondary prodcution threshold pairs, or according to the Geant4 terminology,
 * for all material - cuts couples. The tables contain the *sum of the electronic and radiative*
 * contributions. These tables, stored in this data structure,
 * are **used to determine the continous** part of the **step limit** and **to compute the**
 * sub-threshold realted, **continous energy losses** at each step done by \f$e^-/e^+\f$.
 *
 * The rate of the **dicrete** super-threshold ionisation and bremsstrahlung
 * interactions are characterised by the corresponding **restricted macroscopic cross sections**.
 * These also depend on the primary particle type, kinetic energy, target material -cuts
 * couple. Moreover, the minimum value of the kinetic energy grid is determined by the
 * secondary \f$e^-\f$(ionisation)\f$/\gamma\f$(bremsstrahlung) production energy thresholds
 * that (since Geant4 rquires the user to specify these in lenght) is diffrent in case of
 * each material-cuts couple whenever wither the material or the cut value (in length)
 * is different. Therefore, these *restricted macroscopic cross sction* tables are
 * *built* separately for \f$e^-/e^+\f$ primary particles, separately for *ionisation*
 * and *bremsstrahlung* for all different material - cuts couples with individual
 * kinetic energy grids. These tables are **used to determine the discrete** part of the
 * **step limit**, i.e. the length that the primary \f$e^-/e^+\f$ travels till the next
 * ionisation/bremsstrahlung interaction in the given material - cuts couple,
 * resulting in secondary \f$e^-/\gamma\f$ particle production with initial energy
 * above the secondary \f$e^-/\gamma\f$ production cut in the given material - cuts
 * couple.
 *
 * The *macroscopic cross section* determines the (mean) path length, the primary
 * particles travels, till the next *discrete* interaction in the given *material*
 * (actually material - cuts couple in case of ioni. and brem. since we use the
 * condensed history approach).
 * The *discrete* bremsstrahlung interaction takes place in the vicinity of one
 * of the *elements of the material*. A so-called **target atom selector** is
 * constructed for each model (also for ionisation though its not used), based
 * on the partial contribution of the individual elements of a given
 * material - cuts to the corresponding macroscopic cross section. These data
 * are **used to select the target atom for discrete interaction** at run-time.
 *
 *
 *
 * @note
 * Other interactions, beyond ionisation and bremsstrahlung, are also active
 * in case of \f$e^-/e^+\f$ like Coulomb scattering or annihilation into two
 * \f$\gamma\f$-s in case of \f$e^+\f$. However, their descriptions are rather
 * different compared to these two energy loss processes:
 *    - Coulomb scattering is described by a so-called *multiple scattering model*
 *    - while \f$e^+\f$ annihilation is a discrete process, so similar to the discrete
 *      ionisation and bremsstrahlung interactions, the corresponding cross section
 *      doesn't depend neither the secondary production thresholds nor the element
 *      composition (directly). Unlike ionisation or even more bremsstrahlung, the
 *      cross section for annihilation can be easily computed on-the-fly so there
 *      is no need to pre-compute and store values in tables.
 *
 */

struct G4HepEmElectronData {
  /** Number of G4HepEm material - cuts: number of G4HepEmMCCData structures stored in the G4HepEmMatCutData::fMatCutData array. */
  int        fNumMatCuts;

//// === ENERGY LOSS DATA
  /**
   * @name Energy loss related data members:
   * These members are used to store all continuous energy loss related data (there
   * is a single primary kinetic energy grid for all material - cuts equally spaced
   * in log-scale).
   */
///@{
  /** Number of discrete kinetic energy values in the grid (\f$N\f$). */
  int        fELossEnergyGridSize;
  /** Logarithm of the minimum kinetic energy value of the grid (\f$\ln(E_0)\f$)*/
  double     fELossLogMinEkin;     // log of the E_0
  /** Inverse of the log-scale delta value (\f$ 1/[log(E_{N-1}/E_0)/(N-1)]\f$). */
  double     fELossEILDelta;
  /** The grid of the discrete kinetic energy values (\f$E_0, E_1,\ldots, E_{N-1}\f$).*/
  double*    fELossEnergyGrid;
  /** The energy loss data: **restricted dE/dx, range and inverse range** data.
    *
    * The restricted dE/dx, range (and corresponding inverse range) data values,
    * over the above kinetic energy grid for all material - cuts couples, are
    * stored continuously in this G4HepEmElectronData::fELossData single array.
    * The second derivative values, required for the run-time spline interpolation,
    * are also stored together with the data.
    *
    * The data are stored in the following format for each of the G4HepEmMCCData material - cuts couples,
    * stored in the G4HepEmMatCutData::fMatCutData array:
    *   - for each material - cuts couple, there are \f$N := \f$ G4HepEmElectronData::fELossEnergyGridSize **range** values associated to the primary
    *     \f$e^-/e^+\f$ kinetic energy values stored in G4HepEmElectronData::fELossEnergyGrid. These
    *     \f$R_i, i=0,\ldots,N-1\f$ **range values are stored**
    *     with the corresponding \f$R_i^{''}, i=0,\ldots,N-1\f$
    *     **second derivatives** in the form of \f$R_0,R_0^{''},R_{1},R_{1}^{''},\ldots,R_{N-1},R_{N-1}^{''}\f$
    *     in order **to resonate the best with the run-time access pattern**.
    *   - **then the corresponding** \f$N\f$, \f$dE/dx\f$ **values are stored** in
    *     a similar way  \f$dE/dx_0,dE/dx_0^{''},dE/dx_{1},dE/dx_{1}^{''},\ldots,dE/dx_{N-1},dE/dx_{N-1}^{''}\f$
    *   - since both the range, and the kinetic energy values
    *     are already stored (see above), **only** the corresponding \f$N\f$
    *     **second derivative values associated to the inverse range are stored then** as
    *     \f$S_0,S_1,\ldots,S_{N-1}\f$
    *   - it means, that **there are** \f$5\times N\f$ **energy loss realted data values stored continuously in
    *     the** G4HepEmElectronData::fELossData **array for each material - cuts couples**. Therefore, in the case
    *     of a G4HepEmMCCData material - cuts couple data with the index of \f$\texttt{imc}\f$ (i.e. in the case of
    *     the G4HepEmMCCData, stored at G4HepEmMatCutData::fMatCutData[\f$\texttt{imc}\f$]), the start indices of
    *     the corresponding energy loss related data in the G4HepEmElectronData::fELossData array:
    *     - **range data** starts at the index of \f$\texttt{imc}\times(5\times N\f$)
    *     - **dE/dx data** starts at the index of \f$\texttt{imc}\times(5\times N\f$) + \f$2\times N\f$
    *     - **inverse range data** starts at the index of \f$\texttt{imc}\times(5\times N\f$) + \f$4\times N\f$
    *
    * The total number of data stored in the G4HepEmElectronData::fELossData array is
    * G4HepEmElectronData::fNumMatCuts\f$\times5\times\f$G4HepEmElectronData::fELossEnergyGridSize
    *
    * At run-time, for a given \f$E\f$ primary kinetic energy G4HepEmElectronData::fELossLogMinEkin
    * and G4HepEmElectronData::fELossEILDelta are used to compute the energy bin index \f$i\f$ such that
    * \f$ E_i \leq E < E_{i+1}, i=0,\ldots,N-1\f$. Then for the given material - cuts couple index,
    * the above starts indices can be used to access the corresponding energy loss data and second derivatives
    * associated to the primary kinetic energies of \f$ E_i, E_{i+1}\f$ needed to perform the spline interpolation.
    *
    *
    * @note
    * There is a spline interpolation function in G4HepEmRunUtils, specialised
    * for the above pattern used to store the dE/dx and range data. Using this
    * function **ensures optimal** data **cache utilisation at run-time**.
    * A separate, more traditional spline interpolation function is used for
    * run-time inverse range data interpolation. These are utilised in the
    * G4HepEmElectronManager to ensure the optimal run-time performance (both in
    * terms of memory consumption and speed) when accessing the restricted energy loss
    * related, i.e. stopping power, range and inverse range data in the \f$e^-/e^+\f$ stepping.
    */
  double*    fELossData; // [5xfELossEnergyGridSize x fNumMatCuts]
/// @} */ // end: eloss
  //

//// === MACROSCOPIC CROSS SECTION DATA
  /**
   * @name Restricted macroscopic cross section related data members:
   * These members are used to store all restricted macroscopic cross section related data both for
   * **ionisation** and **bremsstrahlung** for all material - cuts couples.
   */
///@{
  /** Total number of restricted macroscopic cross sections realted data stored in the single G4HepEmElectronData::fResMacXSecData array.*/
  int        fResMacXSecNumData;
  /** Start index of the macroscopic cross section data, for the material - cuts couple with the given index, in the G4HepEmElectronData::fResMacXSecData array.*/
  int*       fResMacXSecStartIndexPerMatCut;  // [fNumMatCuts]
  /** The restricted macroscopic cross section data for **ionisation** and **bremsstrahlung** for all material - cuts couples.
   *
   * All the restricted macroscopic cross section data are stored continuously in this G4HepEmElectronData::fResMacXSecData single array.
   * The *restricted macroscopic cross sections* go to *zero at primary kinetic energies lower than or equal to the secondary
   * production threshold*: at the secondary \f$e^-\f$ production threshold in the case of \f$e^+\f$ and \f$2\times\f$ of it in the case
   * of \f$e^-\f$ ionisation, while the secondary \f$\gamma\f$ production threshold energy in case of bremsstrahlung. It means that,
   * the minimum value of *the primary kinetic energy grid depends on the type of the interaction* (ioni. or brem.) as well as
   * *the production cut values*. Therefore, an individual primary kinetic energy grid is generated, and stored together with the
   * corresponding restricted macroscopic cross section values, for each individual material - cuts couples, separately for
   * ionisation and for bremsstrahlung in each cases.
   *
   * The data are stored in the following format for each of the G4HepEmMCCData material - cuts couples (stored in the
   * G4HepEmMatCutData::fMatCutData array):
   *   - for a G4HepEmMCCData material - cuts couple data with the index of \f$\texttt{imc}\f$ (i.e.
   *     stored at G4HepEmMatCutData::fMatCutData[\f$\texttt{imc}\f$]), the macroscopic scross section realted
   *     data starts at G4HepEmElectronData::fResMacXSecData[\f$\texttt{ioniStarts}\f$], where \f$\texttt{ioniStarts}=\f$G4HepEmElectronData::fResMacXSecStartIndexPerMatCut [\f$\texttt{imc}\f$]
   *   - then relative to this \f$\texttt{ioniStarts}\f$ start index, **first** the restricted macroscopic cross section data for **ionisation**:
   *       - ``[0]``: \f$M:=M^{\text{(ioni)}\texttt{-imc}}\f$: **number of** \f$E_i, i=0,\ldots,M-1\f$ **primary kinetic energy points** over
   *         which the \f$\Sigma:=\Sigma^{\text{(ioni)}\texttt{-imc}}(E_i)\f$ restricted macroscopic cross section **for ionisation**
   *         is computed and stored **for this material - cuts couple** with the index of \f$\texttt{imc}\f$.
   *       - ``[1]``: \f$\texttt{argmax}\{\Sigma(E_i)\}, i=0,\ldots,M-1\f$
   *       - ``[2]``: \f$\texttt{max}\{\Sigma(E_i)\}, i=0,\ldots,M-1\f$
   *       - ``[3]``: \f$\log(E_0)\f$
   *       - ``[4]``: \f$1/[log(E_{M-1}/E_0)/(M-1)]\f$
   *       - ``[5 : 5 + 3xM-1]``: \f$E_0,\Sigma(E_0),\Sigma(E_0)^{''},E_1,\Sigma(E_1),\Sigma(E_1)^{''},\ldots,E_{M-1},\Sigma(E_{M-1}), \Sigma(E_{M-1})^{''}\f$
   *         where \f$^{''}\f$ denotes the second derivatives.
   *   - then continuously from the \f$\texttt{bremStarts} = \texttt{ioniStarts} + 3\times M+5 \f$ index,
   *     the restricted macroscopic cross section data for **bremsstrahlung**:
   *       - ``[0]``: \f$N:=N^{\text{(brem)}\texttt{-imc}}\f$: **number of** \f$E_i, i=0,\ldots,N-1\f$ **primary kinetic energy points** over
   *         which the \f$\Sigma:=\Sigma^{\text{(brem)}\texttt{-imc}}(E_i)\f$ restricted macroscopic cross section **for bremsstrahlung**
   *         is computed and stored **for this material - cuts couple** with the index of \f$\texttt{imc}\f$.
   *       - ``[1]``: \f$\texttt{argmax}\{\Sigma(E_i)\}, i=0,\ldots,N-1\f$
   *       - ``[2]``: \f$\texttt{max}\{\Sigma(E_i)\}, i=0,\ldots,N-1\f$
   *       - ``[3]``: \f$\log(E_0)\f$
   *       - ``[4]``: \f$1/[log(E_{N-1}/E_0)/(N-1)]\f$
   *       - ``[5 : 5 + 3xN-1]``: \f$E_0,\Sigma(E_0),\Sigma(E_0)^{''},E_1,\Sigma(E_1),\Sigma(E_1)^{''},\ldots,E_{N-1},\Sigma(E_{N-1}), \Sigma(E_{N-1})^{''}\f$
   *         where \f$^{''}\f$ denotes again the second derivatives.
   *
   * The total number of data, i.e. the length of the G4HepEmElectronData::fResMacXSecData array,
   * is stored in G4HepEmElectronData::fResMacXSecData.
   *
   * At run-time, for a given \f$E\f$ primary kinetic energy and material - cuts couple with the index of \f$\texttt{imc}\f$,
   *  - the start index of the **macroscopic cross section data for ionisation** is given by
   *    \f$\texttt{ioniStarts}\f$=G4HepEmElectronData::fResMacXSecStartIndexPerMatCut[\f$\texttt{imc}\f$]
   *  - then G4HepEmElectronData::fResMacXSecData[\f$\texttt{ioniStarts}\f$+3] and
   *    G4HepEmElectronData::fResMacXSecData[\f$\texttt{ioniStarts}\f$+4] can be used to compute the
   *    kinetic energy bin index \f$i\f$ such that \f$ E_i \leq E < E_{i+1}, i=0,\ldots,\f$G4HepEmElectronData::fResMacXSecData[\f$\texttt{ioniStarts}\f$]\f$-1\f$.
   *  - then the kinetic energies, macroscopic cross sections and their second derivatives,
   *    associated to the primary kinetic energies of \f$ E_i, E_{i+1}\f$ are used to perform the spline interpolation
   *  - the start index of the corresponding **macroscopic cross section data for bremsstrahlung** is given by
   *    \f$\texttt{bremStarts} = \texttt{ioniStarts} + 5 + 3\times\f$G4HepEmElectronData::fResMacXSecData[\f$\texttt{ioniStarts}\f$]
   *  - then the same procedure can be applied as above to compute the kinetic energy bin index and perform the interpolation,
   *    but now relative to \f$\texttt{bremStarts}\f$ instead of the above \f$\texttt{ioniStarts}\f$
   *
   * @note
   * Note, that all the 6 data, that are needed for the run-time interpolation of the restricted macroscopic scross sections
   * are stored next to each other in the memory for both interactions (ionisation and bremsstrahlung). Moreover, for a given
   * material - cuts couple, the data for the two interactions are stored one after the other. Together with the corresponding
   * special spline interpolation function of G4HepEmRunUtils, that ensures a maximal profit of this memory layout, it makes
   * **optimal utilisation of the** data **cache at run-time**. This special spline interpolation is utilised in the
   * G4HepEmElectronManager to ensure the optimal run-time performance (both in terms of memory consumption and speed) when
   * accessing the restricted macroscopic cross section data in the \f$e^-/e^+\f$ stepping.
   *
   */
  double*    fResMacXSecData; // [fResMacXSecNumData]
/// @} */ // end: macroscopic cross section

//// === TARGET ELEMENT SELECTOR
  /**
   * @name Target element selector related data members:
   * These members store data utilised at run-time for the selection of the target
   * elements (in case of multi element atoms) on which the interaction takes palce.
   * Data are stored for all the interaction models used to describe both **ionisation** and
   * **bremsstrahlung** and for all material - cuts couples.
   *
   * These data are the normalised, element-wise contributions to the corresonding
   * macroscopic cross sections in case of multi element materials. Therefore,
   * similarly to the above restricted macroscopic cross sections, different energy
   * grids are generated for the the different material - cuts couples. The data
   * are also stored in a separate, single continuous array per interaction model.
   * The total number of data, i.e. the size of this single array as well as the start indices
   * of the data, related to the different material - cuts couples, are stored for each
   * of the three interaction models.
   *
   * The data are stored in the following format for each of the individual interaction models,
   * for each the G4HepEmMCCData material - cuts couples (stored in the G4HepEmMatCutData::fMatCutData array):
   *   - for a G4HepEmMCCData material - cuts couple data with the index of \f$\texttt{imc}\f$ (i.e.
   *     stored at G4HepEmMatCutData::fMatCutData[\f$\texttt{imc}\f$]), the element selector
   *     data starts at the index \f$\texttt{iStarts}=\texttt{fElemSelectorXYStartIndexPerMatCut[imc]}\f$ where
   *     \f$XY\f$ is one of the three models, i.e. \f$\{\texttt{Ioni, BremSB, BremRB}\}\f$ for the
   *     *Moller-Bhabha ionisation*, *Seltzer-Berger* or the *relativistic bremsstrahlung models*.
   *   - if the material, associated to this material - cuts couple, is composed of a single element,
   *     \f$\texttt{iStarts}=-1\f$
   *   - the following data are stored otherwise continuously in the appropriate \f$\texttt{fElemSelectorXYData}\f$
   *     array relative to this \f$\texttt{iStarts}\f$ index
   *       - ``[0]``: \f$K:=\f$ *number of discrte* \f$E_i, i=0,\ldots,K-1\f$ *primary particle kinetic energy values* used to compute and store
   *           the \f$P(Z_j,E_i):=\Sigma^{Z_j}(E_i)/\Sigma(E_i)\f$ normalised, element-wise contributions to the macroscopic cross section of the material.
   *       - ``[1]``: \f$Q:=\f$ *number of elements the given material is composed of*. So above, \f$Z_j, j=0,\ldots,Q-1\f$ at each individual
   *           \f$E_i\f$ kinetic energy values. However, since \f$P(Z_{j=Q-1},E_i)=\Sigma^{Z_{Q-1}}(E_i)/\Sigma(E_i) = 1\f$ for all \f$i=0,\ldots,K-1\f$
   *           due to the normalisation, data are computed and stored only for element indices of \f$j=0,\ldots,Q-2\f$.
   *       - ``[2]``: \f$\log(E_0)\f$
   *       - ``[3]``: \f$1/[log(E_{K-1}/E_0)/(K-1)]\f$
   *       - ``[4 : 4 + QxK-1]``: \f$E_0,P(j=0,E_0),P(j=1,E_0),\ldots,P(j=Q-2,E_0), \ldots,\f$ \f$E_{K-1},P(j=0,E_{K-1}),P(j=1,E_{K-1}),\ldots,P(j=Q-2,E_{K-1})\f$
   *
   * At run-time, when performing an interaction described by model \f$XY \in \{\texttt{Ioni, BremSB, BremRB}\}\f$,
   * with primary particle kinetic energy of \f$E\f$ in the material, related to the material - cuts couple with the index of \f$\texttt{imc}\f$,
   *  - the **start index of the** corresponding element selector **data** is \f$\texttt{iStarts}=\texttt{fElemSelectorXYStartIndexPerMatCut[imc]}\f$
   *  - the the corresonding \f$\texttt{fElemSelectorXYData[iStart+2]}\f$ and \f$\texttt{fElemSelectorXYData[iStart+3]}\f$ values can be used to compute the
   *    kinetic energy bin index \f$i\f$ such that \f$ E_i \leq E < E_{i+1}, i=0,\ldots,\texttt{fElemSelectorXYData[iStart]}-1\f$.
   *  - then the kinetic energies and normalised element-wise partial macroscopic cross sections,
   *    associated to the primary kinetic energies of \f$ E_i, E_{i+1}\f$ are used to perform the linear interpolation (smooth function) and
   *    and to sample the target element index from this discrete distribution.
   *
   * @note
   * Note, that all the data, that are needed for the run-time interpolation and for the target element index sampling are stored next to each other in the memory.
   * The implementations of the individual interaction models, that utilise these data for the run-time target atom selection (if needed), make sure that this
   * memory layout is maximally exploited. These ensure the optimal performance, both in terms of memory consumption and speed, when
   * accessing these data performing the correspondign \f$e^-/e^+\f$ interactions.
   */
///@{
  /** Total number of element selector data for the Moller-Bhabha model for e-/e+ ionisation.*/
  int       fElemSelectorIoniNumData;
  /** Indices, at which data starts for a given material - cuts couple.*/
  int*      fElemSelectorIoniStartIndexPerMatCut;     // [#material-cuts couple]
  /** Element selector data for all material - cuts couples with multiple element material.*/
  double*   fElemSelectorIoniData;                    // [fElemSelectorIoniNumData]

  /** Total number of element selector data for the Seltzer-Berger model for e-/e+ bremsstrahlung.*/
  int       fElemSelectorBremSBNumData;
  /** Indices, at which data starts for a given material - cuts couple.*/
  int*      fElemSelectorBremSBStartIndexPerMatCut;   // [#material-cuts couple]
  /** Element selector data for all material - cuts couples with multiple element material.*/
  double*   fElemSelectorBremSBData;                  // [fElemSelectorBremSBNumData]

  /** Total number of element selector data for the relativistic (improved Bethe-Heitler) model for e-/e+ bremsstrahlung.*/
  int       fElemSelectorBremRBNumData;
  /** Indices, at which data starts for a given material - cuts couple.*/
  int*      fElemSelectorBremRBStartIndexPerMatCut;   // [#material-cuts couple]
  /** Element selector data for all material - cuts couples with multiple element material.*/
  double*   fElemSelectorBremRBData;                  // [fElemSelectorBremRBNumData]
/// @} */ // end: target element selectors
};


/**
  * Allocates and pre-initialises the G4HepEmElectronData structure.
  *
  * This method is invoked from the InitElectronData() function declared
  * in the G4HepEmElectronInit header file. The input argument address of the
  * G4HepEmElectronData structure pointer is the one stored in the G4HepEmData
  * member of the `master` G4HepEmRunManager and the initialisation should be
  * done by the master G4HepEmRunManager by invoking the InitElectronData() function
  * for \f$e^-/e^+\f$ particles.
  *
  * @param theElectronData address of a G4HepEmElectronData structure pointer. At termination,
  *   the correspondig pointer will be set to a memory location with a freshly allocated
  *   G4HepEmElectronData structure with all its pointer members set to nullprt.
  *   If the input pointer was not null at input, the pointed memory, including all
  *   dynamic memory members, is freed before the new allocation.
  */
void AllocateElectronData (struct G4HepEmElectronData** theElectronData);

/**
  * Frees a G4HepEmElectronData structure.
  *
  * This function deallocates all dynamically allocated memory stored in the
  * input argument related G4HepEmElectronData structure, deallocates the structure
  * itself and sets the input address to store a pointer to null. This makes the
  * corresponding input stucture cleared, freed and ready to be re-initialised.
  * The input argument is supposed to be the address of the corresponding pointer
  * member of the G4HepEmData member of the `master` G4HepEmRunManager.
  *
  * @param theElectronData memory address that stores pointer to a G4HepEmElectronData
  *  structure. The memory is freed and the input address will store a null pointer
  *  at termination.
  */
void FreeElectronData (struct G4HepEmElectronData** theElectronData);


#ifdef G4HepEm_CUDA_BUILD
  /**
   * Device side version of the G4HepEmElectronData structure.
   *
   * All G4HepEmElectronData are available on the device side as well. However,
   * **the device side memory layout of these electron data are modified in order to facilitate
   * coalesced memory access**. The G4HepEmElectronDataOnDevice data structure is
   * to use to store the electron data on the main device memory with the modified
   * memory layout.
   */
  struct G4HepEmElectronDataOnDevice {
    /** Number of G4HepEm material - cuts (same as on the host side G4HepEmElectronData::fNumMatCuts).*/
    int        fNumMatCuts;

  //// === ENERGY LOSS DATA
    /**
     * @name Energy loss related data members:
     * These members are used to store all continuous energy loss related data (there
     * is a single primary kinetic energy grid for all material - cuts equally spaced
     * in log-scale).
     *
     * While the common kinetic energy grid is described with the same four members
     * and the same way as on the host side, **the host side single continuous data**
     * G4HepEmElectronData::fELossData **array is split into five separate arrays**
     * **on the device side** (according to the stored data types; see below) **in order to facilitate coalesced memory access**.
     *
     * Each of the five data arrays has a lenght of \f$N\times\f$G4HepEmElectronDataOnDevice::fNumMatCuts.
     *   
     * The start indices of the data, in these arrays for a material - cuts couple with the index of \f$\texttt{imc}\f$, is 
     * \f$\texttt{iStarts}=\texttt{imc}\times N\f$.
     * 
     * At run-time, for a given \f$E\f$ primary kinetic energy G4HepEmElectronDataOnDevice::fELossLogMinEkin
     * and G4HepEmElectronDataOnDevice::fELossEILDelta are used to compute the energy bin index \f$i\f$ such that
     * \f$ E_i \leq E < E_{i+1}, i=0,\ldots,N-1\f$. Then for the given material - cuts couple index,
     * the above \f$\texttt{iStarts}=\texttt{imc}\times N\f$ start index can be used to access the corresponding
     * energy loss data and second derivatives associated to the primary kinetic energies of \f$ E_i, E_{i+1}\f$
     * needed to perform the spline interpolation.
     *    
     */
  ///@{
    /** Number of discrete kinetic energy values in the grid (\f$N\f$) (same as G4HepEmElementData::fELossEnergyGridSize on the device). */
    int        fELossEnergyGridSize;
    /** Logarithm of the minimum kinetic energy value of the grid (\f$\ln(E_0)\f$) (same as G4HepEmElementData::fELossLogMinEkin on the device). */
    double     fELossLogMinEkin;     // log of the E_0
    /** Inverse of the log-scale delta value (\f$ 1/[log(E_{N-1}/E_0)/(N-1)]\f$) (same as G4HepEmElementData::fELossEILDelta on the device). */
    double     fELossEILDelta;
    /** The grid of the discrete kinetic energy values (\f$E_0, E_1,\ldots, E_{N-1}\f$) (same as G4HepEmElementData::fELossEnergyGrid on the device). */
    double*    fELossEnergyGrid;
    /** Restricted range values computed over the above primary particle kinetic energy grid
      * for all material - cuts couples. The \f$N:=\f$G4HepEmElectronDataOnDevice::fELossEnergyGridSize
      * range values for each material - cuts couple are stored continuously in this array.*/
    double*    fELossDataRange;
    /** The second derivative values associated to the reange values stored in the above G4HepEmElectronDataOnDevice::fELossDataRange array.*/
    double*    fELossDataRangeSD;
    /** Restricted dE/dx values computed over the above primary particle kinetic energy grid
      * for all material - cuts couples. The \f$N:=\f$G4HepEmElectronDataOnDevice::fELossEnergyGridSize
      * dE/dx values for each material - cuts couple are stored continuously in this array.*/
    double*    fELossDataDEDX;
    /** The second derivative values associated to the dE/dx values stored in the above G4HepEmElectronDataOnDevice::fELossDataDEDX array.*/
    double*    fELossDataDEDXSD;
    /** The second derivative values associated to the inverse-reange values (range and kinetic energy grid values stored in the arrays above).*/
    double*    fELossDataInvRangeSD;
  /// @} */ end: eloss data

  //// ===  Restricted macroscopic cross section data: ioni and brem
    /**
     * @name Restricted macroscopic cross section related data members:
     * These members are used to store all restricted macroscopic cross section related data both for
     * **ionisation** and **bremsstrahlung** for all material - cuts couples.
     *
     * While all the data are stored continuously in the G4HepEmElectronData::fResMacXSecData **single array on the host side**,
     * this is **split into sevar arrays on the device side in order to facilitate coalesced memory access**: 
     *   - data are stored separately for ionisation and for bremsstrahlung
     *   - one array per interaction stores the \f$M\f$ number of \f$E_0, E_i,\ldots,E_{M-1}\f$ primary particle kinetic 
     *     energy grid points for each material - cuts couple, used to compute and store the given \f$\Sigma(E_i)\f$ restricted macroscopic cross section
     *     for the given material - cuts couple
     *   - the four auxiliary data (\f$\texttt{argmax}\{\Sigma(E_i)\}, \texttt{max}\{\Sigma(E_i)\}, \log(E_0), 1/[log(E_{M-1}/E_0)/(M-1)]\f$) are stored
     *     in a separate array for each interactions and for each material - cuts
     *   - the three data, that are needed for the run - time spline interpolations, \f$E_i, \Sigma(E_i), \Sigma(E_i)^{''}\f$ are stored
     *     in three different separate arrays for a given interaction for all material - cuts couples
     *   - an additinal array is used for each interaction to store the index where the above data starts in the three arrays
     * 
     * At run-time, for a given \f$E\f$ primary kinetic energy and G4HepEmMCCData material - cuts couple data index of \f$\texttt{imc}\f$, 
     * first the kinetic energy bin index is computed. This can be done by using the corrsponding auxiliary data \f$\log(E_0),1/[log(E_{M-1}/E_0)/(M-1)]\f$ 
     * that are stored at the indices of \f$4\times\f$G4HepEmElectronDataOnDevice::fNumMatCuts\f$+2\f$ and \f$+3\f$ in the corresonding 
     * \f$\texttt{fResMacXSecXYAuxData}\f$ array (where \f$\texttt{XY}\in \{\texttt{Ioni,Brem}\}\f$). The required kinetic energy bin index \f$i\f$, 
     * such that \f$ E_i \leq E < E_{i+1}, i=0,\ldots,M-1\f$, where \f$M\f$ is the number of kinetic energy values for the given \f$\texttt{XY}\f$ interaction and 
     * material - cuts couple \f$\texttt{imc}\f$ stored in the corresponding \f$\texttt{fResMacXSecNumXYData[imc]}\f$, can be computed.
     * After having the kinetic energy bin index, the appropriate three data arrays can be used to obtain the \f$E_i, \Sigma(E_i), \Sigma(E_i)^{''}, E_{i+1}, \Sigma(E_{i+1}), \Sigma(E_{i+1})^{''}\f$ data required for 
     * the spline interpolation. These data are at the \f$i\f$-th and \f$i+1\f$-th positions in the given arrays relative to the start index for the given material - cuts couple 
     * stored in the correspondig \f$\texttt{fResMacXSecXYDataStart[imc]}\f$.
     *
     */
  ///@{
    /** Start index of the *ionisation* data in the G4HepEmElectronDataOnDevice::fResMacXSecIoniData array per material - cuts couple.*/
    int*       fResMacXSecIoniDataStart; // ioni data first index in fResMacXSecIoniData for mat-cut; [#material-cuts couple]
    /** Number of discrete kinetic energy grid points at which macroscopic cross section data are stored for *ionisation* in the G4HepEmElectronDataOnDevice::fResMacXSecIoniEData, G4HepEmElectronDataOnDevice::fResMacXSecIoniData, G4HepEmElectronDataOnDevice::fResMacXSecIoniSDData  arrays 
      * per material - cuts couple.*/
    int*       fResMacXSecNumIoniData;   // #ioni data for the given mat-cut; [#material-cuts couple]
    /** Same as above for *bremsstrahlung*. */
    int*       fResMacXSecBremDataStart; // same as above for brem
    /** Same as above for *bremsstrahlung*. */
    int*       fResMacXSecNumBremData;   // same as above for brem

    /** Array to store the four auxiliary \f$\texttt{argmax}\{\Sigma(E_i)\}, \texttt{max}\{\Sigma(E_i)\}, \log(E_0), 1/[log(E_{M-1}/E_0)/(M-1)]\f$ data for ionisation per material - cuts couples. */
    double*    fResMacXSecIoniAuxData;   // energyOfMaxVal, maxVal, logEMin, invLogEDelta; [4 x #material-cuts couple]
    /** Data array that stores the kinetic energy grids for *ionisation* for all material - cuts couples. Start index for a given material - cuts are stored in G4HepEmElectronDataOnDevice::fResMacXSecIoniDataStart.*/ 
    double*    fResMacXSecIoniEData;     // ioni mac-xsec energy grid
    /** Data array that stores the restricted macroscopic cross sections for *ionisation*, at discrete primary kinetic energies stored in the above array, for all material - cuts couples. Start index for a given material - cuts are stored in G4HepEmElectronDataOnDevice::fResMacXSecIoniDataStart.*/ 
    double*    fResMacXSecIoniData;      // ioni mac-xsec values
    /** Data array that stores the second derivatives of the above  macroscopic cross sections for *ionisation*, for all material - cuts couples. Start index for a given material - cuts are stored in G4HepEmElectronDataOnDevice::fResMacXSecIoniDataStart.*/ 
    double*    fResMacXSecIoniSDData;    // ioni mac-xsec second derivatives

    /** Same as above for *bremsstrahlung*.*/
    double*    fResMacXSecBremAuxData;   // same as above for brem
    /** Same as above for *bremsstrahlung*.*/
    double*    fResMacXSecBremEData;     //
    /** Same as above for *bremsstrahlung*.*/
    double*    fResMacXSecBremData;      //
    /** Same as above for *bremsstrahlung*.*/
    double*    fResMacXSecBremSDData;    //
  /// @} */ end: macroscopic cross sections

  //// ===   Target element selector data: for the ioni (Moller-Bhabha) and brem models (Seltzer-Berger, Relativistic)
    /**
     * @name Target element selector related data members:
     * These members store data utilised at run-time for the selection of the target
     * elements (in case of multi element atoms) on which the interaction takes place.
     * Data are stored for all the interaction models used to describe both **ionisation** and
     * **bremsstrahlung** and for all material - cuts couples.
     *
     * The **host side** \f$\texttt{fElemSelectorXYData}\f$ **single data array for a given** \f$\texttt{XY} \in \{Ioni,BremSB, BremRB\}\f$ **interaction**
     * **model**, used to store the target element selector related data for all material - cuts couple for the given intercation model, 
     * **is split into three arrays on the device side in order to facilitate coalesced memory access**:
     *   - \f$fElemSelectorNumXYData\f$ array stores the \f$K\f$ number of discrte primary kinetic energy values used to compute the elements-wise data for the \f$XY \in \{Ioni,BremSB, BremRB\}\f$ interaction model, per material - cuts couples.
     *   - \f$fElemSelectorXYAuxData\f$ array stores the two \f$\log(E_0), 1/[log(E_{K-1}/E_0)/(K-1)]\f$ auxiliary data needed for the kinetic energy bin computations for the \f$XY \in \{Ioni,BremSB, BremRB\}\f$ interaction model, per material - cuts couples.
     *   - \f$fElemSelectorXYData\f$ array stores the normalised, element-wise contributions to the macroscopic cross sections over the discrete kinetic energy grid \f$E_0,E_1,\ldots,E_{K-1}\f$ in the format of
     *       - \f$E_0,P(j=0,E_0),P(j=1,E_0),\ldots,P(j=Q-2,E_0), \ldots,\f$ \f$E_{K-1},P(j=0,E_{K-1}),P(j=1,E_{K-1}),\ldots,P(j=Q-2,E_{K-1})\f$
     *       - where \f$j=0,\ldots,Q-2\f$ is element index with \f$Q\f$ being the total number of elements the material is composed of that is stored in the G4HepEmElectronDataOnDevice::fElemSelectorNumElements per material - cuts couples.
     *       
     * An additional \f$fElemSelectorXYDataStar\f$ array is used in each case of \f$\texttt{XY} \in \{Ioni,BremSB, BremRB\}\f$ interaction models 
     * to store the start indices of the data in the corresonding \f$fElemSelectorXYData\f$  array per material - cuts couples.
     *
     * At run-time, for a given interaction model \f$\texttt{XY} \in \{Ioni,BremSB, BremRB\}\f$, \f$E\f$ primary kinetic energy and G4HepEmMCCData material - cuts couple data index of \f$\texttt{imc}\f$, 
     * first the kinetic energy bin index is computed. This can be done by using the corrsponding auxiliary data \f$\log(E_0),1/[log(E_{K-1}/E_0)/(K-1)]\f$ 
     * that stored at the indices of \f$2\times\f$G4HepEmElectronDataOnDevice::fNumMatCuts\f$\f$ and \f$+1\f$ in the corresonding 
     * \f$\texttt{fElemSelectorXYAuxData}\f$ array. The required kinetic energy bin index \f$i\f$, 
     * such that \f$ E_i \leq E < E_{i+1}, i=0,\ldots,K-1\f$, where \f$K\f$ is the number of kinetic energy values for the given \f$\texttt{XY}\f$ interaction and 
     * material - cuts couple \f$\texttt{imc}\f$ stored in the corresponding \f$\texttt{fElemSelectorNumXYData[imc]}\f$, can be computed.
     * After having the kinetic energy bin index, the appropriate \f$fElemSelectorXYData\f$ array can be used to obtain the data \f$E_i,P(j=0,\ldots,Q-2;E_i)\f$ and \f$E_{i+1},P(j=0,\ldots,Q-2;E_{i+1})\f$ needed for the (linear) interpolation and element selection.
     * These data start from the \f$i \to i\times Q\f$-th and \f$i+1 \to (i+1)\times Q\f$-th positions in the given array, relative to the start index for the given material - cuts couple 
     * stored in at correspondig \f$\texttt{fElemSelectorXYDataStart[imc]}\f$ for this material - cuts couple with the index of \f$\texttt{imc}\f$.
     */
    /** Number of elements the multi element material - cuts couples composed of per material - cuts.*/ 
    int*       fElemSelectorNumElements;
    // - start of element selector data and number of energy bins
    //   for each material cuts couple (and for Moller-Bhabha ioni and Seltzer-Berger
    //   Rel. bren models)
    
    /** Start index of element selector data for ionisation (Moller-Bhabha) interaction model per material - cuts couple.*/
    int*       fElemSelectorIoniDataStart;    // [#mat-cuts]
    /** Number of discrete kinetic energy values used to store element selector data for ionisation (Moller-Bhabha) interaction model per material - cuts couple.*/
    int*       fElemSelectorNumIoniData;
    
    /** Start index of element selector data for bremsstrahlung (Seltzer-Berger) interaction model per material - cuts couple.*/
    int*       fElemSelectorBremSBDataStart;
    /** Number of discrete kinetic energy values used to store element selector data for bremsstrahlung (Seltzer-Berger) interaction model per material - cuts couple.*/
    int*       fElemSelectorNumBremSBData;
    
    /** Start index of element selector data for bremsstrahlung (Relativistic) interaction model per material - cuts couple.*/
    int*       fElemSelectorBremRBDataStart;
    /** Number of discrete kinetic energy values used to store element selector data for bremsstrahlung (Relativistic) interaction model per material - cuts couple.*/
    int*       fElemSelectorNumBremRBData;

    /** Array to store the \f$\log(E_0), 1/[log(E_{K-1}/E_0)/(K-1)]\f$ auxiliary data used to compute the kinetic energy bin index for (Moller-Bhabha) interaction model per material - cuts couple.*/
    double*    fElemSelectorIoniAuxData;
    /** Same as above for bremsstrahlung (Seltzer-Berger) interaction model.*/
    double*    fElemSelectorBremSBAuxData;
    /** Same as above for bremsstrahlung (Relativistic) interaction model.*/
    double*    fElemSelectorBremRBAuxData;

    /** Element selector data, discrete kinetic energy values and normalised, element-wise contributions to the macroscopic cross sections, 
      * for ionisation (Moller-Bhabha) interaction model for all material - cuts couples.*/
    double*    fElemSelectorIoniData;
    /** Same as above for bremsstrahlung (Seltzer-Berger) interaction model.*/
    double*    fElemSelectorBremSBData;
    /** Same as above for bremsstrahlung (Relativistic) interaction model.*/
    double*    fElemSelectorBremRBData;
  };

  /**
    * Allocates memory for and copies the G4HepEmElectronData structure from the
    * host to the device.
    *
    * The input arguments are supposed to be the corresponding members of the 
    * G4HepEmData, top level data structure, stored in the `master` G4HepEmRunManager.    
    *
    * @param onHOST    pointer to the host side, already initialised G4HepEmElectronData structure.
    * @param onDEVICE  host side address of a G4HepEmElectronDataOnDevice structure memory pointer. The pointed 
    *   memory is cleaned (if not null at input) and points to the device side memory at termination 
    *   that stores the copied G4HepEmElectronDataOnDevice structure.
    */  
  void CopyElectronDataToDevice(struct G4HepEmElectronData* onHOST, struct G4HepEmElectronDataOnDevice** onDEVICE);
  void CopyElectronDataToDeviceHL(struct G4HepEmElectronData* onHOST, struct G4HepEmElectronData** onDEVICE);

  /**
    * Frees all memory related to the device side G4HepEmElectronDataOnDevice structure referred 
    * by the pointer stored on the host side input argument address.
    *
    * @param onDEVICE host side address of a G4HepEmElectronDataOnDevice structure located on the device side memory.
    *   The correspondig device memory will be freed and the input argument address will be set to null.
    */
  void FreeElectronDataOnDevice(struct G4HepEmElectronDataOnDevice** onDEVICE);
  void FreeElectronDataOnDeviceHL(struct G4HepEmElectronData** onDEVICE);
#endif // DG4HepEm_CUDA_BUILD




#endif // G4HepEmElementData_HH
