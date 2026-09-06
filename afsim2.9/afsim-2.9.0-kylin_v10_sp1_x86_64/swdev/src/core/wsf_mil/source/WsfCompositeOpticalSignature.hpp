// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCOMPOSITEOPTICALSIGNATURE_HPP
#define WSFCOMPOSITEOPTICALSIGNATURE_HPP

#include "wsf_mil_export.h"

#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "UtAtmosphere.hpp"
#include "UtCalendar.hpp"
#include "UtCloneablePtr.hpp"
#include "UtMat3.hpp"
#include "UtTable.hpp"
#include "UtVec3.hpp"
#include "WsfOpticalSignature.hpp"

//! WsfCompositeOpticalSignature provides a dynamic, radiometric calculation of
//! a platform's composite visual and infrared signatures.  The basic shape of
//! the platform is user-defined in terms of radiative surfaces (box, cone, cylinder, hemisphere) that
//! are placed relative to each other.  This geometry allows for projected area calculations
//! that are used in the signature computations.
class WSF_MIL_EXPORT WsfCompositeOpticalSignature : public WsfOpticalSignature
{
public:
   //! A stripped down version of UtMat3d.
   //! At the time this was written, UtMat3d had some serious deficiencies. We only need a very small subset,
   //! so this was created until UtMat3d is cleaned up. Its name is the same length that eliminating this
   //! class means simply changing all references of MyMat3d to UtMat3d - along with a couple of code changes.
   class MyMat3d
   {
   public:
      MyMat3d() { UtMat3d::Identity(mMat); }
      MyMat3d(const MyMat3d&) = default;
      MyMat3d& operator=(const MyMat3d&) = default;

      UtVec3d InverseMultiply(const UtVec3d& aRhs) const
      {
         UtVec3d result;
         UtMat3d::InverseTransform(result.GetData(), mMat, aRhs.GetData());
         return result;
      }

      UtVec3d Multiply(const UtVec3d& aRhs) const
      {
         UtVec3d result;
         UtMat3d::Transform(result.GetData(), mMat, aRhs.GetData());
         return result;
      }

      void Identity() { UtMat3d::Identity(mMat); }

      double mMat[3][3];
   };

   using TablePtr = UtCloneablePtr<UtTable::Table>;

   // Allowable independent variable indices for temperature table lookups.
   enum
   {
      cIV_TIME,
      cIV_THROTTLE,
      cIV_ALTITUDE,
      cIV_SPEED,
      cIV_MACH,
      cIV_COUNT //<! Number of potential independent variables
   };

   enum TemperatureType
   {
      cTT_UNDEFINED,
      cTT_TABLE,
      cTT_AMBIENT,
      cTT_ADIABATIC_WALL
   };

   //! A 'Surface' represents a surface on the platform.
   //! At the current time all surfaces represent 3-dimensional closed surfaces
   //! @note An implementation of a 'Surface' must not contain any data that can be modified after it is defined.
   //! Instances of these are shared between all instances of a particular signature type (hence the 'const'
   //! on ProjectedArea and ComputeTemperature'.)
   class Surface
   {
   public:
      using MyMat3d = WsfCompositeOpticalSignature::MyMat3d;

      //! Input data to 'RadiantIntensity'
      struct RI_Input
      {
         WsfPlatform* mPlatformPtr;
         UtVec3d      mUnit_po; // Platform->Observer
         BandList     mBands;
      };

      //! Input data for 'UpdateGeometry'
      struct UG_Input
      {
         WsfPlatform* mPlatformPtr;
         MyMat3d      mWCS_ToECS_Transform; // The WCS->ECS transform for the platform
         // The current thrust for updating the plume model.
         // This will be less than zero if the mover is not a guided mover.
         double mCurrentThrust;
         // Pointer to the previous surface in the list. Used by the plume model for updating plume blockage.
         Surface* mPrevSurfacePtr;

         // The Sun/Earth data is NOT used currently. It is being retained for possible future needs.
         // UtVec3d      mUnit_vel;                    // Platform velocity
         // UtVec3d      mUnit_ps;                     // Platform->Sun
         // UtVec3d      mUnit_pe;                     // Platform->Earth
         // UtVec3d      mUnit_po;                     // Platform->Observer
      };

      //! Input data for 'UpdateTemperature'
      struct UT_Input
      {
         WsfPlatform* mPlatformPtr;
         double       mAmbientTemperature;
         double       mAdiabaticAdjustmentFactor;
         double       mLookupArgs[cIV_COUNT];
      };

      Surface();
      Surface(const Surface& aSrc) = default;
      virtual ~Surface()           = default;
      Surface& operator=(const Surface& aSrc) = delete;

      virtual Surface* Clone() const;

      virtual bool   InitializeType(UtVec3d& aDefaultLocationECS);
      virtual bool   ProcessInput(UtInput& aInput);
      virtual double ProjectedArea(const UtVec3d& aUnit_v) const;
      virtual double RadiantIntensity(const RI_Input& aInput) const;
      virtual double AdjacentArea(const UtVec3d& aOtherFaceLocECS, const UtVec3d& aOtherFaceNormalECS) const;
      virtual void   CheckAdjacency(const Surface& aOther);
      virtual void   UpdatePlumeRadius(double aPlumeRadius) { mPlumeRadius = aPlumeRadius; }
      virtual void   UpdateGeometry(const UG_Input& aInput);
      virtual void   UpdateTemperature(const UT_Input& aInput);

      bool   AreAdjacent(const UtVec3d& aOtherFaceLocECS,
                         const UtVec3d& aOtherFaceNormalECS,
                         const UtVec3d& aMyFaceLocPCS,
                         const UtVec3d& aMyFaceNormalPCS) const;
      bool   UpdateSharedFaceArea(const Surface& aOther,
                                  const UtVec3d& aMyFaceLocPCS,
                                  const UtVec3d& aMyFaceNormalPCS,
                                  double         aMaxSharedArea,
                                  double&        aSharedArea);
      void   UpdateLocation(UtVec3d& aDefaultLocationECS, const UtVec3d& aSizeSCS, const UtVec3d& aOffsetSCS);
      double UpdatePlumeBlockage(double aArea, double aCosTheta) const;

      TemperatureType mTemperatureType{cTT_UNDEFINED};
      TablePtr        mTemperaturePtr{nullptr};
      double          mTemperatureOffset{0.0};
      double          mRecoveryFactor{0.85};
      double          mGamma{1.4};
      UtVec3d         mLocationECS{0.0, 0.0, 0.0};
      bool            mUseDefaultLocation{true};

      // The ECS->SCS(aka PCS) transform.
      // This is defined in this class as the identity transform. The Orientable class will update
      // this with the oriented version.
      MyMat3d mECS_ToSCS_Transform;

      // This is the temperature evaluated at the current conditions.
      double mTemperature{0.0};

      // The WCS->SCS(aka PCS) (Surface) transform at the current conditions.
      MyMat3d mWCS_ToSCS_Transform;

      // The real or effective radius of the aft X face
      double mAftRadius{0.0};

      // Radius of the attached plume (if the following surface is a 'plume').
      double mPlumeRadius{0.0};

      int mDebugLevel{0}; // Copied from the top-level signature...
   };

   static WsfOpticalSignature* ObjectFactory(const std::string& aTypeName);

   WsfCompositeOpticalSignature();
   WsfCompositeOpticalSignature(const WsfCompositeOpticalSignature& aSrc);
   ~WsfCompositeOpticalSignature() override;
   WsfCompositeOpticalSignature& operator=(const WsfCompositeOpticalSignature&) = delete;

   WsfOpticalSignature* Clone() const override;

   bool Initialize(double aSimTime, WsfPlatform* aPlatformPtr) override;

   bool InitializeType() override;

   std::vector<WsfStringId> GetStateNames() const override;

   bool ProcessInput(UtInput& aInput) override;

   float GetSignature(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation) override;

   bool GetProjectedArea(double aSimTime, WsfStringId aStateId, double aAzimuth, double aElevation, double& aProjectedArea) override;

   bool GetRadiantIntensity(double          aSimTime,
                            WsfStringId     aStateId,
                            const BandList& aBands,
                            double          aAzimuth,
                            double          aElevation,
                            double&         aRadiantIntensity) override;

   static Surface* SurfaceFactory(const std::string& aType, const Surface& aDefaultData);

private:
   //! A state is a named collection of surfaces that represent a 'configuration' of the platform
   class State
   {
   public:
      explicit State(WsfStringId aStateId)
         : mStateId(aStateId)
         , mSurfaces()
      {
      }
      State(const State& aSrc);
      ~State();
      State& operator=(const State&) = delete;

      WsfStringId         mStateId;
      std::list<Surface*> mSurfaces;
   };

   State* SelectState(WsfStringId aId) const;
   void   UpdateGeometry(double aSimTime, State& aState);
   void   UpdateTemperature(double aSimTime, State& aState);

   std::list<State*> mStates;
   unsigned int      mDebugLevel{0};

   //! The following are used only during input processing.
   //@{
   Surface     mDefaultSurface;
   WsfStringId mCurrentStateId{"default"};
   //@}

   //! @name Current geometric information.
   //! This information is updated when the simulation time is updated.
   //! Note that each platform that uses this signature has an instance of this data.
   //!@{
   std::recursive_mutex mGeometryUpdateMutex; // Protect the update during multi-threading.
   double               mGeometryUpdateTime{-1.0};

   // The Sun/Earth data is NOT used currently. It is being retained for possible future needs.
   // See UpdateGeometry for more information...
   // UtCalendar                 mCurrentTime;           // The current simulation time, as a calendar object
   //// All locations and vectors are in WCS.
   // UtVec3d                    mLoc_s;                 // Location of the Sun
   // UtVec3d                    mLoc_p;                 // Location of the platform
   // UtVec3d                    mUnit_vel;              // Unit vector of the platform velocity
   // UtVec3d                    mUnit_ps;               // Unit vector from the platform to the Sun
   // UtVec3d                    mUnit_pe;               // Unit vector from the platform to the Earth
   // UtVec3d                    mUnit_es;               // Unit vector from the Earth to the Sun
   // double                     mDist_ps{0.0};          // Distance from the platform to the Sun
   // double                     mDist_pe{0.0};          // Distance from the platform to the Earth
   // double                     mDist_es{0.0};          // Distance from the Earth to the Sun
   //@}

   //! @name Current surface temperatures.
   //! This information is updated when the simulation time is updated.
   //! Note that each platform that uses this signature has an instance of this data.
   //@{
   std::recursive_mutex mTempUpdateMutex; // Protect the update during multi-threading.
   double               mTempUpdateTime{-1.0};
   UtAtmosphere         mAtmosphere{UtAtmosphere::CreateNewAtmosphereTable()};
   //@}
};

#endif
