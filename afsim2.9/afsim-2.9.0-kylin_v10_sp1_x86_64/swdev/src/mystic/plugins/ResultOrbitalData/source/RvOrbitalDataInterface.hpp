//*****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
//*****************************************************************************

#ifndef RVORBITALDATAINTERFACE_HPP
#define RVORBITALDATAINTERFACE_HPP

#include <QObject>

namespace rv
{
class ResultData;
class EntityState;
} // namespace rv

namespace RvOrbitalData
{

struct OrbitalDataContainer
{
   std::string mName           = "";
   double      mEccentricity   = 0.0; // unitless
   double      mSemiMajorAxis  = 0.0; // length
   double      mInclination    = 0.0; // angle
   double      mRAAN           = 0.0; // angle
   double      mArgOfPeriapsis = 0.0; // angle
   double      mTrueAnomaly    = 0.0; // angle

   /** Check for a platform name to see if we've loaded */
   bool Valid() const { return !mName.empty(); }
};

class Interface : public QObject
{
   Q_OBJECT

public:
   explicit Interface(QObject* aParentPtr);
   ~Interface() override = default;

   /** Set the platform of interest, to see data about it.
    * @param aPlatformName platform of interest.
    */
   void SetPlatformOfInterest(const std::string& aPlatformName);

   /** Advance time and read new data.
    * @param aData data to read from
    */
   void AdvanceTimeRead(const rv::ResultData& aData);

   static OrbitalDataContainer GenerateOrbitalData(const rv::EntityState& aState, double aSimTime);

   /** Get the data container. */
   const OrbitalDataContainer GetOrbitalData();

   /** Get if Orbital Data is being populated. */
   bool IsOrbitalVisible();

private:
   static constexpr int cINVALID_INDEX   = -1;
   static constexpr int cLOOKUP_REQUIRED = 0;
   OrbitalDataContainer mOrbitalData;
   int                  mIndex;
   bool                 mOrbitalVisible;
};
} // namespace RvOrbitalData
#endif
