/**
 * @file OperationalEnvironmentEvaluator.hpp
 * @brief Deterministic geometry, time, attitude, and protected-band checks.
 */

#ifndef NRM_OPERATIONAL_ENVIRONMENT_EVALUATOR_HPP
#define NRM_OPERATIONAL_ENVIRONMENT_EVALUATOR_HPP

#include <algorithm>
#include <cmath>

#include "nrm/NetworkResourceTypes.hpp"

namespace nrm
{
class OperationalEnvironmentEvaluator
{
public:
   static bool Contains(const OperationalArea& aArea, const GeoPoint& aPoint)
   {
      if (aArea.points.size() < 3) return false;
      bool inside = false;
      for (std::size_t i = 0, j = aArea.points.size() - 1;
           i < aArea.points.size(); j = i++)
      {
         const GeoPoint& first = aArea.points[j];
         const GeoPoint& second = aArea.points[i];
         if (OnSegment(first, second, aPoint)) return true;
         const bool crosses = (second.latitudeDeg > aPoint.latitudeDeg) !=
                              (first.latitudeDeg > aPoint.latitudeDeg);
         if (crosses)
         {
            const double longitude =
               (first.longitudeDeg - second.longitudeDeg) *
                  (aPoint.latitudeDeg - second.latitudeDeg) /
                  (first.latitudeDeg - second.latitudeDeg) +
               second.longitudeDeg;
            if (aPoint.longitudeDeg < longitude) inside = !inside;
         }
      }
      return inside;
   }

   static bool Covers(double aAvailableFrom, double aAvailableUntil,
                      double aTaskFrom, double aTaskUntil)
   {
      return std::isfinite(aAvailableFrom) && std::isfinite(aAvailableUntil) &&
             std::isfinite(aTaskFrom) && std::isfinite(aTaskUntil) &&
             aAvailableUntil >= aAvailableFrom && aTaskUntil >= aTaskFrom &&
             aTaskFrom >= aAvailableFrom && aTaskUntil <= aAvailableUntil;
   }

   static bool HeadingWithin(double aActualDeg, double aRequiredDeg,
                             double aToleranceDeg)
   {
      if (!std::isfinite(aActualDeg) || !std::isfinite(aRequiredDeg) ||
          !std::isfinite(aToleranceDeg) || aToleranceDeg < 0.0) return false;
      const double difference = std::abs(Normalize(aActualDeg) - Normalize(aRequiredDeg));
      return std::min(difference, 360.0 - difference) <= aToleranceDeg;
   }

   static bool BandsOverlap(double aCenterHz, double aBandwidthHz,
                            double aProtectedCenterHz, double aProtectedBandwidthHz)
   {
      if (!std::isfinite(aCenterHz) || !std::isfinite(aBandwidthHz) ||
          !std::isfinite(aProtectedCenterHz) ||
          !std::isfinite(aProtectedBandwidthHz) || aBandwidthHz < 0.0 ||
          aProtectedBandwidthHz < 0.0) return false;
      return std::abs(aCenterHz - aProtectedCenterHz) <=
             (aBandwidthHz + aProtectedBandwidthHz) / 2.0;
   }

   static double BearingDeg(const GeoPoint& aSource, const GeoPoint& aDestination)
   {
      const double toRadians = 3.14159265358979323846 / 180.0;
      const double latitude1 = aSource.latitudeDeg * toRadians;
      const double latitude2 = aDestination.latitudeDeg * toRadians;
      const double deltaLongitude =
         (aDestination.longitudeDeg - aSource.longitudeDeg) * toRadians;
      const double y = std::sin(deltaLongitude) * std::cos(latitude2);
      const double x = std::cos(latitude1) * std::sin(latitude2) -
                       std::sin(latitude1) * std::cos(latitude2) *
                          std::cos(deltaLongitude);
      return Normalize(std::atan2(y, x) / toRadians);
   }

private:
   static double Normalize(double aHeading)
   {
      double value = std::fmod(aHeading, 360.0);
      return value < 0.0 ? value + 360.0 : value;
   }

   static bool OnSegment(const GeoPoint& aFirst, const GeoPoint& aSecond,
                         const GeoPoint& aPoint)
   {
      const double cross = (aPoint.longitudeDeg - aFirst.longitudeDeg) *
                              (aSecond.latitudeDeg - aFirst.latitudeDeg) -
                           (aPoint.latitudeDeg - aFirst.latitudeDeg) *
                              (aSecond.longitudeDeg - aFirst.longitudeDeg);
      if (std::abs(cross) > 1.0e-10) return false;
      return aPoint.latitudeDeg >= std::min(aFirst.latitudeDeg, aSecond.latitudeDeg) - 1.0e-10 &&
             aPoint.latitudeDeg <= std::max(aFirst.latitudeDeg, aSecond.latitudeDeg) + 1.0e-10 &&
             aPoint.longitudeDeg >= std::min(aFirst.longitudeDeg, aSecond.longitudeDeg) - 1.0e-10 &&
             aPoint.longitudeDeg <= std::max(aFirst.longitudeDeg, aSecond.longitudeDeg) + 1.0e-10;
   }
};
}

#endif
