// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WKFRESOURCEMANAGER_HPP
#define WKFRESOURCEMANAGER_HPP

#include "wkf_export.h"

#include <condition_variable>
#include <memory>
#include <queue>
#include <set>
#include <unordered_map>

#include <QPointer>
#include <QString>
#include <QStringList>

#include "DtedTileManager.hpp"
#include "GeotiffTileManager.hpp"
#include "UtThread.hpp"
#include "UtVec3.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfAction.hpp"

class GeoElevationTile;

namespace wkf
{
class Scenario;

class DEM_Thread : public UtThread
{
public:
   DEM_Thread();
   ~DEM_Thread() override = default;

   void              Run() override;
   void              Stop(); // lets wait for requests
   GeoElevationTile* GetAndLockTile();
   void              UnlockTile();
   // returns true if a tile is immediately available
   bool RequestTile(double aLat, double aLon);
   void AddDTED_Directory(const std::string& aPath, int aLevel);
   void AddDTED_Directory(const std::string& aPath, int aLevel, int aSouth, int aWest, int aNorth, int aEast);
   void AddGeotiffDirectory(const std::string& aPath);
   void RemoveDirectories();
   void LOS_Request(size_t aRequester, const UtVec3d& aSrc, const UtVec3d& aDst);
   bool LOS_Result(size_t aRequester, std::map<float, float>& aResult);

   // These requests are specifically for potentially simultaneous queries that are used in irregular, unscheduled intervals
   // To use, request an elevation at some lat/lon, then check for the result later after some time has elapsed
   void ElevationRequest(size_t aRequester, const std::pair<double, double>& aLatLon);
   bool ElevationResult(size_t aRequester, std::pair<float, bool>& aElevation);

private:
   std::map<float, float> BuildProfile(size_t aRequestId, const std::pair<UtVec3d, UtVec3d>& aPoints);

   std::unique_ptr<DtedTileManager>             mDtedTileManager;
   std::unique_ptr<Geodata::GeotiffTileManager> mGeotiffTileManager;
   GeoElevationTileManager*                     mActiveTileManager{nullptr};
   GeoElevationTile*                            mActiveTile{nullptr};
   bool                                         mRunning{true};
   std::mutex                                   mMutex;
   bool                                         mUnload{false};
   bool                                         mRequest{false};
   double                                       mRequestLat;
   double                                       mRequestLon;

   std::mutex                                            mElevationRequestMutex;
   std::unordered_map<size_t, std::pair<double, double>> mElevationRequests;
   std::unordered_map<size_t, std::pair<float, bool>>    mElevationResults; // float = elevation result, bool(true) =
                                                                            // successful result, while bool(false) = p
   std::queue<size_t> mElevationRequestOrder;
   std::mutex         mElevationResultMutex;

   std::mutex                                    mLOS_RequestMutex;
   std::map<size_t, std::pair<UtVec3d, UtVec3d>> mLOS_Requests;
   std::map<size_t, std::map<float, float>>      mLOS_Results;
   std::queue<size_t>                            mLOS_RequestOrder;
   std::mutex                                    mLOS_ResultMutex;

   std::mutex              mRequestMutex;
   std::condition_variable mRequestMadeCond;
};

class WKF_EXPORT ResourceManager
{
public:
   ResourceManager();
   virtual ~ResourceManager();

   void AddDTED_Source(const QString& aPath, int aLevel);
   void AddRawDTED_Source(const QString& aPath, int aLevel, int aSouth, int aWest, int aNorth, int aEast);
   void AddGeotiffSource(const QString& aPath);
   void Show();
   void Reset(wkf::Scenario* aScenarioPtr);
   bool QueryElevation(size_t aRequestId, double aLat, double aLon, float& aAlt);
   void LOS_Request(size_t aRequesterId, const UtVec3d& aSrc, const UtVec3d& aTgt);

   bool LOS_Result(size_t aRequester, std::map<float, float>& aResult);

   void ElevationRequest(size_t aRequester, const std::pair<double, double>& aLatLon);
   bool ElevationResult(size_t aRequester, std::pair<float, bool>& aElevation);


   size_t Register() { return mRequesterId++; }
   void   ReleaseRequests(size_t aRequestId);
   // Simple requests (such as elevation querying) can use the default ID instead of registering
   static const size_t cDEFAULT_REQUEST_ID = 0;

private:
   class DEM_Source
   {
   public:
      QString mType;
      QString mPath;

   protected:
      DEM_Source(const QString& aType, const QString& aPath)
         : mType(aType)
         , mPath(aPath)
      {
      }
   };
   class DTED_Source : public DEM_Source
   {
   public:
      DTED_Source(const QString& aStr, int aLevel)
         : DEM_Source("DTED", aStr)
         , mLevel(aLevel)
      {
      }
      int mLevel;

   protected:
      DTED_Source(const QString& aType, const QString& aStr, int aLevel)
         : DEM_Source(aType, aStr)
         , mLevel(aLevel)
      {
      }
   };
   class DTED_RawSource : public DTED_Source
   {
   public:
      DTED_RawSource(const QString& aStr, int aLevel, int aSouth, int aWest, int aNorth, int aEast)
         : DTED_Source("DTED(raw)", aStr, aLevel)
         , mSouth(aSouth)
         , mWest(aWest)
         , mNorth(aNorth)
         , mEast(aEast)
      {
      }
      int mSouth;
      int mWest;
      int mNorth;
      int mEast;
   };
   class GeotiffDEM_Source : public DEM_Source
   {
   public:
      GeotiffDEM_Source(const QString& aPath)
         : DEM_Source("Geotiff(DEM)", aPath)
      {
      }
   };
   QPointer<QDialog>       mDialog;
   std::vector<DEM_Source> mDEM_Sources;
   Action*                 mResourcesAction;
   vespa::VaCallbackHolder mCallbacks;

   std::unique_ptr<DEM_Thread> mDEM_Thread;
   // Manual registration for requests that require more complex operations will start at 1
   size_t mRequesterId{1};
};
} // namespace wkf

#endif
