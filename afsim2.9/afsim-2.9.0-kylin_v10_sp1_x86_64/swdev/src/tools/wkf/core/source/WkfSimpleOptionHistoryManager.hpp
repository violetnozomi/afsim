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

#ifndef WKFSIMPLEOPTIONHISTORYMANAGER
#define WKFSIMPLEOPTIONHISTORYMANAGER

#include "wkf_export.h"

#include <list>
#include <map>

#include "VaCallbackHolder.hpp"
#include "WkfOptionHistoryManager.hpp"

class QString;

namespace wkf
{
class WKF_EXPORT SimpleOptionHistoryManager : public OptionHistoryManager
{
public:
   SimpleOptionHistoryManager(const QString& aPluginName, QObject* aPluginPtr);
   ~SimpleOptionHistoryManager() override = default;

   void UpdateOptionsRulesList(int aOptionId, bool aState, const wkf::PlatformGrouping& aCategory) override;
   void ApplyOptions(const PlatformList& aPlatformPtr) override;
   void Clear() override;

private:
   void ApplyDefaultOptions(QSettings& aSettings, const std::pair<const int, QString>& aIdName);
   void ReadRulesArray(QSettings& aSettings, const std::pair<const int, QString>& aPair);

   // Write the options rules out to QSettings
   void Save(QSettings& aSettings) override;
   // Read the options rules in from QSettings
   void Load(QSettings& aSettings) override;

   using Rule     = std::pair<wkf::PlatformGrouping, bool>;
   using RuleList = std::list<Rule>;
   std::map<int, RuleList> mOptionRules;
};
} // namespace wkf

#endif
