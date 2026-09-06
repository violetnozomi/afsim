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

#ifndef WKFOPTIONHISTORYMANAGER
#define WKFOPTIONHISTORYMANAGER

#include <set>

#include <QObject>

class QSettings;

#include "wkf_export.h"

#include "VaCallbackHolder.hpp"
#include "WkfPlatform.hpp"

namespace wkf
{
class PlatformGrouping;

class WKF_EXPORT OptionHistoryManager : public QObject
{
   Q_OBJECT
public:
   OptionHistoryManager(const QString& mPluginName, QObject* aParent);
   ~OptionHistoryManager() override = default;

   virtual void UpdateOptionsRulesList(int aOptionId, bool aState, const wkf::PlatformGrouping& aCategory) = 0;
   virtual void ApplyOptions(const PlatformList& aPlatformsPtr)                                            = 0;
   virtual void Clear()                                                                                    = 0;
   int          AddOption(const QString& aName, int aParentType = -1);
   // The selected option is true. This differs from plugin option registration's initial checkstate in that it can be
   // used on startup prior to settings being loaded. The derived class is responsible for using the default options to
   // actually modify platform option checkstates
   void        DefaultOptionTrue(const QString& aName);
   QString     OptionName(int aOptionId);
   virtual int GetParentOption(int aOptionId) const { return -1; }

signals:
   void ResetOptionStates();
   void SetPlatformOptionState(int aOptionId, bool aState, wkf::Platform* aPlatform);

protected:
   QString                mPluginName;
   std::map<int, QString> mIdToString;
   std::map<QString, int> mStringToId;
   std::set<QString>      mDefaultOptions;

private:
   virtual void SetParentOption(int aType, int aParent) {}
   int          mMaxId;
   // Write the options rules out to QSettings
   virtual void Save(QSettings& aSettings) = 0;
   // Read the options rules in from QSettings
   virtual void Load(QSettings& aSettings) = 0;

   vespa::VaCallbackHolder mCallbacks;
};
} // namespace wkf

#endif
