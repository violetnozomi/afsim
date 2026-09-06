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
#include "WkfSensorVolumesPrefObject.hpp"

#include <iostream>

#include <QSettings>

wkf::SensorVolumesPrefData::SensorVolumesPrefData()
{
   mModeMap.emplace(std::piecewise_construct, std::forward_as_tuple("default", false), std::forward_as_tuple());
   auto result =
      mModeMap.emplace(std::piecewise_construct, std::forward_as_tuple("default", true), std::forward_as_tuple());
   result.first->second.mFaceColor = QColor(255, 0, 0, 64);
   result.first->second.mEdgeColor = QColor(255, 68, 68, 64);
   result.first->second.mProjColor = QColor(255, 0, 0, 32);
}

wkf::SensorVolumesPrefObject::SensorVolumesPrefObject(QObject* parent)
   : PrefObjectT(parent, "SensorVolumes")
{
}

void wkf::SensorVolumesPrefObject::GetModeDefinition(const std::string& aName,
                                                     bool               aWeapon,
                                                     bool&              aShowFaces,
                                                     QColor&            aFaceColor,
                                                     bool&              aShowEdges,
                                                     QColor&            aEdgeColor,
                                                     bool&              aShowProjection,
                                                     QColor&            aProjColor) const
{
   auto it = mCurrentPrefs.mModeMap.find(std::make_pair(aName, aWeapon));
   if (it == mCurrentPrefs.mModeMap.end())
   {
      it = mCurrentPrefs.mModeMap.find(std::make_pair("default", aWeapon));
   }
   aShowFaces      = it->second.mShowFaces;
   aFaceColor      = it->second.mFaceColor;
   aShowEdges      = it->second.mShowEdges;
   aEdgeColor      = it->second.mEdgeColor;
   aShowProjection = it->second.mShowProjection;
   aProjColor      = it->second.mProjColor;
}

void wkf::SensorVolumesPrefObject::EmitAllSignals()
{
   emit ModesReset();
   for (auto& it : mCurrentPrefs.mModeMap)
   {
      emit ModeChanged(it.first.first,
                       it.first.second,
                       it.second.mShowFaces,
                       it.second.mFaceColor,
                       it.second.mShowEdges,
                       it.second.mEdgeColor,
                       it.second.mShowProjection,
                       it.second.mProjColor);
   }
   emit DrawModeChanged(mCurrentPrefs.mDrawMode);
}

void wkf::SensorVolumesPrefObject::Apply()
{
   EmitAllSignals();
}

wkf::SensorVolumesPrefData wkf::SensorVolumesPrefObject::ReadSettings(QSettings& aSettings) const
{
   SensorVolumesPrefData pData;
   int                   count = aSettings.beginReadArray("mode_");
   for (int i = 0; i < count; ++i)
   {
      aSettings.setArrayIndex(i);
      std::string name           = aSettings.value("name").toString().toStdString();
      bool        weapon         = aSettings.value("weapon").toBool();
      bool        showFaces      = aSettings.value("showFaces").toBool();
      QColor      faceColor      = aSettings.value("faceColor").value<QColor>();
      bool        showEdges      = aSettings.value("showEdges").toBool();
      QColor      edgeColor      = aSettings.value("edgeColor").value<QColor>();
      bool        showProjection = aSettings.value("showProjection").toBool();
      QColor      projColor      = aSettings.value("projectionColor").value<QColor>();
      pData.mModeMap[std::make_pair(name, weapon)] =
         wkf::SensorModeOptions(showFaces, faceColor, showEdges, edgeColor, showProjection, projColor);
   }
   aSettings.endArray();
   pData.mDrawMode =
      static_cast<SensorVolumesPrefData::DrawMode>(aSettings.value("drawMode", mDefaultPrefs.mDrawMode).toInt());
   return pData;
}

void wkf::SensorVolumesPrefObject::SaveSettingsP(QSettings& aSettings) const
{
   aSettings.beginWriteArray("mode_");
   int i = 0;
   for (auto& it : mCurrentPrefs.mModeMap)
   {
      aSettings.setArrayIndex(i);
      aSettings.setValue("name", it.first.first.c_str());
      aSettings.setValue("weapon", it.first.second);
      aSettings.setValue("showFaces", it.second.mShowFaces);
      aSettings.setValue("faceColor", it.second.mFaceColor);
      aSettings.setValue("showEdges", it.second.mShowEdges);
      aSettings.setValue("edgeColor", it.second.mEdgeColor);
      aSettings.setValue("showProjection", it.second.mShowProjection);
      aSettings.setValue("projectionColor", it.second.mProjColor);
      ++i;
   }
   aSettings.endArray();
   aSettings.setValue("drawMode", mCurrentPrefs.mDrawMode);
}
