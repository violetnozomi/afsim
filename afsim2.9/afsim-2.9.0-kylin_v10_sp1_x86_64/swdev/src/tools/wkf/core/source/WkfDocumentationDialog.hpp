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

#ifndef WKFDOCUMENTATIONDIALOG_HPP
#define WKFDOCUMENTATIONDIALOG_HPP

#include "wkf_export.h"

#include <QVBoxLayout>

#include "WkfDialog.hpp"
#include "ui_WkfDocumentationDialog.h"

namespace wkf
{
class MainWindow;

class WKF_EXPORT DocumentationDialog : public Dialog
{
public:
   DocumentationDialog(MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags());
   ~DocumentationDialog() override = default;

protected:
   virtual void PopulateDialog();

   void         CreateDocLink(const QString& link, const QString& text, QLayout* layout);
   QVBoxLayout* GetLayout() const;

private:
   void showEvent(QShowEvent* aEvent) override;

   Ui::WkfDocumentationDialog mUi;

   bool mIsPopulated;
};
} // namespace wkf
#endif
