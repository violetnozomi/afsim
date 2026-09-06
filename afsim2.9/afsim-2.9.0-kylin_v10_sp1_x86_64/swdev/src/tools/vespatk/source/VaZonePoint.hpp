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

#ifndef VaZonePoint_hpp
#define VaZonePoint_hpp

#include "vespatk_export.h"

#include "UtColor.hpp"
#include "VaAttachment.hpp"
#include "VaObject.hpp"
#include "VaPosition.hpp"
#include "VaUniqueId.hpp"

class UtoModelShape;

namespace vespa
{
class VaAttachment;

class VESPATK_DEPRECATED_EXPORT VaZonePoint : public VaObject
{
public:
   VaZonePoint(VaAttachment& aParent, const VaPosition& aPosition);

   ~VaZonePoint() override;

   void Load();

   const VaPosition& GetPosition() const { return mPosition; }
   void              SetPosition(const VaPosition& aPosition, bool aBuild = true);
   void              UpdatePosition();

   void SetVisible(bool aVisible);

   bool GetSelected(VaViewer* aViewerPtr);
   void SetSelected(VaViewer* aViewerPtr, bool aSelected);

   // A predicate for finding a zone point in an STL container.
   struct FindPred
   {
      FindPred(unsigned int aId)
         : mId(aId)
      {
      }
      bool         operator()(VaZonePoint* aPoint) const { return (aPoint->GetUniqueId() == mId); }
      unsigned int mId;
   };

   VaAttachment& GetParent() { return mParent; }

   void SetColor(const UtColor& aColor);

   virtual void GetShapes(VaAttachment::ShapeList& aShapes) const;

private:
   void ComputeXYZ(const VaPosition& aPosition, double aModelXYZ[3]);

   void LoadState();

   VaAttachment&       mParent;
   VaPosition          mPosition;
   VaPosition          mSourcePosition;
   UtoModelShape*      mPointPtr;
   std::string         mPointShapeName;
   std::map<int, bool> mSelected;
   UtColor             mColor;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaZonePoint)
#endif
