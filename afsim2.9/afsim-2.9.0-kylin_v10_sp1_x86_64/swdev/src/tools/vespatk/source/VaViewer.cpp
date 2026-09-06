// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaViewer.hpp"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "UtEarth.hpp"
#include "UtLLAPos.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtMoon.hpp"
#include "UtSphericalEarth.hpp"
#include "Uto.hpp"
#include "UtoCmeTerrain.hpp"
#include "UtoRenderBinManager.hpp"
#include "UtoShaderUtilities.hpp"
#include "UtoViewerHighlight.hpp"
#include "UtoViewerRTTComposite.hpp"
#include "VaAttachment.hpp"
#include "VaAttachmentModel.hpp"
#include "VaAttachmentWingRibbon.hpp"
#include "VaBoundingBox.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotion.hpp"
#include "VaChooser.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaFactory.hpp"
#include "VaObserver.hpp"
#include "VaOverlay.hpp"
#include "VaOverlayModelInstantiations.hpp"
#include "VaOverlayZoomBox.hpp"
#include "VaPosition.hpp"
#include "VaScenario.hpp"
#include "VaTimeManager.hpp"
#include "VaUtils.hpp"
#include "VaViewerManager.hpp"

//#define DO_DBG(A) /* A */
#define DO_DBG(A) A
// KeyRelease #defined on linux
#undef KeyRelease
#undef KeyPress

namespace vespa
{
class ViewFilter : public UtoViewFilter
{
public:
   ViewFilter(unsigned int aViewerId) { mViewerId = aViewerId; }
   ~ViewFilter() override = default;

   void OnAddSymbol(UtoShape& aShape, UtoViewer& aViewer) override
   {
      unsigned int tId = aShape.GetViewerId();
      if ((tId == 0) || (tId == mViewerId))
      {
         aShape.Show(true, aViewer);
      }
      else
      {
         aShape.Show(false, aViewer);
      }
   }
   // Should not be necessary:
   // virtual void OnRemoveSymbol(UtoShape& aShape, UtoViewer& aViewer)
   //{
   //   ...
   //}

   unsigned int mViewerId;
};

const std::string VaViewer::cProjectionModeStrings[] = {"invalid", "orthographic-2d", "orthographic-3d", "perspective"};

VaViewer::VaViewer(int aViewerType /*= HighlightViewer*/, int aViewerOptions /*= Standard*/)
   : mViewerType(aViewerType)
   , mOptions(aViewerOptions)
{
   std::ostringstream oss;
   oss << "viewer_" << GetUniqueId();
   SetName(oss.str().c_str());
   SetType<VaViewer>();

   if (nullptr == mCameraPtr)
   {
      mCameraPtr = new VaCamera(this);
   }
   mCameraPtr->UsePerspectiveProjection();

   mCallbacks.Add(VaObserver::MapLoadedMap.Connect(&VaViewer::MapLoadedMapCB, this, GetUniqueId()));
   mCallbacks.Add(VaObserver::ECI_ModeChanged.Connect(&VaViewer::ECI_ModeChangedCB, this));
   mCallbacks.Add(VaObserver::CameraSetCenter.Connect(&VaViewer::CameraSetCenterCB, this, GetUniqueId()));
   mCallbacks.Add(VaObserver::SetSaturation.Connect(&VaViewer::SetSaturationCB, this, GetUniqueId()));
   mCallbacks.Add(VaObserver::SetIntensity.Connect(&VaViewer::SetIntensityCB, this, GetUniqueId()));
   mCallbacks.Add(VaObserver::SetContrast.Connect(&VaViewer::SetContrastCB, this, GetUniqueId()));
   mCallbacks.Add(VaObserver::CameraUpdated.Connect(&VaViewer::CameraUpdatedCB, this));
   mCallbacks.Add(VaObserver::EntitySelect.Connect(&VaViewer::EntitySelectCB, this));
   mCallbacks.Add(VaObserver::EntitySelected.Connect(&VaViewer::EntitySelectedCB, this));
   mCallbacks.Add(VaObserver::AttachmentSelected.Connect(&VaViewer::AttachmentSelectedCB, this));
   mCallbacks.Add(VaObserver::BeginUserStat.Connect(&VaViewer::BeginUserStatCB, this));
   mCallbacks.Add(VaObserver::EndUserStat.Connect(&VaViewer::EndUserStatCB, this));
}

VaViewer::~VaViewer()
{
   VaObserver::ViewerDestroyed(this);
   for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
   {
      delete iter->second;
   }
   mOverlays.clear();

   delete mViewerPtr;
   delete mCameraPtr;

   mCallbacks.Clear();
}

void VaViewer::Initialize(osgViewer::Viewer* aViewerPtr, bool aShaderable, bool aGeomShaderable)
{
   if (!mInitialized)
   {
      mInitialized = true;

      // Figure out if this computer supports shaders.
      UtoShaderUtilities::EnableShaders(aShaderable);
      UtoShaderUtilities::EnableGeometryShaders(aGeomShaderable);
      SetupViewer(aViewerPtr, aShaderable);

      // Configure the camera.
      UtoViewer::CameraMode cameraMode =
         mCameraPtr->GetUsePerspectiveProjection() ? UtoViewer::PERSPECTIVE : UtoViewer::ORTHOGONAL;
      SetActiveCamera(cameraMode);

      mViewerPtr->SetClearColor(osg::Vec4(0.0F, 0.57F, 0.75F, 1.0F));

      mViewerPtr->Connect(UtoEntityDB::Instance(""));

      VaEnvironment::Instance().Load(this);

      if (mOptions |= Ephemeris)
      {
         mViewerPtr->SetupCelestialCamera();
      }

      VaObserver::ViewerInitialized(this);
   }
   else
   {
      DO_DBG(auto out = ut::log::debug() << "Viewer has already been initialized";
             out.AddNote() << "ID: " << GetUniqueIdString(););
   }

   CenterCamera();
   SetUniform("uto_LightSourceAmbient", 1.0F, 1.0F, 1.0F, 1.0F);
   SetUniform("uto_LightSourceDiffuse", 1.0F, 1.0F, 1.0F, 1.0F);
   SetUniform("uto_LightSourceSpecular", 1.0F, 1.0F, 1.0F, 1.0F);
}

void VaViewer::SetupViewer(osgViewer::Viewer* aViewerPtr, bool aUseShaders)
{
   if (aUseShaders && (mViewerType == RenderToTexture))
   {
      mViewerPtr = new UtoViewerRTTComposite("VESPA Display");
   }
   else if (aUseShaders && (mViewerType == HighlightViewer))
   {
      mViewerPtr = new UtoViewerHighlight(aViewerPtr, "VESPA Display", mSelectionMask);
   }
   else
   {
      mViewerPtr = new UtoViewer(aViewerPtr, "VESPA Display");
   }
   mViewerPtr->SetFilter(GetNewViewFilter());

   if (mOptions & SmallFeatureCulling)
   {
      mViewerPtr->EnableSmallFeatureCulling();
   }
   // default some uniform values
   mViewerPtr->SetSunVector(mSunVector);
   SetTrueScale(mTrueScale);
   SetIconScale(mIconScale);
   SuppressTeamColor(mSuppressTeamColor);
   SetIntensityCB(mIntensity);
   SetSaturationCB(mSaturation);
   SetContrastCB(mContrast);
   ECI_ModeChangedCB(mECI);
}

UtoViewFilter* VaViewer::GetNewViewFilter()
{
   return new ViewFilter(GetUniqueId());
}

void VaViewer::PrepareToRedraw()
{
   for (auto&& it : mOverlaysToBeRemoved)
   {
      RemoveOverlay(it->GetName());
   }
   mOverlaysToBeRemoved.clear();
   for (auto&& it : mSubordinateMap)
   {
      it.second->PrepareToRedraw();
   }
   if (nullptr != mViewerPtr)
   {
      // TODO_NOT_USED bool viewWasUpdated = mUpdateView;
      if (mUpdateView || mECI)
      {
         bool cameraDirty =
            mCameraPtr->Update(VaEnvironment::Instance().GetTime()->GetCurrentTime(), GetWidth(), GetHeight());

         UpdateView();
         CameraUpdated();
         VaObserver::CameraUpdated(this); // moved this here to update labels to the correct position for the current frame,
         // this was before UpdateView before, and they would lag a frame behind

         if (cameraDirty)
         {
            RequestUpdate();
         }
      }

      mViewerPtr->SetSimulationTime(VaEnvironment::Instance().GetTime()->GetCurrentTime());
      // Update the overlays before drawing.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         iter->second->Update();
      }
   }
}

void VaViewer::UpdateView()
{
   mUpdateView = false;

   mCameraPtr->UpdateView(mViewWidth, mViewHeight);

   SetFeedbackFormatAndPrecision();

   ++mUpdateViewCount;
}

void VaViewer::SetFeedbackFormatAndPrecision()
{
   if (mInitialized)
   {
      int itemp          = 0;
      mFeedbackPrecision = GetMetersPerPixel() / GetMetersPerDegreeLat();
      if (mFeedbackPrecision < (0.01 / 3600.0)) //  .01"
      {
         itemp = static_cast<int>(mFeedbackPrecision * 3600.0 * 1000.0);
         itemp &= (~1);
         if (itemp == 0)
         {
            itemp = 1;
         }
         mFeedbackPrecision = itemp / (3600.0 * 1000.0); // .00x"
         mFeedbackFormat    = UtAngle::FmtDMS + 3;
      }
      else if (mFeedbackPrecision < (0.1 / 3600.0)) // .1"
      {
         itemp = static_cast<int>(mFeedbackPrecision * 3600.0 * 100.0);
         itemp &= (~1);
         if (itemp == 0)
         {
            itemp = 1;
         }
         mFeedbackPrecision = itemp / (3600.0 * 100.0); // .0x"
         mFeedbackFormat    = UtAngle::FmtDMS + 2;
      }
      else if (mFeedbackPrecision < (1.0 / 3600.0)) // 1"
      {
         itemp = static_cast<int>(mFeedbackPrecision * 3600.0 * 10.0);
         itemp &= (~1);
         if (itemp == 0)
         {
            itemp = 1;
         }
         mFeedbackPrecision = itemp / (3600.0 * 10.0); // .x"
         mFeedbackFormat    = UtAngle::FmtDMS + 1;
      }
      else if (mFeedbackPrecision < (10.0 / 3600.0)) // 10"
      {
         itemp = static_cast<int>(mFeedbackPrecision * 3600.0);
         itemp &= (~1);
         if (itemp == 0)
         {
            itemp = 1;
         }
         mFeedbackPrecision = itemp / 3600.0; // x"
         mFeedbackFormat    = UtAngle::FmtDMS;
      }
      else if (mFeedbackPrecision < (60.0 / 3600.0)) // 1'
      {
         itemp = static_cast<int>(mFeedbackPrecision * 600.0);
         itemp &= (~1);
         if (itemp == 0)
         {
            itemp = 1;
         }
         mFeedbackPrecision = itemp / 600.0; // x0"
         mFeedbackFormat    = UtAngle::FmtDMS;
      }
      else // 1 degree
      {
         mFeedbackPrecision = 60.0 / 3600.0;
         mFeedbackFormat    = UtAngle::FmtDM;
      }
   }
}

void VaViewer::Resize(int aX, int aY, int aWidth, int aHeight)
{
   mUpdateView = true;
   mViewX      = aX;
   mViewY      = aY;
   mViewWidth  = aWidth;
   mViewHeight = aHeight;

   if (mViewerPtr != nullptr)
   {
      mViewerPtr->SetDisplaySize(UtoSize(mViewWidth, mViewHeight));
      mViewerPtr->GetScreenOverlayManager()->SetSize(UtoSize(mViewWidth, mViewHeight));
   }

   // Notify the overlays directly.
   for (auto& overlay : mOverlays)
   {
      overlay.second->Resize();
   }

   VaObserver::ViewerResized[GetUniqueId()](this);
}

int VaViewer::GetWidth() const
{
   return mViewWidth;
}

int VaViewer::GetHeight() const
{
   return mViewHeight;
}

void VaViewer::AnimateToCenter(const VaPosition& aPosition, double aAz, double aEl, double aDuration)
{
   mCameraPtr->GetCameraMotion()->AnimateToPosition(aPosition, aAz, aEl, 0.5);
   mUpdateView = true;
}

void VaViewer::SetCenter(const VaPosition& aPosition)
{
   mCameraPtr->GetCameraMotion()->SetReferencePosition(aPosition);
   mUpdateView = true;
}

void VaViewer::CenterCamera(bool aCenterOnSelection /* = true */)
{
   if (mViewerPtr == nullptr)
   {
      return;
   }

   // If nothing selected, center on everything in the scenario.
   bool validBound = false;

   if (mCameraPtr->GetMotionType() == VaCameraMotion::cTETHERED)
   {
      mCameraPtr->Reset();
      mUpdateView = true;
      return;
   }

   VaBoundingBox bbox;

   if (aCenterOnSelection)
   {
      const VaHitSet& selectionList = mSelection;
      VaEntityMap     entities;
      CollectEntities(selectionList, entities);
      if (!entities.empty())
      {
         validBound = true;
         VaEnvironment::Instance().GetBound(entities, bbox);
         bbox.Finalize();
         if (entities.size() == 1)
         {
            VaEntity* ent = entities.begin()->second;
            CenterOnEntity(*ent);
            return;
         }
      }
   }

   if (!validBound)
   {
      validBound = VaEnvironment::Instance().GetScenarioBound(bbox);
      bbox.Finalize();
   }

   if (validBound)
   {
      CenterOnBoundingBox(bbox);
   }
}

void VaViewer::CenterOnEntity(const VaEntity& aEntity)
{
   double a;
   double e;
   double d;
   GetCamera()->GetAzElDist(a, e, d);
   if (d < aEntity.GetPosition().GetAlt())
   {
      d += aEntity.GetPosition().GetAlt();
   }
   AnimateToCenter(vespa::VaPosition(aEntity.GetPosition().GetLat(), aEntity.GetPosition().GetLon(), d));
}

void VaViewer::CenterOnBoundingBox(VaBoundingBox aBoundingBox)
{
   // Make sure that bound isn't a single point.
   double minAngle = 0.01;
   double latDiff  = std::abs(aBoundingBox.mMinLLA[0] - aBoundingBox.mMaxLLA[0]);
   if (latDiff < minAngle)
   {
      double padding = (minAngle - latDiff) / 2;
      aBoundingBox.mMinLLA[0] -= padding;
      if (aBoundingBox.mMinLLA[0] < -90.0)
      {
         aBoundingBox.mMinLLA[0] = -90.0;
      }

      aBoundingBox.mMaxLLA[0] += padding;
      if (aBoundingBox.mMaxLLA[0] > 90.0)
      {
         aBoundingBox.mMaxLLA[0] = 90.0;
      }
   }

   double lonDiff = std::abs(aBoundingBox.mMinLLA[1] - aBoundingBox.mMaxLLA[1]);
   if (lonDiff < minAngle)
   {
      double padding = (minAngle - lonDiff) / 2;
      aBoundingBox.mMinLLA[1] -= padding;
      aBoundingBox.mMinLLA[1] = UtMath::NormalizeAngleMinus180_180(aBoundingBox.mMinLLA[1]);

      aBoundingBox.mMaxLLA[1] += padding;
      aBoundingBox.mMaxLLA[1] = UtMath::NormalizeAngleMinus180_180(aBoundingBox.mMaxLLA[1]);
   }

   // Determine the maximum altitude.
   double maxAlt = aBoundingBox.mMinLLA[2];
   if (aBoundingBox.mMaxLLA[2] > maxAlt)
   {
      maxAlt = aBoundingBox.mMaxLLA[2];
   }

   // Arc distance that when plugged into the camera-distance equation (below) will position the camera so that
   // the earth fills most of the screen.
   double distanceToEarthFullView = 8508185.0; // Perspective distance.
   if (mViewerPtr->ActiveCamera() == UtoViewer::ORTHOGONAL)
   {
      distanceToEarthFullView *= 1.55; // Orthographic distance.
   }

   double dummy;
   double cameraDistance = 0.0;
   GetCamera()->GetCameraMotion()->GetAzElDist(dummy, dummy, cameraDistance);

   // great-circle distances are calculated at sea level, but scale linearly with distance from earths center
   double altitudeScalar = (UtSphericalEarth::cEARTH_RADIUS + maxAlt) / UtSphericalEarth::cEARTH_RADIUS;

   // Calculate the distance between the latitude bounds, scaled for altitude
   double distanceLat, tempHeading;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aBoundingBox.mMinLLA[0],
                                                   0.0,
                                                   aBoundingBox.mMaxLLA[0],
                                                   0.0,
                                                   tempHeading,
                                                   distanceLat);
   distanceLat *= altitudeScalar;

   // Calculate the distance between the longitude bounds, scaled for altitude
   double distanceLon;
   UtSphericalEarth::GreatCircleHeadingAndDistance(0.0,
                                                   aBoundingBox.mMinLLA[1],
                                                   0.0,
                                                   aBoundingBox.mMaxLLA[1],
                                                   tempHeading,
                                                   distanceLon);
   distanceLon *= altitudeScalar;

   // Calculate the distance between the center and mean points, scaled for altitude
   double distanceCenterToMean;
   UtSphericalEarth::GreatCircleHeadingAndDistance(aBoundingBox.mCenterLLA[0],
                                                   aBoundingBox.mCenterLLA[1],
                                                   aBoundingBox.mMeanLLA[0],
                                                   aBoundingBox.mMeanLLA[1],
                                                   tempHeading,
                                                   distanceCenterToMean);
   distanceCenterToMean *= altitudeScalar;

   // Calculate the distance from the mean point to the furthest bound.
   double maxDistance = sqrt(distanceLat * distanceLat + distanceLon * distanceLon) + distanceCenterToMean;

   // Add non-linear scale factor to provide a buffer zone that scales with distance.
   double scale = 1.0 + 0.5 * pow(2.72, -(maxDistance / distanceToEarthFullView));
   maxDistance *= scale;

   cameraDistance = 0.0;
   if (maxDistance > 1.0)
   {
      // Calculate the distance the camera needs to be from the earth to fully view the region that is
      // centered on the mean point and extending out by the maximum distance.
      cameraDistance = (0.5 * maxDistance) / tan(0.5 * GetCamera()->GetFovY() * UtMath::cRAD_PER_DEG) + maxAlt;
   }

   VaPosition center(aBoundingBox.mMeanLLA[0], aBoundingBox.mMeanLLA[1], cameraDistance);
   // note there are limits to our ability to get a good view, this will clamp the distance if too extreme
   AnimateToCenter(center, 0, 90, 0.5);
}

bool VaViewer::SetProjectionMode(ProjectionMode aProjectionMode)
{
   bool redraw = false;

   if (mProjectionMode != aProjectionMode)
   {
      bool changedModes = true;
      switch (aProjectionMode)
      {
      case PmOrthographic3D:
         SetActiveCamera(UtoViewer::ORTHOGONAL);
         break;
      case PmPerspective:
         SetActiveCamera(UtoViewer::PERSPECTIVE);
         break;
      default:
         changedModes = false;
         break;
      }

      if (changedModes)
      {
         mProjectionMode = aProjectionMode;

         redraw = true;
      }
   }
   return redraw;
}

std::string VaViewer::GetProjectionModeString(ProjectionMode aProjectionMode)
{
   return cProjectionModeStrings[aProjectionMode < PmMaxNumberProjections ? aProjectionMode : PmInvalid];
}

VaViewer::ProjectionMode VaViewer::GetProjectionModeEnum(const std::string& aProjectionModeString)
{
   ProjectionMode projectionMode = PmInvalid;
   for (int i = 0; i < PmMaxNumberProjections; ++i)
   {
      if (aProjectionModeString == cProjectionModeStrings[i])
      {
         projectionMode = (ProjectionMode)i;
         break;
      }
   }
   return projectionMode;
}

bool VaViewer::ScreenToWorld(unsigned int aX, unsigned int aY, VaPosition& aPosition) const
{
   bool   llaOk = false;
   double lat;
   double lon;
   double alt;
   llaOk = mCameraPtr->ConvertScreenToLLA(aX, aY, lat, lon, alt);
   if (llaOk)
   {
      // lat -= fmod(lat, 1.0);
      // lon -= fmod(lon, 1.0);

      UtLLAPos llaPos(lat, lon, 0.0);
      llaPos.SetLatFormat(mFeedbackFormat);
      llaPos.SetLonFormat(mFeedbackFormat);
      llaPos.SetAltFormat(UtLength::FmtFT + 2); // TODO - VESPA_FRAMEWORK - Put altitude format back in.
      aPosition.SetLLA(llaPos);
   }
   return llaOk;
}

bool VaViewer::WorldToScreen(const VaPosition& aPosition, unsigned int& aX, unsigned int& aY, unsigned int& aZ) const
{
   bool ok = false;
   if (mViewerPtr != nullptr)
   {
      double xyz[3];
      aPosition.GetSceneXYZ(xyz, 0);

      UtoPosition pos(UtoPositionFromArray(xyz));
      ok = mViewerPtr->World().WorldToWindowUnlimited(pos);
      if (ok)
      {
         aX = (unsigned int)((int)pos.x());
         aY = (unsigned int)((int)pos.y());
         aZ = (unsigned int)((int)pos.z());
      }
   }
   return ok;
}

bool VaViewer::WorldToScreen(const VaPosition& aPosition, double& aX, double& aY, double& aZ) const
{
   bool ok = false;
   if (mViewerPtr != nullptr)
   {
      double xyz[3];
      aPosition.GetSceneXYZ(xyz, 0);

      UtoPosition pos(UtoPositionFromArray(xyz));
      ok = mViewerPtr->World().WorldToWindowUnlimited(pos);
      if (ok)
      {
         aX = pos.x();
         aY = pos.y();
         aZ = pos.z();
      }
   }
   return ok;
}

bool VaViewer::LLAToWorld(double aLat, double aLon, double aAlt, VaPosition& aPosition) const
{
   double alt = aAlt;
   double lat = aLat;
   double lon = aLon;
   // if (0 != mMapOptionsPtr)
   //{
   //    alt *= mMapOptionsPtr->GetElevationMultiplier();
   // }
   if (aLat < -1.0e12)
   {
      lat = -90.0;
   }
   if (aLon < -1.0e12)
   {
      lon = -360.0;
   }
   aPosition.SetLLA(UtLLAPos(lat, lon, alt));
   return true;
}

double VaViewer::HeightOfTerrain(const VaPosition& aPosition) const
{
   double height = 0.0;

   if ((mViewerPtr != nullptr) && mViewerPtr->World().IsTerrainValid())
   {
      double pxyz[3];
      mMapProjection.ConvertFromLLA(aPosition.GetLat(), aPosition.GetLon(), 0.0, pxyz);
      double vxyz[3];
      if (mMapProjection.projection == UtoMapProjection::Geocentric)
      {
         double normer = 1.0 / sqrt(pxyz[0] * pxyz[0] + pxyz[1] * pxyz[1] + pxyz[2] * pxyz[2]);
         vxyz[0]       = -pxyz[0] * normer;
         vxyz[1]       = -pxyz[1] * normer;
         vxyz[2]       = -pxyz[2] * normer;
      }
      else
      {
         vxyz[0] = 0;
         vxyz[1] = 0;
         vxyz[2] = -1;
      }
      double ipoint[3];
      double inorm[3];
      mViewerPtr->World().Terrain().Intersect(pxyz, vxyz, ipoint, inorm);
      double lla[3];
      mMapProjection.ConvertToLLA(ipoint, lla);
      height = lla[2];
   }
   return height;
}

void VaViewer::GetCameraPosition(VaPosition& aPosition) const
{
   aPosition = mCameraPtr->GetCameraPosition();
}

double VaViewer::GetMetersPerPixel() const
{
   return mCameraPtr->GetMetersPerPixel();
}

double VaViewer::GetMetersPerDegreeLat() const
{
   return mCameraPtr->GetMetersPerDegreeLat();
}

double VaViewer::GetMetersPerDegreeLon() const
{
   return mCameraPtr->GetMetersPerDegreeLon();
}

VaOverlay* VaViewer::AddOverlay(const std::string&      aOverlayType,
                                const std::string&      aOverlayName,
                                VaOverlay::OverlayScope aOverlayScope)
{
   RemoveOverlay(aOverlayName); // Overwrite anything already holding this name
   VaOverlay* overlayPtr = VaEnvironment::Instance().GetFactory()->CreateOverlay(aOverlayType, aOverlayName);
   if (overlayPtr != nullptr)
   {
      overlayPtr->SetOverlayScope(aOverlayScope);
      VaOverlay* tempOverlayPtr = overlayPtr;
      overlayPtr                = AddOverlay(*tempOverlayPtr);
      delete tempOverlayPtr;
   }
   return overlayPtr;
}

bool VaViewer::AddOverlay(VaOverlay* aOverlayPtr)
{
   if (mOverlays.find(aOverlayPtr->GetName()) != mOverlays.end())
   {
      return false;
   }
   if (aOverlayPtr->AttachViewer(this))
   {
      mOverlays[aOverlayPtr->GetName()] = aOverlayPtr;
      // if (0 != GetMapOptions())
      //{
      //    if (aOverlayPtr->GetOverlayScope() == VaOverlay::cOVERLAY_GLOBAL)
      //    {
      //       GetMapOptions()->AddLayer(aOverlayPtr);
      //    }
      // }
      return true;
   }
   return false;
}

VaOverlay* VaViewer::AddOverlay(const VaOverlay& aOverlay)
{
   VaOverlay* overlayPtr = nullptr;

   auto iter = mOverlays.find(aOverlay.GetName());
   if (iter == mOverlays.end())
   {
      overlayPtr = aOverlay.Clone();
      if (!AddOverlay(overlayPtr))
      {
         // If we didn't attach successfully, then delete the overlay.
         delete overlayPtr;
         overlayPtr = nullptr;
      }
   }
   else
   {
      overlayPtr = iter->second;
   }
   return overlayPtr;
}

bool VaViewer::RemoveOverlay(const std::string& aOverlayName)
{
   bool removed = false;
   auto iter    = mOverlays.find(aOverlayName);
   if (iter != mOverlays.end())
   {
      if (mHeldOverlayPtr)
      {
         if (mHeldOverlayPtr->GetUniqueId() == iter->second->GetUniqueId())
         {
            mHeldOverlayPtr = nullptr;
         }
      }
      // if (0 != GetMapOptions())
      //{
      //    GetMapOptions()->RemoveLayer(iter->second);
      // }

      delete iter->second;
      mOverlays.erase(iter);
      removed = true;
   }
   return removed;
}

void VaViewer::DeleteLater(VaOverlay* aOverlayPtr)
{
   mOverlaysToBeRemoved.emplace_back(aOverlayPtr);
}

bool VaViewer::CheckSphereOcclusion(const vespa::VaPosition& aPos, double aSphereSize /*= UtEarth::cA*/) const
{
   if (MapProjectionIsGeocentric())
   {
      double ecef[3];
      aPos.GetECEF(ecef);
      osg::Vec4 o4(0.0, 0.0, 0.0, 1.0);
      osg::Vec4 t4(ecef[0], ecef[1], ecef[2], 1.0);

      o4 = o4 * GetViewMatrix();
      t4 = t4 * GetViewMatrix();
      osg::Vec3 o(o4.x(), o4.y(), o4.z());
      osg::Vec3 t(t4.x(), t4.y(), t4.z());

      // factor along vector from view to target that is closest to the world-space origin
      double f = (t * o) / (t.length2());
      // if the nearest point is somewhere between the view and the target
      if ((f > 0) && (f < 1))
      {
         // c is the vector closest to world-space origin
         osg::Vec3 c = t * f - o;
         double    d = c.length2();
         // if the length of c is less than the sphere size we clipped the sphere
         if (d < aSphereSize * aSphereSize)
         {
            return false;
         }
      }
   }
   return true;
}

void VaViewer::SetPerspectiveCamera(const osg::Matrix& aViewMatrix,
                                    double             aXYZ[3],
                                    double             aFovY,
                                    double             aNear,
                                    double             aFar,
                                    bool               aProjectionManaged)
{
   if (!mViewerPtr)
   {
      return;
   }
   mViewerPtr->PerspectiveCamera().SetViewMatrix(aViewMatrix, aXYZ[0], aXYZ[1], aXYZ[2]);
   if (aProjectionManaged)
   {
      if (!mOverridePerspective)
      {
         mViewerPtr->PerspectiveCamera().Set(aFovY, aNear, aFar);
      }
      else
      {
         mViewerPtr->SetProjectionMatrixAsFrustum(mOverrideXMin,
                                                  mOverrideXMax,
                                                  mOverrideYMin,
                                                  mOverrideYMax,
                                                  mOverrideNear,
                                                  mOverrideFar);
      }
   }
}

void VaViewer::OverridePerspective(double aXMin, double aXMax, double aYMin, double aYMax, double aNear, double aFar)
{
   mOverridePerspective = true;
   mOverrideYMin        = aYMin;
   mOverrideYMax        = aYMax;
   mOverrideXMin        = aXMin;
   mOverrideXMax        = aXMax;
   mOverrideNear        = aNear;
   mOverrideFar         = aFar;
   if (!mViewerPtr)
   {
      return;
   }
   mViewerPtr->SetProjectionMatrixAsFrustum(mOverrideXMin, mOverrideXMax, mOverrideYMin, mOverrideYMax, mOverrideNear, mOverrideFar);
}

void VaViewer::GetViewport(double& aX, double& aY, double& aW, double& aH) const
{
   const osg::Viewport* vp = mViewerPtr->GetViewport();
   aX                      = vp->x();
   aY                      = vp->y();
   aW                      = vp->width();
   aH                      = vp->height();
}

void VaViewer::SetOrthographicCamera(const osg::Matrix& aViewMatrix, double aZoom, double aNear, double aFar, bool aProjectionManaged)
{
   if (!mViewerPtr)
   {
      return;
   }
   mViewerPtr->OrthoCamera().SetViewMatrix(aViewMatrix);
   if (aProjectionManaged)
   {
      mViewerPtr->OrthoCamera().SetZoom(aZoom);
      if (mMapProjection.projection == UtoMapProjection::Equirectangular)
      {
         mViewerPtr->OrthoCamera().Set(-1.5 * UtMoon::cA, aFar); // aNear, aFar);
      }
      else
      {
         mViewerPtr->OrthoCamera().Set(aNear, aFar);
      }
   }
}

// virtual
int VaViewer::Btn1Click(int aMouseX, int aMouseY, int aState)
{
   bool fullClear = false;
   if (EventOverride(VaObserver::ButtonClick[GetUniqueId()], this, 1, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status = DontPassOn;
   if (Btn1ClickOverlays(aMouseX, aMouseY, aState))
   {
      return status;
   }
   status = PassOn;
   if (mInitialized)
   {
      VaObserver::BeginSelectionChange(this);
      if (mCameraPtr->GetAllowPicking())
      {
         mPickedEntry = VaHitEntry();

         const int state = aState & (~CapsNumLockStates);

         // Attempt to pick an entity and/or attachment.
         if ((state != ShiftKey) && (state != AltKey))
         {
            PickFromScreen(mHitList, aMouseX, aMouseY);

            // If we click without depressing shift or ctrl or alt, clear the selection list.
            if ((state != CtrlKey) && (state != ShiftKey) && (state != AltKey))
            {
               if (!mSuppressMouseSelectionChanges)
               {
                  ClearSelection();
                  fullClear = true;
               }
            }

            if (!mSuppressMouseSelectionChanges)
            {
               if (ChooseAndPickHit(mHitList, aMouseX, aMouseY, true, false, mPickedEntry))
               {
                  fullClear = false;
               }
            }
         }
         else
         {
            // If we click without depressing shift or ctrl or alt, clear the selection list.
            if ((state != CtrlKey) && (state != ShiftKey) && (state != AltKey))
            {
               if (!mSuppressMouseSelectionChanges)
               {
                  ClearSelection();
                  fullClear = true;
               }
            }
         }
      }
      VaObserver::EndSelectionChange(this);
   }
   if (fullClear)
   {
      VaObserver::SelectionCleared(this);
      // send out a signal that this mouse click cleared the selection
   }
   return status;
}

// virtual
int VaViewer::Btn1DblClick(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDoubleClick[GetUniqueId()], this, 1, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status = DontPassOn;
   if (Btn1DblClickOverlays(aMouseX, aMouseY, aState))
   {
      return status;
   }

   status = PassOn;
   if (mInitialized)
   {
      // center and zoom in on cursor position
      SetSavedCursorPosition(aMouseX, aMouseY);

      if (mHitList.empty())
      {
         mCameraPtr->MouseButtonDoublePress(aMouseX, aMouseY, 1, aState);

         mUpdateView = true;
      }
   }
   return status;
}

// virtual
int VaViewer::Btn1Down(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDown[GetUniqueId()], this, 1, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status = DontPassOn;

   if (Btn1DownOverlays(aMouseX, aMouseY, aState))
   {
      return status;
   }

   if (mInitialized)
   {
      SetSavedCursorPosition(aMouseX, aMouseY);

      VaPosition curLLA;
      bool       llaOk = ScreenToWorld(aMouseX, aMouseY, curLLA);

      mCameraPtr->MouseButtonPress(aMouseX, aMouseY, 1, aState);
      mCameraDrag = true;
      mUpdateView = true;

      mDragEntity   = nullptr;
      mDragPossible = false;
      mDragActive   = false;

      const int state = aState & (~CapsNumLockStates);
      if (state == 0)
      {
         std::list<VaHitEntry> hl;
         PickFromScreen(hl, aMouseX, aMouseY);
         for (auto&& it : hl)
         {
            VaEntity* entity = it.FindEntity();
            if (entity)
            {
               if (entity->IsSimpleDraggable())
               {
                  mDragEntity   = entity;
                  mDragLLA      = curLLA;
                  mDragPossible = true;
                  break;
               }
            }
         }
      }
      if (state == ShiftKey)
      {
      }
      else if ((state == CtrlKey) && llaOk)
      {
         // This could be the start of an object drag or it could be the down
         // stroke of a ctrl-click selection.  We must check to see what entities
         // are hit here rather than waiting because we want to record the state
         // when the mouse went down and not after it has been possible dragged.
         PickFromScreen(mHitList, aMouseX, aMouseY);
         if (!mHitList.empty())
         {
            mDragLLA      = curLLA;
            mDragPossible = true;
         }
      }
   }

   return status;
}

// virtual
int VaViewer::Btn1Drag(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDrag[GetUniqueId()], this, 1, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   VaObserver::UpdateCursorPosition[GetUniqueId()](this, aMouseX, aMouseY);

   int status = DontPassOn;

   if (Btn1DragOverlays(aMouseX, aMouseY, aState))
   {
      return status;
   }

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);

      VaPosition curLLA;
      bool       llaOk = ScreenToWorld(aMouseX, aMouseY, curLLA);

      if (mDragEntity && mDragPossible && llaOk && (mCameraPtr->GetAllowPicking()))
      {
         mMoveActive = true;
         UtLLAPos cLLA;
         UtLLAPos dLLA;
         curLLA.GetLLA(cLLA);
         mDragLLA.GetLLA(dLLA);
         UtEntity refbefore;
         refbefore.SetLocationLLA(dLLA.GetLat(), dLLA.GetLon(), dLLA.GetAlt());
         UtEntity refAfter;
         refAfter.SetLocationLLA(cLLA.GetLat(), cLLA.GetLon(), cLLA.GetAlt());
         mDragEntity->MoveRelative(refbefore, refAfter, mFeedbackFormat, -mFeedbackPrecision, cMR_ENTITY, this);
         mDragLLA = curLLA;
         return DontPassOn;
      }
      else if (mDragPossible && llaOk && (mCameraPtr->GetAllowPicking()))
      {
         if (!mDragActive)
         {
            if (SelectionIncludes(mHitList))
            {
               // The button went down on something that was already selected.
               // Simply drag the entire selection
            }
            else if (mSelection.empty())
            {
               // When the selection list is empty, attempt to select the item
               // that was under the cursor when the button went down.  Note that
               // if multiple entries were under the cursor, a menu will popup
               // and require a selection.  This will result this really getting
               // treated as a click rather than a drag.  That's OK though,
               // because the entry will then be selected and it can then be
               // dragged.
               VaHitEntry picked;
               ChooseAndPickHit(mHitList, aMouseX, aMouseY, false, false, picked);
               if (picked.IsEmpty())
               {
                  mDragPossible = false; // Nothing was selected
                  return DontPassOn;
               }
            }
            else
            {
               // Attempted to drag objects(s) where the mouse wasn't over one of
               // the select objects when it was pressed...
               mDragPossible = false;
               return DontPassOn;
            }
            mDragActive = true;
         }

         if ((aMouseX >= 0) && (aMouseY >= 0) && (aMouseX < mViewWidth) && (aMouseY < mViewHeight))
         {
            // Negating the feedback precision is a flag to Position::MoveRelative()
            // to not truncate the resulting position to the feedback precision.
            // If we don't do this then we get creeping error as we drag.  We want
            // to retain full precision until we stop dragging.
            mFeedbackPrecision = -mFeedbackPrecision;

            mMoveActive = true;
            UtLLAPos cLLA;
            UtLLAPos dLLA;
            curLLA.GetLLA(cLLA);
            mDragLLA.GetLLA(dLLA);
            VaEnvironment::Instance().MoveSelection(this, dLLA, cLLA, mFeedbackFormat, mFeedbackPrecision);
            mFeedbackPrecision = -mFeedbackPrecision;
            mDragLLA           = curLLA;
         }
      }

      if (mCameraPtr->MouseMotion(aMouseX, aMouseY, aState))
      {
         mCameraDrag = true;
         mUpdateView = true;
      }
   }

   return status;
}

// virtual
int VaViewer::Btn1Up(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonUp[GetUniqueId()], this, 1, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status  = DontPassOn;
   mCameraDrag = false;

   if (Btn1UpOverlays(aMouseX, aMouseY, aState))
   {
      return status;
   }

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);

      if (mCameraPtr->MouseButtonRelease(aMouseX, aMouseY, 1, aState))
      {
         mUpdateView = true;
      }

      mDragEntity   = nullptr;
      mDragPossible = false;
      mDragActive   = false;
      if (mMoveActive)
      {
         mMoveActive = false;
         VaEnvironment::Instance().MoveSelectionComplete(this);
      }
   }

   return status;
}

// Button 2 operations.

// virtual
int VaViewer::Btn2Down(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDown[GetUniqueId()], this, 2, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status = DontPassOn;

   if (mInitialized)
   {
      SetSavedCursorPosition(aMouseX, aMouseY);

      mCameraPtr->MouseButtonPress(aMouseX, aMouseY, 2, aState);

      mUpdateView = true;
   }

   return status;
}

// virtual
int VaViewer::Btn2Drag(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDrag[GetUniqueId()], this, 2, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   VaObserver::UpdateCursorPosition[GetUniqueId()](this, aMouseX, aMouseY);

   int status = DontPassOn;

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);

      if (mCameraPtr->MouseMotion(aMouseX, aMouseY, aState))
      {
         mUpdateView = true;
      }
   }

   return status;
}

// virtual
int VaViewer::Btn2Up(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonUp[GetUniqueId()], this, 2, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   int status = DontPassOn;

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);

      if (mCameraPtr->MouseButtonRelease(aMouseX, aMouseY, 2, aState))
      {
         mUpdateView = true;
      }
   }

   return status;
}

// virtual
int VaViewer::Btn2Click(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonClick[GetUniqueId()], this, 2, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }
   int status = DontPassOn;

   return status;
}

// Button 3 operations.

// virtual
int VaViewer::Btn3Down(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDown[GetUniqueId()], this, 3, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }
   int status = PassOn;

   if (Btn3DownOverlays(aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }

   if (mInitialized)
   {
      SetSavedCursorPosition(aMouseX, aMouseY);

      VaPosition curLLA;
      ScreenToWorld(aMouseX, aMouseY, curLLA);

      if (mCameraPtr->MouseButtonPress(aMouseX, aMouseY, 3, aState))
      {
         mUpdateView = true;
      }
      ProcessPickOnRightClick(aMouseX, aMouseY, aState);
   }

   return status;
}

// virtual
int VaViewer::Btn3Drag(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonDrag[GetUniqueId()], this, 3, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }
   return 0;
}

// virtual
int VaViewer::Btn3Up(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonUp[GetUniqueId()], this, 3, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }
   int status = DontPassOn;

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);

      if (mCameraPtr->MouseButtonRelease(aMouseX, aMouseY, 3, aState))
      {
         mUpdateView = true;
      }
   }

   return status;
}

// virtual
int VaViewer::Btn3Click(int aMouseX, int aMouseY, int aState)
{
   if (EventOverride(VaObserver::ButtonClick[GetUniqueId()], this, 3, aMouseX, aMouseY, aState))
   {
      return DontPassOn;
   }
   int status = DontPassOn;

   return status;
}


// virtual
int VaViewer::KeyPress(int aMouseX, int aMouseY, int aKey, int aState)
{
   int status = DontPassOn;

   if (KeyPressOverlays(aMouseX, aMouseY, aKey, aState))
   {
      return status;
   }

   static const int mouseDelta = 5;
   int              tempMouseX = mViewWidth / 2;
   int              tempMouseY = mViewHeight / 2;

   const int state = aState & (~CapsNumLockStates);

   if (aKey == vespa::Key_Escape)
   {
      // Swallow the 'Escape' key.  This prevents the gui from treating it as a
      // request to exit the application.
   }
   else if (((aKey == vespa::Key_Left) || (aKey == vespa::Key_4)) && ((state == 0) || (state == vespa::Key_Shift)))
   {
      Btn1Down(tempMouseX, tempMouseY, aState);
      Btn1Drag(tempMouseX - mouseDelta, tempMouseY, aState);
      Btn1Up(tempMouseX - mouseDelta, tempMouseY, aState);
   }
   else if (((aKey == vespa::Key_Right) || (aKey == vespa::Key_6)) && ((state == 0) || (state == vespa::Key_Shift)))
   {
      Btn1Down(tempMouseX, tempMouseY, aState);
      Btn1Drag(tempMouseX + mouseDelta, tempMouseY, aState);
      Btn1Up(tempMouseX + mouseDelta, tempMouseY, aState);
   }
   else if (((aKey == vespa::Key_Up) || (aKey == vespa::Key_8)) && ((state == 0) || (state == vespa::Key_Shift)))
   {
      Btn1Down(tempMouseX, tempMouseY, aState);
      Btn1Drag(tempMouseX, tempMouseY + mouseDelta, aState);
      Btn1Up(tempMouseX, tempMouseY + mouseDelta, aState);
   }
   else if (((aKey == vespa::Key_Down) || (aKey == vespa::Key_2)) && ((state == 0) || (state == vespa::Key_Shift)))
   {
      Btn1Down(tempMouseX, tempMouseY, aState);
      Btn1Drag(tempMouseX, tempMouseY - mouseDelta, aState);
      Btn1Up(tempMouseX, tempMouseY - mouseDelta, aState);
   }
   else if ((aKey == vespa::Key_Minus) && (state == 0))
   {
      Btn2Down(tempMouseX, tempMouseY, aState);
      Btn2Drag(tempMouseX, tempMouseY + mouseDelta, aState);
      Btn2Up(tempMouseX, tempMouseY + mouseDelta, aState);
   }
   else if ((aKey == vespa::Key_Equal) && (state == 0))
   {
      Btn2Down(tempMouseX, tempMouseY, aState);
      Btn2Drag(tempMouseX, tempMouseY - mouseDelta, aState);
      Btn2Up(tempMouseX, tempMouseY - mouseDelta, aState);
   }
   else
   {
      status = PassOn;
   }
   return status;
}

// virtual
int VaViewer::KeyRelease(int aMouseX, int aMouseY, int aKey, int aState)
{
   return PassOn;
}

// virtual
int VaViewer::KeyDblUp(int aMouseX, int aMouseY, int aKey, int aState)
{
   int status = PassOn;
   return status;
}

// virtual
int VaViewer::MouseMove(int aMouseX, int aMouseY)
{
   int status = DontPassOn;

   MouseMoveOverlays(aMouseX, aMouseY);

   VaObserver::UpdateCursorPosition[GetUniqueId()](this, aMouseX, aMouseY);

   if (mInitialized)
   {
      SetCurrentCursorPosition(aMouseX, aMouseY);
   }
   mMouseX = aMouseX;
   mMouseY = aMouseY;
   return status;
}

// virtual
int VaViewer::WheelUp(int aDelta)
{
   return WheelImp(aDelta);
}

// virtual
int VaViewer::WheelDown(int aDelta)
{
   return WheelImp(aDelta);
}


int VaViewer::MouseEnter()
{
   mMouseIsIn = true;
   return DontPassOn;
}

int VaViewer::MouseLeave()
{
   mMouseIsIn = false;
   return DontPassOn;
}

// private
int VaViewer::WheelImp(int aDelta)
{
   int status = DontPassOn;
   if (WheelOverlays(mMouseX, mMouseY, aDelta))
   {
      return status;
   }

   if (mInitialized)
   {
      if (mCameraPtr->MouseScroll(aDelta))
      {
         if (mCameraPtr->GetCameraMotion()->GetMotionType() != VaCameraMotion::cTETHERED)
         {
            // Start the zoom box animation
            ShowZoomBox(aDelta < 0);
         }

         mUpdateView = true;
      }
   }

   return status;
}

// private
void VaViewer::ShowZoomBox(bool aZoomingIn)
{
   if (mZoomBoxPtr == nullptr)
   {
      mZoomBoxPtr = dynamic_cast<VaOverlayZoomBox*>(AddOverlay("zoom_box", "zoom_box-1"));
   }
   if (mZoomBoxPtr != nullptr)
   {
      mZoomBoxPtr->SetVisible(true);
      mZoomBoxPtr->SetZoomDirection(aZoomingIn ? VaOverlayZoomBox::cZOOM_IN : VaOverlayZoomBox::cZOOM_OUT);
      BringOverlayToFront(mZoomBoxPtr);
   }
}

// virtual
int VaViewer::Touch(int aScreenX, int aScreenY)
{
   int  status    = DontPassOn;
   bool fullClear = false;

   if (mInitialized)
   {
      VaObserver::BeginSelectionChange(this);
      if (mCameraPtr->GetAllowPicking())
      {
         mPickedEntry = VaHitEntry();

         // If we click without depressing shift or ctrl, clear the selection list.
         if (!mSuppressMouseSelectionChanges)
         {
            ClearSelection();
            fullClear = true;
         }

         // Attempt to pick an entity and/or attachment.
         VaHitEntry picked;
         if (!mSuppressMouseSelectionChanges)
         {
            if (ChooseAndPickHit(mHitList, aScreenX, aScreenY, true, false, picked))
            {
               fullClear = false;
               // Store the picked entity's unique id.
               if (!picked.IsEmpty())
               {
                  mPickedEntry = picked;
               }
            }
         }
      }
      VaObserver::EndSelectionChange(this);
   }

   if (fullClear)
   {
      // send out a signal that this mouse click cleared the selection
      VaObserver::SelectionCleared(this);
   }
   return status;
}

// virtual
int VaViewer::Swipe(vespa::SwipeDirection aHorizontalDirection, vespa::SwipeDirection aVerticalDirection, double aAngle)
{
   int status = DontPassOn;

   return status;
}

// virtual
int VaViewer::Pan(double aAcceleration, double aDelta)
{
   int status = DontPassOn;

   return status;
}

// virtual
int VaViewer::Pinch(double aScale, double aAngle, int aCenterX, int aCenterY)
{
   int status = DontPassOn;

   if (!mCameraPtr->GesturePinch(aScale, aAngle, aCenterX, aCenterY))
   {
      status = PassOn;
   }

   return status;
}

bool VaViewer::KeyPressOverlays(int aMouseX, int aMouseY, int aKey, int aState)
{
   bool               status = false;
   Overlays::iterator it;
   for (it = mOverlays.begin(); it != mOverlays.end(); ++it)
   {
      if (it->second->KeyPress(aMouseX, aMouseY, aKey, aState))
      {
         status = true;
      }
   }
   return status;
}

void VaViewer::MouseMoveOverlays(int aMouseX, int aMouseY)
{
   Overlays::iterator it;
   for (it = mOverlays.begin(); it != mOverlays.end(); ++it)
   {
      it->second->MouseMove(aMouseX, aMouseY);
   }
}

bool VaViewer::Btn1ClickOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   if (mHeldOverlayPtr != nullptr)
   {
      status = mHeldOverlayPtr->Btn1Click(aMouseX, aMouseY, aState);
   }
   if (!status)
   {
      auto candidate = GetMousedOverlay(aMouseX, aMouseY);
      if (candidate != mOverlays.end())
      {
         status = candidate->second->Btn1Click(aMouseX, aMouseY, aState);
         if (status)
         {
            BringOverlayToFront(candidate->second);
         }
      }
      mHeldOverlayPtr = nullptr;
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            if (iter->second->Btn1Click(aMouseX, aMouseY, aState))
            {
               status = true;
            }
         }
      }
   }
   return status;
}


bool VaViewer::Btn1DblClickOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   if (mHeldOverlayPtr != nullptr)
   {
      status = mHeldOverlayPtr->Btn1DblClick(aMouseX, aMouseY, aState);
   }

   if (!status)
   {
      auto candidate = GetMousedOverlay(aMouseX, aMouseY);
      if (candidate != mOverlays.end())
      {
         status = candidate->second->Btn1DblClick(aMouseX, aMouseY, aState);
         if (status)
         {
            BringOverlayToFront(candidate->second);
         }
      }
      mHeldOverlayPtr = nullptr;
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            if (iter->second->Btn1DblClick(aMouseX, aMouseY, aState))
            {
               status = true;
            }
         }
      }
   }
   return status;
}

bool VaViewer::Btn1DownOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   auto candidate = GetMousedOverlay(aMouseX, aMouseY);
   if (candidate != mOverlays.end())
   {
      status          = candidate->second->Btn1Down(aMouseX, aMouseY, aState);
      mHeldOverlayPtr = candidate->second;
      if (status)
      {
         BringOverlayToFront(candidate->second);
      }
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            if (iter->second->Btn1Down(aMouseX, aMouseY, aState))
            {
               status = true;
            }
         }
      }
   }
   return status;
}

bool VaViewer::Btn1DragOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   if (!mCameraDrag)
   {
      if (mHeldOverlayPtr != nullptr)
      {
         status = mHeldOverlayPtr->Btn1Drag(aMouseX, aMouseY, aState);
      }

      if (!status)
      {
         auto candidate = GetMousedOverlay(aMouseX, aMouseY);
         if (candidate != mOverlays.end())
         {
            status = candidate->second->Btn1Drag(aMouseX, aMouseY, aState);
            if (status)
            {
               BringOverlayToFront(candidate->second);
            }
         }
      }
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            if (iter->second->Btn1Drag(aMouseX, aMouseY, aState))
            {
               status = true;
            }
         }
      }
   }
   return status;
}

bool VaViewer::Btn1UpOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   if (mHeldOverlayPtr != nullptr)
   {
      status = mHeldOverlayPtr->Btn1Up(aMouseX, aMouseY, aState);
   }

   if (!status)
   {
      auto candidate = GetMousedOverlay(aMouseX, aMouseY);

      if (candidate != mOverlays.end())
      {
         status = candidate->second->Btn1Up(aMouseX, aMouseY, aState);
         if (status)
         {
            BringOverlayToFront(candidate->second);
         }
      }
      mHeldOverlayPtr = nullptr;
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            iter->second->Btn1Up(aMouseX, aMouseY, aState);
         }
      }
   }
   return status;
}

bool VaViewer::Btn3DownOverlays(int aMouseX, int aMouseY, int aState)
{
   bool status = false;

   if (mHeldOverlayPtr != nullptr)
   {
      status = mHeldOverlayPtr->Btn3Down(aMouseX, aMouseY, aState);
   }

   if (!status)
   {
      auto candidate = GetMousedOverlay(aMouseX, aMouseY);

      if (candidate != mOverlays.end())
      {
         status = candidate->second->Btn3Down(aMouseX, aMouseY, aState);
         if (status)
         {
            BringOverlayToFront(candidate->second);
         }
      }
      mHeldOverlayPtr = nullptr;
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            iter->second->Btn3Down(aMouseX, aMouseY, aState);
         }
      }
   }
   return status;
}

bool VaViewer::WheelOverlays(int aMouseX, int aMouseY, int aDelta)
{
   bool status = false;

   if (mHeldOverlayPtr != nullptr)
   {
      status = mHeldOverlayPtr->MouseWheel(aMouseX, aMouseY, aDelta);
   }

   if (!status)
   {
      auto candidate = GetMousedOverlay(aMouseX, aMouseY);

      if (candidate != mOverlays.end())
      {
         status = candidate->second->MouseWheel(aMouseX, aMouseY, aDelta);
         if (status)
         {
            BringOverlayToFront(candidate->second);
         }
      }
      mHeldOverlayPtr = nullptr;
   }

   if (!status)
   {
      // Dispatch the mouse event to all overlays that want it.
      for (Overlays::const_iterator iter = mOverlays.begin(); iter != mOverlays.end(); ++iter)
      {
         if (iter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_ALWAYS)
         {
            iter->second->MouseWheel(aMouseX, aMouseY, aDelta);
         }
      }
   }
   return status;
}

VaViewer::Overlays::const_iterator VaViewer::GetMousedOverlay(int aMouseX, int aMouseY) const
{
   UtoScreenOverlayManager::iterator overlayIter;
   Overlays::const_iterator          it = mOverlays.end();
   if (mViewerPtr != nullptr)
   {
      for (overlayIter = mViewerPtr->GetScreenOverlayManager()->Overlays().begin;
           overlayIter != mViewerPtr->GetScreenOverlayManager()->Overlays().end;
           ++overlayIter)
      {
         std::string name          = mViewerPtr->GetScreenOverlayManager()->FindName(overlayIter);
         auto        vaOverlayIter = mOverlays.find(name);
         if (vaOverlayIter != mOverlays.end())
         {
            if ((vaOverlayIter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_FOCUS) &&
                vaOverlayIter->second->SelectableAt(aMouseX, aMouseY))
            {
               it = vaOverlayIter;
            }
            else if (vaOverlayIter->second->GetHandleEventMode() == VaOverlay::cHANDLE_EVENTS_FOCUS)
            {
               vaOverlayIter->second->MousedOffOf(aMouseX, aMouseY);
            }
         }
      }
   }
   return it;
}

void VaViewer::BringOverlayToFront(VaOverlay* aOverlay)
{
   if (aOverlay->GetOverlayType() == VaOverlay::cOVERLAY_WINDOW)
   {
      UtoScreenOverlayManager::iterator it = mViewerPtr->GetScreenOverlayManager()->FindOverlay(aOverlay->GetName());
      mViewerPtr->GetScreenOverlayManager()->BringOverlayToFront(it);
   }
}

void VaViewer::AddSubordinate(VaViewer* aViewerPtr)
{
   mSubordinateMap[aViewerPtr->GetUniqueId()] = aViewerPtr;
   GetViewer()->AddSubordinate(aViewerPtr->GetViewer());
}

void VaViewer::RemoveSubordinate(VaViewer* aViewerPtr)
{
   mSubordinateMap.erase(aViewerPtr->GetUniqueId());
   GetViewer()->RemoveSubordinate(aViewerPtr->GetViewer());
}

VaOverlayModelInstantiations& VaViewer::GetOrCreateModelInstantiationsOverlay()
{
   if (nullptr == mModelInstantitationsOverlayPtr)
   {
      mModelInstantitationsOverlayPtr = new VaOverlayModelInstantiations();
      AddOverlay(mModelInstantitationsOverlayPtr);
   }
   return *mModelInstantitationsOverlayPtr;
}

void VaViewer::ToggleMode()
{
   GetViewer()->ToggleMode();
}

//! The same as ChooseHit(), except that Pick() is called on the chosen entity or attachment
bool VaViewer::ChooseAndPickHit(const VaHitList& aHits,
                                int              aMouseX,
                                int              aMouseY,
                                bool             aToggleSelection,
                                bool             aDoubleClick,
                                VaHitEntry&      aPicked)
{
   if (ChooseHit(aHits, aMouseX, aMouseY, aPicked))
   {
      VaAttachment* attachmentPtr = aPicked.FindAttachment();
      if (attachmentPtr)
      {
         attachmentPtr->Pick(this, aMouseX, aMouseY, aToggleSelection, aDoubleClick, aPicked.GetSubId());
      }
      else
      {
         VaEntity* entity = aPicked.FindEntity();
         if (entity)
         {
            entity->Pick(this, aMouseX, aMouseY, aToggleSelection);
         }
      }
      return true;
   }
   return false;
}

// virtual
void VaViewer::PickFromScreen(VaHitList& aHits,
                              int        aMouseX,
                              int        aMouseY,
                              bool       aHoverMode, /* = false*/
                              bool       aDoubleClick /* = false*/) const
{
   if (mInitialized)
   {
      if (!GetViewer()->GetOSGViewer())
      {
         return;
      }
      aHits.clear();
      std::map<UtoShape*, UtoHitInfoList>* hits =
         mViewerPtr->HitTestSymbol(UtoPoint(aMouseX, aMouseY), "*", 100, !mTrueScale, mIconScale);

      if (nullptr != mModelInstantitationsOverlayPtr)
      {
         // the above check for the validity of OSGViewer should confirm the validity of these matrices
         mModelInstantitationsOverlayPtr->Intersect(aHits,
                                                    aMouseX,
                                                    aMouseY,
                                                    *GetViewer()->GetViewMatrix(),
                                                    *GetViewer()->GetProjectionMatrix(),
                                                    GetViewer()->GetViewport()->width(),
                                                    GetViewer()->GetViewport()->height(),
                                                    !mTrueScale,
                                                    mIconScale,
                                                    aHoverMode);
         // run our custom hittest here, too
      }
      // run hit tests on other things
      unsigned int                              entityUID      = 0;
      int                                       attachmentUID  = -1;
      int                                       additionalInfo = 0;
      std::vector<std::pair<unsigned int, int>> attachmentList;
      // Collate hits by attachment
      std::map<VaAttachment*, std::vector<UtoHitInfoList>> attachmentHits;
      for (auto& hit : *hits)
      {
         entityUID                       = 0;
         attachmentUID                   = -1;
         additionalInfo                  = 0;
         std::string        entityUIDStr = hit.first->AuxData();
         std::istringstream iss(entityUIDStr.c_str());
         iss >> entityUID >> attachmentUID >> additionalInfo;
         VaEntity* pickedEntityPtr = VaEnvironment::Instance().FindEntity(entityUID);

         if (attachmentUID >= 0)
         {
            attachmentList.emplace_back(std::make_pair(additionalInfo, attachmentUID));
            VaAttachment* pickedAttachmentPtr = VaEnvironment::Instance().FindAttachment(attachmentUID);
            if (pickedAttachmentPtr != nullptr)
            {
               int mode = (aHoverMode) ? pickedAttachmentPtr->GetHoverMode() : pickedAttachmentPtr->GetPickMode();
               switch (mode & VaAttachment::cPICK_MODE_MASK)
               {
               case VaAttachment::cPICK_ATTACHMENT:
               {
                  attachmentHits[pickedAttachmentPtr].push_back(hit.second);
               }
               break;
               case VaAttachment::cPICK_ENTITY:
                  aHits.push_back(VaHitEntry::FromEntity(*pickedEntityPtr));
                  break;
               case VaAttachment::cPICK_NOTHING:
                  // Don't add the attachment to the hit list.
                  break;
               default:
                  // Don't add the attachment to the hit list.
                  break;
               }
            }
         }
         else if (nullptr != pickedEntityPtr)
         {
            aHits.push_back(VaHitEntry::FromEntity(*pickedEntityPtr));
         }
      }
      for (auto& attachmentHit : attachmentHits)
      {
         attachmentHit.first->HitMultipleSubparts(attachmentHit.second, aHits);
      }
      if (!attachmentList.empty())
      {
         const UtoRenderBinManager* renderBinMgrPtr = UtoRenderBinManager::Instance();
         if (renderBinMgrPtr != nullptr)
         {
            auto it = renderBinMgrPtr->GetForemostElement(attachmentList);
            if (it != attachmentList.end())
            {
               VaAttachment* attachPtr = VaEnvironment::Instance().FindAttachment(it->second);
               if ((attachPtr != nullptr) && (attachPtr->GetPickMode() & VaAttachment::cPICK_AUTO))
               {
                  // If foremost element in render bin is an attachment with pick mode AUTO,
                  // then don't build list
                  aHits.clear();
                  VaHitEntry hit = VaHitEntry::FromAttachment(*attachPtr, it->first);
                  aHits.push_back(hit);
               }
            }
         }
      }
      delete hits;
   }
}

bool VaViewer::ChooseHit(const VaHitList& aHits, int aMouseX, int aMouseY, VaHitEntry& aPicked)
{
   std::map<VaHitEntry, std::string> pickMap;
   for (const auto& hit : aHits)
   {
      if (hit.IsEntity())
      {
         VaEntity* pickedEntityPtr = hit.FindEntity();
         if (pickedEntityPtr)
         {
            pickMap[hit] = pickedEntityPtr->GetMenuSelectionString();
         }
      }
      else if (hit.IsAttachment())
      {
         VaAttachment* pickedAttachmentPtr = hit.FindAttachment();
         if (pickedAttachmentPtr)
         {
            // TODO: GetMenuSelectionString() sometimes returns an empty string.  For now just don't show these on the popup menu
            std::string selectionString = pickedAttachmentPtr->GetMenuSelectionString(hit.GetSubId());
            if (!selectionString.empty())
            {
               pickMap[hit] = selectionString;
            }
         }
      }
   }

   if (mChooserPtr)
   {
      std::function<void(VaHitEntry)> hoverFn = [this](VaHitEntry hit) { UiHover(hit); };
      aPicked                                 = mChooserPtr->Choose(pickMap, aMouseX, aMouseY, &hoverFn);
      if (!mPreviousSelectionUiHover.IsEmpty())
      {
         HighlightTempSelection(mPreviousSelectionUiHover, false);
      }
   }
   PostChoose(aPicked, pickMap);

   return !aPicked.IsEmpty();
}

void VaViewer::ProcessPickOnRightClick(int aMouseX, int aMouseY, const int aState)
{
   const int state = aState & (~CapsNumLockStates);
   if ((state != ShiftKey) && mCameraPtr->GetAllowPicking() && mPickOnRightClick)
   {
      VaHitList hits;
      PickFromScreen(hits, aMouseX, aMouseY);

      if (!hits.empty())
      {
         ClearSelection();
      }
      VaHitEntry entry;
      if (ChooseAndPickHit(hits, aMouseX, aMouseY, false, false, entry))
      {
      }
   }
}

bool VaViewer::WindowToWorld(UtoPosition& aPos)
{
   return mViewerPtr->DisplayToWorld(aPos);
}

// private
void VaViewer::SetCurrentCursorPosition(int aMouseX, int aMouseY)
{
   double lat, lon, alt;
   mCameraPtr->ConvertScreenToLLA(aMouseX, aMouseY, lat, lon, alt);
   double     mpp = GetMetersPerPixel();
   VaPosition lla(lat, lon, alt);
   VaEnvironment::Instance().SetCurrentCursorData(lla, mpp);
}

void VaViewer::SetSavedCursorPosition(int aMouseX, int aMouseY)
{
   double lat, lon, alt;
   mCameraPtr->ConvertScreenToLLA(aMouseX, aMouseY, lat, lon, alt);
   double     mpp = GetMetersPerPixel();
   VaPosition lla(lat, lon, alt);
   VaEnvironment::Instance().SetSavedCursorData(lla, mpp);
}

void VaViewer::MapLoadedMapCB()
{
   mUpdateView = true;
   mCameraPtr->ForceRefresh();
   if (!mViewerPtr)
   {
      return;
   }

   mMapProjection = mViewerPtr->World().Projection();

   VaObserver::MapProjectionChanged[this->GetUniqueId()](GetUniqueId(), &mMapProjection);

   if (mMapProjection.projection == UtoMapProjection::Geocentric)
   {
      GetViewer()->GetRootNode()->getOrCreateStateSet()->removeDefine("USE_FLAT_MAP_PROJECTION");
      if (mECI)
      {
         GetViewer()->GetRootNode()->getOrCreateStateSet()->removeDefine("USE_ECEF");
         if (GetCelestialNode())
         {
            GetCelestialNode()->getOrCreateStateSet()->removeDefine("USE_ECEF");
         }
      }
      SetFlatMapMode(false);
      VaObserver::CameraSetProjection[this->GetUniqueId()](0);
   }
   else if (mMapProjection.projection == UtoMapProjection::Equirectangular)
   {
      GetViewer()->GetRootNode()->getOrCreateStateSet()->setDefine("USE_FLAT_MAP_PROJECTION");
      GetViewer()->GetRootNode()->getOrCreateStateSet()->setDefine("USE_ECEF");
      if (GetCelestialNode())
      {
         GetCelestialNode()->getOrCreateStateSet()->setDefine("USE_ECEF");
      }
      SetFlatMapMode(true);
      VaObserver::CameraSetProjection[this->GetUniqueId()](1);
   }
}

void VaViewer::ECI_ModeChangedCB(bool aState)
{
   SetUniform("uECI", aState ? 1.0 : 0.0);
   mECI = aState;
   if (mMapProjection.projection != UtoMapProjection::Equirectangular)
   {
      if (mECI)
      {
         GetViewer()->GetRootNode()->getOrCreateStateSet()->removeDefine("USE_ECEF");

         if (GetCelestialNode())
         {
            GetCelestialNode()->getOrCreateStateSet()->removeDefine("USE_ECEF");
         }
      }
      else
      {
         GetViewer()->GetRootNode()->getOrCreateStateSet()->setDefine("USE_ECEF");

         if (GetCelestialNode())
         {
            GetCelestialNode()->getOrCreateStateSet()->setDefine("USE_ECEF");
         }
      }
   }
}

void VaViewer::CameraSetCenterCB()
{
   CenterCamera();
}

void VaViewer::SetIntensityCB(const float& aValue)
{
   mIntensity = aValue;
   if (mViewerPtr)
   {
      if (aValue < 0)
      {
         mViewerPtr->SetUniform("fBrightener", 0.0);
         mViewerPtr->SetUniform("fDarkener", -aValue);
      }
      else
      {
         mViewerPtr->SetUniform("fBrightener", aValue);
         mViewerPtr->SetUniform("fDarkener", 0.0);
      }
   }
}

void VaViewer::SetSaturationCB(const float& aValue)
{
   mSaturation = aValue;
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform("fSaturation", aValue);
   }
}

void VaViewer::SetContrastCB(const float& aValue)
{
   mContrast = aValue;
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform("fContrast", aValue);
   }
}

void VaViewer::CameraUpdatedCB(VaViewer* aViewerPtr) {}

void VaViewer::EntitySelectCB(VaViewer* aViewerPtr, int aState, VaEntity* aEntityPtr)
{
   if ((aViewerPtr == nullptr) || (aViewerPtr == this))
   {
      VaHitEntry key = VaHitEntry::FromEntity(*aEntityPtr);
      if (aState == 1 && aEntityPtr->IsVisible(aViewerPtr))
      {
         mSelection.insert(key);
      }
      else if (aState == 0)
      {
         mSelection.erase(key);
      }

      VaEnvironment::Instance().DirtySelection();

      VaObserver::EntitySelected[aEntityPtr->GetUniqueId()](aViewerPtr, aState, aEntityPtr);
   }
}

void VaViewer::ClearSelection()
{
   VaHitSet oldSelection;
   std::swap(oldSelection, mSelection);
   for (const auto& i : oldSelection)
   {
      if (!i.IsAttachment())
      {
         VaEntity* ent = i.FindEntity();
         if (ent)
         {
            ent->SetSelected(this, false);
         }
      }
      else
      {
         VaAttachment* attachment = i.FindAttachment();
         if (attachment)
         {
            attachment->ClearSelection(this);
         }
      }
   }
}

void VaViewer::EntitySelectedCB(VaViewer* aViewerPtr, int aState, VaEntity* aEntityPtr) {}

void VaViewer::AttachmentSelect(int aState, VaAttachment* aAttachmentPtr, const VaAttachmentSubId& aSubpart)
{
   VaHitEntry entry = VaHitEntry::FromAttachment(*aAttachmentPtr, aSubpart);
   if (aState == 1)
   {
      mSelection.insert(entry);
   }
   else if (aState == 0)
   {
      mSelection.erase(entry);
   }

   VaEnvironment::Instance().DirtySelection();

   VaObserver::AttachmentSelected[aAttachmentPtr->GetUniqueId()](this, aState, aAttachmentPtr, aSubpart);
}

void VaViewer::AttachmentSelectedCB(VaViewer*                aViewerPtr,
                                    int                      aState,
                                    VaAttachment*            aAttachmentPtr,
                                    const VaAttachmentSubId& aSubpart)
{
   if ((aViewerPtr != nullptr) && (aViewerPtr != this))
   {
      return;
   }
   if (!mViewerPtr)
   {
      return;
   }
   if (mShowSelectionIndication)
   {
      if (aAttachmentPtr->IsHighlightable())
      {
         bool  highlightVisibility = aState != 0 && aAttachmentPtr->GetParent().IsVisible(aViewerPtr);
         float white[4]            = {1, 1, 1, 1};
         if (!aAttachmentPtr->Highlight(this, aSubpart, highlightVisibility, white))
         {
            if (aSubpart.mSubId != 0)
            {
               aAttachmentPtr->HighlightSubpart(this, aSubpart, highlightVisibility, white);
            }
            else
            {
               std::vector<std::string> shapes;
               aAttachmentPtr->GetHighlightableShapes(shapes);
               mViewerPtr->HighlightSymbolsWithColor(aAttachmentPtr->GetParent().GetUtoEntity(),
                                                     shapes,
                                                     (highlightVisibility) ? true : false,
                                                     white);
            }
         }
      }
   }
}

void VaViewer::GetMousePosition(int& aX, int& aY)
{
   aX = mMouseX;
   aY = mMouseY;
}

void VaViewer::SetActiveCamera(int aMode)
{
   if (mViewerPtr != nullptr)
   {
      mViewerPtr->SetActiveCamera((UtoViewer::CameraMode)aMode);
   }
}

void VaViewer::ToggleStats()
{
   mViewerPtr->ToggleStats();
}

void VaViewer::ScreenShot(const std::string& aOutputDir, int aInstanceId)
{
   mViewerPtr->ScreenShot(aOutputDir, aInstanceId);
}

void VaViewer::ScreenShotToFilename(const std::string& aFilename)
{
   mViewerPtr->ScreenShotToFilename(aFilename);
}

void VaViewer::ShowSelectionIndication(bool aState)
{
   mShowSelectionIndication = aState;
   mViewerPtr->SuppressSelectionIndication(!mShowSelectionIndication);
}

UtoCmeTerrain* VaViewer::CreateDatabase(const std::string& aFiles, const std::string& aProjection, const double aOffset)
{
   UtoCmeTerrain terrain;
   terrain.SetProjection(aProjection, aOffset);
   terrain.PostLoad();
   // mViewerPtr might be NULL (for example, if you bring up the terrain manager
   // panel *before* the affected VaViewer)
   if (!mViewerPtr)
   {
      return nullptr;
   }
   terrain.SetFiles(aFiles);
   bool ok = mViewerPtr->World().SetTerrain(terrain);
   if (ok)
   {
      MapLoadedMapCB();
      return (UtoCmeTerrain*)&(mViewerPtr->World().Terrain());
   }
   return nullptr;
}

UtoTerrain* VaViewer::GetDatabase()
{
   return &(mViewerPtr->World().Terrain());
}

void VaViewer::ClearTerrain()
{
   if (mViewerPtr)
   {
      mViewerPtr->World().ClearTerrain();
      mViewerPtr->World().SetCME_WCS_Projection();
   }
}

void VaViewer::EnableLighting(bool aState)
{
   if (mViewerPtr)
   {
      SetUniform("VA_DirectionallyLit", aState ? 1.0 : 0.0);
      if (aState)
      {
         if (mViewerPtr->World().IsTerrainValid())
         {
            mViewerPtr->World().Terrain().EnableLighting(true);
            mViewerPtr->World().Terrain().SetDisableColorBlending(true);
         }
         SetUniform("uto_LightSourceAmbient", 0.4F, 0.4F, 0.4F, 1.0F);
         SetUniform("uto_LightSourceDiffuse", 1.0F, 1.0F, 1.0F, 1.0F);
         SetUniform("uto_LightSourceSpecular", 1.0F, 1.0F, 1.0F, 1.0F);
      }
      else
      {
         if (mViewerPtr->World().IsTerrainValid())
         {
            mViewerPtr->World().Terrain().EnableLighting(false);
            mViewerPtr->World().Terrain().SetDisableColorBlending(false);
         }
         SetUniform("uto_LightSourceAmbient", 1.0F, 1.0F, 1.0F, 1.0F);
         SetUniform("uto_LightSourceDiffuse", 1.0F, 1.0F, 1.0F, 1.0F);
         SetUniform("uto_LightSourceSpecular", 1.0F, 1.0F, 1.0F, 1.0F);
      }
   }
}

void VaViewer::UpdateLighting(const UtoColor& aColor, float aSpaceFactor)
{
   if (mViewerPtr)
   {
      SetUniform("uto_LightSourceDiffuse", aColor.RedF(), aColor.GreenF(), aColor.BlueF(), 1.0F);
      SetUniform("uto_LightSourceSpecular", aColor.RedF(), aColor.GreenF(), aColor.BlueF(), 1.0F);

      if (mViewerPtr->World().IsTerrainValid())
      {
         mViewerPtr->World().Terrain().SetUniform("fSpaceFactor", aSpaceFactor);
      }
   }
}

void VaViewer::DumpSceneGraph(const char* aFilename)
{
   if (mViewerPtr)
   {
      std::ofstream ostr(aFilename);
      mViewerPtr->Dump(ostr);
   }
}

void VaViewer::SetFlatMapMode(bool aState)
{
   mFlatMap = aState;
   if (aState)
   {
      mCameraPtr->FlatMapMode(true);
   }
   else
   {
      mCameraPtr->FlatMapMode(false);
   }
}

void VaViewer::SetUniform(const char* aName, const float aValue)
{
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform(aName, aValue);
   }
}

void VaViewer::SetUniform(const char* aName, const unsigned int aIndex, const char* aFilename)
{
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform(aName, aIndex, aFilename);
   }
}

void VaViewer::SetUniform(const char*          aName,
                          const unsigned int   aIndex,
                          const unsigned char* aData,
                          const unsigned int   aWidth,
                          const unsigned int   aHeight)
{
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform(aName, aIndex, aData, aWidth, aHeight);
   }
}

void VaViewer::SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3)
{
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform(aName, aValue1, aValue2, aValue3);
   }
}

void VaViewer::SetUniform(const char* aName, const float aValue1, const float aValue2, const float aValue3, const float aValue4)
{
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform(aName, aValue1, aValue2, aValue3, aValue4);
   }
}

void VaViewer::SetTrueScale(bool aState)
{
   mTrueScale = aState;
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform("VA_TrueScale", aState ? 1.0f : 0.0f);
   }
   // and highlight viewer
}

void VaViewer::SetIconScale(float aValue)
{
   mIconScale = aValue;
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform("VA_IconScale", aValue);
   }
   // and highlight viewer
}

void VaViewer::SuppressTeamColor(bool aState)
{
   mSuppressTeamColor = aState;
   if (mViewerPtr)
   {
      mViewerPtr->SetUniform("VA_SuppressTeamColor", aState ? 1.0f : 0.0f);
   }
}

void VaViewer::SetSunVector(const osg::Vec3& aVector)
{
   mSunVector = aVector;
   if (mViewerPtr)
   {
      mViewerPtr->SetSunVector(mSunVector);
   }
}

void VaViewer::HighlightShape(osg::Node* aNodePtr, bool aEnableHighlight)
{
   float white[4] = {1, 1, 1, 1};
   mViewerPtr->HighlightWithColor(aNodePtr, aEnableHighlight, white);
}

void VaViewer::HighlightShape(osg::Node* aNodePtr, bool aEnableHighlight, const UtColor& aColor)
{
   float color[4];
   aColor.Get(color);

   mViewerPtr->HighlightWithColor(aNodePtr, aEnableHighlight, color);
}

void VaViewer::HighlightNode(osg::Node* aNodePtr, bool aEnableHighlight, const UtColor& aColor)
{
   float color[4];
   aColor.Get(color);

   mViewerPtr->HighlightNode(aNodePtr, aEnableHighlight, color);
}

void VaViewer::HighlightShapes(VaEntity*                       aEntityPtr,
                               const std::vector<std::string>& aShapes,
                               bool                            aEnableHighlight,
                               const UtColor&                  aColor)
{
   if (aEntityPtr != nullptr)
   {
      mViewerPtr->HighlightSymbolsWithColor(aEntityPtr->GetUtoEntity(), aShapes, aEnableHighlight, aColor.GetData());
   }
}

void VaViewer::SetPickOnRightClick(bool aCanPick)
{
   mPickOnRightClick = aCanPick;
}

void VaViewer::BeginUserStatCB(const std::string& aName)
{
   if (nullptr != mViewerPtr)
   {
      mViewerPtr->BeginUserStat(aName);
   }
}

osg::Group* VaViewer::GetSceneRoot()
{
   return mViewerPtr->GetRootNode();
}

void VaViewer::EndUserStatCB(const std::string& aName)
{
   if (nullptr != mViewerPtr)
   {
      mViewerPtr->EndUserStat(aName);
   }
}

bool VaViewer::SelectionIncludes(VaHitList& aHits)
{
   bool selected = false;

   if (!aHits.empty())
   {
      for (VaHitList::const_iterator it = aHits.begin(); it != aHits.end(); ++it)
      {
         if (mSelection.find(*it) != mSelection.end())
         {
            selected = true;
            break;
         }
      }
   }
   return selected;
}

void VaViewer::RemoveAttachmentSelection(VaAttachment* aAttachmentPtr, int aSubpartId)
{
   if (aSubpartId == 0)
   {
      RemoveAttachmentHits(mSelection, aAttachmentPtr);
      aAttachmentPtr->SetSelected(this, false, 0);
   }
   else
   {
      VaHitEntry hit = VaHitEntry::FromAttachment(*aAttachmentPtr, aSubpartId);
      auto       i   = mSelection.find(hit);
      if (i != mSelection.end())
      {
         mSelection.erase(i);
         aAttachmentPtr->SetSelected(this, false, aSubpartId);
         VaHitSet remainingParts;
         AttachmentHits(aAttachmentPtr, mSelection, remainingParts);
         if (remainingParts.empty())
         {
            // We've removed all of the subparts, notify attachment of unselection
            aAttachmentPtr->SetSelected(this, false, 0);
         }
      }
   }
}

const osg::Matrix& VaViewer::GetViewMatrix() const
{
   return GetCamera()->GetCameraMotion()->GetViewMatrix();
}

const osg::Matrix VaViewer::GetProjectionMatrix() const
{
   const osg::Matrix* mat = GetViewer()->GetProjectionMatrix();
   if (mat != nullptr)
   {
      return *mat;
   }
   else
   {
      return osg::Matrix();
   }
}

const float VaViewer::GetIconScale() const
{
   return mIconScale;
}

const float VaViewer::GetTrueScale() const
{
   return mTrueScale;
}

void VaViewer::HighlightTempSelection(const VaHitEntry& aHitEntry, bool aEnable)
{
   if (SelectionIndicationIsShown())
   {
      std::vector<VaAttachment*> attachmentList;
      VaEntity*                  entityPtr = aHitEntry.FindEntity();
      if (entityPtr)
      {
         entityPtr->FindAttachments<VaAttachmentModel>(attachmentList);
         entityPtr->FindAttachments<VaAttachmentWingRibbon>(attachmentList);
      }
      else
      {
         attachmentList.emplace_back(aHitEntry.FindAttachment());
      }

      for (auto* attachmentPtr : attachmentList)
      {
         if (attachmentPtr && attachmentPtr->IsHighlightable())
         {
            UtColor color(.5, 1, .5, 0.7f);

            if (!attachmentPtr->Highlight(this, aHitEntry.GetSubId(), aEnable, color))
            {
               if (aHitEntry.SubId() != 0)
               {
                  attachmentPtr->HighlightSubpart(this, aHitEntry.GetSubId(), aEnable, color);
               }
               else
               {
                  std::vector<std::string> shapes;
                  attachmentPtr->GetShapes(shapes);
                  mViewerPtr->HighlightSymbolsWithColor(attachmentPtr->GetParent().GetUtoEntity(),
                                                        shapes,
                                                        aEnable,
                                                        color.GetData());
               }
            }

            mPreviousSelectionUiHover = aHitEntry;
         }
      }
   }
}

void VaViewer::UiHover(VaHitEntry aEntry)
{
   HighlightTempSelection(mPreviousSelectionUiHover, false);
   mPreviousSelectionUiHover = VaHitEntry();
   HighlightTempSelection(aEntry, true);
}

osg::Group* VaViewer::GetCelestialNode()
{
   return mViewerPtr->GetCelestialNode();
}

} // end namespace vespa
