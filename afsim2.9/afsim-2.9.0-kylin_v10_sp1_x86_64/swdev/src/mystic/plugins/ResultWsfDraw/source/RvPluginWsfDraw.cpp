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
#include "RvPluginWsfDraw.hpp"

#include <QAction>
#include <QMenu>

#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvWsfDrawInterface.hpp"
#include "VaCamera.hpp"
#include "VaCameraMotionTethered.hpp"
#include "VaViewer.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "wsf_draw/WkfWsfDrawDockWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvWsfDraw::Plugin, "WSF Draw", "Displays WSF Draw layers on the map view", "mystic")

RvWsfDraw::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
   , mInterfacePtr(new Interface(this))
   , mDockWidgetPtr(new wkf::WsfDrawDockWidget(wkfEnv.GetMainWindow()))
{
   mDockWidgetPtr->SetModel(*mInterfacePtr);

   mDockWidgetPtr->hide();
}

void RvWsfDraw::Plugin::DrawCommandRead(rv::ResultDb& aData, const rv::MsgDrawCommand& aDrawCommand)
{
   mCachedCommands.Push(aDrawCommand);
}

void RvWsfDraw::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   while (!mCachedCommands.Empty())
   {
      mInterfacePtr->DrawCommand(mCachedCommands.Pop());
   }
   mInterfacePtr->AdvanceTimeRead(aData.GetSimTime());
}

void RvWsfDraw::Plugin::RegularRead(const rv::ResultData& aData)
{
   while (!mCachedCommands.Empty())
   {
      mInterfacePtr->DrawCommand(mCachedCommands.Pop());
   }
}

void RvWsfDraw::Plugin::BuildViewerContextMenu(QMenu* aMenu, vespa::VaViewer* aViewerPtr)
{
   UtVariant v = aViewerPtr->GetData("type");
   if (!v.IsUnset())
   {
      std::string type = v.GetString();
      if (type == "immersive")
      {
         vespa::VaCameraMotionTethered* tether =
            dynamic_cast<vespa::VaCameraMotionTethered*>(aViewerPtr->GetCamera()->GetCameraMotion());
         if (tether)
         {
            wkf::Platform* plat = dynamic_cast<wkf::Platform*>(tether->GetEntity());
            if (plat)
            {
               QList<QString> layerNames = mInterfacePtr->GetLayerStrings(aViewerPtr);
               if (!layerNames.empty())
               {
                  QMenu* drawMenu = new QMenu("WsfDraw", aMenu);
                  for (const auto& it : layerNames)
                  {
                     QString  layerName   = it;
                     QAction* layerAction = new QAction(layerName, drawMenu);
                     layerAction->setCheckable(true);
                     layerAction->setChecked(mInterfacePtr->GetLayerVisibility(aViewerPtr, layerName));
                     connect(layerAction,
                             &QAction::triggered,
                             [aViewerPtr, layerName, this](bool checked)
                             { mInterfacePtr->ShowViewerLayer(aViewerPtr, layerName.toStdString(), checked); });
                     drawMenu->addAction(layerAction);
                  }
                  aMenu->addMenu(drawMenu);
               }
            }
         }
      }
   }
}

void RvWsfDraw::Plugin::ClearScenario(bool aFullReset)
{
   mInterfacePtr->ClearScenario();
   mCachedCommands.Clear();
}
