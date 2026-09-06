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
#ifndef WkPlatformSensorVolumes_HPP
#define WkPlatformSensorVolumes_HPP

#include <map>
#include <string>

namespace vespa
{
class VaEntity;
class VaViewer;
} // namespace vespa

namespace wkf
{
class AttachmentSensorVolume;
class SensorVolumesPrefObject;
} // namespace wkf
namespace rv
{
class MsgPartArticulationStatus;
class MsgEmitterModeDefinition;
} // namespace rv

namespace RvSensorVolumes
{
class PlatformVolumes
{
public:
   PlatformVolumes(vespa::VaEntity& aEntityPtr, const wkf::SensorVolumesPrefObject* aPrefObject, bool aWeapon);
   virtual ~PlatformVolumes() = default;
   void RemoveAttachments();

   void SetVisible(bool aState);

   bool IsDefaultVisible() const;
   bool IsAtAllVisible() const;

   void MarkAll();
   void HideMarked();
   void UpdateAndUnmark(const std::string&                   aName,
                        const rv::MsgPartArticulationStatus* aArticulation,
                        const rv::MsgEmitterModeDefinition*  aMode,
                        vespa::VaEntity*                     aEntityPtr,
                        vespa::VaViewer*                     aViewerPtr);
   void ModesReset();
   void SetSensorVisible(const std::string& aName, bool aState);
   void CheckSensorVisibility(bool& aAll, bool& aSome) const;
   // aUnsureReturn is the value that will be returned if there isn't a conclusive answer.
   bool SensorVisible(const std::string& aName, bool aUnsureReturn) const;

private:
   std::map<std::string, wkf::AttachmentSensorVolume*> mSensorMap;
   std::map<std::string, bool>                         mSensorStateMap;
   bool                                                mVisible;
   const wkf::SensorVolumesPrefObject*                 mPrefObject;
   bool                                                mWeapon;
};
} // namespace RvSensorVolumes
#endif
