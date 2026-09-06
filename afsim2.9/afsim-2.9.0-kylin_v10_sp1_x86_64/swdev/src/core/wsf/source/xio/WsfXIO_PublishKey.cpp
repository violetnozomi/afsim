// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "xio/WsfXIO_PublishKey.hpp"

#include "UtStringUtil.hpp"

UtScriptClass* WsfXIO_PublishKey::sScriptClassPtr = nullptr;

WsfXIO_PublishKey::Field::Field(int aValue)
   : mValue(UtStringUtil::ToString(aValue))
{
}

WsfXIO_PublishKey::Field::Field(size_t aValue)
   : mValue(UtStringUtil::ToString(static_cast<unsigned long>(aValue)))
{
}

WsfXIO_PublishKey::Field& WsfXIO_PublishKey::Field::operator=(int aValue)
{
   mValue = UtStringUtil::ToString(aValue);
   return *this;
}

WsfXIO_PublishKey::Field::operator int() const
{
   return UtStringUtil::ToInt(mValue);
}
