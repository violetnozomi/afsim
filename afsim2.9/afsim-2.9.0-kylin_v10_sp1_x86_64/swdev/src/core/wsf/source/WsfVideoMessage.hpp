// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFVIDEOMESSAGE_HPP
#define WSFVIDEOMESSAGE_HPP

#include "wsf_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
class WsfImage;
#include "WsfMessage.hpp"

/**
   A specialization of WsfMessage that represents an video frame
   to be transmitted over a communications network.
*/

class WSF_EXPORT WsfVideoMessage : public WsfMessage
{
public:
   WsfVideoMessage(WsfPlatform* aPlatformPtr, const WsfImage& aImagePtr);

   ~WsfVideoMessage() override = default;

   //! Standard Clone() method.
   WsfMessage* Clone() const override;

   const char* GetScriptClassName() const override;

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   //! Return the message type used by messages of this class.
   static WsfStringId GetTypeId();
   static const char* GetMessageType();

   //! @return The string ID of the name of the sensor that generated the image contained in the message
   WsfStringId GetComponentNameId() const override;
   //! @return The string ID of the type of the sensor that generated the image contained in the message
   WsfStringId GetComponentTypeId() const override;
   //! @return The string ID of the mode of the sensor that generated the image contained in the message
   WsfStringId GetComponentModeId() const override;

   // Return the pointer to the frame.  This pointer is valid ONLY during the
   // processing of the message.  If you need to retain the information then
   // you must clone the Image!

   WsfImage* GetImage() const { return mImagePtr.get(); }

protected:
   WsfVideoMessage(const WsfVideoMessage& aSrc) = default; // For Clone()
   WsfVideoMessage& operator=(const WsfVideoMessage&) = delete;

private:
   ut::CloneablePtr<WsfImage> mImagePtr;
};

#endif
