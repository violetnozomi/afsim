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
#ifndef RVMETADATADIALOG_HPP
#define RVMETADATADIALOG_HPP

#include <QDialog>

#include "ui_RvMetaDataDialog.h"

namespace RvStatistics
{
class MetaDataDialog : public QDialog
{
public:
   MetaDataDialog(QWidget* aParent);
   ~MetaDataDialog() override = default;

   void SetData(const std::vector<QStringList>& aStringMap);

private:
   Ui::RvMetaDataDialog mUi;
};
} // namespace RvStatistics

#endif
