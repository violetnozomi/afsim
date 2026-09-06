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

#ifndef VaCamera_hpp
#define VaCamera_hpp

#include "vespatk_export.h"

#include "VaCallbackHolder.hpp"
#include "VaCameraBase.hpp"
#include "VaCameraMotion.hpp"

namespace vespa
{
class VaCameraMotion;
class VaEntity;
class VaPosition;
class VaViewer;

class VESPATK_EXPORT VaCamera : public VaCameraBase
{
public:
   // VaCamera may be constructed with aViewerPtr = 0, just please make sure to call VaCamera::SetViewer() with a valid
   // viewer as soon as possible.
   VaCamera(VaViewer* aViewerPtr);

   ~VaCamera() override;

   void Untether(bool aResumeLastUntetheredPosition = true);
   bool Tether(VaEntity* aEntityPtr);

   void UnLookAt();
   bool LookAt(VaEntity* aReferenceEntityPtr, VaEntity* aLookAtEntityPtr, double aAz, double aEl, double aDist);

   void FlatMapMode(bool aState);

   /// Takes ownership of the provided VaCameraMotion.  Untethers if tethered.  Un-lookats if lookated.
   /// Returns true on success, false otherwise.
   bool SetCustomCameraMotion(VaCameraMotion* aCameraMotion);

protected:
   virtual void EntityRemovedCB(VaEntity* aEntityPtr);
   virtual void EntityDeletedCB(VaEntity* aEntityPtr);

   /// this method is redeclared "protected" simply to prevent users from calling it
   void SetCameraMotion(VaCameraMotion* aCameraMotion) override { VaCameraBase::SetCameraMotion(aCameraMotion); }

   unsigned int mEntityUID;
   int          mEntityRemovedUID;
   int          mEntityDeletedUID;

private:
   bool mLockRotations;
};
} // end namespace vespa
#endif
