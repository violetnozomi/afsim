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

#ifndef RV_PLUGIN_DISTRIBUTED_HPP
#define RV_PLUGIN_DISTRIBUTED_HPP

#include "RvMergeDialog.hpp"
#include "RvMergerThread.hpp"
#include "RvPlugin.hpp"

namespace RvMerger
{
class Plugin : public rv::Plugin
{
   Q_OBJECT
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() noexcept override = default;

signals:

   void MergeFinished();

public slots:

   void BeginMerge(const QStringList& aInputs, const QString& aOutput) noexcept;
   void CancelMerge() noexcept;

private:
   MergeDialog  mDialog;
   MergerThread mMerger;
};
} // namespace RvMerger

#endif
