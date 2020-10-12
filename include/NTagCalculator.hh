#ifndef NTAGCALC_HH
#define NTAGCALC_HH 1

#include <vector>

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
 * @brief Gets number of hits within \p tWidth [ns] starting from index \p startIndex.
 * @param T A vector of PMT hit times. [ns]
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The number of hits within \p tWidth [ns] starting from index \p startIndex.
 */
int GetNhitsFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);
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
float GetQSumFromStartIndex(const std::vector<float>& T, const std::vector<float>& Q,
                                            int startIndex, float tWidth);
/**
 * @brief Gets the RMS value of a hit cluster extracted from a hit-time vector.
 * @param T A vector of PMT hit times. [ns]
 * @param startIndex The \p T index of the first hit in a hit cluster or a capture candidate to extract.
 * @param tWidth The width of the time window [ns] to count hits within.
 * @return The RMS value of the extracted hit cluster from the input hit-tme vector \p T.
 */
float GetTRMSFromStartIndex(const std::vector<float>& T, int startIndex, float tWidth);

#endif