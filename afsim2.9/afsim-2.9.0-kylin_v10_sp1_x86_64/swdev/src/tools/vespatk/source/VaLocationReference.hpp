// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef VaLocationReference_hpp
#define VaLocationReference_hpp

#include "vespatk_export.h"

#include <string>

#include "VaCallbackHolder.hpp"
#include "VaPosition.hpp"

namespace vespa
{
class VaEntity;

class VESPATK_EXPORT VaLocationReference
{
public:
   VaLocationReference();
   VaLocationReference(VaEntity* aEntityPtr);
   VaLocationReference(const VaPosition& aPosition);
   VaLocationReference(const VaLocationReference& aSrc);
   virtual ~VaLocationReference();

   VaLocationReference& operator=(const VaLocationReference& aRhs);

   void              SetPosition(const VaPosition& aPosition);
   const VaPosition& GetPosition() const;

   void SetVelocity(const double aVelocity[3]);
   void GetVelocity(double aVelocity[3]) const;

   void SetOrientation(const double aOrientation[3]);
   void GetOrientation(double aOrientation[3]) const;

   void            SetEntity(const VaEntity* aEntityPtr);
   const VaEntity* GetEntity() const { return mEntityPtr; }

private:
   VaLocationReference(VaEntity* aEntityPtr, const VaPosition& aPosition);

   void SetupCallbacks();

   void EntityRemovedOrDeletedCB(VaEntity* aEntityPtr);

   const VaEntity*    mEntityPtr;
   mutable VaPosition mPosition;
   mutable double     mVelocity[3];
   mutable double     mOrientation[3];
   VaCallbackHolder   mCallbacks;
};

} // end namespace vespa

#endif
