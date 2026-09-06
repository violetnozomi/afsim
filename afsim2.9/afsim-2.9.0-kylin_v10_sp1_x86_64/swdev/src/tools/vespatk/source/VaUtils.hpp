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
#ifndef VaUtils_hpp
#define VaUtils_hpp

#include "vespatk_export.h"

#include <sstream>
#include <vector>

#include "UtMemory.hpp"
#include "UtPath.hpp"
#include "UtoTypes.hpp"
#include "VaAttachment.hpp"
#include "VaDefs.hpp"
#include "VaEntity.hpp"
#include "VaEnvironment.hpp"
#include "VaViewer.hpp"

class UtColor;

namespace vespa
{
class VESPATK_EXPORT VaUtils
{
public:
   template<class ET>
   static void GetSelectedEntities(VaViewer* aViewerPtr, std::vector<ET*>& aEntities);

   template<class AT, class AST>
   static void GetSelectedAttachments(VaViewer*          aViewerPtr,
                                      std::vector<AT*>&  aAttachments,
                                      std::vector<AST*>& aAttachmentSubParts);

   static UtoColor glToUtoColor(const float* aColor);

   static UtoColor glToUtoColor(const UtColor& aColor);

   static UtColor UtoToGlColor(const UtoColor& aColor);

   static std::string ToString(const std::string& aValue) { return aValue; }

   template<class T>
   static std::string ToString(const T& aValue);

   template<class T>
   static T ToValue(const std::string& aString);

   static void GetPathPrioritizedFilename(const std::string& aInputFilename,
                                          std::string&       aPathAndFilename,
                                          const std::string& aEnvironmentVariable = "",
                                          const std::string& aKeyString           = "");

   static void AddApplicationDataPath(const std::string& aPath);

   //! Normalizes the angle to be in the range of 0 <= angle <= 360
   //! @param aAngle The angle in degrees.
   inline static double NormalizeAngle0_360(double aAngle)
   {
      while (aAngle > 360.0)
      {
         aAngle -= 360.0;
      }
      while (aAngle < 0.0)
      {
         aAngle += 360.0;
      }
      return aAngle;
   }

   static void    SetSitePath(const UtPath& aPath);    // use these user-designated resources... don't write to them
   static void    SetAppDataPath(const UtPath& aPath); // use these app-designated resources... don't write to them
   static void    SetUserPath(const UtPath& aPath);    // use these user-defined resource... write to them
   static UtPath& GetSitePath();
   static UtPath& GetAppDataPath();
   static UtPath& GetUserPath();

   static std::string GetAbsolutePathFromSource(const std::string& aRelativePath,
                                                const std::string& aSource); // source can be "user" "app" or "site"

   static void Cleanup();
   static void HSV_ToRGB(const float aHSV[3], float aRGB[3]);

private:
   static std::vector<std::string> mApplicationDataPathList;

   static UtPath mSitePath;
   static UtPath mAppDataPath;
   static UtPath mUserPath;
};

template<class ET>
void VaUtils::GetSelectedEntities(VaViewer* aViewerPtr, std::vector<ET*>& aEntities)
{
   const VaHitSet& selection = aViewerPtr->GetSelection();
   for (VaHitSet::iterator iter = selection.begin(); iter != selection.end(); ++iter)
   {
      ET* entityPtr = dynamic_cast<ET*>(iter->FindEntity());
      if (entityPtr != 0)
      {
         aEntities.push_back(entityPtr);
      }
   }
}

template<class AT, class AST>
void VaUtils::GetSelectedAttachments(VaViewer* aViewerPtr, std::vector<AT*>& aAttachments, std::vector<AST*>& aAttachmentSubParts)
{
   const VaHitSet& selection = aViewerPtr->GetSelection();
   for (VaHitSet::const_iterator iter = selection.begin(); iter != selection.end(); ++iter)
   {
      const VaHitEntry& hit           = *iter;
      AT*               attachmentPtr = dynamic_cast<AT*>(hit.FindAttachment());
      if (attachmentPtr != 0)
      {
         aAttachments.push_back(attachmentPtr);
         AST* attachmentSubPartPtr = dynamic_cast<AST*>(attachmentPtr->FindSubPart(hit.GetSubId()));
         if (attachmentSubPartPtr != 0)
         {
            aAttachmentSubParts.push_back(attachmentSubPartPtr);
         }
      }
   }
}

template<class T>
std::string VaUtils::ToString(const T& aValue)
{
   std::ostringstream oss;
   oss << aValue;
   return oss.str().c_str();
}

template<class T>
T VaUtils::ToValue(const std::string& aString)
{
   std::istringstream iss(aString.c_str());
   T                  val;
   iss >> val;
   return val;
}

// Factory method to create a VaAttachment. This method guards against memory leaks
// and makes sure that the created attachment is strongly owned by a parent entity
// before returning a raw pointer of the created attachment back to the caller.
template<class T, class U, class... Args>
T* make_attachment(U& aParent, Args&&... args)
{
   static_assert(std::is_base_of<VaAttachment, T>::value,
                 "Template type argument of make_attachment must derive from VaAttachment");
   static_assert(std::is_base_of<VaEntity, U>::value,
                 "First argument of make_attachment's template type's constructor must derive from VaEntity");

   auto attachment    = ut::make_unique<T>(aParent, std::forward<Args>(args)...);
   auto attachmentPtr = attachment.get();
   aParent.AddAttachment(std::move(attachment));
   return attachmentPtr;
}
} // end namespace vespa

#endif
