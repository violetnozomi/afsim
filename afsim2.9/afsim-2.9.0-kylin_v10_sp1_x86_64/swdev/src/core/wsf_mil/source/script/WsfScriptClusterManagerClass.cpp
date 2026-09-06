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

#include "script/WsfScriptClusterManagerClass.hpp"

#include "UtMemory.hpp"
#include "UtScriptMethodDefine.hpp"
#include "UtScriptRef.hpp"
#include "UtScriptVector.hpp"
#include "WsfAssetPerception.hpp"
#include "WsfClusterManager.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfTrack.hpp"
#include "script/WsfScriptDefs.hpp"

WsfScriptClusterManagerClass::WsfScriptClusterManagerClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   UT_SCRIPT_WRAP_CLASS(WsfClusterManager);

   UT_SCRIPT_STATIC_METHOD(std::unique_ptr<WsfClusterManager>, Create, ())
   {
      return ut::make_unique<WsfClusterManager>(*SIMULATION);
   };

   UT_SCRIPT_METHOD(void, SetNumClustersToCreate, (int aNum)) { self.SetNumClustersToCreate(std::max(aNum, 0)); };

   UT_SCRIPT_METHOD(void, SetClusterMethod, (const std::string& aMethod))
   {
      if (aMethod == "K_MEANS")
      {
         self.SetClusterMethod(WsfClusterManager::cK_MEANS);
      }
      else if (aMethod == "H_TREE_AVG")
      {
         self.SetClusterMethod(WsfClusterManager::cHIERARCHICAL_TREE_AVERAGE);
      }
      else if (aMethod == "H_TREE_MIN")
      {
         self.SetClusterMethod(WsfClusterManager::cHIERARCHICAL_TREE_SINGLE);
      }
      else // aMethod == "H_TREE_MAX"
      {
         self.SetClusterMethod(WsfClusterManager::cHIERARCHICAL_TREE_COMPLETE);
      }
   };

   UT_SCRIPT_METHOD(void, SetDistanceFunction, (const std::string& aFunction))
   {
      // if (aFunction == "POSITION_VELOCITY")
      //{
      //    self.>SetDistanceFunction(WsfClusterManager::cPOSITION_VELOCITY);
      // }
      // else
      if (aFunction == "2D_POSITION_ONLY")
      {
         self.SetDistanceFunction(WsfClusterManager::c2D_POSITION_ONLY);
      }
      else // aFunction == "POSITION_ONLY"
      {
         self.SetDistanceFunction(WsfClusterManager::cPOSITION_ONLY);
      }
   };

   UT_SCRIPT_METHOD(void, SetScriptDistanceFunction, (const std::string& aFunctionName))
   {
      self.SetUserDistanceFunction(&aContext, aFunctionName);
   };

   UT_SCRIPT_WRAP_MEMBER("SetDistanceLimit", SetDistanceLimit);

   // The UT_SCRIPT_WRAP_MEMBER API doesn't provide a way to resolve overloads, so we need to manually resolve
   // the pointers and call the wrapping logic directly without the help of a macro.
   std::vector<std::vector<WsfTrack*>> (WsfClusterManager::*GetClusters_1)(WsfTrackList&) =
      &WsfClusterManager::GetTrackClusters;
   std::vector<std::vector<WsfLocalTrack*>> (WsfClusterManager::*GetClusters_2)(WsfLocalTrackList&) =
      &WsfClusterManager::GetTrackClusters;
   std::vector<std::vector<WsfTrack*>> (WsfClusterManager::*GetClusters_4)(const std::vector<WsfTrack*>&) =
      &WsfClusterManager::GetTrackClusters;
   std::vector<std::vector<WsfPlatform*>> (WsfClusterManager::*GetClusters_5)(const std::vector<WsfPlatform*>&) =
      &WsfClusterManager::GetPlatformClusters;
   std::vector<std::vector<WsfGeoPoint*>> (WsfClusterManager::*GetClusters_6)(const std::vector<WsfGeoPoint*>&) =
      &WsfClusterManager::GetGeoPointClusters;
   std::vector<std::vector<WsfAssetPerception*>> (WsfClusterManager::*GetClusters_7)(
      const std::vector<WsfAssetPerception*>&) = &WsfClusterManager::GetAssetClusters;
   std::vector<std::vector<WsfLocalTrack*>> (WsfClusterManager::*GetClusters_8)(const std::vector<WsfLocalTrack*>&) =
      &WsfClusterManager::GetTrackClusters;
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_1);
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_2);
   AddMethod(ut::make_unique<GetClusters_3>("GetClusters")); // can't be currently implemented with UT_SCRIPT_METHOD macro.
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_4);
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_5);
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_6);
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_7);
   MethodBuilder::WrapFuncPtr(scriptClass, "GetClusters", GetClusters_8);

   unsigned int (WsfClusterManager::*UniqueId_1)(const std::vector<WsfTrack*>&)    = &WsfClusterManager::UniqueId;
   unsigned int (WsfClusterManager::*UniqueId_2)(const std::vector<WsfPlatform*>&) = &WsfClusterManager::UniqueId;
   unsigned int (WsfClusterManager::*UniqueId_3)(const std::vector<WsfGeoPoint*>&) = &WsfClusterManager::UniqueId;
   unsigned int (WsfClusterManager::*UniqueId_4)(const std::vector<WsfAssetPerception*>&) = &WsfClusterManager::UniqueId;
   MethodBuilder::WrapFuncPtr(scriptClass, "UniqueId", UniqueId_1);
   MethodBuilder::WrapFuncPtr(scriptClass, "UniqueId", UniqueId_2);
   MethodBuilder::WrapFuncPtr(scriptClass, "UniqueId", UniqueId_3);
   MethodBuilder::WrapFuncPtr(scriptClass, "UniqueId", UniqueId_4);
   UT_SCRIPT_METHOD(unsigned int, UniqueId, (UtScriptVector<WsfLocalTrack*> aLocalTrackList))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());
      return self.UniqueId(trackList);
   };

   // can't use member for ConvexHull methods, because a deep copy of the return values be performed.
   UT_SCRIPT_METHOD(UtScriptVector<std::unique_ptr<WsfTrack>>, ConvexHull, (UtScriptVector<WsfTrack*> aTrackList))
   {
      auto                                      hull = self.ConvexHull(aTrackList.toStdVector());
      UtScriptVector<std::unique_ptr<WsfTrack>> hullCopy(aContext);
      hullCopy.reserve(hull.size());
      for (const auto& track : hull)
      {
         hullCopy.push_back(ut::clone(track));
      }
      return hullCopy;
   };

   // deep clone not necessary when returning WsfPlatform array, so the wrapper API can be used.
   std::vector<WsfPlatform*> (WsfClusterManager::*ConvexHull_2)(const std::vector<WsfPlatform*>&) =
      &WsfClusterManager::ConvexHull;
   MethodBuilder::WrapFuncPtr(scriptClass, "ConvexHull", ConvexHull_2);

   UT_SCRIPT_METHOD(UtScriptVector<std::unique_ptr<WsfGeoPoint>>, ConvexHull, (UtScriptVector<WsfGeoPoint*> aGeoPointList))
   {
      auto                                         hull = self.ConvexHull(aGeoPointList.toStdVector());
      UtScriptVector<std::unique_ptr<WsfGeoPoint>> hullCopy(aContext);
      hullCopy.reserve(hull.size());
      for (const auto& track : hull)
      {
         hullCopy.push_back(ut::clone(track));
      }
      return hullCopy;
   };

   UT_SCRIPT_METHOD(UtScriptVector<std::unique_ptr<WsfAssetPerception>>,
                    ConvexHull,
                    (UtScriptVector<WsfAssetPerception*> aAssetList))
   {
      auto                                                hull = self.ConvexHull(aAssetList.toStdVector());
      UtScriptVector<std::unique_ptr<WsfAssetPerception>> hullCopy(aContext);
      hullCopy.reserve(hull.size());
      for (const auto& track : hull)
      {
         hullCopy.push_back(ut::clone(track));
      }
      return hullCopy;
   };

   UT_SCRIPT_METHOD(UtScriptVector<WsfLocalTrack>, ConvexHull, (UtScriptVector<WsfLocalTrack*> aLocalTrackList))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());
      std::vector<WsfTrack*> hull = self.ConvexHull(trackList);
      // convert hull of WsfTrack to hull of script's WsfLocalTrack.
      UtScriptVector<WsfLocalTrack> hullCopy(aContext);
      hullCopy.reserve(hull.size());
      for (const auto& track : hull)
      {
         hullCopy.emplace_back(*track);
      }
      return hullCopy;
   };

   WsfGeoPoint (WsfClusterManager::*MeanLocation_1)(const std::vector<WsfTrack*>&) = &WsfClusterManager::MeanLocation;
   WsfGeoPoint (WsfClusterManager::*MeanLocation_2)(const std::vector<WsfPlatform*>&) = &WsfClusterManager::MeanLocation;
   WsfGeoPoint (WsfClusterManager::*MeanLocation_3)(const std::vector<WsfGeoPoint*>&) = &WsfClusterManager::MeanLocation;
   WsfGeoPoint (WsfClusterManager::*MeanLocation_4)(const std::vector<WsfAssetPerception*>&) =
      &WsfClusterManager::MeanLocation;
   MethodBuilder::WrapFuncPtr(scriptClass, "MeanLocation", MeanLocation_1);
   MethodBuilder::WrapFuncPtr(scriptClass, "MeanLocation", MeanLocation_2);
   MethodBuilder::WrapFuncPtr(scriptClass, "MeanLocation", MeanLocation_3);
   MethodBuilder::WrapFuncPtr(scriptClass, "MeanLocation", MeanLocation_4);
   UT_SCRIPT_METHOD(WsfGeoPoint, MeanLocation, (UtScriptVector<WsfLocalTrack*> aLocalTrackList))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());
      return self.MeanLocation(trackList);
   };

   WsfTrack* (WsfClusterManager::*NearestMember_1)(const std::vector<WsfTrack*>&, WsfGeoPoint&) =
      &WsfClusterManager::NearestMember;
   WsfPlatform* (WsfClusterManager::*NearestMember_2)(const std::vector<WsfPlatform*>&, WsfGeoPoint&) =
      &WsfClusterManager::NearestMember;
   WsfGeoPoint* (WsfClusterManager::*NearestMember_3)(const std::vector<WsfGeoPoint*>&, WsfGeoPoint&) =
      &WsfClusterManager::NearestMember;
   WsfAssetPerception* (WsfClusterManager::*NearestMember_4)(const std::vector<WsfAssetPerception*>&, WsfGeoPoint&) =
      &WsfClusterManager::NearestMember;
   MethodBuilder::WrapFuncPtr(scriptClass, "NearestMember", NearestMember_1);
   MethodBuilder::WrapFuncPtr(scriptClass, "NearestMember", NearestMember_2);
   MethodBuilder::WrapFuncPtr(scriptClass, "NearestMember", NearestMember_3);
   MethodBuilder::WrapFuncPtr(scriptClass, "NearestMember", NearestMember_4);
   UT_SCRIPT_METHOD(WsfLocalTrack*, NearestMember, (UtScriptVector<WsfLocalTrack*> aLocalTrackList, WsfGeoPoint & aGeoPoint))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());
      return static_cast<WsfLocalTrack*>(self.NearestMember(trackList, aGeoPoint));
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    NearestEdgePoint,
                    (UtScriptVector<WsfTrack*> aTrackList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* edgePointPtr = new WsfGeoPoint();
      size_t       index1;
      size_t       index2;
      self.NearestEdgePoint(aTrackList.toStdVector(), aGeoPoint, *edgePointPtr, index1, index2);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back((int)index1);
      resultVecPtr->emplace_back((int)index2);
      UtScriptClass* geoPointClassPtr     = aContext.GetTypes()->GetClass("WsfGeoPoint");
      UtScriptRef*   geoPointScriptRefPtr = new UtScriptRef(edgePointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    NearestEdgePoint,
                    (UtScriptVector<WsfPlatform*> aPlatformList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* edgePointPtr = new WsfGeoPoint();
      size_t       index1;
      size_t       index2;
      self.NearestEdgePoint(aPlatformList.toStdVector(), aGeoPoint, *edgePointPtr, index1, index2);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back((int)index1);
      resultVecPtr->emplace_back((int)index2);
      UtScriptClass* geoPointClassPtr     = aContext.GetTypes()->GetClass("WsfGeoPoint");
      UtScriptRef*   geoPointScriptRefPtr = new UtScriptRef(edgePointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    NearestEdgePoint,
                    (UtScriptVector<WsfGeoPoint*> aGeoPointList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* edgePointPtr = new WsfGeoPoint();
      size_t       index1;
      size_t       index2;
      self.NearestEdgePoint(aGeoPointList.toStdVector(), aGeoPoint, *edgePointPtr, index1, index2);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back((int)index1);
      resultVecPtr->emplace_back((int)index2);
      UtScriptClass* geoPointClassPtr     = aContext.GetTypes()->GetClass("WsfGeoPoint");
      UtScriptRef*   geoPointScriptRefPtr = new UtScriptRef(edgePointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    NearestEdgePoint,
                    (UtScriptVector<WsfAssetPerception*> aAssetList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* edgePointPtr = new WsfGeoPoint();
      size_t       index1;
      size_t       index2;
      self.NearestEdgePoint(aAssetList.toStdVector(), aGeoPoint, *edgePointPtr, index1, index2);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back((int)index1);
      resultVecPtr->emplace_back((int)index2);
      UtScriptClass* geoPointClassPtr     = aContext.GetTypes()->GetClass("WsfGeoPoint");
      UtScriptRef*   geoPointScriptRefPtr = new UtScriptRef(edgePointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    NearestEdgePoint,
                    (UtScriptVector<WsfLocalTrack*> aLocalTrackList, WsfGeoPoint & aGeoPoint))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());

      WsfGeoPoint* edgePointPtr = new WsfGeoPoint();
      size_t       index1;
      size_t       index2;
      self.NearestEdgePoint(trackList, aGeoPoint, *edgePointPtr, index1, index2);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back((int)index1);
      resultVecPtr->emplace_back((int)index2);
      UtScriptClass* geoPointClassPtr     = aContext.GetTypes()->GetClass("WsfGeoPoint");
      UtScriptRef*   geoPointScriptRefPtr = new UtScriptRef(edgePointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(geoPointScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(void, Draw, (UtScriptVector<WsfTrack*> aTrackList, double aDuration, UtVec3d& aColor, std::string aType))
   {
      std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower); // convert type to lower case
      WsfClusterManager::DrawType drawtype = (aType == "star") ? (WsfClusterManager::STAR) : (WsfClusterManager::HULL);
      self.Draw(aTrackList.toStdVector(), aDuration, aColor, drawtype);
   };

   UT_SCRIPT_METHOD(void,
                    Draw,
                    (UtScriptVector<WsfPlatform*> aPlatformList, double aDuration, UtVec3d& aColor, std::string aType))
   {
      std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower); // convert type to lower case
      WsfClusterManager::DrawType drawtype = (aType == "star") ? (WsfClusterManager::STAR) : (WsfClusterManager::HULL);
      self.Draw(aPlatformList.toStdVector(), aDuration, aColor, drawtype);
   };

   UT_SCRIPT_METHOD(void,
                    Draw,
                    (UtScriptVector<WsfGeoPoint*> aGeoPointList, double aDuration, UtVec3d& aColor, std::string aType))
   {
      std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower); // convert type to lower case
      WsfClusterManager::DrawType drawtype = (aType == "star") ? (WsfClusterManager::STAR) : (WsfClusterManager::HULL);
      self.Draw(aGeoPointList.toStdVector(), aDuration, aColor, drawtype);
   };

   UT_SCRIPT_METHOD(void,
                    Draw,
                    (UtScriptVector<WsfAssetPerception*> aAssetList, double aDuration, UtVec3d& aColor, std::string aType))
   {
      std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower); // convert type to lower case
      WsfClusterManager::DrawType drawtype = (aType == "star") ? (WsfClusterManager::STAR) : (WsfClusterManager::HULL);
      self.Draw(aAssetList.toStdVector(), aDuration, aColor, drawtype);
   };

   UT_SCRIPT_METHOD(void,
                    Draw,
                    (UtScriptVector<WsfLocalTrack*> aLocalTrackList, double aDuration, UtVec3d& aColor, std::string aType))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());

      std::transform(aType.begin(), aType.end(), aType.begin(), ::tolower); // convert type to lower case
      WsfClusterManager::DrawType drawtype = (aType == "star") ? (WsfClusterManager::STAR) : (WsfClusterManager::HULL);
      self.Draw(trackList, aDuration, aColor, drawtype);
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfTrack*> aTrackList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(aTrackList.toStdVector(), aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfPlatform*> aPlatformList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(aPlatformList.toStdVector(), aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfPlatform*> aPlatformList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(aPlatformList.toStdVector(), aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfGeoPoint*> aGeoPointList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(aGeoPointList.toStdVector(), aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfAssetPerception*> aAssetList, WsfGeoPoint & aGeoPoint))
   {
      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(aAssetList.toStdVector(), aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };

   UT_SCRIPT_METHOD(std::unique_ptr<UtScriptDataList>,
                    CrossLength,
                    (UtScriptVector<WsfLocalTrack*> aLocalTrackList, WsfGeoPoint & aGeoPoint))
   {
      // converting from WsfLocalTrack to WsfTrack list.
      std::vector<WsfTrack*> trackList(aLocalTrackList.begin(), aLocalTrackList.end());

      WsfGeoPoint* leftPointPtr  = new WsfGeoPoint();
      WsfGeoPoint* rightPointPtr = new WsfGeoPoint();
      double       distance;
      self.CrossLength(trackList, aGeoPoint, distance, *leftPointPtr, *rightPointPtr);
      auto resultVecPtr = ut::make_unique<UtScriptDataList>();
      resultVecPtr->emplace_back(distance);
      UtScriptClass* geoPointClassPtr      = aContext.GetTypes()->GetClass("WsfGeoPoint"); // TODO - static?
      UtScriptRef*   leftGeoPtScriptRefPtr = new UtScriptRef(leftPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(leftGeoPtScriptRefPtr));
      UtScriptRef* rightGeoPtScriptRefPtr = new UtScriptRef(rightPointPtr, geoPointClassPtr, UtScriptRef::cMANAGE);
      resultVecPtr->push_back(UtScriptData(rightGeoPtScriptRefPtr));
      return resultVecPtr;
   };
}

////virtual
// void* WsfScriptClusterManagerClass::Create(const UtScriptContext& aContext)
//{
//    return new WsfClusterManager(*WsfScriptContext::GetSIMULATION(aContext));
// }

////virtual
// void* WsfScriptClusterManagerClass::Clone(void* aObjectPtr)
//{
//    return static_cast<WsfClusterManager*>(aObjectPtr)->Clone();
// }

// virtual
void WsfScriptClusterManagerClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfClusterManager*>(aObjectPtr);
}

// This can't currently be implemented using the new script macros because WsfCommandChain::PlatformList would
// resolve to the Array<WsfPlatform> script type instead of WsfPlatformList script type.
// To fix this, it would be best to make WsfCommandChain::PlatformList in WsfCommandChain.hpp a strong alias
// instead of weak alias of std::vector<WsfPlatform*>, so that it is not ambiguous whether it should be
// Array<WsfPlatform> or WsfPlatformList in script. e.g. change  WsfCommandChain::PlatformList to:
// `struct WsfPlatformList { std::vector<WsfPlatform*> mList; };`
UT_DEFINE_SCRIPT_METHOD(WsfScriptClusterManagerClass, WsfClusterManager, GetClusters_3, 1, "Array<Array<WsfPlatform>>", "WsfPlatformList")
{
   std::vector<WsfPlatform*>* platformList =
      static_cast<std::vector<WsfPlatform*>*>(aVarArgs[0].GetPointer()->GetAppObject());
   std::vector<std::vector<WsfPlatform*>> clusters        = aObjectPtr->GetPlatformClusters(*platformList);
   UtScriptClass*                         objectClassPtr  = aContext.GetTypes()->GetClass("WsfPlatform");
   UtScriptClass*                         clusterClassPtr = aContext.GetTypes()->GetClass("Array<WsfPlatform>");
   std::vector<UtScriptData>*             array2dPtr      = new std::vector<UtScriptData>();
   for (size_t i = 0; i < clusters.size(); ++i)
   {
      std::vector<UtScriptData>* arrayPtr = new std::vector<UtScriptData>();
      for (size_t j = 0; j < clusters[i].size(); ++j)
      {
         arrayPtr->push_back(UtScriptData(new UtScriptRef(clusters[i][j], objectClassPtr, UtScriptRef::cDONT_MANAGE)));
      }
      array2dPtr->push_back(UtScriptData(new UtScriptRef(arrayPtr, clusterClassPtr, UtScriptRef::cMANAGE)));
   }
   aReturnVal.SetPointer(new UtScriptRef(array2dPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}
