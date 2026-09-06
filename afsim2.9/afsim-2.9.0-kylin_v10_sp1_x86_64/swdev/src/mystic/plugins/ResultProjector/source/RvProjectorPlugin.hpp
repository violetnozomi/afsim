// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef RVPROJECTORPLUGIN_HPP
#define RVPROJECTORPLUGIN_HPP

#include "RvPlugin.hpp"

namespace RvProjector
{
class Plugin : public rv::Plugin
{
   Q_OBJECT
public:
   explicit Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;


   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

private:
   void CreateProjector(const QString& aPlatformName, const QString& aSensorName);
   void RemoveProjector(const QString& aPlatformName, const QString& aSensorName);
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   std::map<QString, std::map<QString, unsigned int>> mProjectorMap;
};
} // namespace RvProjector
#endif
