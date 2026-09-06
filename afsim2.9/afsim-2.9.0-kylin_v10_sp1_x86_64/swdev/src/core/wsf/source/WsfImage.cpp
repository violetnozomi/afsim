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

#include "WsfImage.hpp"

#include <cassert>
#include <memory>

#include "UtMemory.hpp"
#include "UtVec3.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfProcessor.hpp"
#include "WsfSensor.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"
#include "script/WsfScriptContext.hpp"
#include "script/WsfScriptMessageClass.hpp"

// =================================================================================================
WsfImage::WsfImage(WsfSensor* aOriginatingSensorPtr)
   : mMessageDataTag(0.0)
   , mMessageLength(512)
   , // bits
   mPlatformIndex(0)
   , mPlatformNameId(nullptr)
   , mPlatformTypeId(nullptr)
   , mSensorNameId(nullptr)
   , mSensorTypeId(nullptr)
   , mSensorModeId(nullptr)
   , mFrameTime(0.0)
   , mImageTime(0.0)
   , mImageNumber(0)
   , mStreamNumber(0)
   , mWidth(0.0)
   , mHeight(0.0)
   , mWidthResolution(0.0)
   , mHeightResolution(0.0)
   , mNoiseLevel(0.0)
   , mBackgroundLevel(0.0)
   , mMinimumLevel(0.0)
   , mMaximumLevel(1.0)
   , mCollectionFactor(1.0)
   , mTrackQuality(0.5)
   , mRangeError(0.0)
   , mBearingError(0.0)
   , mElevationError(0.0)
   , mRangeRateError(0.0)
   , mObjects()
{
   if ((aOriginatingSensorPtr != nullptr) && (aOriginatingSensorPtr->GetPlatform() != nullptr))
   {
      mPlatformIndex  = aOriginatingSensorPtr->GetPlatform()->GetIndex();
      mPlatformNameId = aOriginatingSensorPtr->GetPlatform()->GetNameId();
      mPlatformTypeId = aOriginatingSensorPtr->GetPlatform()->GetTypeId();
      mSensorNameId   = aOriginatingSensorPtr->GetNameId();
      mSensorTypeId   = aOriginatingSensorPtr->GetTypeId();
      mSensorModeId   = aOriginatingSensorPtr->GetCurrentMode()->GetNameId();
   }
   UtVec3d::Set(mOriginatorLocationWCS, 0.0);
   UtVec3d::Set(mCenterLocationWCS, 0.0);
}

// =================================================================================================
// virtual
WsfImage* WsfImage::Clone() const
{
   return new WsfImage(*this);
}

// =================================================================================================
void WsfImage::GetOriginatorLocationWCS(double aOriginatorLocationWCS[3]) const
{
   UtVec3d::Set(aOriginatorLocationWCS, mOriginatorLocationWCS);
}

// =================================================================================================
void WsfImage::SetOriginatorLocationWCS(const double aOriginatorLocationWCS[3])
{
   UtVec3d::Set(mOriginatorLocationWCS, aOriginatorLocationWCS);
}

// =================================================================================================
void WsfImage::GetCenterLocationWCS(double aCenterLocationWCS[3]) const
{
   UtVec3d::Set(aCenterLocationWCS, mCenterLocationWCS);
}

// =================================================================================================
void WsfImage::SetCenterLocationWCS(const double aCenterLocationWCS[3])
{
   UtVec3d::Set(mCenterLocationWCS, aCenterLocationWCS);
}

// =================================================================================================
// Nested class methods
// =================================================================================================

// =================================================================================================
WsfImage::Object::Object(WsfPlatform* aPlatformPtr)
   : mSignalLevel(0.0)
   , mPixelCount(1.0)
   , mPixelIntensity(1.0)
   , mTruthIndex(0)
   , mTruthType(nullptr)
   , mTruthSide(nullptr)
{
   if (aPlatformPtr != nullptr)
   {
      UtVec3d::Set(mLocationWCS, 0.0);
      mTruthIndex = aPlatformPtr->GetIndex();
      mTruthType  = aPlatformPtr->GetTypeId();
      mTruthSide  = aPlatformPtr->GetSideId();
   }
}

// =================================================================================================
//! Get the perceived location of the object in WCS.
void WsfImage::Object::GetLocationWCS(double aLocationWCS[3]) const
{
   UtVec3d::Set(aLocationWCS, mLocationWCS);
}

// =================================================================================================
//! Set the perceived location of the object in WCS.
void WsfImage::Object::SetLocationWCS(const double aLocationWCS[3])
{
   UtVec3d::Set(mLocationWCS, aLocationWCS);
}

// =================================================================================================
//! Get platform name of the platform representing this object.
WsfStringId WsfImage::Object::GetTruthName(WsfSimulation& aSimulation) const
{
   return aSimulation.GetPlatformNameId(mTruthIndex);
}

// =================================================================================================
//! The script interface 'class'
// =================================================================================================

class WSF_EXPORT WsfScriptImageClass : public UtScriptClass
{
public:
   WsfScriptImageClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   void* Create(const UtScriptContext& aInstance) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   UT_DECLARE_SCRIPT_METHOD(Originator);
   UT_DECLARE_SCRIPT_METHOD(OriginatorIndex); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(OriginatorLocation);
   UT_DECLARE_SCRIPT_METHOD(SensorName);
   UT_DECLARE_SCRIPT_METHOD(SensorType);
   UT_DECLARE_SCRIPT_METHOD(SensorMode);

   UT_DECLARE_SCRIPT_METHOD(ImageTime);
   UT_DECLARE_SCRIPT_METHOD(ImageNumber);
   UT_DECLARE_SCRIPT_METHOD(SetImageNumber);
   UT_DECLARE_SCRIPT_METHOD(StreamNumber);
   UT_DECLARE_SCRIPT_METHOD(SetStreamNumber);
   UT_DECLARE_SCRIPT_METHOD(CenterLocation);
   UT_DECLARE_SCRIPT_METHOD(Width);
   UT_DECLARE_SCRIPT_METHOD(Height);
   UT_DECLARE_SCRIPT_METHOD(WidthResolution);
   UT_DECLARE_SCRIPT_METHOD(HeightResolution);
   UT_DECLARE_SCRIPT_METHOD(Resolution); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(NoiseLevel);
   UT_DECLARE_SCRIPT_METHOD(BackgroundLevel);
   UT_DECLARE_SCRIPT_METHOD(MinimumLevel);
   UT_DECLARE_SCRIPT_METHOD(MaximumLevel);
   UT_DECLARE_SCRIPT_METHOD(CollectionFactor);

   UT_DECLARE_SCRIPT_METHOD(ObjectCount);
   UT_DECLARE_SCRIPT_METHOD(SignalLevel);
   UT_DECLARE_SCRIPT_METHOD(SignalToNoise); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(PixelCount);    // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(PixelIntensity);
   UT_DECLARE_SCRIPT_METHOD(Location);   // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(TruthIndex); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(TruthName);
   UT_DECLARE_SCRIPT_METHOD(TruthType); // NO_DOC | DEPRECATED
   UT_DECLARE_SCRIPT_METHOD(TruthSide); // NO_DOC | DEPRECATED

   UT_DECLARE_SCRIPT_METHOD(InImage); // NO_DOC | DEPRECATED
};

// =================================================================================================
//! Create the 'class' object for the script system.
//! This is invoked once by WsfScriptManager to create the 'class' object that defines
//! the interface to instances of this class from the script system.
// static
std::unique_ptr<UtScriptClass> WsfImage::CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
{
   return ut::make_unique<WsfScriptImageClass>(aClassName, aScriptTypesPtr);
}

// =================================================================================================
WsfScriptImageClass::WsfScriptImageClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr)
   : UtScriptClass(aClassName, aScriptTypesPtr)
{
   SetClassName("WsfImage");

   mCloneable          = true;
   mIsScriptAccessible = true;

   AddMethod(ut::make_unique<Originator>());
   AddMethod(ut::make_unique<OriginatorIndex>());
   AddMethod(ut::make_unique<OriginatorIndex>("PlatformIndex")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<OriginatorLocation>());
   AddMethod(ut::make_unique<SensorName>());
   AddMethod(ut::make_unique<SensorType>());
   AddMethod(ut::make_unique<SensorMode>());

   AddMethod(ut::make_unique<ImageTime>());
   AddMethod(ut::make_unique<ImageTime>("Time"));
   AddMethod(ut::make_unique<ImageNumber>());
   AddMethod(ut::make_unique<ImageNumber>("Number"));
   AddMethod(ut::make_unique<SetImageNumber>());
   AddMethod(ut::make_unique<SetImageNumber>("SetNumber"));
   AddMethod(ut::make_unique<StreamNumber>());
   AddMethod(ut::make_unique<SetStreamNumber>());
   AddMethod(ut::make_unique<CenterLocation>());
   AddMethod(ut::make_unique<Width>());
   AddMethod(ut::make_unique<Height>());
   AddMethod(ut::make_unique<WidthResolution>());
   AddMethod(ut::make_unique<HeightResolution>());
   AddMethod(ut::make_unique<Resolution>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<NoiseLevel>());
   AddMethod(ut::make_unique<BackgroundLevel>());
   AddMethod(ut::make_unique<MinimumLevel>());
   AddMethod(ut::make_unique<MaximumLevel>());
   AddMethod(ut::make_unique<CollectionFactor>());

   AddMethod(ut::make_unique<ObjectCount>());
   AddMethod(ut::make_unique<ObjectCount>("Count")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<Location>());
   AddMethod(ut::make_unique<Location>("ObjectLocation")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<PixelCount>());
   AddMethod(ut::make_unique<PixelCount>("ObjectPixelCount")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<PixelIntensity>());
   AddMethod(ut::make_unique<SignalLevel>());
   AddMethod(ut::make_unique<SignalToNoise>()); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TruthIndex>());
   AddMethod(ut::make_unique<TruthIndex>("ObjectTruthIndex")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TruthName>());
   AddMethod(ut::make_unique<TruthType>());
   AddMethod(ut::make_unique<TruthType>("ObjectTruthType")); // NO_DOC | DEPRECATED
   AddMethod(ut::make_unique<TruthSide>());
   AddMethod(ut::make_unique<TruthSide>("ObjectTruthSide")); // NO_DOC | DEPRECATED

   AddMethod(ut::make_unique<InImage>()); // NO_DOC | DEPRECATED
}

// =================================================================================================
// virtual
void* WsfScriptImageClass::Create(const UtScriptContext& aInstance)
{
   return nullptr;
}

// =================================================================================================
// virtual
void* WsfScriptImageClass::Clone(void* aObjectPtr)
{
   return static_cast<WsfImage*>(aObjectPtr)->Clone();
}

// =================================================================================================
// virtual
void WsfScriptImageClass::Destroy(void* aObjectPtr)
{
   delete static_cast<WsfImage*>(aObjectPtr);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, Originator, 0, "WsfPlatform", "")
{
   WsfPlatform* platformPtr =
      WsfScriptContext::GetSIMULATION(aContext)->GetPlatformByIndex(aObjectPtr->GetPlatformIndex());
   aReturnVal.SetPointer(new UtScriptRef(platformPtr, aReturnClassPtr));
}

// =================================================================================================
//! Returns the platform index of the platform that contains the reporting sensor.
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, OriginatorIndex, 0, "int", "")
{
   aReturnVal.SetInt(static_cast<int>(aObjectPtr->GetPlatformIndex()));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, OriginatorLocation, 0, "WsfGeoPoint", "")
{
   double locWCS[3];
   aObjectPtr->GetOriginatorLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SensorName, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorNameId().GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SensorType, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorTypeId().GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SensorMode, 0, "string", "")
{
   aReturnVal.SetString(aObjectPtr->GetSensorModeId().GetString());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, ImageTime, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetImageTime());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, ImageNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetImageNumber());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SetImageNumber, 1, "void", "int")
{
   unsigned int imageNumber(aVarArgs[0].GetInt());
   aObjectPtr->SetImageNumber(imageNumber);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, StreamNumber, 0, "int", "")
{
   aReturnVal.SetInt(aObjectPtr->GetStreamNumber());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SetStreamNumber, 1, "void", "int")
{
   unsigned int imageNumber(aVarArgs[0].GetInt());
   aObjectPtr->SetStreamNumber(imageNumber);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, CenterLocation, 0, "WsfGeoPoint", "")
{
   double locWCS[3];
   aObjectPtr->GetCenterLocationWCS(locWCS);
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, Width, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWidth());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, Height, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeight());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, WidthResolution, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetWidthResolution());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, HeightResolution, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetHeightResolution());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, Resolution, 0, "double", "") // NO_DOC | DEPRECATED
{
   aReturnVal.SetDouble(aObjectPtr->GetWidthResolution());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, NoiseLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetNoiseLevel());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, BackgroundLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetBackgroundLevel());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, MinimumLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMinimumLevel());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, MaximumLevel, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetMaximumLevel());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, CollectionFactor, 0, "double", "")
{
   aReturnVal.SetDouble(aObjectPtr->GetCollectionFactor());
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, ObjectCount, 0, "int", "")
{
   int objectCount = static_cast<int>(aObjectPtr->GetObjects().size());
   aReturnVal.SetInt(objectCount);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, Location, 1, "WsfGeoPoint", "int")
{
   double       locWCS[3]   = {0.0, 0.0, 0.0};
   unsigned int objectIndex = aVarArgs[0].GetInt();
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      aObjectPtr->GetObjects()[objectIndex].GetLocationWCS(locWCS);
   }
   WsfGeoPoint* pointPtr = new WsfGeoPoint(locWCS);
   aReturnVal.SetPointer(new UtScriptRef(pointPtr, aReturnClassPtr, UtScriptRef::cMANAGE));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, PixelCount, 1, "double", "int")
{
   double       pixelCount  = 0.0;
   unsigned int objectIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      pixelCount = aObjectPtr->GetObjects()[objectIndex].GetPixelCount();
   }
   aReturnVal.SetDouble(pixelCount);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, PixelIntensity, 1, "double", "int")
{
   double       pixelIntensity = 0.0;
   unsigned int objectIndex    = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      pixelIntensity = aObjectPtr->GetObjects()[objectIndex].GetPixelIntensity();
   }
   aReturnVal.SetDouble(pixelIntensity);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SignalLevel, 1, "double", "int")
{
   double       signal      = 0.0;
   unsigned int objectIndex = aVarArgs[0].GetInt();
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      signal = aObjectPtr->GetObjects()[objectIndex].GetSignalLevel();
   }
   aReturnVal.SetDouble(signal);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, SignalToNoise, 1, "double", "int") // NO_DOC | DEPRECATED
{
   double       signalToNoise = 0.0;
   unsigned int objectIndex   = aVarArgs[0].GetInt();
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      double noise  = aObjectPtr->GetNoiseLevel();
      noise         = (noise <= 0.0) ? 1.0 : noise;
      signalToNoise = aObjectPtr->GetObjects()[objectIndex].GetSignalLevel() / noise;
   }
   aReturnVal.SetDouble(signalToNoise);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, TruthIndex, 1, "int", "int")
{
   size_t truthIndex  = 0;
   size_t objectIndex = static_cast<size_t>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      truthIndex = aObjectPtr->GetObjects()[objectIndex].GetTruthIndex();
   }
   aReturnVal.SetInt(static_cast<int>(truthIndex));
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, TruthName, 1, "string", "int")
{
   WsfStringId  nameId;
   unsigned int objectIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      nameId = aObjectPtr->GetObjects()[objectIndex].GetTruthName(*WsfScriptContext::GetSIMULATION(aContext));
   }
   aReturnVal.SetString(nameId);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, TruthType, 1, "string", "int")
{
   WsfStringId  typeId;
   unsigned int objectIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      typeId = aObjectPtr->GetObjects()[objectIndex].GetTruthType();
   }
   aReturnVal.SetString(typeId);
}

// =================================================================================================
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, TruthSide, 1, "string", "int")
{
   WsfStringId  sideId;
   unsigned int objectIndex = static_cast<unsigned int>(aVarArgs[0].GetInt());
   if (objectIndex < aObjectPtr->GetObjects().size())
   {
      sideId = aObjectPtr->GetObjects()[objectIndex].GetTruthSide();
   }
   aReturnVal.SetString(sideId);
}

// =================================================================================================
//! double targetPixelCount = \<x\>.InImage(int aTargetIndex)
//! Uses the target index to determine if the platform is in the image.
//! Returns the pixel count leaving the decision of whether or not the
//! target is actually seen to the caller.
UT_DEFINE_SCRIPT_METHOD(WsfScriptImageClass, WsfImage, InImage, 1, "double", "int") // NO_DOC | DEPRECATED
{
   double nNumOfPixels_RetVal(0);
   size_t targetIndex(aVarArgs[0].GetInt());

   // Get the list of objects in the image
   WsfImage::ObjectList objectList = aObjectPtr->GetObjects();

   // Set up the iterators
   WsfImage::ObjectListIter objectIter    = objectList.begin();
   WsfImage::ObjectListIter objectIterEnd = objectList.end();

   // Loop through all the objects
   for (; objectIter != objectIterEnd; ++objectIter)
   {
      // For readability
      WsfImage::Object& object = *objectIter;

      // Get the truth index of the object
      size_t imageObjectIndex(object.GetTruthIndex());
      if (imageObjectIndex == targetIndex)
      {
         // Target is in the image
         // Return the pixel count
         nNumOfPixels_RetVal = object.GetPixelCount();
      }
   }

   aReturnVal.SetDouble(nNumOfPixels_RetVal);
}
