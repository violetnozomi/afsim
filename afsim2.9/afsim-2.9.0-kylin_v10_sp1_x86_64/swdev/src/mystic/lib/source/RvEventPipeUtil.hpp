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

#ifndef RVEVENTPIPEUTIL_HPP
#define RVEVENTPIPEUTIL_HPP

#include "RvEventPipeClasses.hpp"
#include "RvExport.hpp"
#include "UtEntity.hpp"
#include "UtPackReflect.hpp"
#include "UtTextDocument.hpp"
#include "UtVec3.hpp"

namespace rv
{
QString RV_EXPORT toString(const rv::AngleLimit& aLimit);
UtVec3d RV_EXPORT toVec3(const rv::Vec3f& aVec);
UtVec3d RV_EXPORT toVec3(const rv::Vec3d& aVec);
void RV_EXPORT    RegisterSerializer(UtPackSerializer& aSerializer);
void RV_EXPORT    GetSchema(UtTextDocument& aDocument);
bool              IsOneTimeMessage(const MsgBase* aMessagePtr);
bool              IsEventMessage(const MsgBase* aMessagePtr);
void RV_EXPORT    BuildUtEntity(float aSimTime, const MsgEntityState& aEntityState, UtEntity& aEntity);
void RV_EXPORT GetCombinedOrientation(const MsgPartArticulationStatus& aMsg, double& aAz, double& aEl, double& aRoll);
} // namespace rv

#endif
