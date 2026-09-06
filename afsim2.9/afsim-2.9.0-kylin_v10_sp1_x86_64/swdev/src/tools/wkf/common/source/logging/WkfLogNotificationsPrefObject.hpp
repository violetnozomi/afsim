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

#ifndef WKF_LOG_NOTIFICATIONS_PREF_OBJECT_HPP
#define WKF_LOG_NOTIFICATIONS_PREF_OBJECT_HPP

#include "wkf_common_export.h"

#include "WkfPrefObject.hpp"

namespace wkf
{
namespace log
{
enum class NotificationLevel
{
   None    = 0,
   Fatal   = 0x1,
   Error   = 0x2,
   Warning = 0x4,

   FatalError = Fatal | Error,
   Everything = Fatal | Error | Warning
};

inline NotificationLevel operator|(NotificationLevel aLeft, NotificationLevel aRight) noexcept
{
   return static_cast<NotificationLevel>(static_cast<int>(aLeft) | static_cast<int>(aRight));
}

inline NotificationLevel operator&(NotificationLevel aLeft, NotificationLevel aRight) noexcept
{
   return static_cast<NotificationLevel>(static_cast<int>(aLeft) & static_cast<int>(aRight));
}

inline NotificationLevel operator~(NotificationLevel aValue) noexcept
{
   // Bit-and with Everything to zero out unused bits.
   return static_cast<NotificationLevel>(~static_cast<int>(aValue)) & NotificationLevel::Everything;
}

inline bool Overlaps(NotificationLevel aLeft, NotificationLevel aRight) noexcept
{
   return (aLeft & ~aRight) != NotificationLevel::None;
}

struct NotificationsPrefData final
{
   NotificationLevel mNotificationLevel = NotificationLevel::FatalError;
   //! mIgnoreNotifications is intentionally NOT stored in a QSettings file.
   NotificationLevel mIgnoreNotifications = NotificationLevel::None;
};

class WKF_COMMON_EXPORT NotificationsPrefObject final : public wkf::PrefObjectT<NotificationsPrefData>
{
   Q_OBJECT

public:
   static constexpr const char* cNAME = "LogOptions";

   NotificationsPrefObject(QObject* aParent = nullptr) noexcept;
   ~NotificationsPrefObject() noexcept override = default;

   NotificationLevel GetEffectiveNotificationLevel() const noexcept;

   void SetIgnore(NotificationLevel aLevel);

signals:
   void EffectiveNotificationLevelChanged(NotificationLevel aLevel);

private:
   // Initialized to true to ensure preferences are applied on startup.
   bool mEffectiveNotificationLevelChanged = true;

   void Apply() override;

   void                  SetPreferenceDataP(const NotificationsPrefData& aPrefData) override;
   NotificationsPrefData ReadSettings(QSettings& aSettings) const override;
   void                  SaveSettingsP(QSettings& aSettings) const override;
};
} // namespace log
} // namespace wkf

Q_DECLARE_METATYPE(wkf::log::NotificationsPrefData);

#endif
