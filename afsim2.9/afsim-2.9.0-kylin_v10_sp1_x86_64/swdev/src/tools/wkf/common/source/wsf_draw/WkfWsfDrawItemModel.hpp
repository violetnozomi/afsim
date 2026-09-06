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

#ifndef WKFWSFDRAWITEMMODEL_HPP
#define WKFWSFDRAWITEMMODEL_HPP

#include "wkf_common_export.h"

#include <QAbstractItemModel>

#include "VaViewer.hpp"
#include "wsf_draw/WkfOverlayWsfDraw.hpp"

namespace wkf
{
class WKF_COMMON_EXPORT WsfDrawItemModel : public QAbstractItemModel
{
public:
   using LayerMap = std::map<QString, wkf::OverlayWsfDraw*>;

   WsfDrawItemModel(QObject* aParent, std::unordered_map<vespa::VaViewer*, LayerMap>& aViewerMap);
   virtual ~WsfDrawItemModel() = default;

   int           columnCount(const QModelIndex& aParent = QModelIndex()) const override { return 1; }
   int           rowCount(const QModelIndex& aParent = QModelIndex()) const override;
   QVariant      data(const QModelIndex& aIndex, int aRole) const override;
   bool          setData(const QModelIndex& aIndex, const QVariant& aValue, int aRole) override;
   QModelIndex   index(int aRow, int aColumn, const QModelIndex& aParent = QModelIndex()) const override;
   QModelIndex   parent(const QModelIndex& aIndex) const override;
   Qt::ItemFlags flags(const QModelIndex& aIndex) const override;

   void ExtendedBrowserToggled(bool aState);
   bool ExtendedBrowserEnabled() { return mExtendedBrowser; }

protected:
   void HandleNewLayer(const QString& aName);
   void HandleClearLayers();
   void ElementAddedCB(const QString& aLayer, unsigned int aIndex);
   void ElementActivatedCB(const QString& aLayer, unsigned int aIndex, bool aState);

private:
   wkf::OverlayWsfDraw*                            GetLayer(const QModelIndex& aIndex) const;
   std::unordered_map<vespa::VaViewer*, LayerMap>& mViewerMap;
   std::vector<QString>                            mLayers;
   bool                                            mExtendedBrowser{false};
};
} // namespace wkf

#endif
