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
#include "VaOSG_ScenegraphBrowserQt.hpp"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QMenu>
#include <QTextEdit>
#include <QTreeView>
#include <osg/Group>
#include <osgDB/WriteFile>

#include "UtQt.hpp"
#include "UtoViewer.hpp"
#include "VaCallbackHolder.hpp"
#include "VaEnvironment.hpp"
#include "VaObserver.hpp"
#include "VaViewer.hpp"
#include "VaViewerManager.hpp"

namespace vespa
{

VaCallbackHolder VaOSG_ScenegraphBrowserQt::mCallbacks;

// static
void VaOSG_ScenegraphBrowserQt::InitializeSubscriptions()
{
   mCallbacks.Add(VaObserver::PopupDialog.Connect(&VaOSG_ScenegraphBrowserQt::PopupDialogCB, "VaOSG_ScenegraphBrowserQt"));
}

// static
void VaOSG_ScenegraphBrowserQt::Shutdown()
{
   mCallbacks.Clear();
}

VaOSG_ScenegraphBrowserQt::VaOSG_ScenegraphBrowserQt(QWidget* parent /* = 0 */, VaViewer* aViewer)
   : QDialog(parent)
{
   QHBoxLayout* layout = new QHBoxLayout();
   setLayout(layout);
   mTreeViewPtr = new QTreeView(this);
   layout->addWidget(mTreeViewPtr);
   mTreeModel = new VaOSG_ScenegraphTreeModel;
   mTreeViewPtr->setModel(mTreeModel);

   if (nullptr != aViewer)
   {
      mTreeModel->SetNode(aViewer->GetViewer()->GetOSGViewer()->getCamera());
   }
   mTreeViewPtr->header()->setSectionResizeMode(QHeaderView::Stretch);

   Qt::WindowFlags flags = windowFlags() | Qt::WindowStaysOnTopHint;
   setWindowFlags(flags);
   setAttribute(Qt::WA_DeleteOnClose, true);

   mTreeViewPtr->setMouseTracking(true);
   mTreeViewPtr->setContextMenuPolicy(Qt::CustomContextMenu);
   QObject::connect(mTreeViewPtr, &QTreeView::customContextMenuRequested, this, &VaOSG_ScenegraphBrowserQt::TreeViewMenuCB);
   QObject::connect(mTreeViewPtr, &QTreeView::entered, this, &VaOSG_ScenegraphBrowserQt::HoverCB);
}

VaOSG_ScenegraphBrowserQt::~VaOSG_ScenegraphBrowserQt()
{
   delete mTreeModel;
}

void VaOSG_ScenegraphBrowserQt::Show()
{
   if (isVisible())
   {
      raise();
   }
   else
   {
      show();
   }
}

void VaOSG_ScenegraphBrowserQt::PopupDialogCB(VaObject* aObjectPtr)
{
   VaViewer* viewer = dynamic_cast<VaViewer*>(aObjectPtr);
   if (viewer == nullptr)
   {
      viewer = VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
   }
   VaOSG_ScenegraphBrowserQt* dialogPtr = new VaOSG_ScenegraphBrowserQt(nullptr, viewer);

   dialogPtr->Show();
}

void VaOSG_ScenegraphBrowserQt::CloseWindowCB()
{
   QDialog::hide();
}

void VaOSG_ScenegraphBrowserQt::HoverCB(const QModelIndex& aIndex)
{
   /*   VaViewer* viewer = VaEnvironment::Instance().GetViewerManager()->CurrentViewer();
      if (0 != viewer)
      {
         if (!mHighlightList.empty())
         {
            std::vector<osg::Node*>::iterator it;
            for (it = mHighlightList.begin(); it != mHighlightList.end(); ++it)
            {
               viewer->HighlightShape(*it, false);
            }
            mHighlightList.clear();
         }

         QStandardItem* item = mTreeModel->itemFromIndex(aIndex);
         if (0 != item)
         {
            int r = item->row();
            QStandardItem* p = item->parent();
            if (0 != p)
            {
               VaOSG_ScenegraphItem* sgItem = (VaOSG_ScenegraphItem*)(p->child(r, 0));

               if (sgItem->IsHoverable())
               {
                  osg::Node* node = sgItem->GetNode();
                  if (0 != node)
                  {
                     viewer->HighlightShape(node, true, UtColor(0.5, 0.0, 0.7));
                     mHighlightList.push_back(node);
                  }
               }
            }
         }
      }*/
}

void VaOSG_ScenegraphTreeModel::SetNode(osg::Node* aNode, QStandardItem* aParentPtr, bool aHoverable)
{
   bool           hoverable      = aHoverable;
   bool           childhoverable = aHoverable;
   QStandardItem* parent         = aParentPtr;
   if (nullptr == parent)
   {
      parent    = invisibleRootItem();
      hoverable = false;
   }
   VaOSG_ScenegraphItem* typeItem = new VaOSG_ScenegraphItem(aNode);
   typeItem->setData(aNode->className(), Qt::DisplayRole);
   if (std::string(aNode->className()) == "Camera")
   {
      hoverable      = false;
      childhoverable = false;
   }
   typeItem->SetHoverable(hoverable);
   QStandardItem* nameItem = new QStandardItem();
   nameItem->setData(aNode->getName().c_str(), Qt::DisplayRole);
   if (aNode->getNumParents() > 1)
   {
      typeItem->setData(QColor(255, 0, 0), Qt::ForegroundRole);
      nameItem->setData(QColor(255, 0, 0), Qt::ForegroundRole);
   }
   QList<QStandardItem*> itemList;
   itemList.push_back(typeItem);
   itemList.push_back(nameItem);
   parent->appendRow(itemList);

   osg::Group* group = aNode->asGroup();
   if (group != nullptr)
   {
      int children = group->getNumChildren();
      for (int i = 0; i < children; ++i)
      {
         osg::Node* child = group->getChild(i);
         SetNode(child, typeItem, childhoverable);
      }
   }
}

VaOSG_ScenegraphItem::VaOSG_ScenegraphItem(osg::Node* aNodePtr)
{
   aNodePtr->addObserver(this);
   mNodePtr = aNodePtr;
}

VaOSG_ScenegraphItem::~VaOSG_ScenegraphItem()
{
   if (nullptr != mNodePtr)
   {
      mNodePtr->removeObserver(this);
   }
}

void VaOSG_ScenegraphItem::objectDeleted(void* aObjectPtr)
{
   mNodePtr            = nullptr;
   int            r    = row();
   QStandardItem* p    = parent();
   int            cols = p->columnCount();
   for (int i = 0; i < cols; ++i)
   {
      QStandardItem* child = p->child(r, i);
      if (child != nullptr)
      {
         child->setData(QColor(128, 128, 128), Qt::ForegroundRole);
      }
   }
}

void VaOSG_ScenegraphBrowserQt::TreeViewMenuCB(const QPoint& aPoint)
{
   QModelIndex idx = mTreeViewPtr->indexAt(aPoint);
   if (idx.isValid())
   {
      QModelIndex    realIdx = idx.sibling(idx.row(), 0);
      QStandardItem* item    = mTreeModel->itemFromIndex(realIdx); // 1. lookup the item under the point

      QMenu                 myMenu(mTreeViewPtr);
      VaOSG_ScenegraphItem* sgItem = (VaOSG_ScenegraphItem*)(item);
      osg::Node*            node   = sgItem->GetNode();

      if (node != nullptr)
      {
         myMenu.addAction("Show data");
      }

      if (myMenu.actions().size() > 0)
      {
         QAction* selectedItem = myMenu.exec(mTreeViewPtr->mapToGlobal(aPoint));
         if (selectedItem)
         {
            if (selectedItem->text() == "Show data")
            {
               osgDB::writeNodeFile(*node, "foo.osg");
               std::ifstream file("foo.osg");
               if (!file)
               {
                  return;
               }

               std::string fileString;
               std::string line;
               while (std::getline(file, line))
               {
                  fileString += line;
                  fileString += "\n";
               }
               QTextEdit* textedit = findChild<QTextEdit*>("textOutput");
               if (nullptr == textedit)
               {
                  textedit = new QTextEdit(this);
                  textedit->setObjectName("textOutput");
                  layout()->addWidget(textedit);
               }
               textedit->setText(fileString.c_str());
            }
         }
      }
   }
}

} // end namespace vespa
