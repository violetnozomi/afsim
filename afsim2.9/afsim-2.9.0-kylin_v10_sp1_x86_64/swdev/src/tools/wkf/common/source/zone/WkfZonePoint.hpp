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

#ifndef WkfZonePoint_hpp
#define WkfZonePoint_hpp

#include "wkf_common_export.h"

#include "UtColor.hpp"
#include "VaAttachment.hpp"
#include "VaObject.hpp"
#include "VaPosition.hpp"
#include "VaUniqueId.hpp"

class UtoModelShape;

namespace vespa
{
class VaAttachment;
} // namespace vespa
namespace wkf
{
class WKF_COMMON_EXPORT ZonePoint : public vespa::VaObject
{
public:
   ZonePoint(vespa::VaAttachment& aParent, const vespa::VaPosition& aPosition);

   ~ZonePoint() override;

   void Load();

   const vespa::VaPosition& GetPosition() const { return mPosition; }
   void                     SetPosition(const vespa::VaPosition& aPosition, bool aBuild = true);
   void                     UpdatePosition();

   void SetVisible(bool aVisible);

   bool GetSelected(vespa::VaViewer* aViewerPtr);
   void SetSelected(vespa::VaViewer* aViewerPtr, bool aSelected);

   // A predicate for finding a zone point in an STL container.
   struct FindPred
   {
      FindPred(unsigned int aId)
         : mId(aId)
      {
      }
      bool         operator()(ZonePoint* aPoint) const { return (aPoint->GetUniqueId() == mId); }
      unsigned int mId;
   };

   vespa::VaAttachment& GetParent() { return mParent; }

   void SetColor(const UtColor& aColor);

   virtual void GetShapes(vespa::VaAttachment::ShapeList& aShapes) const;

private:
   void ComputeXYZ(const vespa::VaPosition& aPosition, double aModelXYZ[3]);

   void LoadState();

   vespa::VaAttachment& mParent;
   vespa::VaPosition    mPosition;
   vespa::VaPosition    mSourcePosition;
   UtoModelShape*       mPointPtr;
   std::string          mPointShapeName;
   std::map<int, bool>  mSelected;
   UtColor              mColor;
};
} // end namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::ZonePoint)
#endif
