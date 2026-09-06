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

#ifndef WSFSIXDOFOBJECT_HPP
#define WSFSIXDOFOBJECT_HPP

#include "wsf_six_dof_export.h"

#include <list>
#include <string>

namespace wsf
{
namespace six_dof
{
class Mover;

class WSF_SIX_DOF_EXPORT Object
{
public:
   Object()          = default;
   virtual ~Object() = default;

   virtual std::string GetName() const { return mObjName; };
   virtual void        SetName(const std::string& aName) { mObjName = aName; };

   // ParentVehicle is the Vehicle this object is attached to (This may be different from Parent)
   virtual Mover* GetParentVehicle() const { return mParentVehicle; }
   virtual void   SetParentVehicle(Mover* aParentVehicle) { mParentVehicle = aParentVehicle; }

protected:
   Object(const Object&) = default;

   std::string mObjName       = "";
   Mover*      mParentVehicle = nullptr;
};
} // namespace six_dof
} // namespace wsf

#endif
