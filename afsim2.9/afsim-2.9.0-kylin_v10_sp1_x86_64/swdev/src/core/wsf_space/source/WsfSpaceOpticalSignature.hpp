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

#ifndef WSFSPACEOPTICALSIGNATURE_HPP
#define WSFSPACEOPTICALSIGNATURE_HPP

#include "wsf_space_export.h"

#include <fstream>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "UtCalendar.hpp"
#include "UtMat3.hpp"
#include "UtVec3.hpp"
#include "WsfOpticalSignature.hpp"

//! WsfSpaceOpticalSignature provides the visual/infrared signature and projected area for space platforms
//! based on the position of the Sun.
class WSF_SPACE_EXPORT WsfSpaceOpticalSignature : public WsfOpticalSignature
{
public:
   //! A stripped down version of UtMat3d.
   //! At the time this was written, UtMat3d had some serious deficiencies. We only need a very small subset,
   //! so this was created until UtMat3d is cleaned up. Its name is the same length that eliminating this
   //! class means simply changing all references of MyMat3d to UtMat3d - along with a couple of code changes.
   class MyMat3d
   {
   public:
      MyMat3d()               = default;
      MyMat3d(const MyMat3d&) = default;
      MyMat3d& operator=(const MyMat3d&) = default;

      UtVec3d InverseMultiply(const UtVec3d& aRhs) const
      {
         UtVec3d result;
         UtMat3d::InverseTransform(result.GetData(), mMat, aRhs.GetData());
         return result;
      }

      double mMat[3][3];
   };

   //! Auxilary data that is shared between computation of components.
   //! This is data that can't be stored as member variables because of multi-threading. Each thread could
   //! could have a different observer and different band.
   struct Data
   {
      UtVec3d mUnit_po; // WCS vector from the platform to the observer
      double  mTemp_p;  // Temperature of the platform (from ComputeThermalSignature)
   };

   //! A 'Surface' represents a surface on the platform.
   //! At the current time all surfaces represent 3-dimensional closed surfaces
   //! @note An implementation of a 'Surface' must not contain any data that can be modified after it is defined.
   //! Instances of these are shared between all instances of a particular signature type (hence the 'const'
   //! on ProjectedArea and Reflect'.)
   class Surface
   {
   public:
      using MyMat3d = WsfSpaceOpticalSignature::MyMat3d;

      //! This contains data needed to create the WCS->SCS
      struct RefData
      {
         WsfPlatform* mPlatformPtr;
         double       mWCS_ToECS_Transform[3][3]; // The WCS->ECS transform for the platform
         //@{
         UtVec3d mUnit_vel; // Platform velocity
         UtVec3d mUnit_ps;  // Platform->Sun
         UtVec3d mUnit_pe;  // Platform->Earth
         UtVec3d mUnit_po;  // Platform->Observer
         int     mDebugLevel;
         //@}
      };

      Surface()
         : mListIndex(0)
         , mReflectance(1.0)
         , mTemperatureChangeRate(0.1) // deg-K / sec
         , mMinimumTemperature(173.15) // deg-K
         , mMaximumTemperature(393.15) // deg-K
      {
      }
      virtual ~Surface() = default;

      virtual bool   ProcessInput(UtInput& aInput);
      virtual double ProjectedArea(const MyMat3d& aWCS_ToSCS_Transform, const UtVec3d& aUnit_v) const;
      virtual double Reflect(const MyMat3d& aWCS_ToSCS_Transform,
                             double         aIntensity,
                             const UtVec3d& aUnit_i,
                             double         aDist_i,
                             const UtVec3d& aUnit_v,
                             unsigned int   aDebugLevel) const;
      virtual void   UpdateTransform(const RefData& aRefData, MyMat3d& aWCS_ToSCS_Transform) const;

      size_t mListIndex; // Index of the surface in the list of surfaces for a state
      double mReflectance;
      double mTemperatureChangeRate;
      double mMinimumTemperature;
      double mMaximumTemperature;
   };

   static WsfOpticalSignature* ObjectFactory(const std::string& aTypeName);

   WsfSpaceOpticalSignature();
   WsfSpaceOpticalSignature(const WsfSpaceOpticalSignature& aSrc);
   ~WsfSpaceOpticalSignature() override = default;
   WsfSpaceOpticalSignature& operator=(const WsfSpaceOpticalSignature&) = delete;

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
      // This is never copied or assigned.
      State(const State&) = delete;
      State& operator=(const State&) = delete;

      WsfStringId                         mStateId;
      std::list<std::unique_ptr<Surface>> mSurfaces;
   };

   //! All of the data that can be shared between instances a particular space signature type
   class SharedData
   {
   public:
      SharedData();
      // This is never copied or assigned.
      SharedData(const SharedData&) = delete;
      SharedData& operator=(const SharedData&) = delete;

      bool InitializeType(WsfObject& aBase);

      bool IsA_ValidState(WsfStringId aId) const;

      bool ProcessInput(UtInput& aInput, WsfObject& aBase);

      const State& SelectState(WsfStringId aId) const;

      Surface          mDefaultSurface;
      WsfStringId      mCurrentStateId;
      const State*     mDefaultStatePtr; // Pointer to the default state within the list of states
      std::list<State> mStates;
      size_t           mMaxSurfaceCount; // Maximum number of surfaces in any state
   };

   double ComputeEarthReflectionSignature(double aSimTime, const BandList& aBands, const State& aState, Data& aData);
   double ComputeSolarReflectionSignature(double aSimTime, const BandList& aBands, const State& aState, Data& aData);
   double ComputeThermalSignature(double aSimTime, const BandList& aBands, const State& aState, Data& aData);

   double ComputeReflection(double          aSimTime,
                            const BandList& aBands,
                            const State&    aState,
                            Data&           aData,
                            double          aIntensity,
                            const UtVec3d&  aUnit_ps,
                            double          aDist_ps);

   void GetEclipseState(double aSimTime, Data& aData);
   void UpdateEclipseTimes(double aSimTime, Data& aData);

   void UpdateState(double aSimTime, const State& aState);

   std::shared_ptr<SharedData> mSharedDataPtr;

   // @name Input values
   //@{
   std::string  mPlotFile;
   std::string  mPlotFieldDelimiter;
   unsigned int mDebugLevel;
   bool         mHighResolutionEclipse;
   //@}

   //! @name Current state information.
   //! This information is updated when the simulation time is updated.
   //!@{
   std::recursive_mutex mStateUpdateMutex; // Protect the state update during multi-threading.
   double               mStateUpdateTime;
   UtCalendar           mCurrentTime; // The current simulation time, as a calendar object
   // All locations and vectors are in WCS.
   UtVec3d              mLoc_s;      // Location of the Sun
   UtVec3d              mLoc_p;      // Location of the platform
   UtVec3d              mUnit_vel;   // Unit vector of the platform velocity
   UtVec3d              mUnit_ps;    // Unit vector from the platform to the Sun
   UtVec3d              mUnit_pe;    // Unit vector from the platform to the Earth
   UtVec3d              mUnit_es;    // Unit vector from the Earth to the Sun
   double               mDist_ps;    // Distance from the platform to the Sun
   double               mDist_pe;    // Distance from the platform to the Earth
   double               mDist_es;    // Distance from the Earth to the Sun
   std::vector<MyMat3d> mTransforms; // WCS->SCS transforms for each surface

   // The time when the platform entered the current eclipse/not-eclipsed state.
   // Note that this can be negative when called during the first orbit of a simulation.
   double mEclipseEntryExitTime;
   bool   mIsEclipsed; // True if currently eclipsed, false if not.
   //@}

   //! @name Eclipse information.
   //! See UpdateEclipseTimes for more information about these variables...
   //@{
   std::recursive_mutex mEclipseUpdateMutex; // Protect the eclipse state update during multi-threading
   double               mNextEclipseUpdateTime;
   double               mEclipseEntryTime;
   double               mEclipseExitTime;
   double               mOrbitalPeriod; // Time to complete one orbit
   // The following two are used only when the platform mover is not a space mover.
   double mLastEclipseUpdateTime; // The time when last called
   bool   mLastEclipseState;      // The eclipse state at the last call
   //@}

   std::ofstream mPlotOfs; // Analysis output stream.
};

#endif
