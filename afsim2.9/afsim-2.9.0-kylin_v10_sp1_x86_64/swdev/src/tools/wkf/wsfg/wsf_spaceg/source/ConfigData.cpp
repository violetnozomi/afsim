// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "ConfigData.hpp"

//! Merge the properties of one configuration into another.
//!
//! This will take the properties from @p aOther and add them to @p aObject,
//! but only if @p aObject does not already contain that property.
//!
//! \param aObject - the configuration into which properties are merged.
//! \param aOther  - the configuration from which properties are merged.
void wsfg::spaceg::ConfigData::Merge(QJsonObject& aObject, const QJsonObject& aOther)
{
   for (auto iter = aOther.constBegin(); iter != aOther.constEnd(); ++iter)
   {
      if (!aObject.contains(iter.key()))
      {
         aObject.insert(iter.key(), iter.value());
      }
   }
}
