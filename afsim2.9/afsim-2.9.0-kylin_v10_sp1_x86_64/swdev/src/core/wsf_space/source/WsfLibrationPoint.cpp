// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfLibrationPoint.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "UtEarth.hpp"
#include "UtJupiter.hpp"
#include "UtMoon.hpp"
#include "UtNewtonRaphson.hpp"
#include "UtSun.hpp"

namespace wsf
{
namespace space
{

LibrationPoint::LibrationPoint(System aSystem)
   : mSystem{aSystem}
{
   mPrimaryBody   = GetPrimaryBody(mSystem);
   mSecondaryBody = GetSecondaryBody(mSystem);
   double mu1     = mPrimaryBody->GetGravitationalParameter();
   double mu2     = mSecondaryBody->GetGravitationalParameter();
   mMuStar        = mu2 / (mu1 + mu2);
   ComputeGamma();
}

//! Get the ECI position of a libration point.
//!
//! Returns the position of the given @p aPoint at the given @p aEpoch in
//! ECI coordinates.
//!
//! \param aPoint - Identifier for which point's position is needed.
//! \param aEpoch - The date and time at which the point's position is needed.
//! \return       - The ECI position of the point.
UtVec3d LibrationPoint::GetPositionECI(Point aPoint, const UtCalendar& aEpoch) const
{
   UpdateTransform(aEpoch);
   UtVec3d nondimPos = GetPointLocation(aPoint);
   nondimPos.Multiply(mPosDiff.Magnitude());
   return mTransform * nondimPos + mPosOriginECI;
}

//! Get the ECI velocity of a libration point.
//!
//! Returns the velocity of the given @p aPoint at the given @p aEpoch in
//! ECI coordinates.
//!
//! \param aPoint - Identifier for which point's velocity is needed.
//! \param aEpoch - The date and time at which the point's velocity is needed.
//! \returns      - The ECI velocity of the point.
UtVec3d LibrationPoint::GetVelocityECI(Point aPoint, const UtCalendar& aEpoch) const
{
   UpdateTransform(aEpoch);
   UtVec3d pos = GetPointLocation(aPoint);
   UtVec3d vel(pos);

   double rsMag = mPosDiff.Magnitude();
   pos.Multiply(rsMag);
   vel.Multiply(mVelDiff.DotProduct(mPosDiff) / rsMag);

   return mTransformDot * pos + mTransform * vel + mVelOriginECI;
}

//! Transform a position into the rotating frame.
//!
//! This will transform the given @p aPositionECI into the rotating frame for the
//! system at the given @p aEpoch.
//!
//! \param aEpoch       - The epoch at which to perform the transformation.
//! \param aPositionECI - The ECI position to transform into the rotating frame.
//! \returns            - The corresponding RF position.
UtVec3d LibrationPoint::TransformPositionECItoRF(const UtCalendar& aEpoch, const UtVec3d& aPositionECI) const
{
   UpdateTransform(aEpoch);
   return mTransform.TransposeMultiply(aPositionECI - mPosOriginECI);
}

//! Transform a position from the rotating frame.
//!
//! This will transform the given @p aPositionRF into the ECI frame. The provided
//! @p aPositionRF is taken to be valid at the given @p aEpoch.
//!
//! \param aEpoch      - The epoch at which to perform the transformation.
//! \param aPositionRF - The RF position to transform into the ECI frame.
//! \returns           - The corresponding ECI position.
UtVec3d LibrationPoint::TransformPositionRFtoECI(const UtCalendar& aEpoch, const UtVec3d& aPositionRF) const
{
   UpdateTransform(aEpoch);
   return mTransform * aPositionRF + mPosOriginECI;
}

//! Transform a velocity into the rotating frame.
//!
//! This will transform the given @p aVelocityECI, taken to be for an object at the
//! given @p aPositionECI, and at the given @p aEpoch, into the rotating frame.
//!
//! \param aEpoch       - The epoch at which to perform the transformation.
//! \param aPositionECI - The ECI position of the object for which the given @p aVelocityECI applies.
//! \param aVelocityECI - The ECI velocity to transform.
//! \returns            - The corresponding RF velocity.
UtVec3d LibrationPoint::TransformVelocityECItoRF(const UtCalendar& aEpoch,
                                                 const UtVec3d&    aPositionECI,
                                                 const UtVec3d&    aVelocityECI) const
{
   UpdateTransform(aEpoch);
   auto tT = mTransform.GetTranspose();
   return tT * (aVelocityECI - mVelOriginECI) - tT * mTransformDot * tT * (aPositionECI - mPosOriginECI);
}

//! Transform a velocity from the rotating frame.
//!
//! This will transform the given @p aVelocityRF, taken to be for an object at
//! the given @p aPositionRF, and at the given @p aEpoch, into the ECI frame.
//!
//! \param aEpoch      - The epoch at which to perform the transformation.
//! \param aPositionRF - The RF position of the object for which the given @p aVelocityRF applies.
//! \param aVelocityRF - The RF velocity to transform.
//! \returns           - The corresponding RF velocity.
UtVec3d LibrationPoint::TransformVelocityRFtoECI(const UtCalendar& aEpoch,
                                                 const UtVec3d&    aPositionRF,
                                                 const UtVec3d&    aVelocityRF) const
{
   UpdateTransform(aEpoch);
   return mTransform * aVelocityRF + mTransformDot * aPositionRF + mVelOriginECI;
}

double LibrationPoint::GetSystemPeriod() const
{
   double retval{0.0};
   switch (mSystem)
   {
   case System::cSUN_EARTH:
      retval = 31536000.0; // Approximately one year
      break;
   case System::cEARTH_MOON:
      retval = 2419200.0; // 28 days
      break;
   case System::cSUN_JUPITER:
      retval = 4332.59 * 86400.0; // ~ 11.8 years
      break;
   case System::cUNKNOWN: // Intentional fall-trhough
   default:
      retval = 31536000.0; // Approximately one year
      break;
   }
   return retval;
}

LibrationPoint::System LibrationPoint::GetSystemFromIdentifier(const std::string& aIdentifier)
{
   System retval{System::cUNKNOWN};
   if (aIdentifier == "earth_moon")
   {
      retval = System::cEARTH_MOON;
   }
   else if (aIdentifier == "sun_earth")
   {
      retval = System::cSUN_EARTH;
   }
   else if (aIdentifier == "sun_jupiter")
   {
      retval = System::cSUN_JUPITER;
   }
   return retval;
}

std::string LibrationPoint::GetIdentifierFromSystem(System aSystem)
{
   std::string retval;
   switch (aSystem)
   {
   case System::cEARTH_MOON:
      retval = "earth_moon";
      break;
   case System::cSUN_EARTH:
      retval = "sun_earth";
      break;
   case System::cSUN_JUPITER:
      retval = "sun_jupiter";
      break;
   case System::cUNKNOWN: // Intentional fall-through
   default:
      retval = "unknown_system";
      break;
   }
   return retval;
}

std::vector<std::string> LibrationPoint::GetSystemIdentifiers()
{
   std::vector<std::string> retval;
   retval.emplace_back(GetIdentifierFromSystem(System::cEARTH_MOON));
   retval.emplace_back(GetIdentifierFromSystem(System::cSUN_EARTH));
   retval.emplace_back(GetIdentifierFromSystem(System::cSUN_JUPITER));
   return retval;
}

LibrationPoint::Point LibrationPoint::GetPointFromIdentifier(const std::string& aIdentifier)
{
   Point retval{Point::cUNKNOWN};
   if (aIdentifier == "l1")
   {
      retval = Point::cL1;
   }
   else if (aIdentifier == "l2")
   {
      retval = Point::cL2;
   }
   else if (aIdentifier == "l3")
   {
      retval = Point::cL3;
   }
   else if (aIdentifier == "l4")
   {
      retval = Point::cL4;
   }
   else if (aIdentifier == "l5")
   {
      retval = Point::cL5;
   }
   return retval;
}

std::string LibrationPoint::GetIdentifierFromPoint(Point aPoint)
{
   std::string retval;
   switch (aPoint)
   {
   case Point::cL1:
      retval = "l1";
      break;
   case Point::cL2:
      retval = "l2";
      break;
   case Point::cL3:
      retval = "l3";
      break;
   case Point::cL4:
      retval = "l4";
      break;
   case Point::cL5:
      retval = "l5";
      break;
   case Point::cUNKNOWN:
   default:
      retval = "unknown_point";
      break;
   }
   return retval;
}

std::vector<std::string> LibrationPoint::GetPointIdentifiers()
{
   std::vector<std::string> retval;
   retval.emplace_back(GetIdentifierFromPoint(Point::cL1));
   retval.emplace_back(GetIdentifierFromPoint(Point::cL2));
   retval.emplace_back(GetIdentifierFromPoint(Point::cL3));
   retval.emplace_back(GetIdentifierFromPoint(Point::cL4));
   retval.emplace_back(GetIdentifierFromPoint(Point::cL5));
   return retval;
}

bool LibrationPoint::ValidLibrationPoint(System aSystem, Point aPoint)
{
   bool systemCheck;
   switch (aSystem)
   {
   case System::cEARTH_MOON:  // Intentional fall-through
   case System::cSUN_EARTH:   //
   case System::cSUN_JUPITER: //
      systemCheck = true;
      break;
   case System::cUNKNOWN: // Intentional fall-through
   default:               //
      systemCheck = false;
      break;
   }

   bool pointCheck;
   switch (aPoint)
   {
   case Point::cL1: // Intentional fall-through
   case Point::cL2: //
   case Point::cL3: //
   case Point::cL4: //
   case Point::cL5: //
      pointCheck = true;
      break;
   case Point::cUNKNOWN: // Intentional fall-through
   default:              //
      pointCheck = false;
      break;
   }
   return systemCheck && pointCheck;
}

std::unique_ptr<ut::CentralBody> LibrationPoint::GetPrimaryBody(System aSystem)
{
   std::unique_ptr<ut::CentralBody> retvalPtr{nullptr};
   switch (aSystem)
   {
   case System::cSUN_EARTH:
      retvalPtr = ut::make_unique<ut::Sun>();
      break;
   case System::cEARTH_MOON:
      retvalPtr = ut::make_unique<ut::EarthEGM96>();
      break;
   case System::cSUN_JUPITER:
      retvalPtr = ut::make_unique<ut::Sun>();
      break;
   case System::cUNKNOWN: // Intentional fall-through
   default:
      throw std::runtime_error{"Unknown Libration point system."};
      break;
   }
   return retvalPtr;
}

std::unique_ptr<ut::CentralBody> LibrationPoint::GetSecondaryBody(System aSystem)
{
   std::unique_ptr<ut::CentralBody> retvalPtr{nullptr};
   switch (aSystem)
   {
   case System::cSUN_EARTH:
      retvalPtr = ut::make_unique<ut::EarthEGM96>();
      break;
   case System::cEARTH_MOON:
      retvalPtr = ut::make_unique<ut::Moon>();
      break;
   case System::cSUN_JUPITER:
      retvalPtr = ut::make_unique<ut::Jupiter>();
      break;
   case System::cUNKNOWN: // Intentional fall-through
   default:
      throw std::runtime_error{"Unknown Libration point system."};
      break;
   }
   return retvalPtr;
}

//! This method computes the non-dimensional positions of the colinear libration points.
//!
//! There is no closed form expression for these points, so one must resort to root-finding.
//! See Vallado, 4th ed, pp. 971-972, Eq 12-18 and those on the top of p. 972.
void LibrationPoint::ComputeGamma()
{
   static constexpr double cTOLERANCE = 1.0e-14;
   double                  guess      = pow(mMuStar / (3.0 * (1.0 - mMuStar)), 1.0 / 3.0);
   mGamma1                            = ut::NewtonRaphson1D([this](double aGamma) { return F_Gamma1(aGamma); },
                                 [this](double aGamma) { return F_PrimeGamma1(aGamma); },
                                 guess,
                                 cTOLERANCE)
                .first;
   mGamma2 = ut::NewtonRaphson1D([this](double aGamma) { return F_Gamma2(aGamma); },
                                 [this](double aGamma) { return F_PrimeGamma2(aGamma); },
                                 guess,
                                 cTOLERANCE)
                .first;
   mGamma3 = ut::NewtonRaphson1D([this](double aGamma) { return F_Gamma3(aGamma); },
                                 [this](double aGamma) { return F_PrimeGamma3(aGamma); },
                                 1.0,
                                 cTOLERANCE)
                .first;
}

double LibrationPoint::F_Gamma1(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   double g5 = g3 * g2;
   return g5 - (3.0 - mMuStar) * g4 + (3.0 - 2.0 * mMuStar) * g3 - mMuStar * g2 + 2.0 * mMuStar * aGamma - mMuStar;
}

double LibrationPoint::F_PrimeGamma1(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   return 5.0 * g4 - 4.0 * (3.0 - mMuStar) * g3 + 3.0 * (3.0 - 2.0 * mMuStar) * g2 - 2.0 * mMuStar * aGamma + 2.0 * mMuStar;
}

double LibrationPoint::F_Gamma2(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   double g5 = g3 * g2;
   return g5 + (3.0 - mMuStar) * g4 + (3.0 - 2.0 * mMuStar) * g3 - mMuStar * g2 - 2.0 * mMuStar * aGamma - mMuStar;
}

double LibrationPoint::F_PrimeGamma2(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   return 5.0 * g4 + 4.0 * (3.0 - mMuStar) * g3 + 3.0 * (3.0 - 2.0 * mMuStar) * g2 - 2.0 * mMuStar * aGamma - 2.0 * mMuStar;
}

double LibrationPoint::F_Gamma3(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   double g5 = g3 * g2;
   return g5 + (2.0 + mMuStar) * g4 + (1.0 + 2.0 * mMuStar) * g3 - (1.0 - mMuStar) * g2 -
          2.0 * (1.0 - mMuStar) * aGamma - (1.0 - mMuStar);
}

double LibrationPoint::F_PrimeGamma3(double aGamma) const
{
   double g2 = aGamma * aGamma;
   double g3 = g2 * aGamma;
   double g4 = g2 * g2;
   return 5.0 * g4 + 4.0 * (2.0 + mMuStar) * g3 + 3.0 * (1.0 + 2.0 * mMuStar) * g2 - 2.0 * (1.0 - mMuStar) * aGamma -
          2.0 * (1.0 - mMuStar);
}

//! Get the location of the libration points in the non-dimensionalized rotating frame.
//!
//! This method returns the coordinates of the libration points in the rotating frame
//! with non-dimensionalized units.
UtVec3d LibrationPoint::GetPointLocation(Point aPoint) const
{
   switch (aPoint)
   {
   case Point::cL1:
      return UtVec3d{1.0 - mGamma1, 0.0, 0.0};
      break;
   case Point::cL2:
      return UtVec3d{1.0 + mGamma2, 0.0, 0.0};
      break;
   case Point::cL3:
      return UtVec3d{-mGamma3, 0.0, 0.0};
      break;
   case Point::cL4:
      return UtVec3d{0.5, sqrt(3.0) / 2.0, 0.0};
      break;
   case Point::cL5:
      return UtVec3d{0.5, -sqrt(3.0) / 2.0, 0.0};
      break;
   case Point::cUNKNOWN: // Intentional fall-through
   default:
      throw std::runtime_error{"Unknown Libration point selection."};
      break;
   }
   return UtVec3d{};
}

void LibrationPoint::UpdateTransform(const UtCalendar& aEpoch) const
{
   // These expressions follow the development in the GMAT Mathematical Specification
   // section 3.4.
   if (aEpoch != mCurrentEpoch)
   {
      mPosOriginECI = mPrimaryBody->GetLocationECI(aEpoch);
      mPosDiff      = mSecondaryBody->GetLocationECI(aEpoch) - mPosOriginECI;
      mVelOriginECI = mPrimaryBody->GetVelocityECI(aEpoch);
      mVelDiff      = mSecondaryBody->GetVelocityECI(aEpoch) - mVelOriginECI;

      // The rotating frame x axis points from the primary to the secondary.
      UtVec3d xHat(mPosDiff);
      double  rsMag = xHat.Normalize();

      // The rotating frame z axis is along the orbital angular momentum of the
      // secondary with respect to the primary.
      UtVec3d zHat;
      zHat.CrossProduct(mPosDiff, mVelDiff).Normalize();

      // The rotating frame y axis completes the right-handed coordinate system.
      UtVec3d yHat;
      yHat.CrossProduct(zHat, xHat);

      // Set the rotation matrix enacting the ECI->RF transform. This highlights
      // one interpretation of the rotation matrix: it is the representation of the
      // new basis vectors in the old basis.
      SetMatrixFromVectors(mTransform, xHat, yHat, zHat);

      // This is the derivative of the xHat vector.
      UtVec3d xHatDot = mVelDiff * (1.0 / rsMag) - xHat * (xHat.DotProduct(mVelDiff) / rsMag);

      // For simplicity, assume that the acceleration of the secondary is perpendicular to
      // the position of the secondary (r_s x a_s = 0). This leads to a zero time
      // derivative of the z basis vector for the rotating frame.
      UtVec3d zHatDot{0.0, 0.0, 0.0};

      // This is the derivative of the yHat vector, subject to the assumption that the
      // zHat vector does not change with time.
      UtVec3d yHatDot;
      yHatDot.CrossProduct(zHat, xHatDot);

      SetMatrixFromVectors(mTransformDot, xHatDot, yHatDot, zHatDot);
   }
}

void LibrationPoint::SetMatrixFromVectors(UtMat3d&       aMatrix,
                                          const UtVec3d& aColumn0,
                                          const UtVec3d& aColumn1,
                                          const UtVec3d& aColumn2)
{
   for (int i = 0; i < 3; ++i)
   {
      aMatrix[3 * i + 0] = aColumn0[i];
      aMatrix[3 * i + 1] = aColumn1[i];
      aMatrix[3 * i + 2] = aColumn2[i];
   }
}

} // namespace space
} // namespace wsf
