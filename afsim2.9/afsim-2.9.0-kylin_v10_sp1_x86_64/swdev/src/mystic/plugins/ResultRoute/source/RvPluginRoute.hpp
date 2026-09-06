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
#ifndef RVPLUGINROUTE_HPP
#define RVPLUGINROUTE_HPP

#include <QTableWidget>

#include "RvEventPipeClasses.hpp"
#include "RvPlugin.hpp"

namespace wkf
{
class AttachmentRoute;
}

namespace RvRoute
{
class RouteDialog : public QDialog
{
public:
   RouteDialog(QWidget* aParent, wkf::AttachmentRoute* aRoutePtr);
   ~RouteDialog() override = default;

   void Populate(const rv::MsgRouteChanged* aRouteData);


private:
   void                    AttachmentSelectedCB(vespa::VaViewer*                aViewerPtr,
                                                int                             aState,
                                                vespa::VaAttachment*            aAttachmentPtr,
                                                const vespa::VaAttachmentSubId& aSubPartId);
   wkf::AttachmentRoute*   mRoutePtr;
   QTableWidget*           mTable;
   vespa::VaCallbackHolder mCallbacks;
   rv::MsgRouteChanged     mRouteData;
};
class Plugin : public rv::Plugin
{
   Q_OBJECT
public:
   explicit Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aOptionId, const wkf::Platform* aPlatformPtr) const override;

private:
   void ClearScenario(bool aFullReset) override;
   int  BuildRoute(wkf::Platform& aPlatform, wkf::AttachmentRoute& aRoute, int aPreviousMessageIdx);
   void ShowRouteInformation(wkf::Platform& aPlatform);

   std::map<size_t, std::pair<unsigned int, wkf::AttachmentRoute*>> mRouteMap;
   std::map<size_t, QPointer<RouteDialog>>                          mDialogMap;
};
} // namespace RvRoute

#endif
