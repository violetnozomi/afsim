// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef MODELBROWSERWIDGET_HPP
#define MODELBROWSERWIDGET_HPP

#include <QWidget>
#include <osg/ref_ptr>

#include "VaModelDatabase.hpp"
#include "VaOSG_ScenegraphBrowserQt.hpp"
#include "ui_ModelBrowser.h"

namespace vespa
{
class VaModelViewer;
}

class QTextEdit;
class QTreeView;

namespace ModelBrowser
{
class Widget : public QWidget
{
public:
   Widget(QWidget* aParent);
   ~Widget() override;

private:
   void    ShowModel(const QString& aName);
   void    LoadModel();
   void    NodeClicked(const QModelIndex& aIndex);
   void    UpdateGridMode(const QString& aString);
   void    UpdateGridOffsetX(double aValue);
   void    UpdateGridOffsetY(double aValue);
   void    UpdateGridOffsetZ(double aValue);
   void    UpdateGridState(bool aValue);
   void    UpdateGrid(const QString& aString, double aX, double aY, double aZ);
   void    PreviewDefinition();
   void    ApplyDefinition();
   void    ResetPreviewAndApply();
   void    ClearTemporaryDef();
   void    ViewerInitialized();
   void    ViewerDestroyedCB(vespa::VaViewer* aViewerPtr);
   void    SceneGraphAction();
   void    BuildModelAndSetsLists();
   QString CurrentSet();

   vespa::VaModelViewer*                             mViewerPtr;
   vespa::VaOSG_ScenegraphTreeModel*                 mModel;
   osg::ref_ptr<osg::Node>                           mNode;
   osg::Node*                                        mHighlightNode;
   QString                                           mGridMode;
   double                                            mOffsetX;
   double                                            mOffsetY;
   double                                            mOffsetZ;
   Ui_ModelBrowserWidget                             mUi;
   std::pair<std::string, vespa::VaModelDefinition*> mTemporaryDef;
   UtPath                                            mSourcePath;
   std::pair<size_t, size_t>                         mOverwriteRange;
   vespa::VaCallbackHolder                           mCallbacks;
};
} // namespace ModelBrowser
#endif
