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
#ifndef RVANNOTATIONDATAEXTENSION_HPP
#define RVANNOTATIONDATAEXTENSION_HPP

#include <QVariant>

#include "RvDataExtension.hpp"

namespace rv
{
class MsgAnnotationDecoration;
class MsgAnnotationPoi;
class MsgAnnotationRangeRing;

class AnnotationDataExtension : public DataExtension
{
public:
   bool        HandlesMessage(int aMessageId) const override;
   QVariant    HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const override;
   void        RegisterSerializer(UtPackSerializer& aSerializer) const override;
   const char* GetSchema() const override;
   bool        IsOneTimeMessage(int aMessageId) const override;
   bool        IsEventMessage(int aMessageId) const override;
   void        AddMessage(ResultDb& aResultDb, MsgBase* aMsg) const override;

private:
   QVariant HandleAnnotationDecoration(const rv::MsgAnnotationDecoration* aMsg, int aColumn, int aRole) const;
   QVariant HandleAnnotationPoi(const rv::MsgAnnotationPoi* aMsg, int aColumn, int aRole) const;
   QVariant HandleAnnotationRangeRing(const rv::MsgAnnotationRangeRing* aMsg, int aColumn, int aRole) const;
};

} // namespace rv

#endif
