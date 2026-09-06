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

#ifndef WKFSHAREDMAPPREFERENCESOBJECT_HPP
#define WKFSHAREDMAPPREFERENCESOBJECT_HPP

#include "wkf_export.h"

#include <set>

#include "VaCallbackHolder.hpp"
#include "WkfPrefObject.hpp"

namespace vespa
{
class VaViewer;
}

namespace wkf
{
struct SharedMapData
{
   std::string mFile{};
   std::string mProjection{"CME_WCS"};
   float       mLatOffset{0.0f};
   bool        mLock{true};
};

struct SharedMapPreferencesData
{
   SharedMapPreferencesData();

   std::map<std::string, SharedMapData> mMaps;

   enum MapType
   {
      IMMERSIVE,
      NAVIGATIONAL
   };
   std::map<MapType, std::string> mProfileMap;
};

class WKF_EXPORT SharedMapPreferencesObject : public PrefObjectT<SharedMapPreferencesData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "SharedMapPreferences";

   SharedMapPreferencesObject(QObject* aParent = nullptr);
   ~SharedMapPreferencesObject() override = default;

   const SharedMapData*                        GetMapData(const std::string& aName) const;
   const std::map<std::string, SharedMapData>& GetMaps() const;

   std::string SetMap(const std::string& aMapName, vespa::VaViewer* aViewerPtr, const std::string& aCheckMap = "") const;

   std::string SetMapProfile(const SharedMapPreferencesData::MapType aType, vespa::VaViewer* aViewerPtr);

   //! restores the default profiles
   void RestoreDefaults() override;

   bool IsSharedPreference() const override { return true; }

signals:
   void SharedMapsChanged();

private:
   void                     SetPreferenceDataP(const SharedMapPreferencesData& aPrefData) override;
   void                     Apply() override;
   SharedMapPreferencesData ReadSettings(QSettings& aSettings) const override;
   void                     SaveSettingsP(QSettings& aSettings) const override;
   void                     ViewerDestroyed(vespa::VaViewer* aViewerPtr);


   std::map<unsigned int, std::pair<vespa::VaViewer*, SharedMapPreferencesData::MapType>> mProfileViewerMap;
   std::set<wkf::SharedMapPreferencesData::MapType>                                       mProfileChanged;
   vespa::VaCallbackHolder                                                                mCallbacks;
   int                                                                                    mViewerDestroyedCallbackId;
};
} // namespace wkf

Q_DECLARE_METATYPE(wkf::SharedMapPreferencesData)
#endif
