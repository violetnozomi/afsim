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

#ifndef POSITIONCONVERTER_HPP
#define POSITIONCONVERTER_HPP

#include "PositionConverterDialog.hpp"
#include "WkfPlugin.hpp"

namespace PositionConverter
{
class Plugin : public wkf::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

private:
   QPointer<Dialog> mDialog;
};
} // namespace PositionConverter
#endif // !POSITIONCONVERTER_HPP
