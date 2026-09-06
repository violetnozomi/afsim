// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
// @warning  This file was automatically generated, edit at your own risk

/**
* Unclassified               U N C L A S S I F I E D               Unclassified
*
* DEVELOPMENT:
*    This document wholly developed with USG funds.
*    For additional information, contact the AFRCO.
*
* ‒  DISTRIBUTION STATEMENT D.  Distribution authorized to the Department
*    of Defense and U.S. DoD contractors only; Critical Technology; 17 Sep 2015.
*    Other requests shall be referred to Air Force Rapid Capabilities Office,
*    Bolling AFB, Washington DC 20032-6400. 
*
* EXPORT CONTROL:
*    WARNING - ITAR CONTROLLED - US ONLY
*    This distribution contains technical data whose export is restricted by
*    the Arms Export Control Act (Title 22, U.S.C., Sec. 2751 et seq. or the
*    Export Administration Act of 1979 as amended Title 50, U.S.C., App.
*    2401-2420 et seq.), as amended. Violation of these export laws are subject
*    to severe criminal penalties.  Disseminate in accordance with provisions of DoD
*    Directive 5230.25.
*/
#ifndef Uci__Type__POST_ComponentSettingsGeneratorSettingsType_h
#define Uci__Type__POST_ComponentSettingsGeneratorSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DoubleMinMaxType_h)
# include "uci/type/DoubleMinMaxType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocusSweepSettingsType_h)
# include "uci/type/PO_ComponentSettingsFocusSweepSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentSettingsGeneratorSettingsType sequence accessor class */
      class POST_ComponentSettingsGeneratorSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentSettingsGeneratorSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentSettingsGeneratorSettingsType
           *
           * @return This accessor's type constant, i.e. POST_ComponentSettingsGeneratorSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentSettingsGeneratorSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentSettingsGeneratorSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The new settings of the visible image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VisibleImageSetting;

         /** The new settings of the infrared image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> InfraredImageSetting;

         /** The new settings of the hyperspectral image generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> HyperspectralImageSetting;

         /** The new settings of the image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> ImageSetting;

         /** The new settings of the visible video clip generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoClipVisibleSetting;

         /** The new settings of the infrared video clip generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoClipInfraredSetting;

         /** The new settings of the hyperspectral video clip generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoClipHyperspectralSetting;

         /** The new settings of the video clip generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoClipSetting;

         /** The new settings of the visible video stream generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoStreamVisibleSetting;

         /** The new settings of the infrared video stream generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoStreamInfraredSetting;

         /** The new settings of the hyperspectral video stream generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoStreamHyperspectralSetting;

         /** The new settings of the stream generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DoubleMinMaxType, uci::type::accessorType::doubleMinMaxType> VideoStreamSetting;

         /** Returns the bounded list that is identified by the VisibleImageSetting.
           *
           * @return The bounded list identified by VisibleImageSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& getVisibleImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VisibleImageSetting.
           *
           * @return The bounded list identified by VisibleImageSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& getVisibleImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VisibleImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVisibleImageSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InfraredImageSetting.
           *
           * @return The bounded list identified by InfraredImageSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& getInfraredImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InfraredImageSetting.
           *
           * @return The bounded list identified by InfraredImageSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& getInfraredImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InfraredImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInfraredImageSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the HyperspectralImageSetting.
           *
           * @return The bounded list identified by HyperspectralImageSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::HyperspectralImageSetting& getHyperspectralImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the HyperspectralImageSetting.
           *
           * @return The bounded list identified by HyperspectralImageSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::HyperspectralImageSetting& getHyperspectralImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the HyperspectralImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setHyperspectralImageSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::HyperspectralImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ImageSetting.
           *
           * @return The bounded list identified by ImageSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::ImageSetting& getImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ImageSetting.
           *
           * @return The bounded list identified by ImageSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::ImageSetting& getImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setImageSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::ImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @return The bounded list identified by VideoClipVisibleSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& getVideoClipVisibleSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @return The bounded list identified by VideoClipVisibleSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& getVideoClipVisibleSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipVisibleSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @return The bounded list identified by VideoClipInfraredSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& getVideoClipInfraredSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @return The bounded list identified by VideoClipInfraredSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& getVideoClipInfraredSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipInfraredSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipHyperspectralSetting.
           *
           * @return The bounded list identified by VideoClipHyperspectralSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipHyperspectralSetting& getVideoClipHyperspectralSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipHyperspectralSetting.
           *
           * @return The bounded list identified by VideoClipHyperspectralSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipHyperspectralSetting& getVideoClipHyperspectralSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipHyperspectralSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipHyperspectralSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipHyperspectralSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipSetting.
           *
           * @return The bounded list identified by VideoClipSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipSetting& getVideoClipSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipSetting.
           *
           * @return The bounded list identified by VideoClipSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipSetting& getVideoClipSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoClipSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @return The bounded list identified by VideoStreamVisibleSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& getVideoStreamVisibleSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @return The bounded list identified by VideoStreamVisibleSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& getVideoStreamVisibleSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamVisibleSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @return The bounded list identified by VideoStreamInfraredSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& getVideoStreamInfraredSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @return The bounded list identified by VideoStreamInfraredSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& getVideoStreamInfraredSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamInfraredSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamHyperspectralSetting.
           *
           * @return The bounded list identified by VideoStreamHyperspectralSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamHyperspectralSetting& getVideoStreamHyperspectralSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamHyperspectralSetting.
           *
           * @return The bounded list identified by VideoStreamHyperspectralSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamHyperspectralSetting& getVideoStreamHyperspectralSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamHyperspectralSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamHyperspectralSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamHyperspectralSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamSetting.
           *
           * @return The bounded list identified by VideoStreamSetting.
           */
         virtual const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& getVideoStreamSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamSetting.
           *
           * @return The bounded list identified by VideoStreamSetting.
           */
         virtual uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& getVideoStreamSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamSetting(const uci::type::POST_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FocusSweepSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by FocusSweepSettings.
           */
         virtual const uci::type::PO_ComponentSettingsFocusSweepSettingsType& getFocusSweepSettings() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FocusSweepSettings.
           *
           * @return The acecssor that provides access to the complex content that is identified by FocusSweepSettings.
           */
         virtual uci::type::PO_ComponentSettingsFocusSweepSettingsType& getFocusSweepSettings()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FocusSweepSettings to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FocusSweepSettings
           */
         virtual void setFocusSweepSettings(const uci::type::PO_ComponentSettingsFocusSweepSettingsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FocusSweepSettings exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FocusSweepSettings is emabled or not
           */
         virtual bool hasFocusSweepSettings() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FocusSweepSettings
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsFocusSweepSettingsType This Accessor's accessor type
           */
         virtual void enableFocusSweepSettings(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsFocusSweepSettingsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FocusSweepSettings */
         virtual void clearFocusSweepSettings()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_ComponentSettingsGeneratorSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsGeneratorSettingsType to copy from
           */
         POST_ComponentSettingsGeneratorSettingsType(const POST_ComponentSettingsGeneratorSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentSettingsGeneratorSettingsType to the contents of the
           * POST_ComponentSettingsGeneratorSettingsType on the right hand side (rhs) of the assignment
           * operator.POST_ComponentSettingsGeneratorSettingsType [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsGeneratorSettingsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::POST_ComponentSettingsGeneratorSettingsType
           * @return A constant reference to this POST_ComponentSettingsGeneratorSettingsType.
           */
         const POST_ComponentSettingsGeneratorSettingsType& operator=(const POST_ComponentSettingsGeneratorSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentSettingsGeneratorSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentSettingsGeneratorSettingsType_h

