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

#ifndef WSFIMAGEMESSAGE_HPP
#define WSFIMAGEMESSAGE_HPP

#include "wsf_export.h"

#include <memory>

class UtScriptClass;
class UtScriptTypes;
class WsfImage;
#include "WsfMessage.hpp"

/**
   A specialization of WsfMessage that represents a still image
   to be transmitted over a communications network.
*/

class WSF_EXPORT WsfImageMessage : public WsfMessage
{
public:
   WsfImageMessage(WsfPlatform* aPlatformPtr, const WsfImage& aImage);
   ~WsfImageMessage() override;

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

   // Return the pointer to the image.  This pointer is valid ONLY during the
   // processing of the message.  If you need to retain the information then
   // you must clone the image!

   WsfImage* GetImage() const { return mImagePtr; }

protected:
   WsfImageMessage(const WsfImageMessage& aSrc); // For Clone();
   WsfImageMessage& operator=(const WsfImageMessage&) = delete;

private:
   WsfImage* mImagePtr;
};

#endif
