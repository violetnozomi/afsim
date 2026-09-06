// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RV_DISTRIBUTED_DIALOG_HPP
#define RV_DISTRIBUTED_DIALOG_HPP

#include <QDialog>
#include <QProgressDialog>

#include "ui_RvMergeDialog.h"

namespace RvMerger
{
class MergeDialog : public QDialog
{
   Q_OBJECT
public:
   MergeDialog(QDialog* aParent);
   ~MergeDialog() noexcept override = default;

   void Reset() noexcept;
   void Abort() noexcept;

signals:

   void Merge(const QStringList& aInputs, const QString& aOutput);
   void MergeCanceled();

public slots:

   void MergeFinished();

private:
   void Accept();

   Ui::RvMergeDialog mUi;
   QProgressDialog   mProgressBar;
};
} // namespace RvMerger

#endif
