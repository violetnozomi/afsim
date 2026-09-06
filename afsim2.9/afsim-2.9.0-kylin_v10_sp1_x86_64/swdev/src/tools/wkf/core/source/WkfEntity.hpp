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

#ifndef WkfEntity_HPP
#define WkfEntity_HPP

#include "wkf_export.h"

#include <QIcon>
#include <QList>

#include "UtCallbackHolder.hpp"
#include "VaEntity.hpp"

namespace wkf
{
class WKF_EXPORT Entity : public vespa::VaEntity
{
public:
   Entity(const std::string& aName);
   ~Entity() override = default;

   void SetDecoration(const QIcon& aIcon, const QString& aDescription);

   const QIcon& GetDecoration() const { return mDecoration; }

   //! Retrieves the requested item.
   //! Default implementation returns GetNamedPositionInfo.
   //! \param aItem The name of the item to return data for.
   //! \param aLabel Out parameter that returns the label to display on the data. Typically in the form "text: ".
   //! \param aValue Out parameter that returns the value to display. Should be formatted with wkf::UnitsObject if
   //! applicable. \returns True on success, false on failure or no info available.
   virtual bool GetNamedInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const;

protected:
   //! For use in implementations of GetNamedInfo.
   //! Parameters and return value are the same as GetNamedInfo.
   bool GetNamedPositionInfo(const std::string& aItem, std::string& aLabel, std::string& aValue) const;

private:
   QIcon mDecoration;
};

using EntityList = QList<wkf::Entity*>;
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::Entity)
#endif
