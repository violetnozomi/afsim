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
#ifndef Uci__Type__PO_ComponentSettingsGeneratorSettingsType_h
#define Uci__Type__PO_ComponentSettingsGeneratorSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsFocusSweepSettingsType_h)
# include "uci/type/PO_ComponentSettingsFocusSweepSettingsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentSettingsGeneratorSettingsType sequence accessor class */
      class PO_ComponentSettingsGeneratorSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsGeneratorSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentSettingsGeneratorSettingsType
           *
           * @return This accessor's type constant, i.e. PO_ComponentSettingsGeneratorSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentSettingsGeneratorSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentSettingsGeneratorSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The new settings of the visible image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VisibleImageSetting;

         /** The new settings of the infrared image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> InfraredImageSetting;

         /** The new settings of the image generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> ImageSetting;

         /** The new settings of the visible video clip generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoClipVisibleSetting;

         /** The new settings of the infrared video clip generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoClipInfraredSetting;

         /** The new settings of the video clip generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoClipSetting;

         /** The new settings of the visible video stream generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoStreamVisibleSetting;

         /** The new settings of the infrared video stream generator. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoStreamInfraredSetting;

         /** The new settings of the stream generator. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> VideoStreamSetting;

         /** Returns the bounded list that is identified by the VisibleImageSetting.
           *
           * @return The bounded list identified by VisibleImageSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& getVisibleImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VisibleImageSetting.
           *
           * @return The bounded list identified by VisibleImageSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& getVisibleImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VisibleImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVisibleImageSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VisibleImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InfraredImageSetting.
           *
           * @return The bounded list identified by InfraredImageSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& getInfraredImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the InfraredImageSetting.
           *
           * @return The bounded list identified by InfraredImageSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& getInfraredImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the InfraredImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setInfraredImageSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::InfraredImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ImageSetting.
           *
           * @return The bounded list identified by ImageSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::ImageSetting& getImageSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ImageSetting.
           *
           * @return The bounded list identified by ImageSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::ImageSetting& getImageSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ImageSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setImageSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::ImageSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @return The bounded list identified by VideoClipVisibleSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& getVideoClipVisibleSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @return The bounded list identified by VideoClipVisibleSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& getVideoClipVisibleSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipVisibleSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipVisibleSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipVisibleSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @return The bounded list identified by VideoClipInfraredSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& getVideoClipInfraredSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @return The bounded list identified by VideoClipInfraredSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& getVideoClipInfraredSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipInfraredSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipInfraredSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipInfraredSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipSetting.
           *
           * @return The bounded list identified by VideoClipSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipSetting& getVideoClipSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoClipSetting.
           *
           * @return The bounded list identified by VideoClipSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipSetting& getVideoClipSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoClipSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoClipSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoClipSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @return The bounded list identified by VideoStreamVisibleSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& getVideoStreamVisibleSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @return The bounded list identified by VideoStreamVisibleSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& getVideoStreamVisibleSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamVisibleSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamVisibleSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamVisibleSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @return The bounded list identified by VideoStreamInfraredSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& getVideoStreamInfraredSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @return The bounded list identified by VideoStreamInfraredSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& getVideoStreamInfraredSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamInfraredSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamInfraredSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamInfraredSetting& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamSetting.
           *
           * @return The bounded list identified by VideoStreamSetting.
           */
         virtual const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& getVideoStreamSetting() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the VideoStreamSetting.
           *
           * @return The bounded list identified by VideoStreamSetting.
           */
         virtual uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& getVideoStreamSetting()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the VideoStreamSetting.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setVideoStreamSetting(const uci::type::PO_ComponentSettingsGeneratorSettingsType::VideoStreamSetting& value)
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
         PO_ComponentSettingsGeneratorSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsGeneratorSettingsType to copy from
           */
         PO_ComponentSettingsGeneratorSettingsType(const PO_ComponentSettingsGeneratorSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsGeneratorSettingsType to the contents of the
           * PO_ComponentSettingsGeneratorSettingsType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentSettingsGeneratorSettingsType [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsGeneratorSettingsType on the right hand side (rhs) of the assignment operator
           *      whose contents are used to set the contents of this uci::type::PO_ComponentSettingsGeneratorSettingsType
           * @return A constant reference to this PO_ComponentSettingsGeneratorSettingsType.
           */
         const PO_ComponentSettingsGeneratorSettingsType& operator=(const PO_ComponentSettingsGeneratorSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsGeneratorSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentSettingsGeneratorSettingsType_h

