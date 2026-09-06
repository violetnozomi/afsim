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

#ifndef RVABOUTDIALOG_HPP
#define RVABOUTDIALOG_HPP

#include "WkfAboutDialog.hpp"

namespace rv
{
class AboutDialog : public wkf::AboutDialog
{
public:
   AboutDialog(wkf::MainWindow* parent, Qt::WindowFlags f = Qt::WindowFlags());
   ~AboutDialog() override = default;
};
} // namespace rv

#endif // RVABOUTDIALOG_HPP