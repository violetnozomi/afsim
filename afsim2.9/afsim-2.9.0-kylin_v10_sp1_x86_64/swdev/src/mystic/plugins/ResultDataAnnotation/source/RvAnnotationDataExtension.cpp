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
#include "RvAnnotationDataExtension.hpp"

#include "RvAnnotationEventPipeClasses.hpp"
#include "RvAnnotationEventPipeClassesRegister.hpp"
#include "RvAnnotationEventPipeSchema.hpp"
#include "RvEnvironment.hpp"
#include "RvResultData.hpp"
#include "RvResultPlatform.hpp"
#include "UtEllipsoidalEarth.hpp"

bool rv::AnnotationDataExtension::HandlesMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case (rv::MsgAnnotationDecoration::cMESSAGE_ID):
   case (rv::MsgAnnotationPoi::cMESSAGE_ID):
   case (rv::MsgAnnotationRangeRing::cMESSAGE_ID):
      return true;
   default:
      return false;
   }
}

QVariant rv::AnnotationDataExtension::HandleMessage(const MsgBase* aMsg, int aColumn, int aRole) const
{
   switch (aMsg->GetMessageId())
   {
   case (rv::MsgAnnotationDecoration::cMESSAGE_ID):
   {
      return HandleAnnotationDecoration(static_cast<const rv::MsgAnnotationDecoration*>(aMsg), aColumn, aRole);
   }
   case (rv::MsgAnnotationPoi::cMESSAGE_ID):
   {
      return HandleAnnotationPoi(static_cast<const rv::MsgAnnotationPoi*>(aMsg), aColumn, aRole);
   }
   case (rv::MsgAnnotationRangeRing::cMESSAGE_ID):
   {
      return HandleAnnotationRangeRing(static_cast<const rv::MsgAnnotationRangeRing*>(aMsg), aColumn, aRole);
   }
   }
   return QVariant();
}

void rv::AnnotationDataExtension::RegisterSerializer(UtPackSerializer& aSerializer) const
{
   UtPack_register_all_wsf_annotation_events_types(aSerializer);
}

const char* rv::AnnotationDataExtension::GetSchema() const
{
   return cRvAnnotationEventPipe_SCHEMA;
}

bool rv::AnnotationDataExtension::IsOneTimeMessage(int aMessageId) const
{
   switch (aMessageId)
   {
   case MsgAnnotationDecoration::cMESSAGE_ID:
   case MsgAnnotationPoi::cMESSAGE_ID:
   case MsgAnnotationRangeRing::cMESSAGE_ID:
      return true;
   }
   return false;
}

bool rv::AnnotationDataExtension::IsEventMessage(int aMessageId) const
{
   return false;
}

void rv::AnnotationDataExtension::AddMessage(ResultDb& aResultDb, MsgBase* aMsg) const
{
   switch (aMsg->GetMessageId())
   {
   case MsgAnnotationDecoration::cMESSAGE_ID:
   case MsgAnnotationPoi::cMESSAGE_ID:
   case MsgAnnotationRangeRing::cMESSAGE_ID:
      rvEnv.AddAnnotationMessage(*aMsg);
      break;
   }
}

QVariant rv::AnnotationDataExtension::HandleAnnotationDecoration(const rv::MsgAnnotationDecoration* aMsg,
                                                                 int                                aColumn,
                                                                 int                                aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "entity name";
      }
      return QString::fromStdString(aMsg->entityName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "decoration type";
      }
      switch (aMsg->decorationType())
      {
      case 0:
         return "text";
      case 1:
         return "preset";
      case 2:
         return "image";
      }
      return "";
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "decoration description";
      }
      return QString::fromStdString(aMsg->decorationInfo());
   }
   }
   return QVariant();
}

QVariant rv::AnnotationDataExtension::HandleAnnotationPoi(const rv::MsgAnnotationPoi* aMsg, int aColumn, int aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "poi name";
      }
      return QString::fromStdString(aMsg->poiName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "latitude";
      }
      return aMsg->lat();
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "longitude";
      }
      return aMsg->lon();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "altitude";
      }
      return aMsg->alt();
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "bullseye";
      }
      return aMsg->bullseye();
   }
   }
   return QVariant();
}

namespace
{
QString VecToString(const rv::Vec3f& aVec)
{
   return (QString::number(aVec.x()) + ", " + QString::number(aVec.y()) + ", " + QString::number(aVec.z()));
}
} // namespace

QVariant rv::AnnotationDataExtension::HandleAnnotationRangeRing(const rv::MsgAnnotationRangeRing* aMsg,
                                                                int                               aColumn,
                                                                int                               aRole) const
{
   switch (aColumn)
   {
   case (rv::MessageBase::cFIELD_DATA):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ring name";
      }
      return QString::fromStdString(aMsg->ringName());
   }
   case (rv::MessageBase::cFIELD_DATA + 1):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "entity name";
      }
      return QString::fromStdString(aMsg->referenceName());
   }
   case (rv::MessageBase::cFIELD_DATA + 2):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range labels";
      }
      return aMsg->showRangeLabels();
   }
   case (rv::MessageBase::cFIELD_DATA + 3):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "range label color";
      }
      return VecToString(aMsg->rangeLabelColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 4):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ring color";
      }
      return VecToString(aMsg->ringColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 5):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "ring width";
      }
      return aMsg->ringWidth();
   }
   case (rv::MessageBase::cFIELD_DATA + 6):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "center radius";
      }
      return aMsg->centerRadius();
   }
   case (rv::MessageBase::cFIELD_DATA + 7):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "additional rings";
      }
      return aMsg->additionalRings();
   }
   case (rv::MessageBase::cFIELD_DATA + 8):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "additional radii";
      }
      return aMsg->additionalRadii();
   }
   case (rv::MessageBase::cFIELD_DATA + 9):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "angle labels";
      }
      return aMsg->showAngleLabels();
   }
   case (rv::MessageBase::cFIELD_DATA + 10):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "angle label color";
      }
      return VecToString(aMsg->angleLabelColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 11):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "radial color";
      }
      return VecToString(aMsg->radialColor());
   }
   case (rv::MessageBase::cFIELD_DATA + 12):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "radial width";
      }
      return aMsg->radialWidth();
   }
   case (rv::MessageBase::cFIELD_DATA + 13):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "center angle";
      }
      return aMsg->centerAngle();
   }
   case (rv::MessageBase::cFIELD_DATA + 14):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "angle width";
      }
      return aMsg->angleWidth();
   }
   case (rv::MessageBase::cFIELD_DATA + 15):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "radial rate";
      }
      return aMsg->radialRate();
   }
   case (rv::MessageBase::cFIELD_DATA + 16):
   {
      if (aRole == Qt::ToolTipRole)
      {
         return "align north";
      }
      return !aMsg->alignPlatform();
   }
   }

   return QVariant();
}
