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
#ifndef RVWSFDRAWINTERFACE_HPP
#define RVWSFDRAWINTERFACE_HPP

#include <QAbstractItemModel>

#include "RvEventPipeClasses.hpp"
#include "wsf_draw/WkfOverlayWsfDraw.hpp"
#include "wsf_draw/WkfWsfDrawItemModel.hpp"

namespace RvWsfDraw
{
class Interface : public wkf::WsfDrawItemModel
{
   Q_OBJECT
public:
   Interface(QObject* aParent);

   void           AdvanceTimeRead(double aTime);
   void           DrawCommand(const rv::MsgDrawCommand& aDrawCommand, bool aHandleDeferred = true);
   void           ShowViewerLayer(vespa::VaViewer* aViewer, const std::string& aName, bool aState);
   QList<QString> GetLayerStrings(vespa::VaViewer* aViewer) const;
   bool           GetLayerVisibility(vespa::VaViewer* aViewer, const QString& aName);

   void ClearScenario();

signals:
   void LayerAdded(const QString& aName);
   void ResetLayers();

private:
   bool MakeVertex(const rv::MsgDrawCommand_Vertex& aMsg, wkf::OverlayWsfDrawBase::VertexObject& aVert);

   void DrawViewerCommand(vespa::VaViewer*          aViewer,
                          LayerMap&                 aLayerMap,
                          const rv::MsgDrawCommand& aDrawCommand,
                          bool                      aHandleDeferred = true);

   void ProcessDeferredCommands();
   void ViewerDestroyedHandler(vespa::VaViewer* aViewer);
   void ViewerInitializedHandler(vespa::VaViewer* aViewer);

   std::vector<rv::MsgDrawCommand> mDeferredCommands;
   std::mutex                      mMutex;

   std::unordered_map<vespa::VaViewer*, LayerMap>             mViewerLayerMaps;
   std::map<vespa::VaViewer*, std::set<wkf::OverlayWsfDraw*>> mUnattachedByViewer;

   // used to draw in other viewers that have not been created when the command are initially processed
   std::vector<rv::MsgDrawCommand> mCommands;

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace RvWsfDraw


#endif
