// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016-2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef P6DOFOBJECT_H
#define P6DOFOBJECT_H

#include "p6dof_export.h"

#include <iostream>
#include <list>

class P6DofScenario;
class P6DofVehicle;

class P6DOF_EXPORT P6DofObject
{
public:
   explicit P6DofObject(P6DofScenario* aScenario);

   virtual ~P6DofObject() = default;

   P6DofObject* Clone() const;

   P6DofScenario* GetScenario() const { return mScenario; }

   std::string GetName() const { return mObjName; }
   void        SetName(const std::string& aName) { mObjName = aName; }

   // ParentVehicle is the Vehicle this object is attached to (This may be different from Parent)
   P6DofVehicle* GetParentVehicle() const { return mParentVehicle; }
   virtual void  SetParentVehicle(P6DofVehicle* aParentVehicle) { mParentVehicle = aParentVehicle; }

protected:
   P6DofObject(const P6DofObject& aSrc) = default;
   P6DofObject& operator=(const P6DofObject& other) = default;

   P6DofScenario* mScenario;
   P6DofVehicle*  mParentVehicle;
   std::string    mObjName;
};

#endif