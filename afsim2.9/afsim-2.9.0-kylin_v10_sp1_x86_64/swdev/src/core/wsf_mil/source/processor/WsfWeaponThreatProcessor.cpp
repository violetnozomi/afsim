// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
//****************************************************************************
// Updated by Infoscitex, a DCS Company
//****************************************************************************

#include "WsfWeaponThreatProcessor.hpp"

#include <algorithm> // std::sort
#include <string>

#include "UtCast.hpp"
#include "WsfPkTable.hpp"
#include "WsfSphericalLethality.hpp"
class UtEntity;
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInputBlock.hpp"
#include "UtLineSegment.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtRandom.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfClassificationProcessor.hpp"
#include "WsfCovariance.hpp"
#include "WsfDraw.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMil.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfStringId.hpp"
#include "WsfThreatProcessor.hpp"
#include "WsfTrack.hpp"
#include "WsfTrackManager.hpp"
#include "WsfTrackMessage.hpp"
#include "WsfTrackNotifyMessage.hpp"

using ClassifierReport     = ClassificationReport;
using ClassifierReportList = ClassificationReportList;

using PkSearchParameters  = WsfWeaponThreatProcessor::PkSearchParameters;
using WsfThreatReport     = WsfWeaponThreatProcessor::WsfThreatReport;
using WsfThreatReportList = WsfWeaponThreatProcessor::WsfThreatReportList;
using ThreatEvaluator     = WsfWeaponThreatProcessor::ThreatEvaluator;

using ThreatReportComparitor = WsfWeaponThreatProcessor::ThreatReportComparitor;

namespace
{
void WeaponThreatProcessorThrowLocal(bool aTruth, const char* aMessage = "")
{
   if (!aTruth)
   {
      auto out = ut::log::error() << "WeaponThreatProcessorThrowLocal: assertion failed.";
      out.AddNote() << "WeaponThreatProcessorThrowLocal: " << aMessage;
      throw UtException("WeaponThreatProcessorThrowLocal: " + std::string(aMessage));
   }
}
} // namespace

// ---- Local structures and classes ----

//!
struct WeaponDatum
{
   WsfStringId mWeaponTypeId;
   double      mProbabilityOnBoard;
};

//!
struct WeaponLoad
{
   WsfStringId              mEntityId;
   std::vector<WeaponDatum> mWeaponList;
};

/**! Stores normalized kinematics for either a platform or a track.
 */
struct StateReport
{
   WsfStringId mPlatformType;
   double      mLocation[3];
   double      mVelocity[3];
   double      mSpeed;
};

//!
struct Geometry
{
   double mTimeCpa;
   double mLocationCPAShooter[3];
   double mLocationCPAOwnship[3];
   double mLocationCPAOwnshipNorm[3];
};

//----------------------------------------------
// Utility functions
//----------------------------------------------

namespace
{
// string GetTypeName(const WeaponDatum& aWeaponDatum)
//{
//    return aWeaponDatum.mWeaponTypeId.GetString();
// }

// string GetEntityName(const WeaponLoad& aWeaponLoad)
//{
//    return aWeaponLoad.mEntityId.GetString();
// }

void GetState(WsfPlatform& aOwnship, StateReport& aOwnshipState)
{
   aOwnship.GetLocationWCS(aOwnshipState.mLocation);
   aOwnship.GetVelocityWCS(aOwnshipState.mVelocity);
   aOwnshipState.mSpeed = // UtVec3d::Magnitude(shooter.mVelocity);
      aOwnship.GetSpeed();
   aOwnshipState.mPlatformType = aOwnship.GetType(); // LBM can we do this? cheating?
   // I was assuming a moving ownship, but the SA test has hover ;}
   ////WeaponThreatProcessorThrowLocal(aOwnshipState.mSpeed > 1.0);
}
void GetState(const WsfTrack& aShooter, StateReport& aShooterState)
{
   bool useTruth = false;
   aShooter.GetLocationWCS(aShooterState.mLocation);
   aShooter.GetVelocityWCS(aShooterState.mVelocity);
   // Need speed just to normalize track heading
   aShooterState.mSpeed = // UtVec3d::Magnitude(shooter.mVelocity);
      aShooter.GetSpeed();
   aShooterState.mPlatformType = aShooter.GetTargetType(); // LBM can we do this? cheating?
   WeaponThreatProcessorThrowLocal(aShooterState.mSpeed > 1.0);
   if (useTruth)
   {
      // WsfPlatform* tgtPlatformPtr = WsfSimulation::GetPlatformByName(aShooter.GetTargetName());
      WsfPlatform* tgtPlatformPtr = aShooter.GetSimulation()->GetPlatformByName(aShooter.GetTargetName());
      if (tgtPlatformPtr != nullptr)
      {
         GetState(*tgtPlatformPtr, aShooterState);
      }
   }
}
// void Draw(const StateReport& aState)
//{
//    static WsfDraw mDraw;
//    //ShowPoint(impactLocWCS, 1.0, 0.5, 0.0);
//    double       aRedValue = 1.0;
//    double       aGreenValue = 0.0;
//    double       aBlueValue = 0.0;
//    int          aPointSize = 6;

//   mDraw.SetId(mDraw.GetNewId());
//   mDraw.SetPointSize(aPointSize);
//   mDraw.SetDuration(WsfDraw::cFOREVER);
//   mDraw.SetColor(aRedValue, aGreenValue, aBlueValue);
//   mDraw.BeginPoints();
//   mDraw.VertexWCS(aState.mLocation);
//   mDraw.VertexWCS(aState.mLocation);// Need two point because of VESPA bug
//   mDraw.End();
//}
void Extrapolate(const StateReport& aState, double aTimeDuration, double aLocationLos[3])
{
   for (int i = 0; i < 3; ++i)
   {
      aLocationLos[i] = aTimeDuration * aState.mVelocity[i] + aState.mLocation[i];
   }
}

void FindCrossingECS(const StateReport& aShooter, const StateReport& aOwnship, Geometry& aGeometry)
{
   WeaponThreatProcessorThrowLocal(!UtMath::NearlyZero(aOwnship.mSpeed, 0.1), "If not moving, can't find crossing!");
   // ECS
   const double origin1[3]    = {0, 0, 0};
   const double location1b[3] = {1, 0, 0};

   const double* origin2 = aOwnship.mLocation;
   // const double* vec2 = aOwnship.mVelocity;
   double       t             = 1.0;
   const double location2b[3] = {t * aOwnship.mVelocity[0] + aOwnship.mLocation[0],
                                 t * aOwnship.mVelocity[1] + aOwnship.mLocation[1],
                                 t * aOwnship.mVelocity[2] + aOwnship.mLocation[2]};

   UtLineSegment seg1(origin1, location1b);
   UtLineSegment seg2(origin2, location2b);

   /////GetIntersectionPoint(seg1,seg2,aIntersectionPoint);
   double modulus1;
   double modulus2;
   double nuShooterAtLineCPA[3];
   seg1.PointOfClosestApproach(seg2, nuShooterAtLineCPA, modulus1, aGeometry.mLocationCPAOwnship, modulus2);

   // We have to re-normalize.  Advance time to when ownship hits the cpa.
   // Possibly the shooter has past.  The mLocationCPAShooter is not valid,
   // as in he reaches it at a separate time.
   double distTraveledOwn[3];
   UtVec3d::Subtract(distTraveledOwn, aGeometry.mLocationCPAOwnship, aOwnship.mLocation);
   double d           = UtVec3d::Magnitude(distTraveledOwn);
   double s           = aOwnship.mSpeed;
   aGeometry.mTimeCpa = d / s; // d=rt t=d/r

   // Extrapolate both.  Although the ownship is just moving to the
   // point already found.  We need it, and the new shooter for subtraction
   // Extrapolate(aOwnship, aGeometry.mTimeCpa, aGeometry.mLocationCPAOwnshipNorm);
   Extrapolate(aShooter, aGeometry.mTimeCpa, aGeometry.mLocationCPAShooter);

   // Re-normalize and we have the ownship position on the new ECS at CPA
   // which can be used to calculate Pk max.
   UtVec3d::Subtract(aGeometry.mLocationCPAOwnshipNorm, aGeometry.mLocationCPAOwnship, aGeometry.mLocationCPAShooter);
}

void ToECS(const StateReport& aShooter, StateReport& aOwnship)
{
   // Normalize to map frame.  This is using the shooter frame at t0.  It's
   // expected that the shooter is a track/perception, and each call he has moved.
   // So the following transforms have to be calculated.
   double track2NEDXform[3][3];
   double track2ECSXform[3][3];
   WsfCovariance::ComputeWCS_ToNED_Transform(aShooter.mLocation, track2NEDXform);
   WsfCovariance::ComputeWCS_ToECS_Transform(aShooter.mVelocity, track2NEDXform, track2ECSXform);

   // Test/check: Actual NED
   ////double shooterVelNed[3];
   ////UtMat3d::Transform(shooterVelNed, track2NEDXform, aShooter.mVelocity);

   double ownPosECS[3];
   double ownVelECS[3];

   // Element-wise subtraction, aR = aX - aY
   UtVec3d::Subtract(aOwnship.mLocation, aOwnship.mLocation, aShooter.mLocation);

   // From WCS, to ECS
   UtMat3d::Transform(ownPosECS, track2ECSXform, aOwnship.mLocation);
   UtMat3d::Transform(ownVelECS, track2ECSXform, aOwnship.mVelocity);

   // Overwrite?  Sure.
   UtVec3d::Set(aOwnship.mLocation, ownPosECS);
   UtVec3d::Set(aOwnship.mVelocity, ownVelECS);
}
/** Only the xing wants to do this, so it can extrapolate along the
 * correct vector to find the cpa geometry.
 */
void ToECS(StateReport& aShooter)
{
   aShooter.mLocation[0] = 0;
   aShooter.mLocation[1] = 0;
   aShooter.mLocation[2] = 0;

   aShooter.mVelocity[0] = aShooter.mSpeed;
   aShooter.mVelocity[1] = 0;
   aShooter.mVelocity[2] = 0;
}

int sI = 0;

// void PrintThreatList(const WsfTrack&        aShooterTrack,
//                      WsfPlatform&           aOwnship,
//                      WsfThreatReportList&   aThreatReportList)
//{
//    int trackID = aShooterTrack.GetTrackId().GetLocalTrackNumber();
//    cout << "WsfWeaponThreatProcessor: Ran (" << sI << ") for true target "
//         << aShooterTrack.GetTargetName().GetString()
//         << " track ID = " << trackID
//         << ", and reported "
//         << aThreatReportList.size() << " threat reports ..." << endl;
//    for (size_t i = 0; i < aThreatReportList.size(); ++i)
//    {
//       WsfThreatReport& report = aThreatReportList[i];
//       cout << "    Report[" << i << "]"
//            << ", entity type = " << report.mEntityTypeId.GetString()
//            << ", weapon type = " << report.mWeaponTypeId.GetString()
//            //<< ", true name = "   << WsfStringId::GetString(report.mTrackNameId)
//            //<< ", track ID = "    << report.mTrackId << ","
//            << endl << "            "
//            <<   "Pe = "          << report.mPe
//            << ", Pw = "         << report.mPw
//            << ", Pod = "         << report.mPoD
//            //<< ", Pk_t0 = "       << report.mPk_t0
//            //<< ", Pt_t0 = "       << report.mPt_t0
//            << ", TimePtMax = "   << report.mTimePtMax
//            << ", Pk_max = "      << report.mPk_max
//            << ", Pt_max = "      << report.mPt_max
//            << endl;
//    }
// }
} // end anonymous namespace

//----------------------------------------------
// PkCalculator
//----------------------------------------------
class PkCalculator
{
public:
   PkCalculator() = default;
   PkCalculator(const PkCalculator& aSrc);
   PkCalculator(PkCalculator&&) = default;
   PkCalculator& operator=(const PkCalculator&) = delete;
   PkCalculator& operator=(PkCalculator&&) = default;
   virtual ~PkCalculator()                 = default;
   virtual PkCalculator* Clone()           = 0;

   virtual double GetPk(double aLocationECSTarget[3], WsfStringId aTargetType) = 0;

   void SetStartingEncounter(const StateReport* aOwnship, const StateReport* aShooter);

public:
   const StateReport* mOwnship           = nullptr;
   const StateReport* mShooter           = nullptr;
   double             mThresholdLaunchPk = 1.0; // Defaulting to 1.0 should force a complete search
};


PkCalculator::PkCalculator(const PkCalculator& aSrc)
   : mOwnship(nullptr)
   , mShooter(nullptr)
   , mThresholdLaunchPk(aSrc.mThresholdLaunchPk) // Defaulting to 1.0 should force a complete search
{
   if (aSrc.mOwnship != nullptr)
   {
      mOwnship = new StateReport(*aSrc.mOwnship);
   }
   if (aSrc.mShooter != nullptr)
   {
      mShooter = new StateReport(*aSrc.mShooter);
   }
}

void PkCalculator::SetStartingEncounter(const StateReport* aOwnship, const StateReport* aShooter)
{
   mOwnship = aOwnship;
   mShooter = aShooter;
}

/** There is one calculation per weapon target pair, and this is a
 * snapshot in time.  So the caller will look up the correct
 * calculation.  Then do a forward search to find the maximum pk.
 */
class PkCalcConstant : public PkCalculator
{
public:
   PkCalcConstant() = default;
   double        GetPk(double aLocationECSTarget[3], WsfStringId aTargetType) override { return 0.8; }
   PkCalculator* Clone() override { return new PkCalcConstant(*this); }

protected:
   PkCalcConstant(const PkCalcConstant& aSrc) = default;
};

// A def to show which file the namespace is in.
#define WsfPkTable WsfPk

class PkCalcTable : public PkCalculator
{
public:
   // The table name is the look ip key the WsfPkTable::TableManager
   // uses.  The pk_table should be defined outside the platform and
   // parsed before use.
   // PkCalcTable(WsfStringId aTableName);
   PkCalcTable(WsfPkTable::Table* aTablePtr);
   PkCalcTable& operator=(const PkCalcTable&) = delete;
   ~PkCalcTable() override                    = default;

   double GetPk(double aLocationECSTarget[3], WsfStringId aTargetType) override;

   PkCalculator* Clone() override { return new PkCalcTable(*this); }

protected:
   PkCalcTable(const PkCalcTable& aSrc)
      : PkCalculator(aSrc)
   {
      mPkTable = nullptr;
      if (aSrc.mPkTable != nullptr)
      {
         // TODO - is this safe for cleanup?
         // mPkTable = new WsfPkTable::Table(*aSrc.mPkTable);
         mPkTable = aSrc.mPkTable;
      }
   }

   void CalcPkParameters(double  aLocationECSTarget[3],
                         double& aAzimuth,
                         double& aElevation,
                         double& aMissileSpeed,
                         double& aTargetSpeed);

   WsfPkTable::Table* mPkTable;
};

// PkCalcTable::PkCalcTable(WsfStringId aTableName)
PkCalcTable::PkCalcTable(WsfPkTable::Table* aTablePtr)
   : PkCalculator()
   , mPkTable(aTablePtr)
{
   ////TableManager&
   // mPkTable = WsfPkTable::TableManager::GetManager().GetTable(aTableName);
   ////////WeaponThreatProcessorThrowLocal(mPkTable);
   if (mPkTable == nullptr)
   {
      ut::log::error() << "PkCalcTable: Given Table Pointer is not valid. Call DRB.";
   }
}

// virtual
double PkCalcTable::GetPk(double aLocationECSTarget[3], WsfStringId aTargetType)
{
   double azimuth;
   double elevation;
   // The notes in pk-table SUGGEST the "missileSpeed" is really
   // the shooter speed.
   ////double missileSpeed;
   double shooterSpeed;
   double targetSpeed;

   CalcPkParameters(aLocationECSTarget, azimuth, elevation, shooterSpeed, targetSpeed);

   return mPkTable->LookUp(aTargetType, azimuth, elevation, shooterSpeed, targetSpeed);
}

// Borrowed from WsfWeaponEffects.  It's kept as a reminder, but we
// could eliminate it because it doesn't do much.
// =================================================================================================
// virtual protected
//! Capture the geometry values needed to do a Pk table lookup.
//! @param aTargetPtr Pointer to the target to be evaluated for geometry.
//! @param aAzimuth Missile-body-relative azimuth to the target.
//! @param aElevation Missile-body-relative elevation to the target.
//! @param aMissileSpeed Missile speed.
//! @param aTargetSpeed Target speed.
void PkCalcTable::CalcPkParameters(double aLocationECSTarget[3],
                                   // WsfPlatform* aTargetPtr,
                                   double& aAzimuth,
                                   double& aElevation,
                                   double& aShooterSpeed,
                                   double& aTargetSpeed)
{
   // In this base class, the assumption is that this is an implicit rather than
   // explicit engagement, and so the Pk determination is done at trigger pull,
   // and the geometry used is the target track, relative to the _launching_
   // platform (implementation will be overridden for explicit weapon engagements).
   // UtVec3dX mslLocWCS;
   // UtVec3dX tgtLocWCS;
   // GetEngagement()->GetTargetPlatform()->GetLocationWCS(tgtLocWCS.GetData());
   // GetEngagement()->GetFiringPlatform()->GetLocationWCS(mslLocWCS.GetData());
   // UtDCM mslWCStoECS = GetEngagement()->GetFiringPlatform()->GetDcmWCStoECS();
   // UtVec3dX tgtLOS_WCS = tgtLocWCS - mslLocWCS;
   // UtVec3dX tgtLOS_ECS = tgtLOS_WCS * mslWCStoECS;
   UtVec3dX tgtLOS_ECS(aLocationECSTarget);
   aAzimuth   = tgtLOS_ECS.Azimuth();
   aElevation = tgtLOS_ECS.Elevation();
   // LAUNCHING PLATFORM VELOCITY !!!
   // aMissileSpeed = GetEngagement()->GetFiringPlatform()->GetSpeed();
   // aTargetSpeed = GetEngagement()->GetTargetPlatform()->GetSpeed();
   aShooterSpeed = mShooter->mSpeed;
   // Yes, ownship is the target
   aTargetSpeed = mOwnship->mSpeed;
}

class PkCalcSpherical2 : public PkCalculator
{
public:
   PkCalcSpherical2();
   PkCalcSpherical2& operator=(const PkCalcSpherical2&) = delete;
   ~PkCalcSpherical2() override                         = default;

   double GetPk(double aLocationECS[3], WsfStringId aTargetType) override
   {
      double pk = 0;
      return pk;
   }

   WsfSphericalLethality* mSphericalLethality;

   PkCalculator* Clone() override { return new PkCalcSpherical2(*this); }

protected:
   PkCalcSpherical2(const PkCalcSpherical2& aSrc)
      : PkCalculator(aSrc)
   {
      if (aSrc.mSphericalLethality != nullptr)
      {
         mSphericalLethality = (WsfSphericalLethality*)aSrc.mSphericalLethality->Clone();
      }
   }
};

// WSF_SPHERICAL_LETHALITY",              new WsfSphericalLethality()
PkCalcSpherical2::PkCalcSpherical2()
   : PkCalculator()
   , mSphericalLethality(nullptr)
{
}

class PkCalcSpherical : public PkCalculator
{
public:
   PkCalcSpherical()
      : PkCalculator()
      , mPkMin(0.0)
      , mPkMax(1.0)
      , mPkRange(1.0)
   {
   }

   double GetPk(double aLocationECS[3], WsfStringId aTargetType) override
   {
      // The slope should be negative, decreasing pk moving away
      double m = (mPkMin - mPkMax) / mPkRange;
      double d = UtVec3d::Magnitude(aLocationECS);
      return d > mPkRange ? 0.0 : std::max(0.0, mPkMax + d * m);
   }

   double mPkMin;
   double mPkMax;
   double mPkRange;

   PkCalculator* Clone() override { return new PkCalcSpherical(*this); }

protected:
   PkCalcSpherical(const PkCalcSpherical& aSrc) = default;
};

class PkCalcElliptical : public PkCalculator
{
public:
   PkCalcElliptical()
      : PkCalculator()
   {
   }

   double GetPk(double aLocationECS[3], WsfStringId aTargetType) override
   {
      double mCenterX(0.0);   // TODO
      double mSemiMajor(0.0); // TODO
      double mSemiMinor(0.0); // TODO
      double mag = UtVec3d::Magnitude(aLocationECS);
      // double cost = aLocationECS[0]/mag;
      // double sint = aLocationECS[1]/mag;
      // double xOne = mSemiMajor*cost + mCenterX;
      // double yOne = mSemiMajor*cost +      0.0;// ellipse centered on nose
      double mage;
      {
         // Find the ellipse radial length through the given point
         double h0              = mCenterX;
         double k0              = 0.0;
         double a               = mSemiMajor;
         double b               = mSemiMinor;
         double x0              = aLocationECS[0] - h0;
         double y0              = aLocationECS[1] - k0;
         double den             = sqrt(a * a * y0 * y0 + b * b * x0 * x0);
         double xei             = a * b * x0 / den + h0;
         double yei             = a * b * y0 / den + k0;
         double aLocationECS[3] = {xei, yei, 0.0}; // TODO Z
         mage                   = UtVec3d::Magnitude(aLocationECS);
      }
      double pk = 1.0 - mag / mage;
      if (mag >= mage)
      {
         pk = 0.0;
      }
      WeaponThreatProcessorThrowLocal(pk < 1.0);
      WeaponThreatProcessorThrowLocal(pk >= 0.0);
      return pk;
   }

   PkCalculator* Clone() override { return new PkCalcElliptical(*this); }

protected:
   PkCalcElliptical(const PkCalcElliptical& aSrc) = default;
};
//----------------------------------------------
// End PkCalculator
//----------------------------------------------

//----------------------------------------------
// PkMaximizer
//----------------------------------------------
/** TODO Should the default be Xing, with on subclass?
 * Should the super class be pure virtual?
 */
/** The Pk Maximizer uses a pk calculator, and uses its
 * heuristic to find the highest pk given the current
 * geometry of the perceived shooter an ownship.
 */
class PkMaximizer
{
public:
   PkMaximizer();
   virtual ~PkMaximizer() = default;

   virtual void SetEncounter(StateReport* aOwnship, StateReport* aShooter);

   virtual void FindMaxPk(const WeaponDatum& aWeaponData, PkCalculator* aPkCalculator, WsfThreatReport& aThreatReport) = 0;

   void SetPkSearchParameters(PkSearchParameters* aPkSearchParameters) { mPkSearchParameters = aPkSearchParameters; }

protected:
   PkSearchParameters* mPkSearchParameters;
   Geometry            mGeometryXing; // FIXME
   StateReport*        mOwnship;
   StateReport*        mShooter;
};

/** The Crossing/Xing Pk Maximizer is a pk calculator, that
 * assumes the shooter dead-reckons and it finds
 * when the shooter crosses ownships centerline.  It assumes
 * that point where the highest pk will occur.  It is crosses
 * in front of ownship, there will be no pk/threat.
 *   There may be an underlying assumption that this occurs
 * when the shooter does detect ownship.
 */
class PkMaximizerXing : public PkMaximizer
{
public:
   PkMaximizerXing();
   ~PkMaximizerXing() override = default;

   void SetEncounter(StateReport* aOwnship, StateReport* aShooter) override;

   void FindMaxPk(const WeaponDatum& aWeaponData, PkCalculator* aPkCalculator, WsfThreatReport& aThreatReport) override;
};

/** The Pure Pursuit Pk Maximizer is a pk calculator, that
 * follows pure pursuit.  At every step, the shooter
 * is aligned at ownship, and the pk is calculated. The
 * highest pk of all the steps is returned.
 */
class PkMaximizerPurePursuit : public PkMaximizer
{
public:
   PkMaximizerPurePursuit();
   ~PkMaximizerPurePursuit() override = default;

   void FindMaxPk(const WeaponDatum& aWeaponData, PkCalculator* aPkCalculator, WsfThreatReport& aThreatReport) override;
};
// class PkMaximizerOpt : public PkMaximizer
//{
//    public:
//       PkMaximizerOpt();
//       virtual ~PkMaximizerOpt();
//
//       virtual void FindMaxPk(WeaponDatum*     aWeaponData,
//                              PkCalculator*    aPkCalculator,
//                              WsfThreatReport& aThreatReport);
// };

//---------------------------------------
PkMaximizer::PkMaximizer()
   : mPkSearchParameters(nullptr)
   , mGeometryXing()
   , mOwnship(nullptr)
   , mShooter(nullptr)
{
}

// virtual
void PkMaximizer::SetEncounter(StateReport* aOwnship, StateReport* aShooter)
{
   mOwnship = aOwnship;
   mShooter = aShooter;
}

//---------------------------------------

PkMaximizerXing::PkMaximizerXing()
   : PkMaximizer()
{
}

// virtual
void PkMaximizerXing::SetEncounter(StateReport* aOwnship, StateReport* aShooter)
{
   mShooter = aShooter;
   mOwnship = aOwnship;

   //----------
   // Modifies (normalizes)  both states
   ToECS(*aShooter, *aOwnship);
   ToECS(*aShooter);

   //----------
   FindCrossingECS(*mShooter, *mOwnship, mGeometryXing);
   //----------
}

// virtual
void PkMaximizerXing::FindMaxPk(const WeaponDatum& aWeaponData, PkCalculator* aPkCalculator, WsfThreatReport& aThreatReport)
{
   aThreatReport.mTimePtMax = mGeometryXing.mTimeCpa;
   aThreatReport.mPk_max    = aPkCalculator->GetPk(mGeometryXing.mLocationCPAOwnshipNorm, mOwnship->mPlatformType);
}

//---------------------------------------
PkMaximizerPurePursuit::PkMaximizerPurePursuit()
   : PkMaximizer()
{
}

/** Q: Should we use false position?  A: With stepping, each jump
 * might have to walk to the 't'.  Although we could persist
 * the trajectory, and just find the pk at the jump point.
 *   I think the benefit would occur when the trajectory equation
 * is derived, so a jump could fall between normal steps.
 */

// virtual
void PkMaximizerPurePursuit::FindMaxPk(const WeaponDatum& aWeaponData,
                                       PkCalculator*      aPkCalculator,
                                       WsfThreatReport&   aThreatReport)
{
   // Advance time, and at each step, point the shooter at the dead-reckoning
   // ownship, and find and record the pk for that step.  If we have a launch
   // pk, AND it is exceeded, we just step to that time and return the "pk"
   // and time.

   // Don't permanently change the states
   const StateReport& own = *mOwnship;
   const StateReport& shr = *mShooter;

   aPkCalculator->SetStartingEncounter(&own, &shr);

   // To use extrapolate, we need a local state.
   // Shooter state X at time i.
   StateReport shrXi;
   UtVec3d::Set(shrXi.mLocation, shr.mLocation);
   UtVec3d::Set(shrXi.mVelocity, shr.mVelocity);

   double launchPk = aPkCalculator->mThresholdLaunchPk;
   double t        = 0.0;

   // TODO Performance parameters
   double dt       = 2.0;
   double maxSteps = 50;

   //-------------------
   // static index of times ran.  I wanted to show the 1st and k'ed
   static bool draw = false; // sI == 1 || sI == 24; // TODO
   // If we are sending the pursuit curves to WsfDraw,
   // step longer for visual effect.
   if (draw)
   {
      maxSteps = 100;
   }
   //-------------------

   double maxDuration = mPkSearchParameters->mDuration;
   //-------------------

   ////double shtrStepDist = shr.mSpeed*dt;
   //
   // double ownLocXiWCS[3];
   ////UtVec3d::Set(shrLocXiWCS, own.mLocation);
   // UtVec3d::Set(shrLocXiWCS, shr.mLocation);

   double shtr2OwnWCS[3];
   double pk = 0.0;

   for (int i = 0; i < maxSteps && t < maxDuration; ++i, t += dt)
   {
      // FIXME:  We have the normalized geometry, but we need the original
      // geometry to integrate this.
      // WeaponThreatProcessorThrowLocal(false, "TODO");
      StateReport ownXi;
      // UtVec3d::Set(ownXi.mLocation, own.mLocation);
      Extrapolate(*mOwnship, t, ownXi.mLocation);
      UtVec3d::Set(ownXi.mVelocity, own.mVelocity);

      UtVec3d::Subtract(shtr2OwnWCS, ownXi.mLocation, shrXi.mLocation);
      // Assumption: Infinite g's.
      // We have the new direction vector, norm and find velocity.
      // Then we have enough to convert to ECS (it's dyn) and
      // call the pk calculation
      double ds2o = UtVec3d::Magnitude(shtr2OwnWCS);
      WeaponThreatProcessorThrowLocal(ds2o != 0, "Div Zero.");
      // Down up
      UtVec3d::Multiply(shtr2OwnWCS, shr.mSpeed / ds2o);
      UtVec3d::Set(shrXi.mVelocity, shtr2OwnWCS);

      // Move the shooter along the new vv
      Extrapolate(shrXi, dt, shrXi.mLocation);

      // if (draw) Draw(shrXi);

      // For the pk calculation, re-normalize along using state Xi
      ToECS(shrXi, ownXi);

      // And find this next pk

      pk = aPkCalculator->GetPk(ownXi.mLocation, mOwnship->mPlatformType);

      // Escape/quit if we exceed the threshold pk. Note: we
      // very well may step noticeably over the threshold,
      // such that the reported pk will also be noticeably
      // greater than the threshold
      if (!draw && pk >= launchPk)
      {
         break;
      }
   }
   draw = false;

   // if (sI == 0)
   //{
   //    FindMaxPk(aWeaponData, aPkCalculator, aThreatReport);
   // }
   // if (t < 0.1 && pk < launchPk)
   //{
   //    FindMaxPk(aWeaponData, aPkCalculator, aThreatReport);
   // }

   // aThreatReport.mTimePtMax = mGeometryXing.mTimeCpa;
   // aThreatReport.mPk_max = aPkCalculator->GetPk(mGeometryXing.mLocationCPAOwnshipNorm);
   aThreatReport.mTimePtMax = t;
   aThreatReport.mPk_max    = pk;
}

//---------------------------------------
// PkMaximizerOpt::PkMaximizerOpt()
//   : PkMaximizer()
//{
//}
//
////virtual
// PkMaximizerOpt::~PkMaximizerOpt()
//{
// }
//----------------------------------------------
//  End PkMaximizer
//----------------------------------------------
//----------------------------------------------
//  ThreatEvaluator
//----------------------------------------------
ThreatEvaluator::ThreatEvaluator(WsfScenario& aScenario)
   : mPkSearchParameters()
   , mScenarioPtr(&aScenario)
{
}

class ThreatEvaluatorP : public ThreatEvaluator
{
public:
   ThreatEvaluatorP(WsfScenario& aScenario);
   ThreatEvaluatorP(const ThreatEvaluatorP& aSrc);
   ThreatEvaluatorP& operator=(const ThreatEvaluatorP&) = delete;
   ~ThreatEvaluatorP() override;

   ThreatEvaluator* Clone() const override;
   bool             ProcessInput(UtInput& aInput) override;

   void CreateThreatList(const WsfTrack& aShooterTrack, WsfPlatform& aOwnship, WsfThreatReportList& aThreatReportList) override;

protected:
   void ReadWeaponTypeBlock(UtInput& aInput);

private:
   std::string                          mTrackClassifierName;
   std::map<WsfStringId, WeaponLoad*>   mEntity2WeaponLoad;
   std::map<WsfStringId, PkCalculator*> mWeapon2PkCalcMap;
   PkMaximizer*                         mPkMaximizer;
   std::map<WsfStringId, bool>          mWarningNoWeapons;
   std::map<WsfStringId, bool>          mWarningNoPkCalc;

   bool mTest;
};

ThreatEvaluatorP::ThreatEvaluatorP(WsfScenario& aScenario)
   : ThreatEvaluator(aScenario)
   , mTrackClassifierName()
   , mEntity2WeaponLoad()
   , mWeapon2PkCalcMap()
   , mPkMaximizer(nullptr)
   , mWarningNoWeapons()
   , mWarningNoPkCalc()
   , mTest(false)
{
   // FIXME I think PurePursuit will be the only option,
   //  albeit it will be slower.  If we do want more options, move this
   //  to process input.
   mPkMaximizer //= new PkMaximizerXing;
      = new PkMaximizerPurePursuit();
}

ThreatEvaluatorP::ThreatEvaluatorP(const ThreatEvaluatorP& aSrc)
   : ThreatEvaluator(aSrc)
   , mTrackClassifierName(aSrc.mTrackClassifierName)
   ,
   //, mEntity2WeaponLoad(aSrc.mEntity2WeaponLoad),
   //, mWeapon2PkCalcMap(aSrc.mWeapon2PkCalcMap),
   mPkMaximizer(nullptr)
   , mWarningNoWeapons(aSrc.mWarningNoWeapons)
   , mWarningNoPkCalc(aSrc.mWarningNoPkCalc)
   , mTest(aSrc.mTest)
{
   mPkMaximizer //= new PkMaximizerXing;
      = new PkMaximizerPurePursuit();


   std::map<WsfStringId, PkCalculator*>::const_iterator it;
   for (it = aSrc.mWeapon2PkCalcMap.begin(); it != aSrc.mWeapon2PkCalcMap.end(); ++it)
   {
      mWeapon2PkCalcMap[it->first] = it->second->Clone();
   }

   std::map<WsfStringId, WeaponLoad*>::const_iterator it2;
   for (it2 = aSrc.mEntity2WeaponLoad.begin(); it2 != aSrc.mEntity2WeaponLoad.end(); ++it2)
   {
      mEntity2WeaponLoad[it2->first] = new WeaponLoad(*it2->second);
   }
}

// virtual
ThreatEvaluator* ThreatEvaluatorP::Clone() const
{
   return new ThreatEvaluatorP(*this);
}

// virtual
ThreatEvaluatorP::~ThreatEvaluatorP()
{
   delete mPkMaximizer;

   std::map<WsfStringId, PkCalculator*>::iterator it;
   for (it = mWeapon2PkCalcMap.begin(); it != mWeapon2PkCalcMap.end(); ++it)
   {
      delete it->second;
   }
   mWeapon2PkCalcMap.clear();

   std::map<WsfStringId, WeaponLoad*>::iterator it2;
   for (it2 = mEntity2WeaponLoad.begin(); it2 != mEntity2WeaponLoad.end(); ++it2)
   {
      delete it2->second;
   }
   mEntity2WeaponLoad.clear();
}

// virtual
bool ThreatEvaluatorP::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command   = aInput.GetCommand();

   if (command == "track_classifier")
   {
      aInput.ReadCommand(mTrackClassifierName);
   }
   else if (command == "weapons_types_table")
   {
      UtInputBlock blockA(aInput, "end_weapons_types_table");
      // Read entity elements
      while (blockA.ReadCommand())
      {
         command = aInput.GetCommand();
         if (command == "weapon_type")
         {
            ReadWeaponTypeBlock(aInput);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown command inside 'weapon_type'");
         }
      }
   }
   else if (command == "weapons_load_table")
   {
      UtInputBlock blockA(aInput, "end_weapons_load_table");
      // Read entity elements
      while (blockA.ReadCommand())
      {
         command = aInput.GetCommand();
         if (command == "entity_load")
         {
            UtInputBlock block(aInput, "end_entity_load");

            std::string keyEntityType;
            WsfStringId keyEntityTypeId;

            while (block.ReadCommand())
            {
               command = block.GetCommand();

               if (command == "entity_type")
               {
                  aInput.ReadCommand(keyEntityType);
                  keyEntityTypeId = WsfStringId(keyEntityType);
                  if (mEntity2WeaponLoad[keyEntityTypeId] != nullptr)
                  {
                     WeaponThreatProcessorThrowLocal(false, "Duplicate entries FOR an entity are not allowed!");
                     throw UtInput::BadValue(aInput, "Duplicate entries FOR an entity are not allowed!");
                  }
               }
               else if (command == "weapons")
               {
                  UtInputBlock                         block(aInput, "end_weapons");
                  std::map<WsfStringId, UtInputBlock*> dupTest;
                  WeaponLoad*                          wx = new WeaponLoad();
                  mEntity2WeaponLoad[keyEntityTypeId]     = wx;
                  while (block.ReadCommand())
                  {
                     std::string weaponType   = block.GetCommand();
                     WsfStringId weaponTypeId = WsfStringId(weaponType);
                     if (dupTest[weaponTypeId] != nullptr)
                     {
                        WeaponThreatProcessorThrowLocal(false, "Duplicate entries in 'weapons are not allowed! ");
                        throw UtInput::BadValue(aInput, "Duplicate entries in 'weapons are not allowed!");
                     }
                     // We're just setting it to something.
                     dupTest[weaponTypeId] = &block;

                     double probOnBoard;
                     aInput.ReadValue(probOnBoard);
                     aInput.ValueInClosedRange(probOnBoard, 0.0, 1.00001);

                     // Store it.
                     WeaponDatum w;
                     w.mWeaponTypeId       = weaponTypeId;
                     w.mProbabilityOnBoard = probOnBoard;
                     wx->mWeaponList.emplace_back(w);

                     bool forcePkCalcCheat = false;
                     if (forcePkCalcCheat)
                     {
                        if (mWeapon2PkCalcMap[weaponTypeId] == nullptr)
                        {
                           mWeapon2PkCalcMap[weaponTypeId] = new PkCalcConstant();
                        }
                     }
                  }
                  // Is post processing element needed????
                  // AddEntityMID(keyEntityTypeId, pcid, aMisIdList);
               }
            } // loop on entity_load
         }    // else if entity_load
         else
         {
            throw UtInput::BadValue(aInput, "Unknown command inside 'weapons_load_table'");
         }
      } // loop on weapons_load_table
   }    // else if weapons_load_table
   else if (command == "pk_maximizer")
   {
      // pk_maximizer
      //    type               pure_pursuit
      //    step_distance      20 m
      //    step_count         100
      // end_pk_maximizer
      std::string pkMaxType; //"pure_pursuit"
      double      stepDistance = 1.0;
      int         stepCount    = 10;

      UtInputBlock blockA(aInput, "end_pk_maximizer");
      // Read entity elements
      while (blockA.ReadCommand())
      {
         command = aInput.GetCommand();
         if (command == "type")
         {
            aInput.ReadCommand(pkMaxType);
         }
         else if (command == "step_distance")
         {
            aInput.ReadValueOfType(stepDistance, UtInput::cLENGTH);
            aInput.ValueInClosedRange(stepDistance, 0.001, 1E5);
         }
         else if (command == "step_count")
         {
            aInput.ReadValue(stepCount);
            aInput.ValueInClosedRange(stepCount, 0, 10000);
         }
         else
         {
            throw UtInput::BadValue(aInput, "Unknown command inside 'weapon_type'");
         }
      }
   }
   else if (command == "test")
   {
      // aInput.ReadValue(mTest);
      mTest = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

void ThreatEvaluatorP::ReadWeaponTypeBlock(UtInput& aInput)
{
   UtInputBlock blockWt(aInput, "end_weapon_type");
   std::string  weaponTypeName;
   WsfStringId  weaponTypeId("");
   double       thresholdLaunchPk = -1;
   bool         onePkRead         = false;
   while (blockWt.ReadCommand())
   {
      std::string command = aInput.GetCommand();
      if (command == "type_name")
      {
         aInput.ReadValue(weaponTypeName);
         weaponTypeId = WsfStringId(weaponTypeName);
      }
      else if (command == "launch_pk")
      {
         aInput.ReadValue(thresholdLaunchPk);
      }
      else if (command == "pk_table")
      {
         if (weaponTypeId == "")
         {
            throw UtInput::BadValue(aInput, "Weapon type must be read before the pk calculation");
         }

         if (onePkRead || (mWeapon2PkCalcMap[weaponTypeId] != nullptr))
         {
            throw UtInput::BadValue(aInput, "Only 1 pk calculation per weapon type.");
         }

         std::string pkTableName("");

         UtInputBlock block(aInput, "end_pk_table");
         while (block.ReadCommand())
         {
            command = block.GetCommand();
            if (command == "table_name")
            {
               aInput.ReadValue(pkTableName);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown command");
            }
         }

         if (pkTableName.empty())
         {
            throw UtInput::BadValue(aInput, "A pk_table needs a 'table_name' parameter.");
         }

         WsfPkTable::Table* tPtr = WsfMilExtension::Find(*GetScenario())->GetPkTableManager().FindTableName(pkTableName);
         if (tPtr == nullptr)
         {
            throw UtInput::BadValue(aInput, "pk_table not found: " + pkTableName);
         }
         PkCalcTable* pkcalc        = new PkCalcTable(tPtr);
         pkcalc->mThresholdLaunchPk = thresholdLaunchPk;

         if (mWeapon2PkCalcMap[weaponTypeId] == nullptr)
         {
            mWeapon2PkCalcMap[weaponTypeId] = pkcalc;
         }
         else
         {
            delete pkcalc;
            throw UtInput::BadValue(aInput, "Only 1 pk calculation per weapon type.");
         }

         onePkRead = true;
      }
      else if (command == "pk_cone")
      {
         if (onePkRead)
         {
            throw UtInput::BadValue(aInput, "Only 1 pk calculation per weapon type.");
         }

         UtInputBlock block(aInput, "end_pk_cone");
         while (block.ReadCommand())
         {
            // string s = block.GetCommand();
            WeaponThreatProcessorThrowLocal(false, "TODO");
         }
         onePkRead = true;
      }
      else if (command == "pk_spherical")
      {
         if (weaponTypeId == "")
         {
            throw UtInput::BadValue(aInput, "Weapon type must be read before the pk calculation");
         }

         if (onePkRead || (mWeapon2PkCalcMap[weaponTypeId] != nullptr))
         {
            throw UtInput::BadValue(aInput, "Only 1 pk calculation per weapon type.");
         }

         PkCalcSpherical* pkcalc    = new PkCalcSpherical();
         pkcalc->mThresholdLaunchPk = thresholdLaunchPk;

         UtInputBlock block(aInput, "end_pk_spherical");
         while (block.ReadCommand())
         {
            command = block.GetCommand();
            if (command == "pk_min")
            {
               aInput.ReadValue(pkcalc->mPkMin);
            }
            else if (command == "pk_max")
            {
               aInput.ReadValue(pkcalc->mPkMax);
            }
            else if (command == "range")
            {
               aInput.ReadValueOfType(pkcalc->mPkRange, UtInput::cLENGTH);
            }
            else
            {
               throw UtInput::BadValue(aInput, "Unknown command");
            }
         }

         if (mWeapon2PkCalcMap[weaponTypeId] == nullptr)
         {
            mWeapon2PkCalcMap[weaponTypeId] = pkcalc;
         }
         else
         {
            delete pkcalc;
            throw UtInput::BadValue(aInput, "Only 1 pk calculation per weapon type.");
         }

         onePkRead = true;
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown command inside 'weapons_load_table'");
      }
   }
}

//----------------------------------------------
// end ThreatEvaluator
//----------------------------------------------

/** For the given track, find the list of possible entity type, and for each a list
 * of possible weapons, and then the PK and POD to find the PT.  Since ownship will
 * be normalized to the threat, there is no savings splitting this function up.
 */
void ThreatEvaluatorP::CreateThreatList(const WsfTrack&      aShooterTrack,
                                        WsfPlatform&         aOwnship,
                                        WsfThreatReportList& aThreatReportList)
{
   sI++;
   // aThreatReportList.mTrackId = trackId;
   double pod = 1.0; // mDetector->GetPoD(aOwnship);

   if (!aShooterTrack.LocationValid())
   {
      ut::log::warning() << "CreateThreatList: Track has no 'valid' location. Skipping.";
      return;
   }

   // We can sort of do something with this.  We could use pursuit only @ t0, since
   // we don't know how fat it will hone down on ownship.
   if (!aShooterTrack.VelocityValid())
   {
      ut::log::warning() << "CreateThreatList: Track has no 'valid' velocity. Skipping.";
      return;
   }

   const WsfProcessor* processor = aOwnship.GetComponent<WsfProcessor>(WsfStringId(mTrackClassifierName));
   if (processor == nullptr)
   {
      ut::log::warning() << "CreateThreatList: No access to ClassifierProcessor.";
      return;
   }

   const WsfClassificationProcessor* classifierProcessor = dynamic_cast<const WsfClassificationProcessor*>(processor);
   if (classifierProcessor == nullptr)
   {
      ut::log::error() << "CreateThreatList: 'classifier' is not type 'WsfClassificationProcessor'.";
      return;
   }

   // Use the track to get the class list.
   ClassifierReportList classifierReportList;
   classifierProcessor->CreateClassList(aShooterTrack, classifierReportList);

   static bool printedClassiferResults = true;
   if (!printedClassiferResults)
   {
      printedClassiferResults = true;
      WsfClassificationProcessor::PrintClassList(classifierReportList, aShooterTrack.GetTargetType());
   }

   // int trackNameId = aShooterTrack.GetTargetName();
   WsfStringId       trackNameId = aShooterTrack.GetTargetName();
   const WsfTrackId& trackId     = aShooterTrack.GetTrackId(); //.GetLocalTrackNumber();

   StateReport ownship;
   StateReport shooter;

   // Get their current perceived kinematics
   GetState(aShooterTrack, shooter);
   GetState(aOwnship, ownship);

   // Test the arc limit here.
   if (false)
   {
      // Arc Problem: What if slightly behind BUT high above
      // so that a 90 deg maneuver will align???.  The better test
      // is given a "g", how long to align? And given that, who
      // cares about limiting; just find the threat for it.
      ToECS(shooter, ownship);

      // Reset the modified state(s) for the next processes.
      GetState(aOwnship, ownship);
   }

   // In this call, "Xing" finds the xing for the platform, because it's used for
   // all weapons on that platform. The Opt will store it, and then search for
   // whichever point in the given pk calculation.
   //   PurePursuit can generate a locus of geometries, and that same set
   // can be used for each weapon type.
   mPkMaximizer->SetPkSearchParameters(&GetPkSearchParameters());
   mPkMaximizer->SetEncounter(&ownship, &shooter);

   for (ClassifierReport& cr : classifierReportList)
   {
      if (cr.mTypeScore == 0)
      {
         auto out = ut::log::error() << "GetThreatList: Candidate Pe entity type is 0.0.";
         out.AddNote() << "Skipping: " << cr.mEntityTypeId.GetString();
         continue;
      }

      WeaponLoad* wx = mEntity2WeaponLoad[cr.mEntityTypeId];
      if ((wx == nullptr) || wx->mWeaponList.empty())
      {
         bool& warned = mWarningNoWeapons[cr.mEntityTypeId];
         if (!warned)
         {
            warned   = true; // Setting the "address"
            auto out = ut::log::error() << "GetThreatList: Candidate entity has no known weapons.";
            out.AddNote() << "Skipping: " << cr.mEntityTypeId.GetString();
         }
         continue;
      }

      for (const WeaponDatum& weaponData : wx->mWeaponList)
      {
         // Low prob weapons might have been pruned, but we could
         // also skip them here.
         double pwo = weaponData.mProbabilityOnBoard;

         PkCalculator* pkCalc = mWeapon2PkCalcMap[weaponData.mWeaponTypeId];
         if (pkCalc == nullptr)
         {
            bool& warned = mWarningNoPkCalc[weaponData.mWeaponTypeId];
            if (!warned)
            {
               warned   = true; // Setting the "address"
               auto out = ut::log::error() << "GetThreatList: No pkCalculator found for weapon.";
               out.AddNote() << "Weapon: " << weaponData.mWeaponTypeId.GetString();
               out.AddNote() << "Candidate Entity Type: " << cr.mEntityTypeId.GetString();
               out.AddNote() << "Skipping this combo.";
            }
            continue;
         }
         pkCalc->SetStartingEncounter(&ownship, &shooter);

         // Finally, get a report address and fill it out.
         aThreatReportList.emplace_back();
         WsfThreatReport& report = aThreatReportList[aThreatReportList.size() - 1];

         report.mTrackNameId = trackNameId;
         report.mTrackId     = trackId;
         report.mPoD         = pod;

         report.mEntityTypeId = cr.mEntityTypeId;
         report.mPe           = cr.mTypeScore;

         report.mPw = pwo;

         report.mWeaponTypeId = weaponData.mWeaponTypeId;

         report.mPk_t0 = pkCalc->GetPk(ownship.mLocation, ownship.mPlatformType);
         report.mPt_t0 = report.mPoD * report.mPe * report.mPk_t0 * pwo;

         //---------------------------------
         // pk maxer will set the max pk and time it happens.
         mPkMaximizer->FindMaxPk(weaponData, pkCalc, report);
         report.mPt_max = report.mPoD * report.mPe * report.mPk_max * pwo;
         //---------------------------------
      }
   }
}

//----------------------------------------------
// main WsfWeaponThreatProcessor
//----------------------------------------------

WsfWeaponThreatProcessor::WsfWeaponThreatProcessor(WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfWeaponThreatProcessor", "PROCESSOR")
   , mThreatEvaluator(nullptr)
   , mThreatReportList()
{
   mThreatEvaluator = new ThreatEvaluatorP(aScenario);
}

WsfWeaponThreatProcessor::WsfWeaponThreatProcessor(const WsfWeaponThreatProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mThreatEvaluator(aSrc.mThreatEvaluator->Clone())
   , mThreatReportList()
{
}

// virtual
WsfWeaponThreatProcessor::~WsfWeaponThreatProcessor()
{
   delete mThreatEvaluator;
}

bool WsfWeaponThreatProcessor::ProcessInput(UtInput& aInput)
{
   WeaponThreatProcessorThrowLocal((mThreatEvaluator != nullptr), "Where is my ThreatEvaluator!");
   bool myCommand = mThreatEvaluator->ProcessInput(aInput);
   if (!myCommand)
   {
      myCommand = WsfScriptProcessor::ProcessInput(aInput);
   }
   return myCommand;
}

// virtual
bool WsfWeaponThreatProcessor::Initialize(double aSimTime)
{
   // WsfScriptContext* aContext = &GetScriptContext();
   return WsfScriptProcessor::Initialize(aSimTime);
}

//! Use
// virtual
bool WsfWeaponThreatProcessor::ProcessMessage(double aSimTime, const WsfMessage& aMessage)
{
   bool processed = true;
   return processed;
}

// virtual
void WsfWeaponThreatProcessor::CreateThreatList(const WsfTrack&      aShooterTrack,
                                                WsfPlatform&         aOwnship,
                                                WsfThreatReportList& aThreatReportList)
{
   if (mThreatEvaluator != nullptr)
   {
      mThreatEvaluator->CreateThreatList(aShooterTrack, aOwnship, aThreatReportList);
   }
   else
   {
      ut::log::error() << "WsfWeaponThreatProcessor: mThreatEvaluator is not accessible.";
   }
}

bool Contains(std::vector<int>& aQueryTargetIds, const WsfTrack& aShooterTrack)
{
   // bool xxx = std::find(aQueryTargetIds.begin(), aQueryTargetIds.end(), aShooterTrack.GetTargetIndex()) != aQueryTargetIds.end();
   for (const auto& queryTargetId : aQueryTargetIds)
   {
      if (ut::cast_to_size_t(queryTargetId) == aShooterTrack.GetTargetIndex())
      {
         return true;
      }
   }
   return false;
}

bool Contains(std::vector<WsfTrackId>& aQueryTrackIds, const WsfTrack& aShooterTrack)
{
   // bool xxx = std::find(aQueryTrackIds.begin(), aQueryTrackIds.end(), aShooterTrack.GetTrackId()) != aQueryTrackIds.end();
   for (auto& aQueryTrackId : aQueryTrackIds)
   {
      if (aQueryTrackId == aShooterTrack.GetTrackId())
      {
         return true;
      }
   }
   return false;
}

void WsfWeaponThreatProcessor::CreateThreatList(WsfPlatform& aOwnship)
{
   bool             mSearchAllTracks = true;
   std::vector<int> mQueryTrackIds;
   // vector<WsfTrackId> mQueryTrackIds2;

   // WsfTrackList& rawTrackList = aOwnship.GetMasterRawTrackList();
   WsfLocalTrackList& masterTrackList = aOwnship.GetMasterTrackList();

   if (masterTrackList.GetTrackCount() == 0)
   {
      ut::log::info()
         << "WsfWeaponThreatProcessor::CreateThreatList: Platform has NO tracks, so NOT running threat evaluation.";
      return;
   }

   for (unsigned int i = 0; i < masterTrackList.GetTrackCount(); ++i)
   {
      const WsfLocalTrack* shooterTrack = masterTrackList.GetTrackEntry(i);
      if (mSearchAllTracks || Contains(mQueryTrackIds, *shooterTrack))
      {
         CreateThreatList(*shooterTrack, aOwnship, mThreatReportList);
      }
   }

   // Sort the reports by whatever the user set it to.
   std::sort(mThreatReportList.begin(),
             mThreatReportList.end(),
             ThreatReportComparitor(mThreatEvaluator->GetPkSearchParameters().mQuerySort));
}


// Test functions below! . .
//                           .
//                           .
//                          \ /
//                           .


struct Inner
{
   static void Test_CreateGeometryWCS_1(StateReport& shooter, StateReport& ownship)
   {
      // double tx = 12.940952255126039;
      // double a = 0.2617993877991494;
      double n = 965.9258262890684;
      double h = 258.81904510252076;
      // double k = 64.70476127563019;
      // double m = 901.2210650134382;

      double oCpaNED[3]    = {n, h, 0.0};
      double oCpaNEDVel[3] = {0, -20, 0.0};
      double sCpaNEDVel[3] = {5, 0, 0.0};
      shooter.mSpeed       = 5;
      ownship.mSpeed       = 20;

      // double oCpaNEDVelX1[3]  = {901.2210650134382, -5.0,   0.0};
      // double sCpaNEDVelX1[3]  = {5,                  0.0,   0.0};

      double refLLA[3] = {40, -90, 0.0};
      // double refWCSX[3];
      // The conversion will fill in refWCS.
      double* refWCS = shooter.mLocation;
      // UtEllipsoidalEarth::ConvertECEFToLLA(aLocationWCS, lat, lon, alt);
      /////UtEllipsoidalEarth::ConvertLLAToECEF(refLLA[0],refLLA[1],refLLA[2], refWCSX);
      // memcpy(xxx,xxx,sizeof(double)*3);
      double nedXform[3][3];
      UtEllipsoidalEarth::ComputeNEDTransform(refLLA[0], refLLA[1], refLLA[2], nedXform, refWCS);

      //// This version appears to find the WCS offset vector vs absolute?
      /////UtEllipsoidalEarth::ConvertLocalToECEF(nedXform, oCpaNED, ownship.mLocation);
      UtMat3d::InverseTransform(ownship.mLocation, nedXform, oCpaNED);
      // See this
      /////UtEntity::ConvertECSToWCS
      ////UtMat3d::InverseTransform(aLocationWCS, mWCSToECSTransform, aLocationECS);
      ////UtVec3d::Add(aLocationWCS, aLocationWCS, mLocationWCS);
      UtVec3d::Add(ownship.mLocation, ownship.mLocation, refWCS);


      // Create velocities
      // double sCpaNED[3] = {k, 0.0, 0.0};
      UtMat3d::InverseTransform(shooter.mVelocity, nedXform, sCpaNEDVel);
      UtMat3d::InverseTransform(ownship.mVelocity, nedXform, oCpaNEDVel);
   }

   /** TestTIM2, own and true t1 **/
   static void Test_CreateGeometryWCS_2(StateReport& aShooter, StateReport& aOwnship)
   {
      double shooterLocWcs[] = {-36414.858470831874, -4967527.6372321146, 3987444.2969812327};

      double shooterVelWcs[] = {0.87326704045836867, 32.326071290346256, 40.009913888498311};

      double ownLocWcs[] = {-36743.662383192823, -4968009.5701718628, 3986844.8575296043};

      double ownVelWcs[] = {
         0.53676118175751264,
         32.325602491224934,
         40.016221720330137,
      };

      UtVec3d::Set(aShooter.mLocation, shooterLocWcs);
      UtVec3d::Set(aShooter.mVelocity, shooterVelWcs);
      UtVec3d::Set(aOwnship.mLocation, ownLocWcs);
      UtVec3d::Set(aOwnship.mVelocity, ownVelWcs);

      {
         // See if LLA and NED velocity looks right
         double ownLLA[3];
         UtEllipsoidalEarth::ConvertECEFToLLA(ownLocWcs, ownLLA[0], ownLLA[1], ownLLA[2]);
         double nedXform[3][3];
         double ownWsc2[3];
         UtEllipsoidalEarth::ComputeNEDTransform(ownLLA[0], ownLLA[1], ownLLA[2], nedXform, ownWsc2);
         double ownVelNed[3];
         UtMat3d::Transform(ownVelNed, nedXform, ownVelWcs);
         //?? ownVelNed[2] = ownVelNed[2];
      }
      {
         // See if LLA and NED velocity looks right
         double shooterLLA[3];
         UtEllipsoidalEarth::ConvertECEFToLLA(shooterLocWcs, shooterLLA[0], shooterLLA[1], shooterLLA[2]);
         double nedXform[3][3];
         double shooterWsc2[3];
         UtEllipsoidalEarth::ComputeNEDTransform(shooterLLA[0], shooterLLA[1], shooterLLA[2], nedXform, shooterWsc2);
         double shooterVelNed[3];
         UtMat3d::Transform(shooterVelNed, nedXform, shooterVelWcs);
         //?? shooterVelNed[2] = shooterVelNed[2];
      }
   }

   static void Test_FindCrossingECS()
   {
      Test_FindCrossingECS_2();
      Test_FindCrossingECS_1();
   }

   static void Test_FindCrossingECS_2()
   {
      double      eps = 0.001;
      StateReport shooter;
      StateReport ownship;

      Test_CreateGeometryWCS_2(shooter, ownship);

      ToECS(shooter, ownship);
      ToECS(shooter);

      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[0], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[1], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[2], eps), "");
   }

   static void Test_FindCrossingECS_1()
   {
      StateReport shooter;
      StateReport ownship;

      Test_CreateGeometryWCS_1(shooter, ownship);

      double tx = 12.940952255126039;
      // double a = 0.2617993877991494;
      double n = 965.9258262890684;
      double h = 258.81904510252076;
      // double k = 64.70476127563019;
      double m   = 901.2210650134382;
      double eps = 0.001;

      ToECS(shooter, ownship);
      ToECS(shooter);

      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[0], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[1], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mLocation[2], eps), "");

      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(ownship.mLocation[0], n, eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(ownship.mLocation[1], h, eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(ownship.mLocation[2], eps), "");

      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(shooter.mVelocity[0], shooter.mSpeed, eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mVelocity[1], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(shooter.mVelocity[2], eps), "");

      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(ownship.mVelocity[0], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(ownship.mVelocity[1], -ownship.mSpeed, eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(ownship.mVelocity[2], eps), "");

      Geometry geometryXing;
      FindCrossingECS(shooter, ownship, geometryXing);

      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(geometryXing.mTimeCpa, tx, eps), "");

      double* oCpa = geometryXing.mLocationCPAOwnshipNorm;
      WeaponThreatProcessorThrowLocal(UtMath::NearlyEqual(oCpa[0], m, eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(oCpa[1], eps), "");
      WeaponThreatProcessorThrowLocal(UtMath::NearlyZero(oCpa[2], eps), "");
   }
};
