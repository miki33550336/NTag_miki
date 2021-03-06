/*******************************************
*
* @file NTagCalculator.hh
*
* @brief Defines calculator functions which
* are commonly used throughout NTag.
*
********************************************/

#ifndef NTAGCALCULATOR_HH
#define NTAGCALCULATOR_HH 1

#include <algorithm>
#include <array>
#include <map>
#include <vector>

#include <TString.h>
#include <TVector3.h>

static std::map<int, TString> pidMap;
static std::map<int, TString> intMap;

/**
 * @brief Get dot produt of two arrays.
 * @param vec1 A size-3 float array.
 * @param vec2 A size-3 float array.
 * @return The dot product of the given two arrays.
 */
float Dot(const float vec1[3], const float vec2[3]);

/**
 * @brief Get norm of a size-3 float array.
 * @param vec A size-3 float array.
 * @return The norm of the given array.
 */
float Norm(const float vec[3]);

/**
 * @brief Get norm of the vector with given x, y, z coordinates.
 * @param x X coordinate of a vector.
 * @param y Y coordinate of a vector.
 * @param z Z coordinate of a vector.
 * @return The norm of the vector with given coordinates.
 */
float Norm(float x, float y, float z);

/**
 * @brief Get distance between two points specified by the two given size-3 float arrays.
 * @param vec1 A size-3 float array of coordinates of a vector 1.
 * @param vec2 A size-3 float array of coordinates of a vector 2.
 * @return The distance between vector 1 and 2.
 */
float GetDistance(const float vec1[3], const float vec2[3]);

/**
 * @brief Get i-th Legendre polynomial P_i(x) evaluated at x.
 * @param i The order of Legendre polynomial.
 * @param x The x value to evaluate the polynomial.
 * @return The i-th Legendre polynomial P_i(x) evaluated at x.
 */
float GetLegendreP(int i, float& x);

/**
 * @brief Gets the RMS value of a hit-time vector.
 * @param T A vector of PMT hit times. [ns]
 * @return The RMS value of a hit-time vector \p T.
 */
float GetTRMS(const std::vector<float>& T);

/**
 * @brief Slice \c nElements elements from a given vector \c vec, starting from index \c startIndex.
 * @param vec A vector to slice.
 * @param startIndex The starting index of the output.
 * @param nElements Number of elements in the sliced output vector.
 * @param indexOrder The shuffling array. If given, all indices that are used to slice will be shuffled
 * so that the output will be \c vec[indexOrder[indices]] rather than \c vec[indices].
 * @return Sliced vector \c vec[startIndex:startIndex+nElements].
 */
template <typename T>
std::vector<T> SliceVector(const std::vector<T>& vec, int startIndex, int nElements, int* indexOrder=0)
{
    std::vector<T> slicedVec;

    for (int i = startIndex; i < startIndex + nElements; i++) {
        if (indexOrder) {
            slicedVec.push_back(vec[indexOrder[i]]);
        }
        else
            slicedVec.push_back(vec[i]);
    }

    return slicedVec;
}

/**
 * @brief Slice a sorted time vector starting from index \p startIndex within \p tWidth [ns].
 * @param sortedT A vector of PMT hit times. [ns] Must be sorted in ascending order!
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The number of hits within \p tWidth [ns] starting from index \p startIndex.
 */
std::vector<float> GetVectorFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth);

/**
 * @brief Gets number of hits within \p tWidth [ns] starting from index \p startIndex.
 * @param sortedT A vector of PMT hit times. [ns] Must be sorted in ascending order!
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The number of hits within \p tWidth [ns] starting from index \p startIndex.
 */
int GetNhitsFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth);

/**
 * @brief Gets the summed charge [p.e.] of a hit cluster or a capture candidate, starting from
 * index \p startIndex within a time window with \p tWidth [ns].
 * @param sortedT A vector of PMT hit times. [ns] Must be sorted in ascending order!
 * @param Q A vector of deposited charge. [p.e.] Each element of \p Q must correspond to the element of
 * \p T with the same index.
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The summed charge [p.e.] of a hit cluster or a capture candidate from \p startIndex within
 * \p tWidth [ns].
 */
float GetQSumFromStartIndex(const std::vector<float>& sortedT, const std::vector<float>& Q,
                                            int startIndex, float tWidth);

/**
 * @brief Gets the RMS value of a hit cluster extracted from a hit-time vector.
 * @param sortedT A vector of PMT hit times. [ns] Must be sorted in ascending order!
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate to extract.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The RMS value of the extracted hit cluster from the input hit-tme vector \p T.
 */
float GetTRMSFromStartIndex(const std::vector<float>& sortedT, int startIndex, float tWidth);

/**
 * @brief Gets number of hits within \p tWidth [ns] whose center comes at time \p centerTime [ns].
 * @param T A vector of PMT hit times. [ns]
 * @param centerTime The exact time [ns] to search for hits around.
 * @param tWidth The width of the time window [ns] to count hits within. \p centerTime comes in the center
 * of this \p tWidth.
 * @return The number of hits within \p tWidth [ns] whose center comes at time \p centerTime [ns].
 */
int GetNhitsFromCenterTime(const std::vector<float>& T, float centerTime, float tWidth);

/**
 * @brief Calculates the mean direction of the hits.
 * @param vertex An array of vertex coordinates. [cm]
 * @param PMTID A vector of PMT cable IDs.
 * @return The \c TVector3 instance of the the averaged direction vector from \c vertex to each PMT.
 */
TVector3 GetMeanDirection(const std::vector<int>& PMTID, float vertex[3]);

/**
 * @brief Calculates distance to the wall in the averaged direction from a vertex to hit PMTs.
 * @param vertex An array of vertex coordinates. [cm]
 * @param PMTID A vector of PMT cable IDs.
 * @return The distance to the wall in the averaged direction from \c vertex to each PMT.
 */
float GetDWallInMeanDirection(const std::vector<int>& PMTID, float vertex[3]);

/**
 * @brief Calculates the mean angle in the averaged direction from a vertex to hit PMTs.
 * @param vertex An array of vertex coordinates. [cm]
 * @param PMTID A vector of PMT cable IDs.
 * @return The mean angle in the averaged direction from a vertex to hit PMTs.
 */
float GetMeanAngleInMeanDirection(const std::vector<int>& PMTID, float vertex[3]);

/**
 * @brief Calculates an opening angle given three unit vectors.
 * @param uA A unit vector.
 * @param uB A unit vector.
 * @param uC A unit vector.
 * @return The opening angle (deg) defined by `uA`, `uB`, and `uC`.
 */
float GetOpeningAngle(TVector3 uA, TVector3 uB, TVector3 uC);

/**
 * @brief Calculates the mean of the value distribution of the given vector.
 * @param vec The vector to calculate mean.
 * @return The mean of the value distribution of \c vec.
 */
template <typename T>
float GetMean(const std::vector<T>& vec)
{
    return std::accumulate(vec.begin(), vec.end(), 0.0) / (float)(vec.size());
}

/**
 * @brief Calculates the median of the value distribution of the given vector.
 * @param vec The vector to calculate median.
 * @return The median of the value distribution of \c vec.
 */
template <typename T>
float GetMedian(const std::vector<T>& vec)
{
    std::vector<T> v = vec;
    std::sort(v.begin(), v.end());
    int N = v.size();

    if (N % 2 == 0)
        return (v[N/2 - 1] + v[N/2]) / 2.;
    else
        return v[N/2];
}

/**
 * @brief Calculates the skewness of the value distribution of the given vector.
 * @param vec The vector to calculate skewness.
 * @return The skewness of the value distribution of \c vFloat.
 */
template <typename T>
float GetSkew(const std::vector<T>& vec)
{
    float m3 = 0;
    float mean = GetMean(vec);
    float N = vec.size();

    for (auto const& value: vec) {
        m3 += pow((value - mean), 3.);
    }
    m3 /= N;

    return m3 / pow(GetTRMS(vec), 1.5);
}

/**
 * @brief Calculates the mean, stdev, and skewness of opening angles from input vertex to given PMT positions.
 * @param PMTID A vector of hit PMT cable IDs.
 * @param vertex Input vertex.
 * @return A size-2 array of mean (element 0) and standard deviation (element 1).
 */
std::array<float, 4> GetOpeningAngleStats(const std::vector<int>& PMTID, float vertex[3]);

/**
 * @brief Returns particle name given a PDG encoding.
 * @param pid The PDG encoding of a particle.
 * @return Particle name in `TString`.
 */
TString GetParticleName(int pid);

/**
 * @brief Returns interaction name given a Geant3 interaction code.
 * @param lmec The Geant3 interaction code of an interaction.
 * @return Interaction name in `TString`.
 */
TString GetInteractionName(int lmec);

#endif