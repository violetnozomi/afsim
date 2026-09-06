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

#include "RvWsfDrawInterface.hpp"

#include "VaObserver.hpp"
#include "VaViewerManager.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "wsf_draw/WkfOverlayWsfDraw.hpp"

namespace
{
bool IsImmersive(vespa::VaViewer* aViewer)
{
   UtVariant v = aViewer->GetData("type");
   if (!v.IsUnset())
   {
      return v.GetString() == "immersive";
   }
   return false;
}
} // namespace

RvWsfDraw::Interface::Interface(QObject* aParent)
   : wkf::WsfDrawItemModel(aParent, mViewerLayerMaps)
{
   mCallbacks.Add(vespa::VaObserver::ViewerInitialized.Connect(&RvWsfDraw::Interface::ViewerInitializedHandler, this));
   mCallbacks.Add(vespa::VaObserver::ViewerDestroyed.Connect(&RvWsfDraw::Interface::ViewerDestroyedHandler, this));

   // Add all viewers that were initialized prior to this constructor
   for (const auto& viewerIt : vaEnv.GetViewerManager()->GetViewers())
   {
      if (viewerIt == vaEnv.GetStandardViewer() || IsImmersive(viewerIt))
      {
         mViewerLayerMaps.emplace(viewerIt, LayerMap());
      }
   }
   mViewerLayerMaps.emplace(vaEnv.GetStandardViewer(), LayerMap());
}

void RvWsfDraw::Interface::DrawCommand(const rv::MsgDrawCommand& aDrawCommand, bool aHandleDeferred)
{
   mCommands.push_back(aDrawCommand);
   for (auto& viewerIt : mViewerLayerMaps)
   {
      DrawViewerCommand(viewerIt.first, viewerIt.second, aDrawCommand, aHandleDeferred);
   }
}

void RvWsfDraw::Interface::AdvanceTimeRead(double aTime)
{
   // handle deferred commands
   if (mMutex.try_lock())
   {
      ProcessDeferredCommands();
      mMutex.unlock();
   }
   for (auto& viewerIt : mUnattachedByViewer)
   {
      if (!viewerIt.second.empty())
      {
         if (nullptr != viewerIt.first)
         {
            for (auto&& it : viewerIt.second)
            {
               viewerIt.first->AddOverlay(it);
               if (viewerIt.first == vaEnv.GetStandardViewer())
               {
                  emit LayerAdded(it->GetName().c_str());
               }
            }
            mUnattachedByViewer[viewerIt.first].clear();
         }
      }
   }
}

bool RvWsfDraw::Interface::MakeVertex(const rv::MsgDrawCommand_Vertex& aMsg, wkf::OverlayWsfDrawBase::VertexObject& aVert)
{
   const rv::Vec3f& v = aMsg.xyz();
   aVert.mPosition[0] = v.x();
   aVert.mPosition[1] = v.y();
   aVert.mPosition[2] = v.z();
   switch (aMsg.type())
   {
   case rv::MsgDrawCommand_VertexType::unset_vertex:
   case rv::MsgDrawCommand_VertexType::absolute_wcs:
      break;
   case rv::MsgDrawCommand_VertexType::relative_zero:
   case rv::MsgDrawCommand_VertexType::relative_ecs:
   case rv::MsgDrawCommand_VertexType::relative_ned:
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         aVert.mReferenceEntityPtr = scenarioPtr->FindPlatformByIndex(aMsg.relativePlatformIndex());
         if (!aVert.mReferenceEntityPtr && aMsg.relativePlatformIndex())
         {
            aVert.mReferenceEntityPtr = scenarioPtr->CreatePlaceholderPlatform(aMsg.relativePlatformIndex());
         }
         if (!aVert.mReferenceEntityPtr)
         {
            return false;
         }
      }
      break;
   }
   case rv::MsgDrawCommand_VertexType::relative_ss:
   {
      auto* scenarioPtr = vaEnv.GetStandardScenario();
      if (scenarioPtr)
      {
         aVert.mReferenceEntityPtr = scenarioPtr->FindPlatformByIndex(aMsg.relativePlatformIndex());
         if (!aVert.mReferenceEntityPtr && aMsg.relativePlatformIndex())
         {
            aVert.mReferenceEntityPtr = scenarioPtr->CreatePlaceholderPlatform(aMsg.relativePlatformIndex());
            if (!aVert.mReferenceEntityPtr)
            {
               return false;
            }
         }
      }
      break;
   }
   }
   switch (aMsg.type())
   {
   case rv::MsgDrawCommand_VertexType::unset_vertex:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cUNSET_VERTEX;
      break;
   case rv::MsgDrawCommand_VertexType::absolute_wcs:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cABSOLUTE_WCS;
      break;
   case rv::MsgDrawCommand_VertexType::relative_zero:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cRELATIVE_ZERO;
      break;
   case rv::MsgDrawCommand_VertexType::relative_ecs:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cRELATIVE_ECS;
      break;
   case rv::MsgDrawCommand_VertexType::relative_ned:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cRELATIVE_NED;
      break;
   case rv::MsgDrawCommand_VertexType::relative_ss:
      aVert.mVertexType = wkf::OverlayWsfDrawBase::cABSOLUTE_SCREEN;
      break;
   default:
      break;
   }
   return true;
}

void RvWsfDraw::Interface::DrawViewerCommand(vespa::VaViewer*          aViewer,
                                             LayerMap&                 aLayerMap,
                                             const rv::MsgDrawCommand& aDrawCommand,
                                             bool                      aHandleDeferred)
{
   bool    newLayer = false;
   QString layer    = "WSF_DRAW";
   if (!aDrawCommand.layer().empty())
   {
      layer = QString::fromStdString(aDrawCommand.layer());
   }
   if (vaEnv.GetStandardScenario() == nullptr)
   {
      // we don't have a scenario, yet.  Defer executing the command;
      mDeferredCommands.push_back(aDrawCommand);
      return;
   }
   if (aHandleDeferred)
   {
      mMutex.lock();
      ProcessDeferredCommands();
      mMutex.unlock();
   }

   if (aLayerMap.find(layer) == aLayerMap.end())
   {
      wkf::OverlayWsfDraw* overlay = new wkf::OverlayWsfDraw(layer.toStdString());
      aLayerMap[layer]             = overlay;
      if (aViewer != nullptr)
      {
         aViewer->AddOverlay(aLayerMap[layer]);
         newLayer = true;
         if (aViewer == vaEnv.GetStandardViewer())
         {
            connect(overlay,
                    &wkf::OverlayWsfDraw::ElementAdded,
                    [this, layer](int aIndex) { ElementAddedCB(layer, aIndex); });
            connect(overlay,
                    &wkf::OverlayWsfDraw::ElementActivated,
                    [this, layer](int aIndex, bool aState) { ElementActivatedCB(layer, aIndex, aState); });
         }
      }
      else
      {
         mUnattachedByViewer[aViewer].insert(aLayerMap[layer]);
      }
   }

   wkf::OverlayWsfDraw*               draw  = aLayerMap[layer];
   const rv::MsgDrawCommand_ItemList& items = aDrawCommand.items();

   float time = aDrawCommand.simTime();

   for (size_t i = 0; i < items.size(); ++i)
   {
      const rv::MsgDrawCommand_Item& item = items[i];

      unsigned int  color = item.color();
      unsigned char a     = (unsigned char)((color >> 24)); // Uto is backwards in alpha
      unsigned char b     = (unsigned char)(0xff & (color >> 16));
      unsigned char g     = (unsigned char)(0xff & (color >> 8));
      unsigned char r     = (unsigned char)(0xff & (color));

      float endTime = time + item.duration();

      switch (item.drawType())
      {
      case rv::MsgDrawCommand_DrawType::line:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1, v2;
         if (MakeVertex(item.vertex1(), v1) && MakeVertex(item.vertex2(), v2))
         {
            draw->AddLine(time, endTime, item.id(), v1, v2, item.drawSize(), item.drawStyle(), r, g, b, a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::point:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddPoint(time, endTime, item.id(), v1, item.drawSize(), r, g, b, a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::icon:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddIcon(time, endTime, item.id(), v1, item.orientation().x(), item.text(), r, g, b, a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::ellipse:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddEllipse(time,
                             endTime,
                             item.id(),
                             v1,
                             item.orientation().x(),
                             item.axis().x(),
                             item.axis().y(),
                             item.drawSize(),
                             item.drawStyle(),
                             item.drawStyle2(),
                             r,
                             g,
                             b,
                             a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::erase:
      {
         draw->Erase(time, item.id());
      }
      break;
      case rv::MsgDrawCommand_DrawType::ellipsoid:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddEllipsoid(time,
                               endTime,
                               item.id(),
                               v1,
                               item.orientation().x(),
                               item.orientation().y(),
                               item.orientation().z(),
                               item.axis().x(),
                               item.axis().y(),
                               item.axis().z(),
                               item.drawSize(),
                               item.drawStyle(),
                               item.drawStyle2(),
                               r,
                               g,
                               b,
                               a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::quadrilateral:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         wkf::OverlayWsfDrawBase::VertexObject v2;
         wkf::OverlayWsfDrawBase::VertexObject v3;
         wkf::OverlayWsfDrawBase::VertexObject v4;
         if (MakeVertex(item.vertex1(), v1) && MakeVertex(item.vertex2(), v2) && MakeVertex(item.vertex3(), v3) &&
             MakeVertex(item.vertex4(), v4))
         {
            draw->AddQuadrilateral(time, endTime, item.id(), v1, v2, v3, v4, r, g, b, a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::text:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddText(time, endTime, item.id(), v1, item.drawSize(), item.text(), r, g, b, a);
         }
      }
      break;
      case rv::MsgDrawCommand_DrawType::timer:
      {
         wkf::OverlayWsfDrawBase::VertexObject v1;
         if (MakeVertex(item.vertex1(), v1))
         {
            draw->AddTimer(time, endTime, item.id(), v1, item.drawSize(), r, g, b, a);
         }
      }
      break;
      }
   }
   if (newLayer)
   {
      if (aViewer == vaEnv.GetStandardViewer())
      {
         emit LayerAdded(layer);
         HandleNewLayer(layer);
      }
      else
      {
         aLayerMap[layer]->SetVisible(false);
      }
   }
}

void RvWsfDraw::Interface::ProcessDeferredCommands()
{
   if (!mDeferredCommands.empty())
   {
      std::vector<rv::MsgDrawCommand> def = mDeferredCommands;
      mDeferredCommands.clear();
      for (auto& it : def)
      {
         DrawCommand(it, false);
      }
   }
}

void RvWsfDraw::Interface::ViewerDestroyedHandler(vespa::VaViewer* aViewer)
{
   auto mapIt = mViewerLayerMaps.find(aViewer);
   if (mapIt != mViewerLayerMaps.end())
   {
      for (auto& layerIt : mapIt->second)
      {
         aViewer->RemoveOverlay(layerIt.second->GetName());
      }
   }
   mViewerLayerMaps.erase(aViewer);
}

void RvWsfDraw::Interface::ViewerInitializedHandler(vespa::VaViewer* aViewer)
{
   if (IsImmersive(aViewer))
   {
      mViewerLayerMaps.emplace(aViewer, LayerMap());
      for (const auto& it : mCommands)
      {
         DrawViewerCommand(aViewer, mViewerLayerMaps[aViewer], it);
      }
   }
}

void RvWsfDraw::Interface::ShowViewerLayer(vespa::VaViewer* aViewer, const std::string& aName, bool aState)
{
   auto layerMap = mViewerLayerMaps.find(aViewer);
   if (layerMap != mViewerLayerMaps.end())
   {
      auto layer = layerMap->second.find(QString::fromStdString(aName));
      if (layer != layerMap->second.end())
      {
         layer->second->SetVisible(aState);
      }
   }
}

QList<QString> RvWsfDraw::Interface::GetLayerStrings(vespa::VaViewer* aViewer) const
{
   QList<QString> stringList;
   auto           mapIt = mViewerLayerMaps.find(aViewer);
   if (mapIt != mViewerLayerMaps.end())
   {
      for (const auto& it : mapIt->second)
      {
         stringList.push_back(it.first);
      }
   }
   return stringList;
}

bool RvWsfDraw::Interface::GetLayerVisibility(vespa::VaViewer* aViewer, const QString& aName)
{
   auto mapIt = mViewerLayerMaps.find(aViewer);
   if (mapIt != mViewerLayerMaps.end())
   {
      auto layerIt = mapIt->second.find(aName);
      if (layerIt != mapIt->second.end())
      {
         return layerIt->second->IsVisible();
      }
   }
   return false;
}

void RvWsfDraw::Interface::ClearScenario()
{
   mDeferredCommands.clear();
   for (auto& viewerIt : mUnattachedByViewer)
   {
      for (auto&& it : viewerIt.second)
      {
         delete it;
      }
      viewerIt.second.clear();
   }
   for (auto& viewerIt : mViewerLayerMaps)
   {
      for (auto&& it : viewerIt.second)
      {
         viewerIt.first->RemoveOverlay(it.first.toStdString());
      }
      viewerIt.second.clear();
   }

   HandleClearLayers();
   emit ResetLayers();
}

// Commented code from DrawCommand function
/*
   std::cout << "Draw Command:"<< std::endl;
   std::cout << "  SimTime = " << aDrawCommand.simTime() << std::endl;
   if (aDrawCommand.layerValid())
   {
      std::cout << "  Layer = " << aDrawCommand.layer() << std::endl;
   }
   ViEventPipe::MsgDrawCommand_ItemList il = aDrawCommand.items();
   std::cout << "  #Items = " << il.size() << std::endl;
   for (auto&& it : il)
   {
      std::cout << "    drawtype = " << (unsigned int)(it.drawType()) << std::endl;
      if (it.axisValid())
      {
         std::cout << "    axis = " << it.axis().x() << " " << it.axis().y() << " " << it.axis().z() << std::endl;
      }
      if (it.colorValid())
      {
         std::cout << "    color = " << it.color() << std::endl;
      }
      if (it.drawSizeValid())
      {
         std::cout << "    drawsize = " << (unsigned int)(it.drawSize()) << std::endl;
      }
      if (it.drawStyleValid())
      {
         std::cout << "    drawstyle = " << (unsigned int)(it.drawStyle()) << std::endl;
      }
      if (it.drawStyle2Valid())
      {
         std::cout << "    drawstyle2 = " << (unsigned int)(it.drawStyle2()) << std::endl;
      }
      if (it.durationValid())
      {
         std::cout << "    duration = " << it.duration() << std::endl;
      }
      if (it.idValid())
      {
         std::cout << "    id = " << it.id() << std::endl;
      }
      if (it.orientationValid())
      {
         std::cout << "    orientation = " << it.orientation().x() << " " << it.orientation().y() << " " <<
   it.orientation().z() << std::endl;
      }
      if (it.textValid())
      {
         std::cout << "    text = " << it.text() << std::endl;
      }
      if (it.vertex1Valid())
      {
         std::cout << "    Vertex1: type = " << (unsigned int)(it.vertex1().type()) << std::endl;
         if (it.vertex1().relativePlatformIndexValid())
         {
            std::cout << "      platform index = " << it.vertex1().relativePlatformIndex() << std::endl;
         }
         if (it.vertex1().xyzValid())
         {
            std::cout << "      xyz = " << it.vertex1().xyz().x() << " " << it.vertex1().xyz().y() << " " <<
   it.vertex1().xyz().z() << std::endl;
         }
      }
      if (it.vertex2Valid())
      {
         std::cout << "    Vertex2: type = " << (unsigned int)(it.vertex2().type()) << std::endl;
         if (it.vertex2().relativePlatformIndexValid())
         {
            std::cout << "      platform index = " << it.vertex2().relativePlatformIndex() << std::endl;
         }
         if (it.vertex2().xyzValid())
         {
            std::cout << "      xyz = " << it.vertex2().xyz().x() << " " << it.vertex2().xyz().y() << " " <<
   it.vertex2().xyz().z() << std::endl;
         }
      }
      if (it.vertex3Valid())
      {
         std::cout << "    Vertex3: type = " << (unsigned int)(it.vertex3().type()) << std::endl;
         if (it.vertex3().relativePlatformIndexValid())
         {
            std::cout << "      platform index = " << it.vertex3().relativePlatformIndex() << std::endl;
         }
         if (it.vertex3().xyzValid())
         {
            std::cout << "      xyz = " << it.vertex3().xyz().x() << " " << it.vertex3().xyz().y() << " " <<
   it.vertex3().xyz().z() << std::endl;
         }
      }
      if (it.vertex4Valid())
      {
         std::cout << "    Vertex4: type = " << (unsigned int)(it.vertex4().type()) << std::endl;
         if (it.vertex4().relativePlatformIndexValid())
         {
            std::cout << "      platform index = " << it.vertex4().relativePlatformIndex() << std::endl;
         }
         if (it.vertex4().xyzValid())
         {
            std::cout << "      xyz = " << it.vertex4().xyz().x() << " " << it.vertex4().xyz().y() << " " <<
   it.vertex4().xyz().z() << std::endl;
         }
      }
   }*/
