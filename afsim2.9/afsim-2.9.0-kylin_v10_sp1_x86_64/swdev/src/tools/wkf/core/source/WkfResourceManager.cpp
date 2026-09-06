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

#include "WkfResourceManager.hpp"

#include <algorithm>

#include <QBoxLayout>
#include <QMenu>
#include <QTreeWidget>

#include "DtedTile.hpp"
#include "UtEarth.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtMath.hpp"
#include "UtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfObserver.hpp"

wkf::ResourceManager::ResourceManager()
{
   mResourcesAction = new wkf::Action(QIcon::fromTheme("disks"), "Scenario Resources", wkfEnv.GetMainWindow());
   QObject::connect(mResourcesAction, &QAction::triggered, [this]() { Show(); });
   QMenu* toolsMenu = wkfEnv.GetMainWindow()->GetMenuByName("Tools");
   if (toolsMenu != nullptr)
   {
      toolsMenu->menuAction()->setVisible(false);
      toolsMenu->addAction(mResourcesAction);
   }

   mCallbacks.Add(wkf::Observer::StandardScenarioAdded.Connect(&ResourceManager::Reset, this));
   mDEM_Thread = ut::make_unique<DEM_Thread>();
   mDEM_Thread->Start();
}

wkf::ResourceManager::~ResourceManager()
{
   mDEM_Thread->Stop();
   mDEM_Thread->Join();
}

void wkf::ResourceManager::AddDTED_Source(const QString& aPath, int aLevel)
{
   mDEM_Sources.emplace_back(DTED_Source(aPath, aLevel));
   mDEM_Thread->AddDTED_Directory(aPath.toStdString(), aLevel);
}

void wkf::ResourceManager::AddRawDTED_Source(const QString& aPath, int aLevel, int aSouth, int aWest, int aNorth, int aEast)
{
   mDEM_Sources.emplace_back(DTED_RawSource(aPath, aLevel, aSouth, aWest, aNorth, aEast));
   mDEM_Thread->AddDTED_Directory(aPath.toStdString(), aLevel, aSouth, aWest, aNorth, aEast);
}

void wkf::ResourceManager::AddGeotiffSource(const QString& aPath)
{
   mDEM_Sources.emplace_back(GeotiffDEM_Source(aPath));
   mDEM_Thread->AddGeotiffDirectory(aPath.toStdString());
}

void wkf::ResourceManager::Show()
{
   if (!mDialog)
   {
      mDialog = new QDialog(wkfEnv.GetMainWindow());
      mDialog->setAttribute(Qt::WA_DeleteOnClose);
      mDialog->setWindowTitle("Resources");
      mDialog->setLayout(new QVBoxLayout(mDialog));
      QTreeWidget* treePtr = new QTreeWidget(mDialog);
      treePtr->setColumnCount(6);
      mDialog->layout()->addWidget(treePtr);

      if (!mDEM_Sources.empty())
      {
         QTreeWidgetItem* dtedTop    = nullptr;
         QTreeWidgetItem* geotiffTop = nullptr;
         for (auto&& item : mDEM_Sources)
         {
            if (item.mType == "DTED(raw)")
            {
               DTED_RawSource* drs = static_cast<DTED_RawSource*>(&item);
               if (!dtedTop)
               {
                  dtedTop = new QTreeWidgetItem(QStringList{"DTED"});
                  treePtr->addTopLevelItem(dtedTop);
               }
               dtedTop->addChild(new QTreeWidgetItem(QStringList{item.mPath,
                                                                 QString::number(drs->mLevel),
                                                                 QString::number(drs->mSouth),
                                                                 QString::number(drs->mWest),
                                                                 QString::number(drs->mNorth),
                                                                 QString::number(drs->mEast)}));
            }
            else if (item.mType == "DTED")
            {
               DTED_Source* ds = static_cast<DTED_Source*>(&item);
               if (!dtedTop)
               {
                  dtedTop = new QTreeWidgetItem(QStringList{"DTED"});
                  treePtr->addTopLevelItem(dtedTop);
               }
               dtedTop->addChild(new QTreeWidgetItem(QStringList{item.mPath, QString::number(ds->mLevel)}));
            }
            else if (item.mType == "Geotiff(DEM)")
            {
               if (!geotiffTop)
               {
                  geotiffTop = new QTreeWidgetItem(QStringList{"Geotiff"});
                  treePtr->addTopLevelItem(geotiffTop);
               }
               geotiffTop->addChild(new QTreeWidgetItem(QStringList{item.mPath}));
            }
         }
      }
      treePtr->expandAll();
      treePtr->resizeColumnToContents(0);
      // connect to clears...
      // connect to adds...
      // add controls...
   }

   mDialog->show();
}

void wkf::ResourceManager::Reset(wkf::Scenario* aScenarioPtr)
{
   mDEM_Sources.clear();
   mDEM_Thread->RemoveDirectories();
}

bool wkf::ResourceManager::QueryElevation(size_t aRequestId, double aLat, double aLon, float& aAlt)
{
   GeoElevationTile* tile = mDEM_Thread->GetAndLockTile();
   if (tile)
   {
      if (tile->Contains(aLat, aLon))
      {
         if (0 == tile->GetInterpValue(aLat, aLon, aAlt))
         {
            mDEM_Thread->UnlockTile();
            return true;
         }
         mDEM_Thread->UnlockTile();
         return false;
      }
      else
      {
         mDEM_Thread->UnlockTile();
         if (mDEM_Thread->RequestTile(aLat, aLon))
         {
            return QueryElevation(aRequestId, aLat, aLon, aAlt);
         }
         return false;
      }
   }
   else
   {
      if (mDEM_Thread->RequestTile(aLat, aLon))
      {
         return QueryElevation(aRequestId, aLat, aLon, aAlt);
      }
      return false;
   }
}

void wkf::ResourceManager::LOS_Request(size_t aRequestId, const UtVec3d& aSrc, const UtVec3d& aTgt)
{
   mDEM_Thread->LOS_Request(aRequestId, aSrc, aTgt);
}

bool wkf::ResourceManager::LOS_Result(size_t aRequester, std::map<float, float>& aResult)
{
   return mDEM_Thread->LOS_Result(aRequester, aResult);
}

void wkf::ResourceManager::ElevationRequest(size_t aRequester, const std::pair<double, double>& aLatLon)
{
   mDEM_Thread->ElevationRequest(aRequester, aLatLon);
}

bool wkf::ResourceManager::ElevationResult(size_t aRequester, std::pair<float, bool>& aElevation)
{
   return mDEM_Thread->ElevationResult(aRequester, aElevation);
}

void wkf::ResourceManager::ReleaseRequests(size_t aRequestId) {}

wkf::DEM_Thread::DEM_Thread()
   : UtThread()
{
   mDtedTileManager = ut::make_unique<DtedTileManager>();
   mDtedTileManager->SetTileCacheSize(100);
   mGeotiffTileManager = ut::make_unique<Geodata::GeotiffTileManager>();
}

void wkf::DEM_Thread::Run()
{
   while (mRunning)
   {
      if (mRequest && mActiveTileManager)
      {
         std::lock_guard<std::mutex> lock(mMutex);
         GeoElevationTile*           tile = mActiveTileManager->LoadElevationTile(mRequestLat, mRequestLon);
         if (!tile->IsDummyTile())
         {
            mActiveTile = tile;
         }
         mRequest = false;
      }
      else
      {
         // wait for new requests
         std::unique_lock<std::mutex> lock(mRequestMutex);
         mRequestMadeCond.wait(lock,
                               [&]()
                               {
                                  return !mLOS_RequestOrder.empty() || !mElevationRequestOrder.empty() ||
                                         (mRequest && mActiveTileManager) || !mRunning;
                               });
      }
      bool                        req = false;
      size_t                      activereq;
      std::pair<UtVec3d, UtVec3d> reqpoints;
      {
         std::unique_lock<std::mutex> reqLock(mLOS_RequestMutex, std::try_to_lock);
         if (reqLock.owns_lock())
         {
            if (!mLOS_RequestOrder.empty())
            {
               req       = true;
               activereq = mLOS_RequestOrder.front();
               mLOS_RequestOrder.pop();
               reqpoints = mLOS_Requests[activereq];
               mLOS_Requests.erase(activereq);
            }
         }
      }
      if (req)
      {
         // execute on the request
         std::map<float, float>      temp = BuildProfile(activereq, reqpoints);
         std::lock_guard<std::mutex> lock(mLOS_ResultMutex);
         // retrieve the results
         mLOS_Results[activereq] = std::move(temp);
      }

      // Section for irregularly scheduled requests
      bool                      elevReq = false;
      size_t                    activeElevReq;
      std::pair<double, double> latLon;
      {
         std::unique_lock<std::mutex> elevReqLock(mElevationRequestMutex, std::try_to_lock);
         if (elevReqLock.owns_lock() && !mElevationRequestOrder.empty())
         {
            elevReq       = true;
            activeElevReq = mElevationRequestOrder.front();
            mElevationRequestOrder.pop();
            latLon = mElevationRequests[activeElevReq];
            mElevationRequests.erase(activeElevReq);
         }
      }
      if (elevReq)
      {
         std::lock_guard<std::mutex> lock(mMutex);
         if (mActiveTileManager)
         {
            GeoElevationTile* tile = mActiveTileManager->LoadElevationTile(latLon.first, latLon.second);
            if (!tile->IsDummyTile())
            {
               float elevation;
               if (0 == tile->GetInterpValue(latLon.first, latLon.second, elevation))
               {
                  std::lock_guard<std::mutex> lock(mElevationResultMutex);
                  mElevationResults[activeElevReq] = std::pair<float, bool>(elevation, true);
               }
               else
               {
                  std::lock_guard<std::mutex> lock(mElevationResultMutex);
                  mElevationResults.erase(activeElevReq);
               }
            }
            else
            {
               std::lock_guard<std::mutex> lock(mElevationResultMutex);
               mElevationResults.erase(activeElevReq);
            }
         }
         else
         {
            std::lock_guard<std::mutex> lock(mElevationResultMutex);
            mElevationResults.erase(activeElevReq);
         }
      }
   }
}

void wkf::DEM_Thread::Stop()
{
   {
      std::lock_guard<std::mutex> lock(mRequestMutex);
      mRunning = false;
   }
   mRequestMadeCond.notify_one();
}

GeoElevationTile* wkf::DEM_Thread::GetAndLockTile()
{
   if (mMutex.try_lock())
   {
      if (!mActiveTile)
      {
         UnlockTile();
         return nullptr;
      }
      return mActiveTile;
   }
   return nullptr;
}

void wkf::DEM_Thread::UnlockTile()
{
   mMutex.unlock();
}

bool wkf::DEM_Thread::RequestTile(double aLat, double aLon)
{
   if (!mRequest)
   {
      std::lock_guard<std::mutex> lock(mMutex);
      if (mActiveTile != nullptr)
      {
         if (mActiveTile->Contains(aLat, aLon))
         {
            return true;
         }
         if (mActiveTileManager)
         {
            mActiveTileManager->UnloadElevationTile(mActiveTile);
         }
         mActiveTile = nullptr;
      }
      if (mActiveTileManager != nullptr)
      {
         mActiveTile = mActiveTileManager->GetCachedTile(aLat, aLon);
         if (mActiveTile)
         {
            if (!mActiveTile->IsDummyTile())
            {
               return true;
            }
         }
      }
      mActiveTile = nullptr;
      mRequestLat = aLat;
      mRequestLon = aLon;
      {
         std::lock_guard<std::mutex> reqlock(mRequestMutex);
         mRequest = true;
      }
      mRequestMadeCond.notify_one();
      return false;
   }
   return false;
}

void wkf::DEM_Thread::AddDTED_Directory(const std::string& aPath, int aLevel, int aSouth, int aWest, int aNorth, int aEast)
{
   std::lock_guard<std::mutex> lock(mMutex);
   mDtedTileManager->AddDirectory(aPath.c_str(), aLevel, aSouth, aWest, aNorth, aEast);
   {
      std::lock_guard<std::mutex> reqlock(mRequestMutex);
      mActiveTileManager = mDtedTileManager.get();
   }
   mRequestMadeCond.notify_one();
}

void wkf::DEM_Thread::AddDTED_Directory(const std::string& aPath, int aLevel)
{
   std::lock_guard<std::mutex> lock(mMutex);
   mDtedTileManager->AddDirectory(aPath.c_str(), aLevel);
   {
      std::lock_guard<std::mutex> reqlock(mRequestMutex);
      mActiveTileManager = mDtedTileManager.get();
   }
   mRequestMadeCond.notify_one();
}

void wkf::DEM_Thread::AddGeotiffDirectory(const std::string& aPath)
{
   std::lock_guard<std::mutex> lock(mMutex);
   mGeotiffTileManager->AddDirectory(aPath);
   {
      std::lock_guard<std::mutex> reqlock(mRequestMutex);
      mActiveTileManager = mGeotiffTileManager.get();
   }
   mRequestMadeCond.notify_one();
}

void wkf::DEM_Thread::RemoveDirectories()
{
   std::lock_guard<std::mutex> lock(mMutex);
   mActiveTile = nullptr;
   mDtedTileManager->RemoveDirectories();
   mGeotiffTileManager->RemoveDirectories();
}

void wkf::DEM_Thread::LOS_Request(size_t aRequester, const UtVec3d& aSrc, const UtVec3d& aDst)
{
   bool isNewRequest = false;
   {
      std::lock_guard<std::mutex> lock(mLOS_RequestMutex);
      if (mLOS_Requests.find(aRequester) == mLOS_Requests.end())
      {
         std::lock_guard<std::mutex> reqlock(mRequestMutex);
         mLOS_RequestOrder.push(aRequester);
         isNewRequest = true;
      }
      mLOS_Requests[aRequester] = std::make_pair(aSrc, aDst);
   }
   if (isNewRequest)
   {
      mRequestMadeCond.notify_one();
   }
}

bool wkf::DEM_Thread::LOS_Result(size_t aRequester, std::map<float, float>& aResult)
{
   bool                         retval{false};
   std::unique_lock<std::mutex> lock(mLOS_ResultMutex, std::try_to_lock);
   if (lock.owns_lock())
   {
      auto&& res = mLOS_Results.find(aRequester);
      if (res != mLOS_Results.end())
      {
         aResult = res->second;
         mLOS_Results.erase(res);
         retval = true;
      }
   }
   return retval;
}

std::map<float, float> wkf::DEM_Thread::BuildProfile(size_t aRequestId, const std::pair<UtVec3d, UtVec3d>& aPoints)
{
   std::map<float, float> retval;
   if (mActiveTileManager == nullptr)
   {
      return retval;
   }

   // Calculate the unit vector between the source and destination positions.
   double unitSrcToDst[3];
   UtVec3d::Set(unitSrcToDst,
                aPoints.second.Get(0) - aPoints.first.Get(0),
                aPoints.second.Get(1) - aPoints.first.Get(1),
                aPoints.second.Get(2) - aPoints.first.Get(2));
   const double srcToDstDist = UtVec3d::Magnitude(unitSrcToDst);

   UtVec3d::Normalize(unitSrcToDst);

   // Set Up Variables for main loop

   double currPosECEF[3];
   double currLat;
   double currLon;
   double currAlt;
   double stepDist;
   double currDist = 0;

   UtVec3d::Set(currPosECEF, aPoints.first.GetData());

   double srcLat;
   double srcLon;
   double srcAlt;
   UtEllipsoidalEarth::ConvertECEFToLLA(aPoints.first.GetData(), srcLat, srcLon, srcAlt);
   GeoElevationTile* currTilePtr = mActiveTileManager->LoadElevationTile(srcLat, srcLon);
   double            latInterval = currTilePtr->GetLatInterval();
   float             terrainHeightF;
   currTilePtr->GetElevInterp(srcLat, srcLon, terrainHeightF);
   double heightAboveTerrain = srcAlt - terrainHeightF;
   retval[0.0]               = heightAboveTerrain;

   // Start of main loop
   bool badData = false;
   while (currDist < srcToDstDist)
   {
      if (badData)
      {
         stepDist = 30.0;
         badData  = false;
      }
      else
      {
         static const double maxLatInterval = 1.0 / 3600.0; // One arcsecond == Level 2 DTED (degrees)
         if (latInterval < maxLatInterval)
         {
            // The step distance is less than the ~30m DTED level 2 limit.
            // Use the smaller value in the step distance computation.
            stepDist = std::max(heightAboveTerrain / 3.0, latInterval * UtMath::cRAD_PER_DEG * UtEarth::cA);
         }
         else // Clamp to the one arcsecond (~30m) limit
         {
            stepDist = std::max(heightAboveTerrain / 3.0, 30.0);
         }
      }

      currPosECEF[0] += stepDist * unitSrcToDst[0];
      currPosECEF[1] += stepDist * unitSrcToDst[1];
      currPosECEF[2] += stepDist * unitSrcToDst[2];
      currDist += stepDist;

      UtEllipsoidalEarth::ConvertECEFToLLA(currPosECEF, currLat, currLon, currAlt);

      // the geotiff tile manager may have a better tile, even if the point is located in this tile, so we need to
      // query it differently than DTED, where we expect all tiles to be non-overlapping
      if (mActiveTileManager == mGeotiffTileManager.get())
      {
         if (!mGeotiffTileManager->IsBestTile(dynamic_cast<Geodata::GeotiffTile*>(currTilePtr), currLat, currLon))
         {
            if ((currTilePtr != mActiveTile) && (!currTilePtr->IsDummyTile()))
            {
               mActiveTileManager->UnloadElevationTile(currTilePtr);
            }
            currTilePtr = mActiveTileManager->LoadElevationTile(currLat, currLon);
         }
         currTilePtr->GetElevInterp(currLat, currLon, terrainHeightF);
      }
      else if (currTilePtr->GetElevInterp(currLat, currLon, terrainHeightF) != 0)
      // Interpolate tempHeight from DTED file data, if we've gone off the
      // map (status != 0) load the new tile and try again.
      {
         if ((currTilePtr != mActiveTile) && (!currTilePtr->IsDummyTile()))
         {
            mActiveTileManager->UnloadElevationTile(currTilePtr);
         }
         currTilePtr = mActiveTileManager->LoadElevationTile(currLat, currLon);
         currTilePtr->GetElevInterp(currLat, currLon, terrainHeightF);
      }

      // We need to account for the 'holes' in the DTED map data.
      // Approximately -400 meters MSL is the lowest point on Earth (Dead Sea shore in Israel)
      // In short: if the calculated height is below the Dead Sea level, we
      // know it's bad data, so don't update terrainHeightF (use the last good
      // data point known).
      if (terrainHeightF < -500.0f)
      {
         badData = true;
      }
      else
      {
         heightAboveTerrain              = currAlt - double(terrainHeightF);
         retval[currDist / srcToDstDist] = heightAboveTerrain;
      }
   }
   if ((currTilePtr != mActiveTile) && (!currTilePtr->IsDummyTile()))
   {
      mActiveTileManager->UnloadElevationTile(currTilePtr);
   }
   return retval;
}

void wkf::DEM_Thread::ElevationRequest(size_t aRequester, const std::pair<double, double>& aLatLon)
{
   bool isNewRequest = false;
   {
      std::lock_guard<std::mutex> lock(mElevationRequestMutex);
      if (mElevationRequests.find(aRequester) == mElevationRequests.end())
      {
         std::lock_guard<std::mutex> reqlock(mRequestMutex);
         mElevationRequestOrder.push(aRequester);
         isNewRequest = true;
      }
      mElevationRequests[aRequester] = aLatLon;
   }
   {
      std::lock_guard<std::mutex> lock(mElevationResultMutex);
      // Provide a placeholder so that the caller of the result function knows something is still pending
      mElevationResults[aRequester] = std::make_pair(0.0F, false);
   }
   if (isNewRequest)
   {
      mRequestMadeCond.notify_one();
   }
}

// This will only return false if no elevation request has been made.
bool wkf::DEM_Thread::ElevationResult(size_t aRequester, std::pair<float, bool>& aElevation)
{
   bool                         retVal = false;
   std::unique_lock<std::mutex> lock(mElevationRequestMutex, std::try_to_lock);
   if (lock.owns_lock())
   {
      auto&& result = mElevationResults.find(aRequester);
      if (result != mElevationResults.end())
      {
         aElevation = result->second;
         retVal     = true;
      }
   }
   return retVal;
}
