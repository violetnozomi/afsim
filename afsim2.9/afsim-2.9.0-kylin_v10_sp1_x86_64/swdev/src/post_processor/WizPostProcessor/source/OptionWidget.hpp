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

#ifndef OPTIONWIDGET_HPP
#define OPTIONWIDGET_HPP

#include "ProxyInterface.hpp"
#include "ReportWidget.hpp"

namespace PostProcessor
{
// base class for all option widgets in the Report Dialog.
// These are created by the Type Widgets (e.g. EngagementType)
class OptionWidget : public ReportWidget
{
      Q_OBJECT
   public:
      explicit OptionWidget(const std::string& aName)
         : ReportWidget()
         , mName(aName)
      {
         mIsOption = true;
      }
      ~OptionWidget() override = default;
      std::string GetName() { return mName; }

      void emitWeaponAdded(const QVector<ProxyInterface::PlatformPart>& aWeapons) { emit WeaponAdded(aWeapons); }
      void emitWeaponRemoved(const QVector<ProxyInterface::PlatformPart>& aWeapons) { emit WeaponRemoved(aWeapons); }


   signals:
      void ReportTypePressed();
      void WeaponAdded(const QVector<ProxyInterface::PlatformPart>& aWeapons);
      void WeaponRemoved(const QVector<ProxyInterface::PlatformPart>& aWeapons);

   protected:
      std::string mName;

};
}
#endif