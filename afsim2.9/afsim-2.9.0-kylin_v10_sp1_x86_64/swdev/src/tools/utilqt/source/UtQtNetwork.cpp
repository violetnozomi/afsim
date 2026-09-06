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

#include "UtQtNetwork.hpp"

#include <QHostAddress>
#include <QRegExpValidator>

UtQtIpv4LineEdit::UtQtIpv4LineEdit(QWidget* aParent /*=nullptr*/)
   : QLineEdit(aParent)
{
   QString           ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"; // 0-255
   QRegExp           ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
   QRegExpValidator* ipValidator = new QRegExpValidator(ipRegex, this);
   setValidator(ipValidator);
}

UtQtMulticastIpv4LineEdit::UtQtMulticastIpv4LineEdit(QWidget* aParent /*= nullptr*/)
{
   QString           firstOctet = "(?:22[4-9]|23[0-9])";                       // 224-239
   QString           ipRange    = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])"; // 0-255
   QRegExp           ipRegex("^" + firstOctet + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");
   QRegExpValidator* ipValidator = new QRegExpValidator(ipRegex, this);
   setValidator(ipValidator);
}

QStringList UtQtNetwork::GetIpv4Interfaces(QNetworkInterface::InterfaceFlags aFlags)
{
   QStringList list;
   for (const auto& it : QNetworkInterface::allInterfaces())
   {
      if ((it.flags() & aFlags) == aFlags)
      {
         for (const auto& jt : it.addressEntries())
         {
            if (jt.ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
               list.push_back(jt.ip().toString());
            }
         }
      }
   }
   return list;
}

QStringList UtQtNetwork::GetBroadcastIpv4Interfaces()
{
   return GetIpv4Interfaces(QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanBroadcast);
}

QStringList UtQtNetwork::GetMulticastIpv4Interfaces()
{
   return GetIpv4Interfaces(QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanMulticast);
}
