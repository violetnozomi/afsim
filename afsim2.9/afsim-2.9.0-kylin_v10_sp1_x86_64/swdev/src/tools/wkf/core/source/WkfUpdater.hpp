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

#ifndef WKFUPDATER_HPP
#define WKFUPDATER_HPP

#include "wkf_export.h"

#include <memory>

#include <QString>

namespace wkf
{
class WKF_EXPORT Updater
{
public:
   Updater(const QString& aUpdaterName, const std::string& aPlatformName);
   virtual ~Updater() = default;

   static constexpr const char* cMIME_IDENTIFIER = "cMimeIdentifier";

   // Return the value as a string
   virtual QString GetValueString() const = 0;

   // Return whether the data value is valid (i.e. data is not empty)
   bool IsValid() const { return mDataValid; }

   // Return name of the Updater
   const QString&        GetUpdaterName() const { return mName; }
   virtual const QString GetDisplayName() const { return GetUpdaterName() + " "; }

   virtual void PendingDelete() {}

protected:
   const std::string& GetPlatformOfInterest() const { return mPlatformOfInterest; }

   bool mDataValid{false};

private:
   QString     mName;
   std::string mPlatformOfInterest;
};

class WKF_EXPORT UnitlessValueUpdater : public Updater
{
public:
   UnitlessValueUpdater(const QString& aUpdaterName, const std::string& aPlatformName)
      : Updater(aUpdaterName, aPlatformName)
   {
   }

   QString GetValueString() const override;

   void SetPrecision(int aPrecision)
   {
      mPrecision = aPrecision;
      mFormat    = 'f';
   }

   // return the value in numerical form
   virtual double GetValueDouble() const;

protected:
   double mValue{0.0};
   int    mPrecision{6};
   char   mFormat{'g'};
};
} // namespace wkf

namespace std
{
// Specialization of default_delete used as deleter type for std::unique_ptr<wkf::Updater>
template<>
struct default_delete<wkf::Updater>
{
   void operator()(wkf::Updater* updater) const
   {
      updater->PendingDelete();
      delete updater;
   }
};
} // namespace std

namespace wkf
{
template<typename T, typename... Args>
std::unique_ptr<Updater> make_updater(Args&&... args)
{
   return std::unique_ptr<Updater>{new T{std::forward<Args>(args)...}};
}
} // namespace wkf

#endif
