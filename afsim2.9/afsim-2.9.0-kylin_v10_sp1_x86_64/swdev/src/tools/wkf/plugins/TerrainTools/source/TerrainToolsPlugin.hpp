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

#ifndef TERRAINTOOLSPLUGIN_HPP
#define TERRAINTOOLSPLUGIN_HPP

#include "VaEntityPtr.hpp"
#include "VaLocationReference.hpp"
#include "WkfPlugin.hpp"

namespace wkf
{
class AttachmentRubberband;
}

namespace TerrainTools
{
class LOS_Ruler;

class Plugin : public wkf::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

private:
   void LineOfSightFrom(const vespa::VaLocationReference& aLocRef);
   void MakeLOS(const wkf::AttachmentRubberband& aRubberband);
   void RubberbandUpdater(wkf::AttachmentRubberband& aRubberband);

   LOS_Ruler*                    mActiveRulerPtr{nullptr};
   vespa::EntityPtr<wkf::Entity> mSrcEntity;
};
} // namespace TerrainTools

#endif
