// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVPLUGINBAT_HPP
#define RVPLUGINBAT_HPP

#include "RvBAT_DockWindow.hpp"
#include "RvPlugin.hpp"

namespace RvBAT
{
class Interface;
class Plugin : public rv::Plugin
{
   Q_OBJECT

public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);
   ~Plugin() override = default;

private slots:

   /** Reset(clear) the BAT window.
    * @param aFullReset full reset if true.
    */
   void ClearScenario(bool aFullReset) override;

   /** Update the GUI and check for theme changes. */
   void GuiUpdate() override;

private:
   PluginUiPointer<DockWindow> mDockWindowPtr; ///< Pointer to the DockWindow.
};
} // namespace RvBAT
#endif
