// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVVASCENARIOMANAGERINTERFACE_HPP
#define RVVASCENARIOMANAGERINTERFACE_HPP

#include <map>
#include <queue>

#include <QEvent>
#include <QMutex>
#include <QObject>

#include "RvResultDb.hpp"
#include "UtCallbackHolder.hpp"
#include "WkfDefines.hpp"

namespace rv
{
class ResultData;
}
namespace wkf
{
class Platform;
class Scenario;
} // namespace wkf

namespace RvVaScenarioManager
{
class Interface : public QObject
{
   friend class Plugin;

public:
   Interface(QObject* parent);
   ~Interface() override;

private slots:
   // Executed on the simulation thread to read and write data from/to the simulation
   void AdvanceTimeRead(const rv::ResultData& aData);
   void PlatformAdded(rv::ResultDb& aData, const rv::MsgPlatformInfo& aInfo);
   // Executed on GUI thread
   void GuiUpdate();
   void ClearScenario(bool aFullReset);

private:
   struct VisualPart
   {
      bool        mVisible;
      double      mReadTime;
      bool        mDirty;
      bool        mArticulationDirty;
      std::string mIcon;
      double      mLocation[3];
      double      mOrientation[3];
   };
   struct PlatformState
   {
      std::string                       mName{};
      std::string                       mIcon{};
      std::string                       mSide{};
      rv::StringList                    mTypeList{};
      bool                              mVisible{false};
      wkf::SpatialDomain                mSpatialDomain{wkf::SPATIAL_DOMAIN_UNKNOWN};
      double                            mLocationWCS[3]     = {0.0};
      double                            mVelocityWCS[3]     = {0.0};
      double                            mAccelerationWCS[3] = {0.0};
      double                            mOrientationWCS[3]  = {0.0};
      std::map<std::string, VisualPart> mVisualPart;

      wkf::Platform* mEntityPtr{nullptr};
   };
   enum AliveState
   {
      cUNKNOWN,
      cALIVE,
      cDEAD,
      cREMOVED
   };
   using PlatformStateMap = std::map<unsigned int, PlatformState>;
   using PlatformMap      = std::map<unsigned int, rv::ResultPlatform*>;

   QMutex           mMutex;
   PlatformMap      mPlatforms;
   PlatformStateMap mPlatformStates;
   wkf::Scenario*   mScenarioPtr; // The VTK scenario (not the WSF scenario)
   double           mSimTime;
   bool             mResetCamera;
   bool             mValidEntityFound;
};
} // namespace RvVaScenarioManager
#endif // RVVASCENARIOMANAGERINTERFACE_HPP
