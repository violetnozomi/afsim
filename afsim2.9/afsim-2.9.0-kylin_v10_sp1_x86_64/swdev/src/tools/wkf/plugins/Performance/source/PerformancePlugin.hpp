// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef PERFORMANCEPLUGIN_HPP
#define PERFORMANCEPLUGIN_HPP

#include "PerformanceDialog.hpp"
#include "WkfPlugin.hpp"

namespace Performance
{
class Plugin : public wkf::Plugin
{
   Q_OBJECT
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

protected:
   void GuiUpdate() override;

private:
   QPointer<Dialog> mDialog;
};
} // namespace Performance
#endif
