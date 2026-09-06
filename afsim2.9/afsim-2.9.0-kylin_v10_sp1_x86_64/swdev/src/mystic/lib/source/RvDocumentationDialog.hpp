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

#ifndef WKDOCUMENTATIONDIALOG_HPP
#define WKDOCUMENTATIONDIALOG_HPP

#include "WkfDocumentationDialog.hpp"

namespace rv
{
class DocumentationDialog : public wkf::DocumentationDialog
{
public:
   DocumentationDialog(wkf::MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags());

protected:
   void PopulateDialog() override;
};
} // namespace rv
#endif