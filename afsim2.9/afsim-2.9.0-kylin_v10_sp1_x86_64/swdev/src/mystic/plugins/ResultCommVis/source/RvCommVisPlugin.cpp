// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvCommVisPlugin.hpp"

#include <QAction>
#include <QMenu>

#include "RvEventPipeClasses.hpp"
#include "RvResultData.hpp"
#include "RvResultDb.hpp"
#include "RvResultPlatform.hpp"
#include "UtQtMemory.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvCommVis::Plugin,
                          "CommVis",
                          "Comms Visualization Plugin.",
                          "mystic",
                          false) // Plugin not loaded by default until it is completed

RvCommVis::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::Plugin(aPluginName, aUniqueId)
{
   // Provide access from tools menu
   wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
   QMenu*           toolsMenuPtr  = mainWindowPtr->GetMenuByName("Tools");

   if (toolsMenuPtr)
   {
      auto dlgActionPtr = ut::qt::make_qt_ptr<wkf::Action>(QIcon::fromTheme("star"), "CommVis...", mainWindowPtr);
      connect(dlgActionPtr, &QAction::triggered, [this]() { Start(); });
      toolsMenuPtr->addAction(dlgActionPtr);
   }
}

void RvCommVis::Plugin::Start()
{
   if (mDialog == nullptr)
   {
      wkf::MainWindow* mainWindowPtr = wkfEnv.GetMainWindow();
      mDialog                        = ut::qt::make_qt_ptr<wkf::CommVisDialog>(mCommData, mainWindowPtr);
   }
   // The pendingStartEvent must be sent (and be sent last) to trigger off the initial build of the scene
   wkf::CommEvent pendingStartEvent(0.0, wkf::CommEventType::PendingStartEvent);
   mCommData.AddCommData(pendingStartEvent);

   mDialog->Update();
   mDialog->show();
}

void RvCommVis::Plugin::Plugin::NetworkInfoRead(rv::ResultDb& aData, const rv::MsgNetworkInfo& aNetworkInfo)
{
   wkf::CommEvent              addNetworkEvent(0.0, wkf::CommEventType::AddNetwork);
   wkf::CommEvent::NetworkInfo networkInfo;
   networkInfo.mNetworkName    = aNetworkInfo.networkName();
   networkInfo.mNetworkAddress = aNetworkInfo.networkAddress();
   networkInfo.mNetworkType    = aNetworkInfo.networkType();
   addNetworkEvent.SetNetworkInfo(networkInfo);
   mCommData.AddCommData(addNetworkEvent);
}

void RvCommVis::Plugin::Plugin::CommInfoRead(rv::ResultDb& aData, const rv::MsgCommInfo& aCommInfo)
{
   wkf::CommEvent addCommEvent(0.0, wkf::CommEventType::AddComm);

   wkf::CommEvent::NetworkInfo networkInfo;
   networkInfo.mNetworkName    = aCommInfo.networkName();
   networkInfo.mNetworkAddress = aCommInfo.networkAddress();

   addCommEvent.SetNetworkInfo(networkInfo);

   wkf::CommEvent::CommInfo commInfo;
   commInfo.mCommName     = aCommInfo.commName();
   commInfo.mCommAddress  = aCommInfo.commAddress();
   commInfo.mPlatformName = aCommInfo.parentPlatform();
   commInfo.mCommType     = aCommInfo.commType();
   addCommEvent.SetCommInfo(commInfo);

   auto links = aCommInfo.links();
   // Loop through the pairs and create a list of links to this comm
   std::vector<wkf::CommEvent::LinkInfo> linkInfo;
   for (const auto& curLink : links)
   {
      wkf::CommEvent::LinkInfo curLinkInfo;
      curLinkInfo.mDestinationPlatformName = curLink.platformName();
      curLinkInfo.mDestinationCommName     = curLink.commName();
      linkInfo.emplace_back(curLinkInfo);
   }

   addCommEvent.SetLinks(linkInfo);
   mCommData.AddCommData(addCommEvent);
}

void RvCommVis::Plugin::Plugin::RouterInfoRead(rv::ResultDb& aData, const rv::MsgRouterInfo& aRouterInfo)
{
   // The next iteration of the plugin will take the information provided in the aRouterInfo message and create gateways
   // and routers from it.
}

void RvCommVis::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   // The next iteration of the plugin will take the information provided in the aData and use it to create messages
   // passed in the comm network.
}
