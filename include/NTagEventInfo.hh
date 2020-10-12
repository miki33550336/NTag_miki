/*******************************************
*
* @file NTagEventInfo.hh
*
* @brief Defines NTagEventInfo.
*
********************************************/

#ifndef NTAGEVENTINFO_HH
#define NTAGEVENTINFO_HH 1

#include <array>
#include <vector>
#include <ctime>

#include <TString.h>
#include <TMVA/Reader.h>

#undef MAXPM
#undef MAXPMA

#include <skparmC.h>
#include <geopmtC.h>

#include "NTagMessage.hh"
#include "NTagTMVA.hh"
#include "NTagTMVAVariables.hh"
#include "NTagCandidate.hh"

/******************************************
*
* @brief Vertex mode for NTagEventInfo
*
* Each option sets the type of prompt
* (neutron creation) vertex to be used
* in capture candidate search.
* This affects the behavior of the member
* function NTagEventInfo::SetPromptVertex.
*
* The ToF to each hit PMT is based on
* the prompt vertex specified by this mode.
*
* Vertex mode can be set using
* NTagEventInfo::SetVertexMode.
*
* @see NTagEventInfo::SetPromptVertex
* @see NTagEventInfo::SetVertexMode
*
*******************************************/
enum VertexMode
{
    mAPFIT,  ///< Use APFit vertex. Default in class NTagZBS. Generally applies to all ATMPD data with APFit applied.
    mBONSAI, ///< Use BONSAI vertex. Default in class NTagROOT. Generally applies to all LOWE data with BONSAI applied.
    mCUSTOM, ///< Use custom prompt vertex. This mode is automatically set if NTagEventInfo::SetCustomVertex is called.
             ///< Use this option for Am/Be calibration data if you're not using BONSAI.
    mTRUE,   ///< Use true prompt vertex.
             ///< Only applicable if the input is an MC file with the \c skvect common filled.
             ///< This feature fully supports all files generated using \b SKDETSIM.
    mSTMU    ///< Use the vertex where a stopping muon has stopped inside the tank. Not supported yet.
};

namespace NTagConstant{
    constexpr float (*PMTXYZ)[3] = geopmt_.xyzpm;
    constexpr float C_WATER = 21.5833;
}

namespace NTagDefault{
    constexpr int N10TH = 7;
    constexpr int N10MX = 50;
    constexpr int N200MX = 200;
    constexpr float T0TH = 5.;
    constexpr float T0MX = 535.;
    constexpr float VTXSRCRANGE = 4000.;
    constexpr float TMATCHWINDOW = 40.;
    constexpr float TMINPEAKSEP = 50.;
    constexpr int ODHITMX = 16;
}

/**********************************************************
 *
 * @brief The container of raw TQ hit information,
 * event variables, and manipulating function library.
 *
 * @details NTagEventInfo has two purposes:
 * - Containing event variables such as
 * #qismsk and candidate capture variables such as #vN10n
 * - Providing a set of manipulating functions that are
 * used in event handling
 *
 * Most of the functions have appropriate names that
 * are self-explanatory, so please refer to each member
 * function to get ideas on what this class has to offer.
 *
 * NTagEventInfo::SearchCaptureCandidates is the core
 * utility provided by this class as the capture candidates
 * are sought right in this function. If the search
 * algorithm for candidates has to change by any reason,
 * NTagEventInfo::SearchCaptureCandidates should be the
 * first place to have a look.
 *
 * Since this class is merely a container of member
 * event variables with a bunch of manipulating functions,
 * this class by itself cannot read any file
 * nor process events without help of its subclass NTagIO.
 * NTagIO, as a subclass of NTagEventInfo, uses the
 * inherited member functions to process each event.
 * Refer to NTagIO::ReadFile,
 * which uses the member functions provided by
 * NTagEventInfo in a specified order in
 * NTagIO::ReadMCEvent for MC events and
 * NTagIO::ReadDataEvent, NTagIO::ReadSHEEvent,
 * and NTagIO::ReadAFTEvent for data events, for the
 * event-wise instructions applied to the input file.
 *
 * ### Member variables ###
 *
 * Variables saved to trees are not private, but protected
 * so that they can be utilized in subclasses.
 *
 ********************************************************/
class NTagEventInfo
{
    public:
        /**
        * @brief Constructor of class NTagEventInfo.
        * @details Default search settings for capture candidates, i.e., the range of N10 and T0,
        * are set in this constructor. You can always change the settings using the setter functions
        * provided with this class, but only if you use them before NTagIO::ReadFile is called.
        * Cuts in calculating TMVA classifier output can also be set using NTagTMVA::SetReaderCutRange method.
        * @param verbose The verbosity of NTagEventInfo and all of its daughter classes.
        */
        NTagEventInfo(Verbosity verbose);
        /**
        * @brief Destructor of class NTagEventInfo.
        */
        virtual ~NTagEventInfo();

        ////////////////////////////////
        // Functions to set variables //
        ////////////////////////////////

            /***********/
            /* Data/MC */
            /***********/

            /**
             * @brief Saves basic event information to member variables.
             * @details Saved variables: #runNo, #subrunNo, #eventNo, #qismsk, #nhitac, #trgOffset
             */
            virtual void SetEventHeader();
            /**
             * @brief Saves prompt vertex (neutron creation vertex).
             * @details Saved variables: #pvx, #pvy, #pvz
             */
            virtual void SetPromptVertex();
            /**
             * @brief Saves variables generated by APFit.
             * @details Saved variables: #evis, #apNRings, #vAPRingPID,
             * #vAPMom, #vAPMomE, #vAPMomMu, #apNMuE, #apNDecays
             */
            virtual void SetAPFitInfo();
            /**
             * @brief Saves prompt-related variables generated by BONSAI.
             * @details Saved variables: #evis
             */
            virtual void SetLowFitInfo();
            /**
             * @brief Extracts TQ hit arrays from input file and append it to the raw hit vectors.
             * @details Saved variables: #vTISKZ, #vQISKZ, #vCABIZ
             */
            virtual void AppendRawHitInfo();
            /**
             * @brief Subtracts ToF from each raw hit time in #vTISKZ and sort.
             * @details Saved variables: #vUnsortedT_ToF, #vSortedT_ToF, #vSortedPMTID, #vSortedQ
             */
            virtual void SetToFSubtractedTQ();

            /***********/
            /* MC-only */
            /***********/

            /**
             * @brief Saves true variables extractable only from MC.
             * @details Saved variables: #nVec, #vecx, #vecy, #vecz, #vVecPID, #vVecPX, #vVecPY, #vVecPZ, #vVecMom,
             * #neutIntMode, #nVecInNeut, #nNInNeutVec, #neutIntMom,
             * #nSavedSec, #nTrueCaptures, #vSecPID, #vSecIntID, #vParentPID,
             * #vSecVX, #vSecVY, #vSecVZ, #vSecDWall, #vSecPX, #vSecPY, #vSecPZ, #vSecMom, #vSecT, #vCapID
             * #vTrueCT, #vCapVX, #vCapVY, #vCapVZ, #vNGamma, #vTotGammaE, #vCapID
             */
            virtual void SetMCInfo();
            /**
             * @brief Reads secondary bank from input file and fills \c secndprt common. Called inside
             * NTagEventInfo::SetMCInfo.
             * @see: NTagROOT::ReadSecondaries
             */
            virtual void ReadSecondaries();
            /**
             * @brief Sets true capture information.
             * @details Saved variables: #vIsCapture, #vIsGdCapture, #vDoubleCount, #vCTDiff,
             * #vTrueCapVX, #vTrueCapVY, #vTrueCapVZ
             */
            //virtual void SetTrueCaptureInfo();

        /////////////
        // Tagging //
        /////////////

        /**
         * @brief The main search function for candidate selection before applying neural network.
         * @details NTagEventInfo::SavePeakFromHit is called to save peaks that match 
         * the primary selection conditions as neutron capture candidates.
         * See the source code for the details.
         * Saved variables: #vFirstHitID, #vBeta14_10, #nCandidates
         * #vFirstHitID, #vBeta14_10, #vN1300, #vBeta14_50,
         * #vTRMS10n, #vN10n, #vReconCTn,
         * #vIsCapture, #vIsGdCapture, #vDoubleCount, #vCTDiff,
         * #vTrueCapVX, #vTrueCapVY, #vTrueCapVZ
         * @see: <a href="https://kmcvs.icrr.u-tokyo.ac.jp/svn/rep/skdoc/atmpd/publish/neutron2013/technote/
         * tn_neutron2.pdf">Tristan's ntag technote</a> for the description of Neut-fit.
         */
        virtual void SearchCaptureCandidates();
        /**
         * @brief Function for setting capture variables.
         * @details For MC input, NTagEventInfo::SetTrueCaptureInfo is invoked to separate true captures from false candidates.
         * See the source code for the details.
         * Saved variables: #vTRMS10n, #vN10n, #vReconCTn, #vN1300, #vBeta14_50, #vNvx, #vNvy, #vNvz
         * #vIsCapture, #vIsGdCapture, #vDoubleCount, #vCTDiff,
         * #vTrueCapVX, #vTrueCapVY, #vTrueCapVZ
         */
        virtual void SetCandidateVariables();
        /**
         * @brief Get classifier output from TMVA.
         * @details Calls NTagTMVA::GetOutputFromCandidate which calculates the classifer output.
         * The cuts applied in calculating the TMVA classifier output are set in the constructor NTagEventInfo::NTagEventInfo.
         * Saved variable: #vTMVAOutput
         * @see NTagTMVA::GetOutputFromCandidate
         */
        //virtual void GetTMVAOutput();

        ////////////////
        // Calculator //
        ////////////////

        /**
         * @brief Get norm of a size-3 float array.
         * @param vec A size-3 float array.
         * @return The norm of the given array.
         */
        //inline float Norm(const float vec[3]);
        /**
         * @brief Get norm of the vector with given x, y, z coordinates.
         * @param x X coordinate of a vector.
         * @param y Y coordinate of a vector.
         * @param z Z coordinate of a vector.
         * @return The norm of the vector with given coordinates.
         */
        //inline float Norm(float x, float y, float z);
        /**
         * @brief Get distance between two points specified by the two given size-3 float arrays.
         * @param vec1 A size-3 float array of coordinates of a vector 1.
         * @param vec2 A size-3 float array of coordinates of a vector 2.
         * @return The distance between vector 1 and 2.
         */
        //float GetDistance(const float vec1[3], const float vec2[3]);
        /**
         * @brief Get i-th Legendre polynomial P_i(x) evaluated at x.
         * @param i The order of Legendre polynomial.
         * @param x The x value to evaluate the polynomial.
         * @return The i-th Legendre polynomial P_i(x) evaluated at x.
         */
        //float GetLegendreP(int i, float& x); // legendre polynomials for betas
        /**
         * @brief Evaluate &beta;_i values of a hit cluster for i = 1...5 and return those in an array.
         * @param PMTID A vector of PMT cable IDs. The locations of the PMTs are fetched from #PMTXYZ.
         * @param startIndex The starting index of a hit cluster.
         * @param nHits The number of total hits in a cluster.
         * @return An size-6 array of &beta; values. The i-th element of the returned array
         * is the i-th &beta; value. The 0-th element is a dummy filled with 0.
         * @see For the details of the &beta; values, see Eq. (5) of the SNO review article at
         * <a href="https://arxiv.org/pdf/1602.02469.pdf">arXiv:1602.02469</a>.
         */
        //std::array<float, 6> GetBetaArray(const std::vector<int>& PMTID, int startIndex, int nHits);

        ////////////////////////////////
        // Variable-related functions //
        ////////////////////////////////

            /***********************************/
            /* functions on capture candidates */
            /***********************************/

            /**
             * @brief Get reconstructed capture time of a capture candidate, with setting the triggered time as t=0.
             * @details The return value of this function is a reconstructed counterpart of the true capture time that is
             * recorded in the global simulation time. This function is applicable to MC events only.
             * @param candidateID The ID of a capture candidate.
             * @return Reconstructed capture time subtracted by the event's trigger offset.
             * @see NTagEventInfo::IsCapture, NTagEventInfo::IsGdCapture, NTagEventInfo::TrueCaptureTime
             */
            //inline float ReconCaptureTime(int candidateID);
            /**
             * @brief Checks if a caputure candidate is indeed a true capture.
             * @details It compares the reconstructed capture time (NTagEventInfo::ReconCaptureTime) of the candidate
             * with recorded true capture times (#vTrueCT) in an MC event,
             * and if the two times are separated within a specified time window (#TMATCHWINDOW),
             * then returns \c true. Otherwise returns \c false.
             * @param candidateID The ID of a capture candidate.
             * @param bSave If \c true, the candidate ID is
             * @return \c true if the reconstructed capture time matches a true capture time, otherwise \c false.
             */
            //int IsCapture(int candidateID, bool bSave=false);
            /**
             * @brief Checks if a caputure candidate is indeed a capture due to Gd.
             * @details It compares the reconstructed capture time (NTagEventInfo::ReconCaptureTime) of the candidate
             * with recorded true capture times (#vTrueCT) in an MC event,
             * and if the two times are separated within a specified time window (#TMATCHWINDOW),
             * then returns \c true. Otherwise returns \c false.
             * @param candidateID The ID of a capture candidate.
             * @return \c true if the reconstructed capture time matches a true capture time, otherwise \c false.
             */
            //int IsGdCapture(int candidateID);
            /**
             * @brief Looks for the matching true capture time for \p candidateID from #vTrueCT.
             * @param candidateID The ID of a capture candidate.
             * @return A matching true capture time if \p candidateID is a true capture, otherwise -9999.
             */
            //float TrueCaptureTime(int candidateID);
            /**
             * @brief Gets a true capture vertex if the given capture candidate is a true capture.
             * @param candidateID The ID of a capture candidate.
             * @return A matching true capture vertex if \p candidateID is a true capture, otherwise {0, 0, 0}.
             */
            //std::array<float, 3> TrueCaptureVertex(int candidateID);

            /*********************/
            /* functions on hits */
            /*********************/

            /**
             * @brief Sort ToF-subtracted hit vector #vUnsortedT_ToF.
             * @details Saved variables: #vSortedT_ToF, #vSortedQ, #vSortedPMTID.
             */
            void SortToFSubtractedTQ();
            /**
             * @brief Gets number of hits within \p tWidth [ns] starting from index \p startIndex.
             * @param T A vector of PMT hit times. [ns]
             * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
             * @param tWidth The width of the time window [ns] to count hits within.
             * @return The number of hits within \p tWidth [ns] starting from index \p startIndex.
             */
            //int GetNhitsFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);
            /**
             * @brief Gets number of hits within \p tWidth [ns] whose center comes at time \p centerTime [ns].
             * @param T A vector of PMT hit times. [ns]
             * @param centerTime The exact time [ns] to search for hits around.
             * @param tWidth The width of the time window [ns] to count hits within. \p centerTime comes in the center
             * of this \p tWidth.
             * @return The number of hits within \p tWidth [ns] whose center comes at time \p centerTime [ns].
             */
            //int GetNhitsFromCenterTime(const std::vector<float>& T, float centerTime, float tWidth);
            /**
             * @brief Gets the summed charge [p.e.] of a hit cluster or a capture candidate, starting from
             * index \p startIndex within a time window with \p tWidth [ns].
             * @param T A vector of PMT hit times. [ns]
             * @param Q A vector of deposited charge. [p.e.] Each element of \p Q must correspond to the element of
             * \p T with the same index.
             * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
             * @param tWidth The width of the time window [ns] to count hits within.
             * @return The summed charge [p.e.] of a hit cluster or a capture candidate from \p startIndex within
             * \p tWidth [ns].
             */
            //float GetQSumFromStartIndex(const std::vector<float>& T, const std::vector<float>& Q,
            //                                            int startIndex, float tWidth);
            /**
             * @brief Gets ToF from a vertex specified by \p vertex to a PMT with cable ID \p pmtID.
             * @param vertex A size-3 \c float array of x, y, and z-coordinates of some point in the SK coordinate system.
             * @param pmtID Cable ID of a PMT.
             * @return The time-of-flight (ToF) of a photon from the given vertex to the specified PMT.
             */
            float GetToF(float vertex[3], int pmtID);
            /**
             * @brief Gets the RMS value of a hit-time vector.
             * @param T A vector of PMT hit times. [ns]
             * @return The RMS value of a hit-time vector \p T.
             */
            //float GetTRMS(const std::vector<float>& T);
            /**
             * @brief Gets the RMS value of a hit cluster extracted from a hit-time vector.
             * @param T A vector of PMT hit times. [ns]
             * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate to extract.
             * @param tWidth The width of the time window [ns] to count hits within.
             * @return The RMS value of the extracted hit cluster from the input hit-tme vector \p T.
             */
            //float GetTRMSFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);
            /**
             * @brief Gets the minimum RMS value of hit-times by searching for the minizing vertex.
             * @param T A vector of PMT hit times. [ns]
             * @param PMTID A vector of PMT cable IDs corresponding to each hit in \p T.
             * @param fitVertex The array to have minimizing vertex coordinates filled.
             * @return The RMS value of the extracted hit cluster from the input hit-tme vector \p T.
             * \p fitVertex is also returned as the coordinates of the TRMS minimizing vertex of \p T.
             * @note The input hit-time vector must not have ToF subtracted as ToF will be subtracted inside this function.
             */
            //float MinimizeTRMS(const std::vector<float>& T, const std::vector<int>& PMTID, float fitVertex[3]);
            /**
             * @brief Gets the ToF-subtracted version of an input hit-time vector \p T.
             * @param T A vector of PMT hit times. [ns]
             * @param PMTID A vector of PMT cable IDs corresponding to each hit in \p T.
             * @param vertex A size-3 array of vertex coordinates to calculate ToF from.
             * @param doSort If \c true, the returned vector is sorted in ascending order.
             * @return The ToF-subtracted version of an input hit-time vector \p T. If \p doSort is set to \c true,
             * the returned vector is sorted in ascending order.
             * @note The input hit-time vector must not have ToF subtracted as ToF will be subtracted inside this function.
             */
            std::vector<float> GetToFSubtracted(const std::vector<float>& T, const std::vector<int>& PMTID,
                                                  float vertex[3], bool doSort=false);

        /////////////////////////////
        // Member variable control //
        /////////////////////////////

        /**
         * @brief Clears all member vectors and sets all member variables to a default value (which is mostly 0).
         */
        virtual void Clear();
        /**
         * @brief Saves the secondary of the given index. Called inside NTagEventInfo::SetMCInfo.
         * @param secID The index of the secondary particle saved in the \c secndprt common block.
         * @details Saved variables: #vSecPID, #vSecIntID, #vParentPID, #vSecVX, #vSecVY, #vSecVZ,
         * #vSecDWall, #vSecPX, #vSecPY, #vSecPZ, #vSecMom, #vSecT, #vCapID, #nSavedSec
         */
        virtual void SaveSecondary(int secID);
        /**
         * @brief Saves the peak from the input index of the sorted ToF-subtracted hit-time vector #vSortedT_ToF.
         * @param hitID The index of the first hit of the peak to save.
         * @details Saved variables: #vFirstHitID, #vBeta14_10, #nCandidates
         */
        virtual void SavePeakFromHit(int hitID);
        /**
         * @brief Checks if the raw hit vector #vTISKZ is empty.
         * @details This function is used as a flag for an SHE event.
         * If this function is \c false, the previous event must have been an SHE event.
         * @return \c true if #vTISKZ is empty, otherwise \c false.
         */
        bool IsRawHitVectorEmpty() { return vTISKZ.empty(); }

        ////////////////////////
        // Set tag conditions //
        ////////////////////////

        /**
         * @brief Set limits #N10TH and #N10MX for N10.
         * @param low Lower limit for N10.
         * @param high Upper limit for N10.
         */
        inline void SetN10Limits(int low, int high=NTagDefault::N10MX) { N10TH = low; N10MX = high; }
        /**
         * @brief Set upper limit N200MX for N200.
         * @param max Upper limit for N200.
         */
        inline void SetN200Max(int max) { N200MX = max; }
        /**
         * @brief Set limits #T0TH and #T0MX for T0.
         * @param low Lower limit for T0. [ns]
         * @param high Upper limit for T0. [ns]
         * @note Both #T0TH and #T0MX should be in the form of global recorded hit time.
         * Please take into account that tthe rigger offset is ~1,000 ns in this format.
         */
        inline void SetT0Limits(float low, float high=NTagDefault::T0MX) { T0TH = low; T0MX = high; }
        /**
         * @brief Set vertex search range #VTXSRCRANGE in NTagEventInfo::MinimizeTRMS.
         * Use this function to cut T0 of the capture candidates.
         * @param cut Vertex search range to be used in NTagEventInfo::MinimizeTRMS
         */
        inline void SetDistanceCut(float cut) { VTXSRCRANGE = cut; }
        /**
         * @brief Set the width #TMATCHWINDOW of the time window used in true-to-reconstructed capture mapping.
         * @param t Width of the time window for capture mapping. [ns]
         * @see NTagEventInfo::IsCapture, NTagEventInfo::IsGdCapture
         */
        inline void SetTMatchWindow(float t) { TMATCHWINDOW = t; }
        /**
         * @brief Set the minimum peak separation #TMINPEAKSEP.
         * @param t Width of minimum peak separation. [ns]
         * @see NTagEventInfo::SearchCaptureCandidates
         */
        inline void SetTPeakSeparation(float t) { TMINPEAKSEP = t; }
        /**
         * @brief Set the upper limit #ODHITMX for the number of OD hits.
         * @param q Upper limit for the number of OD hits.
         * @note The parameter #ODHITMX is not used at the moment.
         */
        inline void SetMaxODHitThreshold(float q) { ODHITMX = q; }
        /**
         * @brief Sets #VertexMode #fVertexMode.
         * @param m Vertex mode to use in NTagEventInfo.
         * @see #VertexMode
         */
        inline void SetVertexMode(VertexMode m) { fVertexMode = m; }
        /**
         * @brief Chooses whether to use TMVA or not. Sets #useTMVA.
         * @param b If \c true, TMVA is used to produce classifier output. (#vTMVAOutput)
         */
        inline void UseTMVA(bool b) { useTMVA = b; }
        /**
         * @brief Sets custom vertex to start searching for candidates. Sets #customvx, #customvy, #customvz.
         * @param x X coordinate in SK coordinate system [cm]
         * @param y Y coordinate in SK coordinate system [cm]
         * @param z Z coordinate in SK coordinate system [cm]
         * @see #VertexMode
         */
        inline void SetCustomVertex(float x, float y, float z)
                                   { customvx = x; customvy = y; customvz = z; fVertexMode = mCUSTOM; }
        /**
         * @brief Choose whether to save residual TQ vectors (#vSortedT_ToF, #vSortedQ, #vSortedPMTID) or not. Sets #saveTQ.
         * @param b If \c true, #NTagIO::restqTree is written to the output file filled with residual TQ vectors.
         */
        inline void SetSaveTQAs(bool b) { saveTQ = b; }
        
        /**
         * @brief 
         */
        inline Verbosity GetVerbosity() { return fVerbosity; }
         
        void InitializeCandidateVariableVectors();
        void ExtractCandidateVariables();
        void DumpCandidateVariables();
         

        // TMVA tools
        /// All input variables to TMVA are controlled by this class!
        NTagTMVA    TMVATools;

    private:
        const float (*PMTXYZ)[3]; ///< A map from PMT cable ID to coordinates. Equivalent to \c geopmt_.xyzpm.
        //const float C_WATER;      ///< Speed-of-light in water. [cm/ns]

        // Tag conditions
        int         N10TH,        ///< Lower limit for N10. @see NTagEventInfo::SetN10Limits
                    N10MX,        ///< Upper limit for N10. @see NTagEventInfo::SetN10Limits
                    N200MX;       ///< Upper limit for N200. @see NTagEventInfo::SetN200Max
        float       VTXSRCRANGE;  ///< Vertex search range in NTagEventInfo::MinimizeTRMS. @see NTagEventInfo::SetDistanceCut
        float       T0TH,         ///< Lower limit for T0. @see: NTagEventInfo::SetT0Limits
                    T0MX;         ///< Upper limit for T0. @see: NTagEventInfo::SetT0Limits
        float       TMATCHWINDOW; ///< Width of the true-reconstructed capture time matching window. [ns]
                                  ///< @see: NTagEventInfo::SetTMatchWindow
        float       TMINPEAKSEP;  ///< Minimum candidate peak separation. [ns] @see: NTagEventInfo::SetTPeakSeparation
        float       ODHITMX;      ///< Threshold on the number of OD hits. Not used at the moment.

        // Prompt-vertex-related
        float       customvx,     ///< X coordinate of a custom prompt vertex
                    customvy,     ///< Y coordinate of a custom prompt vertex
                    customvz;     ///< Z coordinate of a custom prompt vertex
        int         fVertexMode;  ///< #VertexMode of class NTagInfo and all inheriting classes.
        
        std::vector<int> reverseIndex; ///< Inverse map from indices of vSortedT_ToF to indices of vTISKZ.
        
        bool candidateVariablesInitalized;

    protected:
    
        /** # of processed events */
        int nProcessedEvents;
        
        /** Raw trigger time (`skhead_.nt48sk`) */
        int preRawTrigTime[3];
    
        // Signal TQ source
        TFile* fSigTQFile; 
        TTree* fSigTQTree;
        
        // Raw TQ hit vectors
        std::vector<int>    vCABIZ; ///< A vector of PMT cable IDs of all recorded hits from an event.
                                    ///< Forms a triplet with #vTISKZ and #vQISKZ.
        std::vector<float>  vTISKZ, ///< A vector of hit times [ns] of all recorded hits from an event.
                                    ///< Forms a triplet with #vCABIZ and #vQISKZ.
                            vQISKZ; ///< A vector of deposited charge [p.e.] of all recorded hits from an event.
                                    ///< Forms a triplet with #vCABIZ and #vTISKZ.
        std::vector<int>    vISIGZ; ///< A vector of signal flags (0: bkg, 1: sig) of all recorded hits from an event.
                                    ///< If #fSigTQFile is not \c NULL, it is saved in NTagEventInfo::AppendRawHitInfo.
        std::vector<float>* vSIGT; ///< A vector to save signal hit times from #fSigTQTree temporarily. Not included in output.
        std::vector<int>*   vSIGI; ///< A vector to save signal hit PMT IDs from #fSigTQTree temporarily. Not included in output.
                                    
        // Processed TQ hit vectors
        std::vector<int>    vSortedPMTID;   ///< A vector of PMT cable IDs corresponding to each hit
                                            ///< sorted by ToF-subtracted hit time in ascending order.
                                            ///< Forms a triplet with #vSortedT_ToF and #vSortedQ.
        std::vector<float>  vSortedT_ToF,   ///< A vector of ToF-subtracted hit times [ns] sorted in ascending order.
                                            ///< Forms a triplet with #vSortedPMTID and #vSortedQ.
                            vUnsortedT_ToF, /*!< A vector of unsorted ToF-subtracted hit times [ns],
                                                 in the order as in #vTISKZ. */
                            vSortedQ;       ///< A vector of deposited charge [p.e.] corresponding to each hit
                                            ///< sorted by ToF-subtracted hit time in ascending order.
                                            ///< Forms a triplet with #vSortedT_ToF and #vSortedPMTID.
        std::vector<int>    vSortedSigFlag; ///< A vector of signal flags (0: bkg, 1: sig) corresponding to each hit
                                            ///< in #vSortedT_ToF.

        NTagMessage msg;        ///< NTag Message printer.
        Verbosity   fVerbosity; ///< Verbosity.
        bool        bData,      /*!< Set \c true for data events, \c false for MC events.
                                     Automatically determined by the run number at NTagIO::CheckMC. */
                    useTMVA,    /*!< Set \c true if using TMVA, otherwise \c false.
                                     Can be set to \c false from command line with option `-noMVA`. */
                    saveTQ;    /*!< Set \c true if saving the ToF-subtracted TQ vectors, otherwise \c false.
                                     Can be set to \c true from command line with option `-saveTQ`. */


        /************************************************************************************************/
        /* Event information variables                                                                  */
        /************************************************************************************************/


        /////////////////////////
        // Data/fit event info //
        /////////////////////////

        // SK data variables
        int    runNo,     ///< Run # of an event.
               subrunNo,  ///< Subrun # of an event.
               eventNo,   ///< Event # of an event.
               nhitac,    ///< Number of OD hits within 1.3 us around the main trigger of an event.
               nqiskz,    ///< Number of all hits recorded in #vTISKZ.
               trgType;   ///< Trigger type. MC: 0, SHE: 1, SHE+AFT: 2
        float  trgOffset, ///< Trigger offset of an event. Default set to 1,000 [ns].
               tDiff,     ///< Time difference from the current event to the previous event. [ms]
               qismsk;    /*!< Total p.e. deposited in ID within 1.3 us around
                               the main trigger of an event. */

        // Prompt vertex variables
        float  pvx,       ///< X position of a prompt-peak vertex (estimated neutron creation point).
               pvy,       ///< Y position of a prompt-peak vertex (estimated neutron creation point).
               pvz,       ///< Z position of a prompt-peak vertex (estimated neutron creation point).
               dWall,     ///< Distance from the prompt vertex to the tank wall.
               evis;      ///< Visible energy (the amount of energy if the peak was from an electron).

        // APFit variables
        int                 apNRings,   ///< Number of rings found by APFit.
                            apNMuE,     ///< Number of mu-e decays found by APFit.
                            apNDecays;  ///< Number of decay electrons?
        std::vector<int>    vAPRingPID; ///< Vector of PID corresponding to each ring found by APFit. [Size: #apNRings]
        std::vector<float>  vAPMom,     /*!< Vector of momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */
                            vAPMomE,    /*!< Vector of e-like momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */
                            vAPMomMu;   /*!< Vector of mu-like momentum corresponding to each ring found by APFit.
                                             [Size: #apNRings] */

        // Variables for neutron capture candidates
        int                 nCandidates,      ///< Number of total found neutron capture candidates.
                            maxN200;          ///< Maximum N200 found in neutron capture candidates.
        float               maxN200Time,      ///< First hit time of the candidate with maximum N200
                            firstHitTime_ToF; /*!< The earliest hit time in an event, subtracted by the ToF
                                                   from the prompt vertex. */
        std::vector<int>    vFirstHitID,      ///< Vector of all indices of the earliest hit in each candidate.
                                              ///< The indices are based off #vSortedT_ToF.
                            vN10n,            ///< Vector of Neut-fit N10.  [Size: #nCandidates]
                            vN1300;           ///< Vector of N1300. [Size: #nCandidates]
        std::vector<float>  vTRMS10n,         ///< Vector of Neut-fit TRMS in 10 ns window. [Size: #nCandidates]
                            vTRMS50,          ///< Vector of TRMS in 10 ns window. [Size: #nCandidates]
                            vReconCTn,        ///< Vector of Neut-fit reconstructed capture time. [Size: #nCandidates]
                            vNvx,             /*!< Vector of X coordinates of Neut-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vNvy,             /*!< Vector of Y coordinates of Neut-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vNvz,             /*!< Vector of Z coordinates of Neut-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vBSvx,            /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vBSvy,            /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vBSvz,            /*!< Vector of X coordinates of BONSAI-fit capture vertex.
                                                   [Size: #nCandidates] */
                            vBSReconCT,       ///< Vector of BONSAI-fit capture time. [ns] [Size: #nCandidates]
                            vBeta14_10,       ///< Vector of &beta;_14's in 10 ns window. [Size: #nCandidates]
                            vBeta14_50,       ///< Vector of &beta;_14's in 50 ns window. [Size: #nCandidates]
                            vTMVAOutput;      ///< Vector of TMVA classifier outputs. [Size: #nCandidates]
                            
        std::vector<std::vector<float>> *vHitRawTimes, ///< Vector of residual hit times. [Size: #nCandidates]
                                        *vHitResTimes; ///< Vector of residual hit times. [Size: #nCandidates]
        std::vector<std::vector<int>>   *vHitCableIDs, ///< Vector of hit cable IDs. [Size: #nCandidates]
                                        *vHitSigFlags; ///< Vector of signal flags. (0: bkg, 1: sig) [Size: #nCandidates]
        


        /////////////////////////
        // MC truth event info //
        /////////////////////////

        // Variables for true neutron capture
        int                 nTrueCaptures; ///< Number of true neutron captures in MC.
        std::vector<int>    vNGamma,       /*!< Vector of the total numbers of emitted gammas in true capture.
                                                [Size: #nTrueCaptures] */
                            vCandidateID;  ///< Vector of matching capture candidate IDs. [Size: #nTrueCaptures]
        std::vector<float>  vTrueCT,       ///< Vector of true capture times. [ns] [Size: #nTrueCaptures]
                            vCapVX,        /*!< Vector of X coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vCapVY,        /*!< Vector of Y coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vCapVZ,        /*!< Vector of Z coordinates of true capture vertices. [cm]
                                                [Size: #nTrueCaptures] */
                            vTotGammaE;    ///< Vector of the total emitted gamma energies. [MeV] [Size: #nTrueCaptures]

        // Variables for neutron capture candidates
        std::vector<int>    vIsGdCapture, /*!< Vector of true-Gd-capture flags. 1 for \c true and 0 for \c false.
                                               [Size: #nCandidates] */
                            vIsCapture,   /*!< Vector of true-capture flags. 1 for \c true and 0 for \c false.
                                               [Size: #nCandidates] */
                            vDoubleCount; /*!< Vector of double-count flags. 1 for \c true and 0 for \c false.
                                               [Size: #nCandidates] */
        std::vector<float>  vTrueCapVX,   /*!< Vector of X coordinate of corresponding true capture vertex.
                                               [Size: #nCandidates] */
                            vTrueCapVY,   /*!< Vector of Y coordinate of corresponding true capture vertex.
                                               [Size: #nCandidates] */
                            vTrueCapVZ,   /*!< Vector of Z coordinate of corresponding true capture vertex.
                                               [Size: #nCandidates] */
                            vCTDiff;      /*!< Vector of capture time differences between a capture candidate and corresponding
                                               true capture. [Size: #nCandidates] */

        // Variables from secondaries
        int                 nSavedSec,  ///< Number of saved secondaries.
                            nAllSec;    ///< Number of all secondaries in the input file.
        std::vector<int> 	vSecPID,    ///< Vector of saved secondary PIDs. [Size: #nSavedSec]
                            vSecIntID,  ///< Vector of saved secondary interaction IDs. [Size: #nSavedSec]
                            vParentPID, ///< Vector of saved parent PIDs. [Size: #nSavedSec]
                            vCapID;     /*!< Vector of true capture indices that generates the secondary.
                                            -1 is pushed back if the secondary is not from a neutron capture.
                                            [Size: #nSavedSec] */
        std::vector<float>  vSecVX,     ///< Vector of X coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecVY,     ///< Vector of Y coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecVZ,     ///< Vector of Z coordinates of secondary vertices. [cm] [Size: #nSavedSec]
                            vSecPX,     /*!< Vector of X-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecPY,     /*!< Vector of Y-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecPZ,     /*!< Vector of Z-direction initial momenta of secondaries. [MeV/c]
                                             [Size: #nSavedSec] */
                            vSecDWall,  /*!< Vector of distances from secondary vertices to the SK barrel. [cm]
                                             [Size: #nSavedSec] */
                            vSecMom,    ///< Vector of initial momenta of secondaries. [MeV/c] [Size: #nSavedSec]
                            vSecT;      ///< Vector of secondary creation times. [ns] [Size: #nSavedSec]

        // Variables for neutrino interaction
        int                 nNInNeutVec,  ///< Number of neutrons in NEUT vectors.
                            neutIntMode,  ///< NEUT interaction mode. @see: \c nemodsel.F of NEUT.
                            nVecInNeut;   ///< Number of NEUT vectors.
        float               neutIntMom;   ///< NEUT interaction momentum.
        std::vector<int>    vNeutVecPID;  ///< Vector of NEUT vector PIDs. [Size: #nVecInNeut]

        // Variables from primary stack
        int                 nVec;    ///< Number of primary vectors in MC.
        float               vecx,    ///< X coordinate of the primary vertex.
                            vecy,    ///< Y coordinate of the primary vertex.
                            vecz;    ///< Z coordinate of the primary vertex.
        std::vector<int>    vVecPID; ///< Vector of primary vector PIDs. [Size: #nVec]
        std::vector<float>  vVecPX,  ///< Vector of X-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecPY,  ///< Vector of Y-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecPZ,  ///< Vector of Z-direction initial momenta of primary vectors. [MeV/c] [Size: #nVec]
                            vVecMom; ///< Vector of initial momenta of primary vectors. [MeV/c] [Size: #nVec]

        /************************************************************************************************/
        
        std::vector<NTagCandidate> vCandidates;
        
        IVecMap iCandidateVarMap;
        FVecMap fCandidateVarMap;
        
    friend class NTagCandidate;
};

#endif