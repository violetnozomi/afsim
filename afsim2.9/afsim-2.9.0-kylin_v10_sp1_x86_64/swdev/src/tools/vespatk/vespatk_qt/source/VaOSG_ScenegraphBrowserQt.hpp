// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VaOSG_ScenegraphBrowserQt_hpp
#define VaOSG_ScenegraphBrowserQt_hpp

#include "vespatk_qt_export.h"

#include <vector>

#include <QDialog>
#include <QStandardItemModel>
#include <osg/Observer>

#include "VaCallbackHolder.hpp"
#include "VaObject.hpp"

class QTreeView;

namespace osg
{
class Node;
}

namespace vespa
{
class VaViewer;

class VESPATK_QT_EXPORT VaOSG_ScenegraphItem : public QStandardItem, public osg::Observer
{
public:
   VaOSG_ScenegraphItem(osg::Node* aNodePtr);
   ~VaOSG_ScenegraphItem() override;

   void objectDeleted(void* aObjectPtr) override;

   osg::Node* GetNode() { return mNodePtr; }
   bool       IsHoverable() { return mHoverable; }
   void       SetHoverable(bool aState) { mHoverable = aState; }

private:
   osg::Node* mNodePtr;
   bool       mHoverable;
};

class VESPATK_QT_EXPORT VaOSG_ScenegraphTreeModel : public QStandardItemModel
{
public:
   ~VaOSG_ScenegraphTreeModel() override = default;

   void SetNode(osg::Node* aRootNode, QStandardItem* aParentPtr = nullptr, bool aHoverable = true);
};

class VESPATK_QT_EXPORT VaOSG_ScenegraphBrowserQt : public QDialog
{
public:
   static void InitializeSubscriptions();
   static void Shutdown();
   explicit VaOSG_ScenegraphBrowserQt(QWidget* parent = nullptr, VaViewer* aViewer = nullptr);
   ~VaOSG_ScenegraphBrowserQt() override;

   virtual void Show();

   virtual void HoverCB(const QModelIndex& aIndex);
   virtual void TreeViewMenuCB(const QPoint& aPoint);

protected:
private:
   std::vector<osg::Node*> mHighlightList;

   void CloseWindowCB();

   QTreeView*                 mTreeViewPtr;
   VaOSG_ScenegraphTreeModel* mTreeModel;

   static void PopupDialogCB(VaObject* aObjectPtr);

   static VaCallbackHolder mCallbacks;
};
} // namespace vespa

#endif
