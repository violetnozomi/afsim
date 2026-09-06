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

#ifndef WKFPROXYSTYLE_HPP
#define WKFPROXYSTYLE_HPP

#include <QProxyStyle>

namespace wkf
{
class ProxyStyle : public QProxyStyle
{
public:
   ProxyStyle(QStyle* aStyle = nullptr);

   QPixmap generatedIconPixmap(QIcon::Mode iconMode, const QPixmap& pixmap, const QStyleOption* opt) const override;
};
} // namespace wkf

#endif
