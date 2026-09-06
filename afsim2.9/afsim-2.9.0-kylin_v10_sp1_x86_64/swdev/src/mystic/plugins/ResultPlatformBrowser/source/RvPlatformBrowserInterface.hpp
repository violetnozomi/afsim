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

#ifndef RVPLATFORMBROWSERINTERFACE_HPP
#define RVPLATFORMBROWSERINTERFACE_HPP

#include "WkfGeneralPrefObject.hpp"
#include "platform_browser/WkfPlatformBrowserInterface.hpp"

namespace rv
{
class MsgPlatformInfo;
class ResultData;
} // namespace rv

namespace RvPlatformBrowser
{
class Interface : public wkf::PlatformBrowserInterface
{
public:
   Interface();
   ~Interface() override = default;

   void Update();
   void ClearScenario();

   void ShowInactiveChanged(bool aState);

private:
};
} // namespace RvPlatformBrowser
#endif
