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

#include "ModelBrowserWidget.hpp"

#include <QCompleter>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextStream>
#include <osg/Node>
#include <osgDB/ReadFile>

#include "UtInput.hpp"
#include "UtRunEnvManager.hpp"
#include "VaModelDatabase.hpp"
#include "VaModelViewer.hpp"
#include "VaModelVisitor.hpp"
#include "VaOSG_ScenegraphBrowserQt.hpp"
#include "VaObserver.hpp"
#include "VaWidget.hpp"
#include "WkfEnvironment.hpp"
#include "WkfVtkEnvironment.hpp"

#ifdef GetMessage
#undef GetMessage
#endif

ModelBrowser::Widget::Widget(QWidget* aParent)
   : QWidget(aParent)
   , mModel(nullptr)
   , mNode(nullptr)
   , mHighlightNode(nullptr)
   , mGridMode("XY")
   , mOffsetX(0.0)
   , mOffsetY(0.0)
   , mOffsetZ(0.0)
   , mTemporaryDef("", nullptr)
{
   mUi.setupUi(this);
   BuildModelAndSetsLists();
   mUi.mSets->setCurrentText("DEFAULT");

   vespa::VaWidget* widget = new vespa::VaWidget(vespa::VaViewer::ModelViewer, vespa::VaViewer::SmallFeatureCulling, this);
   mUi.mViewWidget->setLayout(new QVBoxLayout(mUi.mViewWidget));
   mUi.mViewWidget->layout()->setMargin(0);
   mUi.mViewWidget->layout()->addWidget(widget);
   mViewerPtr = dynamic_cast<vespa::VaModelViewer*>(widget->GetViewer());
   if (mViewerPtr->GetViewer())
   {
      ViewerInitialized();
   }
   else
   {
      connect(widget, &vespa::VaWidget::Initialized, this, &Widget::ViewerInitialized);
   }
   mCallbacks.Add(vespa::VaObserver::ViewerDestroyed.Connect(&Widget::ViewerDestroyedCB, this));
}

ModelBrowser::Widget::~Widget()
{
   ClearTemporaryDef();
   if (mViewerPtr)
   {
      mViewerPtr->ClearRawModel();
      mNode = nullptr;
   }
   else
   {
      mNode.release();
   }
   delete mModel;
}

void ModelBrowser::Widget::ShowModel(const QString& aName)
{
   QString model = mUi.mModels->currentText();
   QString set   = CurrentSet();
   ClearTemporaryDef();
   mUi.mModelGraph->setModel(nullptr);
   delete mModel;
   if (mHighlightNode)
   {
      mViewerPtr->HighlightShape(mHighlightNode, false);
      mHighlightNode = nullptr;
   }
   mViewerPtr->ClearRawModel();
   mNode = nullptr;
   mNode = vaEnv.GetModelDatabase()->RequestSharedModel(model.toStdString(), set.toStdString());

   mViewerPtr->ShowRawModel(mNode);
   mModel = new vespa::VaOSG_ScenegraphTreeModel;
   mModel->SetNode(mNode, nullptr, false);
   mUi.mModelGraph->setModel(mModel);

   const vespa::VaModelDefinition* def = vaEnv.GetModelDatabase()->GetModelDefinition(model.toStdString());
   if (def)
   {
      mSourcePath = def->GetDefinitionSourceFile(set.toStdString());
      mSourcePath.Up();

      std::string               defFile  = def->GetDefinitionSourceFile(set.toStdString());
      std::pair<size_t, size_t> defRange = def->GetDefinitionRange(set.toStdString());
      QFile                     file(defFile.c_str());
      file.open(QIODevice::ReadOnly | QFile::Text);
      size_t f = (defRange.first > 0) ? defRange.first - 1 : 0;
      if (file.isOpen())
      {
         QTextStream ts(&file);
         QString     text = ts.readAll();
         mUi.mDefinition->setText(text.mid(f, defRange.second - f - 1));
         file.close();
      }
      mOverwriteRange = std::make_pair(f, defRange.second - 1);
      mUi.mDefinition->setReadOnly(def->IsReadOnly(set.toStdString()));
      mUi.mPreview->setEnabled(!def->IsReadOnly(set.toStdString()));
      mUi.mApply->setEnabled(false);
   }
}

void ModelBrowser::Widget::LoadModel()
{
   QString file = wkf::getOpenFileName(this, "Open a model file", QString(), "*.*");
   if (!file.isEmpty())
   {
      ClearTemporaryDef();
      osg::Node* node = osgDB::readNodeFile(file.toStdString());
      if (node)
      {
         mSourcePath = UtRunEnvManager::GetCmeRelBase() + "/site";
         mUi.mModelGraph->setModel(nullptr);
         delete mModel;
         if (mHighlightNode)
         {
            mViewerPtr->HighlightShape(mHighlightNode, false);
            mHighlightNode = nullptr;
         }
         mViewerPtr->ClearRawModel();
         mNode = nullptr;
         mNode = node;

         osg::Matrix matrix;
         vaEnv.GetModelDatabase()->TreatNode(mNode, matrix, 1.0, false, true);

         mViewerPtr->ShowRawModel(mNode);
         mModel = new vespa::VaOSG_ScenegraphTreeModel;
         mModel->SetNode(mNode, nullptr, false);
         mUi.mModelGraph->setModel(mModel);
         QString shortname = UtPath(file.toStdString()).GetFileName(false).c_str();
         mUi.mDefinition->setText("model " + shortname + "\n   filename " + file + "\nend_model\n");

         mOverwriteRange = std::make_pair(0, 0);
         mUi.mDefinition->setReadOnly(false);
         mUi.mPreview->setEnabled(true);
         mUi.mApply->setEnabled(false);
      }
      else
      {
         QMessageBox::warning(this, "Error", "Error could not open " + file);
      }
   }
}

void ModelBrowser::Widget::NodeClicked(const QModelIndex& aIndex)
{
   QStandardItem* item = mModel->itemFromIndex(aIndex);
   if (nullptr != item)
   {
      int            r = item->row();
      QStandardItem* p = item->parent();
      if (nullptr != p)
      {
         vespa::VaOSG_ScenegraphItem* sgItem = (vespa::VaOSG_ScenegraphItem*)(p->child(r, 0));

         if (mHighlightNode)
         {
            mViewerPtr->HighlightShape(mHighlightNode, false);
            mHighlightNode = nullptr;
         }
         osg::Node* node = sgItem->GetNode();
         if (nullptr != node)
         {
            mViewerPtr->HighlightShape(node, true, UtColor(1.0, 0.5, 0.7));
            mHighlightNode = node;
         }
      }
   }
}

void ModelBrowser::Widget::UpdateGridMode(const QString& aString)
{
   mGridMode = aString;
   UpdateGrid(mGridMode, mOffsetX, mOffsetY, mOffsetZ);
}

void ModelBrowser::Widget::UpdateGridOffsetX(double aValue)
{
   mOffsetX = aValue;
   UpdateGrid(mGridMode, mOffsetX, mOffsetY, mOffsetZ);
}

void ModelBrowser::Widget::UpdateGridOffsetY(double aValue)
{
   mOffsetY = aValue;
   UpdateGrid(mGridMode, mOffsetX, mOffsetY, mOffsetZ);
}

void ModelBrowser::Widget::UpdateGridOffsetZ(double aValue)
{
   mOffsetZ = aValue;
   UpdateGrid(mGridMode, mOffsetX, mOffsetY, mOffsetZ);
}

void ModelBrowser::Widget::UpdateGridState(bool aValue)
{
   if (!aValue)
   {
      mViewerPtr->HideGrid();
   }
   else
   {
      UpdateGrid(mGridMode, mOffsetX, mOffsetY, mOffsetZ);
   }
}

void ModelBrowser::Widget::UpdateGrid(const QString& aString, double aX, double aY, double aZ)
{
   if (aString == "XY")
   {
      mViewerPtr->UseGrid(aX, aY, aZ, vespa::VaModelViewer::XY);
   }
   else if (aString == "YZ")
   {
      mViewerPtr->UseGrid(aX, aY, aZ, vespa::VaModelViewer::YZ);
   }
   else if (aString == "XZ")
   {
      mViewerPtr->UseGrid(aX, aY, aZ, vespa::VaModelViewer::XZ);
   }
}

void ModelBrowser::Widget::PreviewDefinition()
{
   try
   {
      ClearTemporaryDef();

      QString defstr = mUi.mDefinition->toPlainText();
      mTemporaryDef  = vaEnv.GetModelDatabase()->LoadTemporaryDefinition(defstr.toStdString(), mSourcePath);

      mUi.mModelGraph->setModel(nullptr);
      delete mModel;
      if (mHighlightNode)
      {
         mViewerPtr->HighlightShape(mHighlightNode, false);
         mHighlightNode = nullptr;
      }
      mViewerPtr->ClearRawModel();
      std::set<std::string> sets = mTemporaryDef.second->GetSets();
      std::string           set  = CurrentSet().toStdString();
      if (sets.find(set) != sets.end())
      {
         mNode = mTemporaryDef.second->RequestSharedModel(CurrentSet().toStdString());
      }
      else
      {
         bool block = mUi.mSets->blockSignals(true);
         set        = *sets.begin();
         if (set == "")
         {
            mUi.mSets->setCurrentText("DEFAULT");
         }
         else
         {
            if (mUi.mSets->findText(set.c_str()) == -1)
            {
               mUi.mSets->addItem(set.c_str());
            }
            mUi.mSets->setCurrentText(set.c_str());
         }
         mUi.mSets->blockSignals(block);
         mNode = mTemporaryDef.second->RequestSharedModel(CurrentSet().toStdString());
      }

      mViewerPtr->ShowRawModel(mNode);
      mModel = new vespa::VaOSG_ScenegraphTreeModel;
      mModel->SetNode(mNode, nullptr, false);
      mUi.mModelGraph->setModel(mModel);

      mUi.mDefinition->setText(mTemporaryDef.second->GetDefinitionSourceFile(CurrentSet().toStdString()).c_str());
      mUi.mDefinition->setReadOnly(false);
      mUi.mPreview->setEnabled(true);
      mUi.mApply->setEnabled(true);
   }
   catch (UtInput::ExceptionBase& exception)
   {
      QMessageBox::warning(this,
                           "Error",
                           QString(exception.GetMessage().c_str()) + "\n             " +
                              exception.GetLocation().c_str() + "\n");
   }
   catch (std::exception& e)
   {
      QMessageBox::warning(this, "Error", QString("Exception thrown from model: ") + e.what());
      throw;
   }
}

void ModelBrowser::Widget::ApplyDefinition()
{
   QString selSet   = mUi.mSets->currentText();
   QString filename = QString(mSourcePath.GetSystemPath().c_str()) + "/models.txt";
   QFile   inFile(filename);
   inFile.open(QIODevice::ReadOnly | QFile::Text);
   if (inFile.isOpen())
   {
      QTextStream ts(&inFile);
      QString     text = ts.readAll();
      if ((mOverwriteRange.first == mOverwriteRange.second) && (mOverwriteRange.first == 0))
      {
         text.append(QString("\n\n") + mTemporaryDef.second->GetDefinitionSourceFile(selSet.toStdString()).c_str());
         // append our def to the end of the file
      }
      else
      {
         text.replace(mOverwriteRange.first,
                      mOverwriteRange.second - mOverwriteRange.first,
                      mTemporaryDef.second->GetDefinitionSourceFile(selSet.toStdString()).c_str());
      }
      inFile.close();
      QFile outFile(filename);
      outFile.open(QIODevice::WriteOnly | QFile::Text | QIODevice::Truncate);
      if (outFile.isOpen())
      {
         QTextStream os(&outFile);
         os << text;
         outFile.close();
         vaEnv.GetModelDatabase()->Reset();
         wkfEnv.LoadModelDatabase();
         // reset our local state, by:
         QString selName = mTemporaryDef.first.c_str();
         BuildModelAndSetsLists();
         mUi.mModels->setCurrentText(selName); // this calls ShowModel() to reset our temporary definition data
         mUi.mSets->setCurrentText(selSet);
         // picking the active model name
         ResetPreviewAndApply();
         return;
      }
   }
   else
   {
      QFile outFile(filename);
      if (!outFile.exists()) // if it exists, we shouldn't have got here, fail out
      {
         if (!QDir(mSourcePath.GetSystemPath().c_str()).exists())
         {
            QDir().mkdir(mSourcePath.GetSystemPath().c_str());
         }
         outFile.open(QIODevice::WriteOnly | QFile::Text | QIODevice::Truncate);
         if (outFile.isOpen())
         {
            QTextStream os(&outFile);
            os << mTemporaryDef.second->GetDefinitionSourceFile("").c_str();
            outFile.close();
            vaEnv.GetModelDatabase()->Reset();
            wkfEnv.LoadModelDatabase();
            // reset our local state, by:
            QString selName = mTemporaryDef.first.c_str();
            BuildModelAndSetsLists();
            mUi.mModels->setCurrentText(selName); // this calls ShowModel() to reset our temporary definition data
                                                  // picking the active model name
            mUi.mSets->setCurrentText(selSet);
            ResetPreviewAndApply();
            return;
         }
      }
   }
   QMessageBox::warning(this, "ERROR", "Could not write to model file: " + filename);
}

void ModelBrowser::Widget::ResetPreviewAndApply()
{
   if (!mUi.mDefinition->isReadOnly())
   {
      mUi.mPreview->setEnabled(true);
   }
   mUi.mApply->setEnabled(false);
}

void ModelBrowser::Widget::ClearTemporaryDef()
{
   if (mTemporaryDef.second)
   {
      delete mTemporaryDef.second;
      mTemporaryDef.second = nullptr;
      mTemporaryDef.first  = "";
   }
}

void ModelBrowser::Widget::ViewerInitialized()
{
   mUi.mGridAlignment->addItem("XY");
   mUi.mGridAlignment->addItem("XZ");
   mUi.mGridAlignment->addItem("YZ");
   mUi.mGridAlignment->setCurrentText("XY");
   connect(mUi.mGridAlignment, &QComboBox::currentTextChanged, this, &Widget::UpdateGridMode);
   connect(mUi.mGridBox, &QGroupBox::toggled, this, &Widget::UpdateGridState);
   connect(mUi.mX_Offset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Widget::UpdateGridOffsetX);
   connect(mUi.mY_Offset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Widget::UpdateGridOffsetY);
   connect(mUi.mZ_Offset, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &Widget::UpdateGridOffsetZ);
   connect(mUi.mModels, &QComboBox::currentTextChanged, this, &Widget::ShowModel);
   connect(mUi.mSets, &QComboBox::currentTextChanged, this, &Widget::ShowModel);
   connect(mUi.mLoadFromFile, &QPushButton::clicked, this, &Widget::LoadModel);
   mUi.mModels->setCurrentText("unknown");
   connect(mUi.mModelGraph, &QTreeView::clicked, this, &Widget::NodeClicked);
   connect(mUi.mPreview, &QPushButton::clicked, this, &Widget::PreviewDefinition);
   connect(mUi.mApply, &QPushButton::clicked, this, &Widget::ApplyDefinition);
   connect(mUi.mDefinition, &QTextEdit::textChanged, this, &Widget::ResetPreviewAndApply);

   mViewerPtr->UseGrid(0, 0, 0, vespa::VaModelViewer::XY);
}

void ModelBrowser::Widget::SceneGraphAction()
{
   vespa::VaObserver::PopupDialog[vespa::VaCallbackGroupId("VaOSG_ScenegraphBrowserQt")](mViewerPtr);
}

void ModelBrowser::Widget::BuildModelAndSetsLists()
{
   bool sigsBlocked =
      mUi.mModels->blockSignals(true); // we don't need to change any models until we are done with the process
   bool setsigsBlocked = mUi.mSets->blockSignals(true);
   mUi.mModels->clear();
   mUi.mSets->clear();
   std::set<std::string> setlist = vaEnv.GetModelDatabase()->GetSetList();
   QStringList           sl;
   sl.append("DEFAULT");
   for (auto&& set : setlist)
   {
      if (set != "")
      {
         sl.append(set.c_str());
      }
   }
   mUi.mSets->addItems(sl);
   std::set<std::string> namelist = vaEnv.GetModelDatabase()->GetNameList();
   QStringList           ml;
   for (auto&& it : namelist)
   {
      ml.append(it.c_str());
   }
   mUi.mModels->setEditable(true);
   mUi.mModels->addItems(ml);
   mUi.mModels->blockSignals(sigsBlocked);
   mUi.mSets->blockSignals(setsigsBlocked);

   QCompleter* completer = new QCompleter(ml, mUi.mModels);
   mUi.mModels->setCompleter(completer);
}

QString ModelBrowser::Widget::CurrentSet()
{
   QString set = mUi.mSets->currentText();
   if (set == "DEFAULT")
   {
      return "";
   }
   return set;
}

void ModelBrowser::Widget::ViewerDestroyedCB(vespa::VaViewer* aViewerPtr)
{
   if (mViewerPtr)
   {
      if (aViewerPtr->GetUniqueId() == mViewerPtr->GetUniqueId())
      {
         mViewerPtr = nullptr;
      }
   }
}
