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
#ifndef RVPLUGINQUANTUMTASKERDATA_HPP
#define RVPLUGINQUANTUMTASKERDATA_HPP

#include "RvPlugin.hpp"

namespace RvQuantumTasker
{
class Dialog;

class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;

private slots:
   void ShowMatrixDialog();

private:
   QPointer<Dialog> mQuantumTaskerMatrixWidgetPtr;
};
} // namespace RvQuantumTasker
#endif
