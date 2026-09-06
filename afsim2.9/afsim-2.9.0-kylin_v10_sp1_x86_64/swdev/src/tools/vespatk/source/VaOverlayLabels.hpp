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

#ifndef VaOverlayLabels_hpp
#define VaOverlayLabels_hpp

#include "vespatk_export.h"

#include <string>
#include <vector>

#include "VaOverlay.hpp"

class UtoTextShape;

namespace vespa
{
class VESPATK_DEPRECATED_EXPORT VaOverlayLabels : public VaOverlay
{
public:
   struct Label
   {
      Label(const std::string& aShapeName, UtoTextShape* aShapePtr)
         : mShapeName(aShapeName)
         , mShapePtr(aShapePtr)
         , mBound(true)
      {
      }
      std::string   mShapeName;
      UtoTextShape* mShapePtr;
      bool          mBound;
   };

   VaOverlayLabels();
   ~VaOverlayLabels() override;

   VaOverlayLabels* Clone() const override;

   bool Initialize() override;
   void Update() override;

   int    CreateLabel();
   void   DeleteLabel(int aLabelIndex);
   void   ClearLabels();
   Label* FindLabel(int aLabelIndex) const;
   void   ShowLabel(int aLabelIndex, bool aShow);

private:
   VaOverlayLabels(const VaOverlayLabels& aSrc);

   typedef std::vector<Label*> LabelList;
   LabelList                   mLabels;
   // Maintains a list of label indices to be reused.
   std::vector<int> mRemovedIndices;
};
} // end namespace vespa

VA_DECLARE_OBJECT_TYPE(vespa::VaOverlayLabels)

#endif
