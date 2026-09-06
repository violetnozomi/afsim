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

#ifndef CONFIGDATA_H
#define CONFIGDATA_H

#include <QJsonObject>

namespace wsfg
{

namespace spaceg
{

namespace ConfigData
{

void Merge(QJsonObject& aObject, const QJsonObject& aOther);

} // namespace ConfigData

} // namespace spaceg

} // namespace wsfg

#endif // CONFIGDATA_H
