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

#include "RvBAT_Scene.hpp"

#include "RvBAT_Interface.hpp"
#include "RvBAT_MovableView.hpp"

RvBAT::Scene::Scene(const size_t aSceneId, const RvBAT::SceneType aSceneType, QObject* aParent)
   : QGraphicsScene(aParent)
   , mSceneType(aSceneType)
   , mId(aSceneId)
{
}

const QString RvBAT::Scene::GetTreeNameFromId(const size_t aId) const
{
   QString treeName;
   // Get to the interface through the parent view
   auto movableView = dynamic_cast<MovableView*>(parent());
   if (movableView != nullptr)
   {
      // If there is a tree with aId, return its name
      treeName = movableView->GetInterfacePtr()->GetTreeNameFromId(aId);
   }
   return treeName;
}
