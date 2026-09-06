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
#ifndef RVPLUGINVASCENARIOMANAGER_HPP
#define RVPLUGINVASCENARIOMANAGER_HPP

#include "RvEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "UtConcurrentQueue.hpp"
#include "wsf_draw/WkfWsfDrawDockWidget.hpp"

namespace wkf
{
class Environment;
}

namespace RvWsfDraw
{
class Interface;

class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   void BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr) override;
   //      virtual void GuiUpdate() override;
   void DrawCommandRead(rv::ResultDb& aData, const rv::MsgDrawCommand& aDrawCommand) override;

   void ClearScenario(bool aFullReset) override;

private:
   // managed by Qt
   Interface* mInterfacePtr;

   wkf::WsfDrawDockWidget* mDockWidgetPtr;

   UtConcurrentQueue<rv::MsgDrawCommand> mCachedCommands;
};
} // namespace RvWsfDraw
#endif
