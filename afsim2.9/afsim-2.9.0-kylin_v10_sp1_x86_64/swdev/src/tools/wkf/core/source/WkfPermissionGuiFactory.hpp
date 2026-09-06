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

#ifndef WKF_PERMISSION_GUI_FACTORY_HPP
#define WKF_PERMISSION_GUI_FACTORY_HPP

#include <QWidget>

#include "UtReflectConstVisitor.hpp"
#include "WkfPermissionPrimitives.hpp"

namespace wkf
{
namespace permission
{
class WKF_EXPORT CategoryPage : public QWidget
{
   Q_OBJECT

public:
   CategoryPage(QWidget* aParent, QString aCategory) noexcept;

   const QString& GetCategory() const noexcept;

signals:
   void ApplyChanges();
   void RevertChanges();
   void RestoreDefaults();

private:
   const QString mCategory;
};

class WKF_EXPORT GuiFactory : public ut::reflect::ConstVisitor<GuiFactory, PermissionNode, QWidget*(CategoryPage&)>
{
public:
   GuiFactory() noexcept;

   QWidget* Fallback(PermissionNode& aNode, CategoryPage& aPage) const override;

   void SetupDefaultFactories() noexcept;

   CategoryPage* MakeCategoryPage(Category& aNode) const;

private:
   QWidget* MakeCheckBox(BoolPermission& aNode, CategoryPage& aPage) const;
   QWidget* MakeSpinBox(IntPermission& aNode, CategoryPage& aPage) const;
   QWidget* MakeLineEdit(StringPermission& aNode, CategoryPage& aPage) const;
   QWidget* MakeTextEdit(StringListPermission& aNode, CategoryPage& aPage) const;
   QWidget* MakeGroupBox(ObjectPermission& aNode, CategoryPage& aPage) const;
};
} // namespace permission
} // namespace wkf

#endif
