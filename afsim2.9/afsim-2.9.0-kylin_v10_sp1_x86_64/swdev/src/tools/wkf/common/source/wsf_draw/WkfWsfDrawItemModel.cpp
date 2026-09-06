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

#include "WkfWsfDrawItemModel.hpp"

#include <QAction>
#include <QMenu>

#include "VaTimeManager.hpp"
#include "WkfEnvironment.hpp"
#include "WkfMainWindow.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "WkfWsfDrawPrefObject.hpp"

#define SHOW_SUB_DRAWINGS // tie this to a preference

wkf::WsfDrawItemModel::WsfDrawItemModel(QObject* aParent, std::unordered_map<vespa::VaViewer*, LayerMap>& aViewerMap)
   : QAbstractItemModel(aParent)
   , mViewerMap(aViewerMap)
{
   wkfEnv.RegisterPrefObject(new WsfDrawPrefObject(this)); // create and register the pref object
   QMenu* devMenuPtr = wkfEnv.GetMainWindow()->GetMenuByName("Developer");
   if (devMenuPtr)
   {
      wkf::Action* extendedPtr = new wkf::Action("Extended Draw Browser", wkfEnv.GetMainWindow());
      extendedPtr->setCheckable(true);
      extendedPtr->setChecked(ExtendedBrowserEnabled());
      connect(extendedPtr, &QAction::toggled, this, &wkf::WsfDrawItemModel::ExtendedBrowserToggled);
      devMenuPtr->addAction(extendedPtr);

      // CATCH STATE CHANGES
      connect(wkfEnv.GetPreferenceObject<wkf::WsfDrawPrefObject>(),
              &wkf::WsfDrawPrefObject::ExtendedBrowserChanged,
              [extendedPtr, this](bool aState)
              {
                 extendedPtr->setChecked(aState);
                 ExtendedBrowserToggled(aState);
              });
   }
}

void wkf::WsfDrawItemModel::HandleNewLayer(const QString& aName)
{
   beginInsertRows(QModelIndex(), mLayers.size(), mLayers.size());
   mLayers.emplace_back(aName);
   endInsertRows();
   QModelIndex idx = index(mLayers.size() - 1, 0);
   dataChanged(idx, idx, QVector<int>{Qt::CheckStateRole});
}

void wkf::WsfDrawItemModel::HandleClearLayers()
{
   beginResetModel();
   mLayers.clear();
   endResetModel();
}

void wkf::WsfDrawItemModel::ElementAddedCB(const QString& aLayer, unsigned int aIndex)
{
   if (mExtendedBrowser)
   {
      auto it = std::find(mLayers.begin(), mLayers.end(), aLayer);
      if (it != mLayers.end())
      {
         int row = it - mLayers.begin();

         beginInsertRows(index(row, 0), aIndex, aIndex);
         endInsertRows();
      }
   }
}

void wkf::WsfDrawItemModel::ElementActivatedCB(const QString& aLayer, unsigned int aIndex, bool aState)
{
   if (mExtendedBrowser)
   {
      auto it = std::find(mLayers.begin(), mLayers.end(), aLayer);
      if (it != mLayers.end())
      {
         int row = it - mLayers.begin();

         dataChanged(index(aIndex, 0, index(row, 0)), index(aIndex, 0, index(row, 0)));
      }
   }
}

wkf::OverlayWsfDraw* wkf::WsfDrawItemModel::GetLayer(const QModelIndex& aIndex) const
{
   if (aIndex.row() < ut::cast_to_int(mLayers.size()))
   {
      QString    name = mLayers.at(aIndex.row());
      const auto vit  = mViewerMap.find(vaEnv.GetStandardViewer());
      if (vit != mViewerMap.end())
      {
         const auto oit = vit->second.find(name);
         if (oit != vit->second.end())
         {
            return oit->second;
         }
      }
   }
   return nullptr;
}

int wkf::WsfDrawItemModel::rowCount(const QModelIndex& aParent) const
{
   if (aParent.isValid())
   {
      if (mExtendedBrowser)
      {
         if (!aParent.parent().isValid()) // its parent is top level
         {
            wkf::OverlayWsfDraw* layer = GetLayer(aParent);
            if (layer)
            {
               return layer->GetElements().size();
            }
         }
      }
   }
   else
   {
      return mLayers.size();
   }
   return 0;
}

QVariant wkf::WsfDrawItemModel::data(const QModelIndex& aIndex, int aRole) const
{
   if (aRole == Qt::DisplayRole)
   {
      if (aIndex.parent().isValid())
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex.parent());
         if (layer)
         {
            const wkf::OverlayWsfDraw::Element& element   = layer->GetElements().at(aIndex.row());
            QString                             hexstring = QString::number(element.mColor, 16).toUpper();
            while (hexstring.length() < 8)
            {
               hexstring = '0' + hexstring;
            }
            QString label = QString::number(element.mID) + ": " + element.GetType();
            return label;
         }
      }
      else
      {
         if (ut::cast_to_int(mLayers.size()) > aIndex.row())
         {
            return mLayers.at(aIndex.row());
         }
      }
   }
   else if (aRole == Qt::CheckStateRole)
   {
      if (aIndex.parent().isValid())
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex.parent());
         if (layer)
         {
            if (flags(aIndex) & Qt::ItemIsEnabled)
            {
               const wkf::OverlayWsfDraw::Element& element = layer->GetElements().at(aIndex.row());
               double                              time    = vaEnv.GetTime()->GetCurrentTime();
               if (element.IsDisplayable(time))
               {
                  if (layer->IsVisible())
                  {
                     return layer->IsActive(aIndex.row()) ? Qt::Checked : Qt::Unchecked;
                  }
               }
            }
         }
      }
      else
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex);
         if (layer)
         {
            return layer->IsVisible() ? Qt::Checked : Qt::Unchecked;
         }
      }
   }
   else if (aRole == Qt::UserRole)
   {
      if (aIndex.parent().isValid())
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex.parent());
         if (layer)
         {
            const wkf::OverlayWsfDraw::Element& element = layer->GetElements().at(aIndex.row());
            unsigned int                        color   = element.mColor;
            unsigned char                       r       = static_cast<unsigned char>(color >> 24);
            unsigned char                       g       = static_cast<unsigned char>(color >> 16);
            unsigned char                       b       = static_cast<unsigned char>(color >> 8);
            return QColor(r, g, b);
         }
      }
   }
   return QVariant();
}

bool wkf::WsfDrawItemModel::setData(const QModelIndex& aIndex, const QVariant& aValue, int aRole)
{
   if (aRole == Qt::CheckStateRole)
   {
      if (!aIndex.parent().isValid())
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex);
         if (layer)
         {
            layer->SetVisible(aValue == Qt::Checked);
            dataChanged(aIndex, aIndex, QVector<int>{aRole});
            dataChanged(index(0, 0, aIndex),
                        index(rowCount(aIndex), 0, aIndex),
                        QVector<int>{aRole}); // update children to remove their checkboxes
            return true;
         }
      }
      else
      {
         wkf::OverlayWsfDraw* layer = GetLayer(aIndex.parent());
         if (layer)
         {
            layer->ActivateElement(aIndex.row(), aValue == Qt::Checked);
            dataChanged(aIndex, aIndex, QVector<int>{aRole});
         }
      }
   }
   return QAbstractItemModel::setData(aIndex, aValue, aRole);
}

QModelIndex wkf::WsfDrawItemModel::index(int aRow, int aColumn, const QModelIndex& aParent) const
{
   // this is using the id argument to point to the parents' rows.
   // this is used in parent() to find the way from an index to the parent's index
   // since this structure only has two depths (the layer and the commands) this is a safe way to
   // identify where a node is in the structure
   // if this is at root level, the parent will be made with QModelIndex() which sets the row to -1
   return createIndex(aRow, aColumn, aParent.row());
}

QModelIndex wkf::WsfDrawItemModel::parent(const QModelIndex& aIndex) const
{
   // as stated, the internalId is holding the row of the parent.
   // an un-parented node will have -1 stored in the field
   if (aIndex.internalId() != static_cast<unsigned int>(-1))
   {
      return index(aIndex.internalId(), 0);
   }
   return QModelIndex();
}

Qt::ItemFlags wkf::WsfDrawItemModel::flags(const QModelIndex& aIndex) const
{
   if (!aIndex.parent().isValid()) // top level
   {
      return (QAbstractItemModel::flags(aIndex) | Qt::ItemIsUserCheckable);
   }
   else
   {
      wkf::OverlayWsfDraw* layer = GetLayer(aIndex.parent());
      if (layer)
      {
         const wkf::OverlayWsfDraw::Element& element = layer->GetElements().at(aIndex.row());
         double                              time    = vaEnv.GetTime()->GetCurrentTime();
         if (element.IsDisplayable(time))
         {
            return QAbstractItemModel::flags(aIndex) | Qt::ItemIsUserCheckable;
         }
         else
         {
            return QAbstractItemModel::flags(aIndex) & ~Qt::ItemIsEnabled;
         }
      }
   }

   return QAbstractItemModel::flags(aIndex);
}

void wkf::WsfDrawItemModel::ExtendedBrowserToggled(bool aState)
{
   beginResetModel();
   mExtendedBrowser = aState;
   endResetModel();
   wkfEnv.GetPreferenceObject<wkf::WsfDrawPrefObject>()->SetExtendedBrowserEnabled(aState);
}
