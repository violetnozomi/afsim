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

#ifndef WSFIMAGE_HPP
#define WSFIMAGE_HPP

#include "wsf_export.h"

#include <memory>
#include <string>
#include <vector>

#include "UtScriptAccessible.hpp"
class UtScriptClass;
class UtScriptTypes;

class WsfPlatform;
class WsfProcessor;
class WsfSensor;
class WsfSimulation;
#include "WsfStringId.hpp"

//! An object that represents an 'image' (picture) from a sensor.
//!
//! The creator/maintainer of a image should create an instance of the image
//! using the proper constructor to identify its source source.
class WSF_EXPORT WsfImage : public UtScriptAccessible
{
public:
   //! A nested class that represents the appearance of a single object (platform) in the image.
   class WSF_EXPORT Object
   {
   public:
      Object() = default;
      Object(WsfPlatform* aPlatformPtr);

      //! Get/Set the perceived location of the object in WCS.
      void GetLocationWCS(double aLocationWCS[3]) const;
      void SetLocationWCS(const double aLocationWCS[3]);

      //! Get/Set the signal level (absolute, not dB).
      double GetSignalLevel() const { return mSignalLevel; }
      void   SetSignalLevel(double aSignalLevel) { mSignalLevel = aSignalLevel; }

      //! Get/Set the approximate number of pixels occupied by this object.
      double GetPixelCount() const { return mPixelCount; }
      void   SetPixelCount(double aPixelCount) { mPixelCount = aPixelCount; }

      //! Get/Set the approximate pixel intensity of the object.
      //! This assumes that the occupied pixels are uniformly lit.
      double GetPixelIntensity() const { return mPixelIntensity; }
      void   SetPixelIntensity(double aPixelIntensity) { mPixelIntensity = aPixelIntensity; }

      //! Get/Set the truth platform index of the platform representing this object.
      size_t GetTruthIndex() const { return mTruthIndex; }
      void   SetTruthIndex(unsigned int aTruthIndex) { mTruthIndex = aTruthIndex; }

      WsfStringId GetTruthName(WsfSimulation& aSimulation) const;

      //! Get/Set the truth type ID of the platform representing this object.
      WsfStringId GetTruthType() const { return mTruthType; }
      void        SetTruthType(WsfStringId aTruthType) { mTruthType = aTruthType; }

      //! Get/Set the truth side of the platform representing this object.
      WsfStringId GetTruthSide() const { return mTruthSide; }
      void        SetTruthSide(WsfStringId aTruthSide) { mTruthSide = aTruthSide; }

   private:
      double      mLocationWCS[3]{0.0, 0.0, 0.0};
      double      mSignalLevel{0.0};
      double      mPixelCount{1.0};
      double      mPixelIntensity{1.0};
      size_t      mTruthIndex{0};
      WsfStringId mTruthType{nullptr};
      WsfStringId mTruthSide{nullptr};
   };

   using ObjectList     = std::vector<Object>;
   using ObjectListIter = std::vector<Object>::iterator;

   WsfImage() = default;
   WsfImage(WsfSensor* aOriginatingSensorPtr);

   virtual ~WsfImage() = default;

   virtual WsfImage* Clone() const;

   const char* GetScriptClassName() const override { return "WsfImage"; }

   static std::unique_ptr<UtScriptClass> CreateScriptClass(const std::string& aClassName, UtScriptTypes* aScriptTypesPtr);

   //! Get/Set the message data tag used for tagging the message.
   double GetMessageDataTag() const { return mMessageDataTag; }

   //! Set the 'data Id' to be used when the track is attached to a message.
   void SetMessageDataTag(double aMessageDataTag) { mMessageDataTag = aMessageDataTag; }

   //! Get/Set the declared message length (in bits) to be used when sending
   //! this image over the communications network.
   //!
   //! @note This is NOT the  physical length of this object as it contains just about everything
   //! that any sensor would want to send.  The length used by these methods should be the 'logical'
   //! length that a real-life communications message would have reporting a image from this sensor.
   int  GetMessageLength() const { return mMessageLength; }
   void SetMessageLength(int aMessageLength) { mMessageLength = aMessageLength; }

   //! Get/Set the location of the sensor.
   void GetOriginatorLocationWCS(double aOriginatorLocationWCS[3]) const;
   void SetOriginatorLocationWCS(const double aOriginatorLocationWCS[3]);

   //! Get/Set the approximate location of the center of the image.
   void GetCenterLocationWCS(double aCenterLocationWCS[3]) const;
   void SetCenterLocationWCS(const double aCenterLocationWCS[3]);

   //! Get/Set the simulation when time the image was taken.
   double GetImageTime() const { return mImageTime; }
   void   SetImageTime(double aImageTime) { mImageTime = aImageTime; }

   //! Get/Set the image number of image within a stream of images.
   //! For a still image (photograph, SAR image, etc) this will be 0.
   //! For a video stream, the first frame will be 1 and will be incremented
   //! for each subsequent frame.
   //!
   //! @note It is the zero/non-zero value that denotes if this image
   //! is a still or a frame within a video stream.
   unsigned int GetImageNumber() const { return mImageNumber; }
   void         SetImageNumber(unsigned int aImageNumber) { mImageNumber = aImageNumber; }

   //! Get/Set the stream number of which the image is a part.
   //! This number in incremented each time the producing sensor is turned-on.
   unsigned int GetStreamNumber() const { return mStreamNumber; }
   void         SetStreamNumber(unsigned int aStreamNumber) { mStreamNumber = aStreamNumber; }

   //! Get the platform index of the platform that contains the reporting sensor.
   size_t GetPlatformIndex() const { return mPlatformIndex; }

   //! Get the name (ID) of the platform with the sensor that produced this image.
   WsfStringId GetPlatformNameId() const { return mPlatformNameId; }

   //! Get the type (ID) of the platform with the sensor that produced this image.
   WsfStringId GetPlatformTypeId() const { return mPlatformTypeId; }

   //! Get the name (ID) of the sensor that produced this image.
   WsfStringId GetSensorNameId() const { return mSensorNameId; }

   //! Get the type (ID) of the sensor that produced this image.
   WsfStringId GetSensorTypeId() const { return mSensorTypeId; }

   //! Get the name (ID) of the mode of the sensor that was active when this image was produced.
   WsfStringId GetSensorModeId() const { return mSensorModeId; }

   //! Get/Set the frame time for an image stream.
   //! The frame time is zero for static images.
   double GetFrameTime() const { return mFrameTime; }
   void   SetFrameTime(double aFrameTime) { mFrameTime = aFrameTime; }

   //! Get/Set the width of the image (pixels)
   double GetWidth() const { return mWidth; }
   void   SetWidth(double aWidth) { mWidth = aWidth; }

   //! Get/Set the height of the image (pixels)
   double GetHeight() const { return mHeight; }
   void   SetHeight(double aHeight) { mHeight = aHeight; }

   //! Get/Set the width resolution of a pixel (m).
   double GetWidthResolution() const { return mWidthResolution; }
   void   SetWidthResolution(double aWidthResolution) { mWidthResolution = aWidthResolution; }

   //! Get/Set the height resolution of a pixel (m).
   double GetHeightResolution() const { return mHeightResolution; }
   void   SetHeightResolution(double aHeightResolution) { mHeightResolution = aHeightResolution; }

   //! Get/Set the noise signal level.
   double GetNoiseLevel() const { return mNoiseLevel; }
   void   SetNoiseLevel(double aNoiseLevel) { mNoiseLevel = aNoiseLevel; }

   //! Get/Set the background signal level.
   //! For a SAR sensor this would be the clutter (CNR * N)
   double GetBackgroundLevel() const { return mBackgroundLevel; }
   void   SetBackgroundLevel(double aBackgroundLevel) { mBackgroundLevel = aBackgroundLevel; }

   //! Get/Set the signal value corresponding to a pixel intensity of 0.0
   double GetMinimumLevel() const { return mMinimumLevel; }
   void   SetMinimumLevel(double aMinimumLevel) { mMinimumLevel = aMinimumLevel; }

   //! Get/Set the signal value corresponding to a pixel intensity of 1.0
   double GetMaximumLevel() const { return mMaximumLevel; }
   void   SetMaximumLevel(double aMaximumLevel) { mMaximumLevel = aMaximumLevel; }

   //! @name Methods for sensor measurement errors
   //@{
   //! Get the standard deviation of the error in the range measurement.
   double GetRangeError() const { return mRangeError; }
   //! Set the standard deviation of the error in the range measurement.
   void SetRangeError(double aRangeError) { mRangeError = aRangeError; }
   //! Get the standard deviation of the error in the bearing measurement.
   double GetBearingError() const { return mBearingError; }
   //! Set the standard deviation of the error in the bearing measurement.
   void SetBearingError(double aBearingError) { mBearingError = aBearingError; }
   //! Get the standard deviation of the error in the elevation measurement.
   double GetElevationError() const { return mElevationError; }
   //! Set the standard deviation of the error in the elevation measurement.
   void SetElevationError(double aElevationError) { mElevationError = aElevationError; }
   //! Get the standard deviation of the error in the range rate measurement.
   double GetRangeRateError() const { return mRangeRateError; }
   //! Set the standard deviation of the error in the range rate measurement.
   void SetRangeRateError(double aRangeRateError) { mRangeRateError = aRangeRateError; }
   //@}

   //! @name Methods for the 'collection factor' attribute.
   //!
   //! The 'collection factor' is typically only used by the SAR sensor, and reflects the achieved
   //! quality (resolution) compared to what was requested. For non-SAR sensors this value will be
   //! 1. For SAR sensors it will be the ratio of the desired azimuth resolution over the actual
   //! azimuth resolution.
   //@{
   //! Get the collection factor.
   double GetCollectionFactor() const { return mCollectionFactor; }
   //! Set the collection factor.
   void SetCollectionFactor(double aCollectionFactor) { mCollectionFactor = aCollectionFactor; }
   //@}

   //! @name Methods for 'track quality' stored in the mode of the sensor that produced the image.
   //@{
   //! Get the track quality from the mode of the sensor that produced the image.
   double GetTrackQuality() const { return mTrackQuality; }
   //! Set the track quality from the mode of the sensor that produced the image.
   void SetTrackQuality(double aTrackQuality) { mTrackQuality = aTrackQuality; }
   //@}

   //! @name Methods for storing or retrieve objects in the image.
   //@{
   //! Add an object to list of those contained in the image.
   void AddObject(Object& aObject) { mObjects.push_back(aObject); }

   //! Get the list of objects contained in the image.
   const ObjectList& GetObjects() const { return mObjects; }
   //@}

private:
   double      mMessageDataTag{0.0};
   int         mMessageLength{512};
   size_t      mPlatformIndex{0}; //!< Index of the platform that owns the sensor
   WsfStringId mPlatformNameId{nullptr};
   WsfStringId mPlatformTypeId{nullptr};
   WsfStringId mSensorNameId{nullptr};
   WsfStringId mSensorTypeId{nullptr};
   WsfStringId mSensorModeId{nullptr};
   //! For an image stream, the anticipated time between frames.
   //! (i.e.: the frame_time of the producing sensor).
   double mFrameTime{0.0};
   //! The time when the image was produced.
   double mImageTime{0.0};
   //! The number of the image within the stream.
   //! This will be 0 for static images (spot SAR, photograph).
   unsigned int mImageNumber{0};
   //! The stream number or, for static images, the number of the image.
   unsigned int mStreamNumber{0};

   double mOriginatorLocationWCS[3]{0.0, 0.0, 0.0}; //!< The location of the originator at the time of the report
   double mCenterLocationWCS[3]{0.0, 0.0, 0.0};     //!< The approximate location of the center of the image.

   double mWidth{0.0};            //!< The width of the image (in pixels)
   double mHeight{0.0};           //!< The height of the image (in pixels)
   double mWidthResolution{0.0};  //!< The resolution of a pixel in width (m)
   double mHeightResolution{0.0}; //!< The resolution of a pixel in height (m)
   double mNoiseLevel{0.0};       //!< The 'noise' level.
   double mBackgroundLevel{0.0};  //!< The signal of the background (CNR * N for SAR)
   double mMinimumLevel{0.0};     //!< The signal level corresponding to a pixel intensity of 0.0
   double mMaximumLevel{1.0};     //!< The signal level corresponding to a pixel intensity of 1.0
   double mCollectionFactor{1.0}; //!< Ratio of the achieved resolution vs. the desired resolution
   double mTrackQuality{0.5};     //!< The track quality from the sensor mode that created the image.

   //! The measurement errors from the sensor that created the image
   //@{
   double mRangeError{0.0};
   double mBearingError{0.0};
   double mElevationError{0.0};
   double mRangeRateError{0.0};
   //@}

   ObjectList mObjects;
};

#endif
