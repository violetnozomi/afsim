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

#include "RvPlatformSensorVolumes.hpp"

#include <iostream>

#include "RvEventPipeClasses.hpp"
#include "RvEventPipeUtil.hpp"
#include "RvMilEventPipeClasses.hpp"
#include "UtLatPos.hpp"
#include "UtSphericalEarth.hpp"
#include "VaEntity.hpp"
#include "VaUtils.hpp"
#include "sensor_volume/WkfAttachmentSensorVolume.hpp"
#include "sensor_volume/WkfSensorVolumesPrefObject.hpp"


RvSensorVolumes::PlatformVolumes::PlatformVolumes(vespa::VaEntity&                    aEntityPtr,
                                                  const wkf::SensorVolumesPrefObject* aPrefObject,
                                                  bool                                aWeapon)
   : mVisible(true)
   , mPrefObject(aPrefObject)
   , mWeapon(aWeapon)
{
   // create the attachment on the platform
}

void RvSensorVolumes::PlatformVolumes::RemoveAttachments()
{
   for (auto& it : mSensorMap)
   {
      vespa::VaEntity* ent = &(it.second->GetParent());
      ent->RemoveAttachment(it.second->GetUniqueId());
   }
   mSensorMap.clear();
}

void RvSensorVolumes::PlatformVolumes::SetVisible(bool aState)
{
   mVisible = aState;
   for (auto& it : mSensorStateMap)
   {
      it.second = aState;
   }
   MarkAll();
   HideMarked();
}

bool RvSensorVolumes::PlatformVolumes::IsDefaultVisible() const
{
   return mVisible;
}

bool RvSensorVolumes::PlatformVolumes::IsAtAllVisible() const
{
   bool all;
   bool some;
   CheckSensorVisibility(all, some);
   return some;
}

void RvSensorVolumes::PlatformVolumes::MarkAll()
{
   if (IsAtAllVisible())
   {
      std::map<std::string, wkf::AttachmentSensorVolume*>::iterator it;
      for (it = mSensorMap.begin(); it != mSensorMap.end(); ++it)
      {
         int numBeams = it->second->NumberBeams();
         for (int i = 0; i < numBeams; ++i)
         {
            it->second->GetBeam(i).SetMarked(true);
         }
      }
   }
}

void RvSensorVolumes::PlatformVolumes::HideMarked()
{
   if (IsAtAllVisible())
   {
      std::map<std::string, wkf::AttachmentSensorVolume*>::iterator it;
      for (it = mSensorMap.begin(); it != mSensorMap.end(); ++it)
      {
         int numBeams = it->second->NumberBeams();
         for (int i = 0; i < numBeams; ++i)
         {
            wkf::SensorBeam& beam = it->second->GetBeam(i);
            if (beam.IsMarked())
            {
               beam.SetVisible(false);
            }
         }
      }
   }
}

void RvSensorVolumes::PlatformVolumes::UpdateAndUnmark(const std::string&                   aName,
                                                       const rv::MsgPartArticulationStatus* aArticulation,
                                                       const rv::MsgEmitterModeDefinition*  aMode,
                                                       vespa::VaEntity*                     aEntityPtr,
                                                       vespa::VaViewer*                     aViewerPtr)
{
   if (IsAtAllVisible() || IsDefaultVisible())
   {
      wkf::SensorVolumesPrefData::DrawMode drawMode = mPrefObject->GetDrawMode();
      if (mSensorMap.find(aName) == mSensorMap.end())
      {
         mSensorMap[aName] = vespa::make_attachment<wkf::AttachmentSensorVolume>(*aEntityPtr, aViewerPtr, aName);
      }
      bool nonCueing = false;
      if ((drawMode == wkf::SensorVolumesPrefData::cSLEW_VOLUME) ||
          (drawMode == wkf::SensorVolumesPrefData::cCUE_VOLUME)) // these are should not be rotated by the part
      {
         if (mSensorMap[aName]->NumberBeams() == 0) // if the beams don't exist, add them (beam is a pair aName, beamId)
         {
            mSensorMap[aName]->AddBeam();
         }
         wkf::SensorBeam& beam    = mSensorMap[aName]->GetBeam(0);
         bool             visible = mVisible;
         const auto&      it      = mSensorStateMap.find(aName);
         if (it != mSensorStateMap.end())
         {
            visible = it->second;
         }
         else
         {
            mSensorStateMap[aName] = mVisible;
         }
         if (visible && aEntityPtr->IsVisible(aViewerPtr))
         {
            beam.SetVisible(true);
            beam.SetMarked(false);
         }
         else
         {
            beam.SetVisible(false);
            beam.SetMarked(true);
         }
         if (mSensorMap[aName]->ModeName() != aMode->modeName()) // else if the mode is different, update
         {
            rv::BeamDefinition beamDef = aMode->beamList()[0];
            float              minR    = beamDef.minRange();
            float              maxR    = beamDef.maxRange();
            if (std::isinf(maxR))
            {
               maxR = 63710000.0; // radius of the earth times 10
            }
            if (drawMode == wkf::SensorVolumesPrefData::cSLEW_VOLUME)
            {
               beam.RebuildRectangular(0.0,
                                       minR,
                                       maxR,
                                       aMode->slewLimit().minAzimuth(),
                                       aMode->slewLimit().minElevation(),
                                       aMode->slewLimit().maxAzimuth(),
                                       aMode->slewLimit().maxElevation());
            }
            else // cue volume
            {
               beam.RebuildRectangular(0.0,
                                       minR,
                                       maxR,
                                       aMode->cueLimit().minAzimuth(),
                                       aMode->cueLimit().minElevation(),
                                       aMode->cueLimit().maxAzimuth(),
                                       aMode->cueLimit().maxElevation());
            }
         }
      }
      else
      {
         unsigned int numBeams   = aMode->beamList().GetSize();
         bool         modeChange = (mSensorMap[aName]->ModeName() != aMode->modeName());
         for (unsigned int i = 0; i < numBeams; ++i)
         {
            if (mSensorMap[aName]->NumberBeams() <= i) // if the beams don't exist, add them (beam is a pair aName, beamId)
            {
               mSensorMap[aName]->AddBeam();
            }
            wkf::SensorBeam& beam    = mSensorMap[aName]->GetBeam(i);
            bool             visible = mVisible;
            const auto&      it      = mSensorStateMap.find(aName);
            if (it != mSensorStateMap.end())
            {
               visible = it->second;
            }
            else
            {
               mSensorStateMap[aName] = mVisible;
            }
            if (visible && aEntityPtr->IsVisible(aViewerPtr))
            {
               beam.SetVisible(true);
               beam.SetMarked(false);
            }
            else
            {
               beam.SetVisible(false);
               beam.SetMarked(true);
            }
            if (modeChange ||
                aMode->beamList()[i].fov().shape() ==
                   rv::FovShape::equatorial) // else if the mode is different or it has an equatorial FOV, update
            {
               rv::BeamDefinition beamDef = aMode->beamList()[i];
               int                shape   = beamDef.fov().shape();
               float              minR    = beamDef.minRange();
               float              maxR    = beamDef.maxRange();
               if (std::isinf(maxR))
               {
                  maxR = 63710000.0; // radius of the earth times 10
               }
               float tilt = beamDef.antennaTilt();
               //         beamDef.stabilized();
               //         beamDef.r0_range();
               //         beamDef.r0_rangeType();
               if (drawMode == wkf::SensorVolumesPrefData::cSCAN_VOLUME)
               {
                  beam.RebuildRectangular(tilt,
                                          minR,
                                          maxR,
                                          beamDef.scanLimit().minAzimuth(),
                                          beamDef.scanLimit().minElevation(),
                                          beamDef.scanLimit().maxAzimuth(),
                                          beamDef.scanLimit().maxElevation());
               }
               else if (drawMode == wkf::SensorVolumesPrefData::cBEAM_WIDTH)
               {
                  double azMax = 0.0;
                  double azMin = 0.0;
                  double elMax = 0.0;
                  double elMin = 0.0;
                  if (beamDef.scanMode() & rv::AngleMode::azimuth)
                  {
                     azMin = beamDef.scanLimit().minAzimuth();
                     azMax = beamDef.scanLimit().maxAzimuth();
                  }
                  else if (beamDef.beamWidthAzValid())
                  {
                     azMax = 0.5 * beamDef.beamWidthAz();
                     azMin = -azMax;
                  }
                  if (beamDef.scanMode() & rv::AngleMode::elevation)
                  {
                     elMin = beamDef.scanLimit().minElevation();
                     elMax = beamDef.scanLimit().maxElevation();
                  }
                  else if (beamDef.beamWidthElValid())
                  {
                     elMax = 0.5 * beamDef.beamWidthEl();
                     elMin = -elMax;
                  }
                  beam.RebuildRectangular(tilt, minR, maxR, azMin, elMin, azMax, elMax);
               }
               else if ((drawMode == wkf::SensorVolumesPrefData::cFIELD_OF_VIEW) || (!beamDef.fov().points().empty()))
               {
                  if (!beamDef.fov().points().empty())
                  {
                     if (shape == rv::FovShape::rectangular) // rectangular
                     {
                        float minAz = beamDef.fov().points()[0].x();
                        float minEl = beamDef.fov().points()[0].y();
                        float maxAz = beamDef.fov().points()[1].x();
                        float maxEl = beamDef.fov().points()[1].y();
                        beam.RebuildRectangular(tilt, minR, maxR, minAz, minEl, maxAz, maxEl);
                     }
                     else if (shape == rv::FovShape::circular)
                     {
                        float halfAngle = beamDef.fov().points()[0].x();
                        beam.RebuildCircular(tilt, minR, maxR, halfAngle);
                     }
                     else if (shape == rv::FovShape::polygonal || shape == rv::FovShape::equatorial)
                     {
                        std::vector<std::pair<float, float>> points;
                        int                                  numPoints = beamDef.fov().points().GetSize();
                        for (int j = 0; j < numPoints; ++j)
                        {
                           if (shape == rv::FovShape::equatorial) // Equatorial field of view, need to rotate by
                                                                  // parallactic angle.
                           {
                              double az;
                              double el;
                              double roll;
                              rv::GetCombinedOrientation(*aArticulation, az, el, roll);
                              const double parallacticAngle =
                                 UtSphericalEarth::ConvertAzElToParallacticAngle(az, el, aEntityPtr->GetPosition().GetLat());
                              const double pointX = beamDef.fov().points()[j].x();
                              const double pointY = beamDef.fov().points()[j].y();
                              const double x      = cos(parallacticAngle) * pointX + sin(parallacticAngle) * pointY;
                              const double y      = -sin(parallacticAngle) * pointX + cos(parallacticAngle) * pointY;
                              points.emplace_back(x, y);
                           }
                           else
                           {
                              points.emplace_back(beamDef.fov().points()[j].x(), beamDef.fov().points()[j].y());
                           }
                        }
                        beam.RebuildPolygonal(tilt, minR, maxR, points);
                     }
                  }
               }
               else if (drawMode == wkf::SensorVolumesPrefData::cCALCULATED)
               {
                  // this will return shapes mostly like those prior to 2.8
                  double azMax = 0.0;
                  double azMin = 0.0;
                  double elMax = 0.0;
                  double elMin = 0.0;
                  // if scanning, that volume will be shown instead of the beam
                  if (beamDef.scanMode() & rv::AngleMode::azimuth)
                  {
                     azMin = beamDef.scanLimit().minAzimuth();
                     azMax = beamDef.scanLimit().maxAzimuth();
                  }
                  else if (beamDef.beamWidthAzValid())
                  {
                     azMax = 0.5 * beamDef.beamWidthAz();
                     azMin = -azMax;
                  }
                  else if (aMode->slewMode() & rv::AngleMode::azimuth)
                  {
                     azMin     = aMode->slewLimit().minAzimuth();
                     azMax     = aMode->slewLimit().maxAzimuth();
                     nonCueing = true;
                  }
                  else
                  {
                     azMin = -UtMath::cPI;
                     azMax = UtMath::cPI;
                  }
                  if (beamDef.scanMode() & rv::AngleMode::elevation)
                  {
                     elMin = beamDef.scanLimit().minElevation();
                     elMax = beamDef.scanLimit().maxElevation();
                  }
                  else if (beamDef.beamWidthElValid())
                  {
                     elMax = 0.5 * beamDef.beamWidthEl();
                     elMin = -elMax;
                  }
                  else if (aMode->slewMode() & rv::AngleMode::elevation)
                  {
                     elMin     = aMode->slewLimit().minElevation();
                     elMax     = aMode->slewLimit().maxElevation();
                     nonCueing = true;
                  }
                  else
                  {
                     elMin = -UtMath::cPI_OVER_2;
                     elMax = UtMath::cPI_OVER_2;
                  }

                  elMin += beamDef.antennaTilt();
                  elMax += beamDef.antennaTilt();

                  beam.RebuildRectangular(tilt, minR, maxR, azMin, elMin, azMax, elMax);
               }
            }
         }
      }

      if (aArticulation)
      {
         double trans[3] = {aArticulation->locationECS().x(),
                            aArticulation->locationECS().y(),
                            aArticulation->locationECS().z()};
         if (drawMode == wkf::SensorVolumesPrefData::cSLEW_VOLUME) // these are should not be rotated by the part
         {
            double zero[3] = {0.0, 0.0, 0.0};
            mSensorMap[aName]->Articulate(zero, zero, trans); // articulate
         }
         else if ((drawMode == wkf::SensorVolumesPrefData::cCUE_VOLUME) || nonCueing)
         {
            double zero[3] = {0.0, 0.0, 0.0};
            double slew[3] = {aArticulation->slewECS().x(), aArticulation->slewECS().y(), aArticulation->slewECS().z()};
            mSensorMap[aName]->Articulate(slew, zero, trans); // articulate
         }
         else
         {
            double slew[3] = {aArticulation->slewECS().x(), aArticulation->slewECS().y(), aArticulation->slewECS().z()};
            double cue[3]  = {aArticulation->cuePCS().x(), aArticulation->cuePCS().y(), aArticulation->cuePCS().z()};
            mSensorMap[aName]->Articulate(slew, cue, trans); // articulate
         }
      }
      bool   showFaces;
      bool   showEdges;
      bool   showProjections;
      QColor fColor;
      QColor eColor;
      QColor pColor;
      mPrefObject->GetModeDefinition(aMode->modeName(), mWeapon, showFaces, fColor, showEdges, eColor, showProjections, pColor);
      mSensorMap[aName]->SetDisplayData(showFaces, fColor, showEdges, eColor, showProjections, pColor);
      mSensorMap[aName]->SetModeName(aMode->modeName());
   }
   else
   {
      const auto& it = mSensorMap.find(aName);
      if (it != mSensorMap.end())
      {
         int nb = it->second->NumberBeams();
         for (int i = 0; i < nb; i++)
         {
            it->second->GetBeam(i).SetVisible(false);
         }
      }
   }
}

void RvSensorVolumes::PlatformVolumes::ModesReset()
{
   for (auto& it : mSensorMap)
   {
      bool   showFaces;
      bool   showEdges;
      bool   showProjections;
      QColor fColor;
      QColor eColor;
      QColor pColor;
      mPrefObject->GetModeDefinition(it.second->ModeName(), mWeapon, showFaces, fColor, showEdges, eColor, showProjections, pColor);
      it.second->SetDisplayData(showFaces, fColor, showEdges, eColor, showProjections, pColor);
   }
}

void RvSensorVolumes::PlatformVolumes::SetSensorVisible(const std::string& aName, bool aState)
{
   mSensorStateMap[aName] = aState;
}

void RvSensorVolumes::PlatformVolumes::CheckSensorVisibility(bool& aAll, bool& aSome) const
{
   if (mSensorStateMap.empty())
   {
      aAll  = mVisible;
      aSome = mVisible;
      return;
   }
   else
   {
      aSome = false;
      aAll  = true;
      for (auto it : mSensorStateMap)
      {
         if (!it.second)
         {
            aAll = false;
         }
         else
         {
            aSome = true;
         }
      }
      return;
   }
}

bool RvSensorVolumes::PlatformVolumes::SensorVisible(const std::string& aName, bool aUnsureReturn) const
{
   const auto& it = mSensorStateMap.find(aName);
   if (it != mSensorStateMap.end())
   {
      return it->second;
   }
   return aUnsureReturn;
}
