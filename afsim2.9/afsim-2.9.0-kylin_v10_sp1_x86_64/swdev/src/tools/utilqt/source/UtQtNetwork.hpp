// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UTQTIPEDIT_HPP
#define UTQTIPEDIT_HPP

#include "utilqt_export.h"

#include <QLineEdit>
#include <QNetworkInterface>

class UTILQT_EXPORT UtQtIpv4LineEdit : public QLineEdit
{
   Q_OBJECT
public:
   UtQtIpv4LineEdit(QWidget* aParent = nullptr);
};

class UTILQT_EXPORT UtQtMulticastIpv4LineEdit : public QLineEdit
{
   Q_OBJECT
public:
   UtQtMulticastIpv4LineEdit(QWidget* aParent = nullptr);
};

namespace UtQtNetwork
{
UTILQT_EXPORT QStringList GetIpv4Interfaces(QNetworkInterface::InterfaceFlags aFlags);
UTILQT_EXPORT QStringList GetBroadcastIpv4Interfaces();
UTILQT_EXPORT QStringList GetMulticastIpv4Interfaces();
} // namespace UtQtNetwork

#endif
