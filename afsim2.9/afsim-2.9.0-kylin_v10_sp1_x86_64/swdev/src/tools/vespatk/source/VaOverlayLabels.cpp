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

#include "VaOverlayLabels.hpp"

#include <sstream>

#include "UtoTextShape.hpp"

namespace vespa
{
VaOverlayLabels::VaOverlayLabels()
   : VaOverlay("labels_overlay")
   , mLabels()
{
   SetType<VaOverlayLabels>();
}

VaOverlayLabels::VaOverlayLabels(const VaOverlayLabels& aSrc)
   : VaOverlay(aSrc)
   , mLabels()
{
   SetType<VaOverlayLabels>();
}

VaOverlayLabels::~VaOverlayLabels()
{
   ClearLabels();
}

VaOverlayLabels* VaOverlayLabels::Clone() const
{
   return new VaOverlayLabels(*this);
}

bool VaOverlayLabels::Initialize()
{
   return true;
}

void VaOverlayLabels::Update() {}

int VaOverlayLabels::CreateLabel()
{
   int labelIndex = -1;

   // Create a unique shape name for the label.

   std::ostringstream  oss;
   static unsigned int uid = 0;
   oss << "VaAttachmentLabel_" << uid++;
   std::string shapeName = oss.str().c_str();

   UtoTextShape shape;
   shape.SetBackdropType(UtoTextShape::BACKDROP_OUTLINE);
   shape.SetSize(8);

   // Add the label to our shape map.
   UtoTextShape* shapePtr = dynamic_cast<UtoTextShape*>(BindShape(shapeName, shape));
   if (shapePtr != nullptr)
   {
      // Reuse indices if possible
      if (!mRemovedIndices.empty())
      {
         labelIndex = mRemovedIndices.back();
         mRemovedIndices.pop_back();
      }
      else
      {
         labelIndex = (int)mLabels.size();
         mLabels.push_back(nullptr);
      }
      mLabels[labelIndex] = new Label(shapeName, shapePtr);
   }
   return labelIndex;
}

void VaOverlayLabels::DeleteLabel(int aLabelIndex)
{
   if ((aLabelIndex < (int)mLabels.size()) && (aLabelIndex >= 0) && (mLabels[aLabelIndex] != nullptr))
   {
      Label* labelPtr = mLabels[aLabelIndex];
      UnbindShape(labelPtr->mShapeName);
      delete labelPtr;
      mLabels[aLabelIndex] = nullptr;
      mRemovedIndices.push_back(aLabelIndex);
   }
}

void VaOverlayLabels::ClearLabels()
{
   for (int i = 0; i < (int)mLabels.size(); ++i)
   {
      DeleteLabel(i);
   }
   mLabels.clear();
   mRemovedIndices.clear();
}

VaOverlayLabels::Label* VaOverlayLabels::FindLabel(int aLabelIndex) const
{
   Label* labelPtr = nullptr;
   if ((aLabelIndex < (int)mLabels.size()) && (aLabelIndex >= 0))
   {
      labelPtr = mLabels[aLabelIndex];
   }
   return labelPtr;
}

void VaOverlayLabels::ShowLabel(int aLabelIndex, bool aShow)
{
   Label* labelPtr = FindLabel(aLabelIndex);
   if (labelPtr != nullptr)
   {
      if (aShow)
      {
         if ((labelPtr->mShapePtr) && (!labelPtr->mBound))
         {
            UtoTextShape* shapePtr = dynamic_cast<UtoTextShape*>(BindShape(labelPtr->mShapeName, *labelPtr->mShapePtr));
            if (shapePtr != nullptr)
            {
               labelPtr->mBound = true;
               delete labelPtr->mShapePtr;
               // Update the shape pointer.
               labelPtr->mShapePtr = shapePtr;
            }
         }
      }
      else if (labelPtr->mBound)
      {
         // Make a copy of the shape, since the unbind will delete the current copy.
         // The new copy will be needed when we decide to show the label again.
         labelPtr->mShapePtr = dynamic_cast<UtoTextShape*>(labelPtr->mShapePtr->Clone());
         labelPtr->mBound    = false;
         UnbindShape(labelPtr->mShapeName);
      }
   }
}
} // end namespace vespa
