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
#ifndef RVMEMORYPREFOBJECT_HPP
#define RVMEMORYPREFOBJECT_HPP

#include "WkfPrefObject.hpp"

namespace rv
{
struct MemoryPrefData
{
   unsigned int mPagesInCache{350};
};

class MemoryPrefObject : public wkf::PrefObjectT<MemoryPrefData>
{
public:
   static constexpr const char* cNAME = "MemoryOptions";

   MemoryPrefObject(QObject* parent = nullptr);
   ~MemoryPrefObject() override = default;

   unsigned int GetPagesInCache() const { return mCurrentPrefs.mPagesInCache; }

private:
   void Apply() override {}

   void SetPagesInCache(unsigned int aPages);

   MemoryPrefData ReadSettings(QSettings& aSettings) const override;
   void           SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace rv

Q_DECLARE_METATYPE(rv::MemoryPrefData)
#endif
