// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__MISP_PackingPlanType_h
#define Uci__Type__MISP_PackingPlanType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Type__TailNumberType_h)
# include "uci/type/TailNumberType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__MISP_OperationalModeEnum_h)
# include "uci/type/MISP_OperationalModeEnum.h"
#endif

#if !defined(Uci__Type__UMID_VideoID_Type_h)
# include "uci/type/UMID_VideoID_Type.h"
#endif

#if !defined(Uci__Type__UMID_AudioID_Type_h)
# include "uci/type/UMID_AudioID_Type.h"
#endif

#if !defined(Uci__Type__UMID_DataID_Type_h)
# include "uci/type/UMID_DataID_Type.h"
#endif

#if !defined(Uci__Type__UMID_SystemID_Type_h)
# include "uci/type/UMID_SystemID_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the MISP_PackingPlanType sequence accessor class */
      class MISP_PackingPlanType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~MISP_PackingPlanType()
         { }

         /** Returns this accessor's type constant, i.e. MISP_PackingPlanType
           *
           * @return This accessor's type constant, i.e. MISP_PackingPlanType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::mISP_PackingPlanType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const MISP_PackingPlanType& accessor)
            throw(uci::base::UCIException) = 0;


         /** The platform designation metadata item distinguishes which platform is carrying the motion imagery generating payload
           * equipment; [Minimum length: 1] [Maximum length: 20] [Pattern: "[A-Za-z0-9 \-_]{1,20}"]
           */
         typedef xs::String PlatformDesignation;

         /** Indicates the type of the image source.; [Minimum length: 1] [Maximum length: 20] [Pattern: "[ -~]{1,20}"] */
         typedef xs::String ImageSourceSensor;

         /** [Minimum length: 1] [Maximum length: 10] [Pattern: "[ -~]{1,10}"] */
         typedef xs::String PlatformCallSign;

         /** [Minimum length: 1] [Maximum length: 40] [Pattern: "[A-Za-z0-9 \-_]{1,40}"] */
         typedef xs::String ClassificationOrMarkingSystem;

         /** [Minimum length: 1] [Maximum length: 480] [Pattern: "[ -~]{1,480}"] */
         typedef xs::String ClassificationComment;

         /** [Minimum length: 1] [Maximum length: 16] [Pattern: "[A-Za-z0-9 \-_]{1,16}"] */
         typedef xs::String ItemDesignator;

         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual const uci::type::MissionID_Type& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual uci::type::MissionID_Type& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionID
           */
         virtual void setMissionID(const uci::type::MissionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformDesignation.
           *
           * @return The value of the string data type identified by PlatformDesignation.
           */
         virtual const uci::type::MISP_PackingPlanType::PlatformDesignation& getPlatformDesignation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformDesignation.
           *
           * @return The value of the string data type identified by PlatformDesignation.
           */
         virtual uci::type::MISP_PackingPlanType::PlatformDesignation& getPlatformDesignation()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformDesignation to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPlatformDesignation(const uci::type::MISP_PackingPlanType::PlatformDesignation& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformDesignation(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformDesignation to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformDesignation(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageSourceSensor.
           *
           * @return The value of the string data type identified by ImageSourceSensor.
           */
         virtual const uci::type::MISP_PackingPlanType::ImageSourceSensor& getImageSourceSensor() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ImageSourceSensor.
           *
           * @return The value of the string data type identified by ImageSourceSensor.
           */
         virtual uci::type::MISP_PackingPlanType::ImageSourceSensor& getImageSourceSensor()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSourceSensor to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setImageSourceSensor(const uci::type::MISP_PackingPlanType::ImageSourceSensor& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSourceSensor to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageSourceSensor(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ImageSourceSensor to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setImageSourceSensor(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformTailNumber.
           *
           * @return The value of the string data type identified by PlatformTailNumber.
           */
         virtual const uci::type::TailNumberType& getPlatformTailNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformTailNumber.
           *
           * @return The value of the string data type identified by PlatformTailNumber.
           */
         virtual uci::type::TailNumberType& getPlatformTailNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformTailNumber to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPlatformTailNumber(const uci::type::TailNumberType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformTailNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformTailNumber(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformTailNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformTailNumber(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformTailNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformTailNumber is emabled or not
           */
         virtual bool hasPlatformTailNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformTailNumber
           *
           * @param type = uci::type::accessorType::tailNumberType This Accessor's accessor type
           */
         virtual void enablePlatformTailNumber(uci::base::accessorType::AccessorType type = uci::type::accessorType::tailNumberType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformTailNumber */
         virtual void clearPlatformTailNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AirfieldBarometricPressure.
           *
           * @return The value of the simple primitive data type identified by AirfieldBarometricPressure.
           */
         virtual xs::Double getAirfieldBarometricPressure() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AirfieldBarometricPressure.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAirfieldBarometricPressure(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AirfieldBarometricPressure exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AirfieldBarometricPressure is emabled or not
           */
         virtual bool hasAirfieldBarometricPressure() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AirfieldBarometricPressure
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableAirfieldBarometricPressure(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AirfieldBarometricPressure */
         virtual void clearAirfieldBarometricPressure()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AirFieldElevation.
           *
           * @return The value of the simple primitive data type identified by AirFieldElevation.
           */
         virtual xs::Double getAirFieldElevation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AirFieldElevation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAirFieldElevation(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AirFieldElevation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AirFieldElevation is emabled or not
           */
         virtual bool hasAirFieldElevation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AirFieldElevation
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableAirFieldElevation(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AirFieldElevation */
         virtual void clearAirFieldElevation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformCallSign.
           *
           * @return The value of the string data type identified by PlatformCallSign.
           */
         virtual const uci::type::MISP_PackingPlanType::PlatformCallSign& getPlatformCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the PlatformCallSign.
           *
           * @return The value of the string data type identified by PlatformCallSign.
           */
         virtual uci::type::MISP_PackingPlanType::PlatformCallSign& getPlatformCallSign()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformCallSign to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPlatformCallSign(const uci::type::MISP_PackingPlanType::PlatformCallSign& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformCallSign(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the PlatformCallSign to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPlatformCallSign(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PlatformCallSign exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PlatformCallSign is emabled or not
           */
         virtual bool hasPlatformCallSign() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PlatformCallSign
           *
           * @param type = uci::type::mISP_PackingPlanType::accessorType::platformCallSign This Accessor's accessor type
           */
         virtual void enablePlatformCallSign(uci::base::accessorType::AccessorType type = uci::type::mISP_PackingPlanType::accessorType::platformCallSign)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PlatformCallSign */
         virtual void clearPlatformCallSign()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalMode.
           *
           * @return The value of the enumeration identified by OperationalMode.
           */
         virtual const uci::type::MISP_OperationalModeEnum& getOperationalMode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationalMode.
           *
           * @return The value of the enumeration identified by OperationalMode.
           */
         virtual uci::type::MISP_OperationalModeEnum& getOperationalMode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalMode(const uci::type::MISP_OperationalModeEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationalMode.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationalMode(uci::type::MISP_OperationalModeEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield OperationalModeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasOperationalMode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getOperationalMode will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::mISP_OperationalModeEnum This Accessor's accessor type
           */
         virtual void enableOperationalMode(uci::base::accessorType::AccessorType type = uci::type::accessorType::mISP_OperationalModeEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearOperationalMode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationOrMarkingSystem.
           *
           * @return The value of the string data type identified by ClassificationOrMarkingSystem.
           */
         virtual const uci::type::MISP_PackingPlanType::ClassificationOrMarkingSystem& getClassificationOrMarkingSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationOrMarkingSystem.
           *
           * @return The value of the string data type identified by ClassificationOrMarkingSystem.
           */
         virtual uci::type::MISP_PackingPlanType::ClassificationOrMarkingSystem& getClassificationOrMarkingSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationOrMarkingSystem to the string accessed
           * by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setClassificationOrMarkingSystem(const uci::type::MISP_PackingPlanType::ClassificationOrMarkingSystem& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationOrMarkingSystem to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationOrMarkingSystem(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationOrMarkingSystem to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationOrMarkingSystem(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClassificationOrMarkingSystem exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClassificationOrMarkingSystem is emabled or not
           */
         virtual bool hasClassificationOrMarkingSystem() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClassificationOrMarkingSystem
           *
           * @param type = uci::type::mISP_PackingPlanType::accessorType::classificationOrMarkingSystem This Accessor's accessor
           *      type
           */
         virtual void enableClassificationOrMarkingSystem(uci::base::accessorType::AccessorType type = uci::type::mISP_PackingPlanType::accessorType::classificationOrMarkingSystem)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClassificationOrMarkingSystem */
         virtual void clearClassificationOrMarkingSystem()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationComment.
           *
           * @return The value of the string data type identified by ClassificationComment.
           */
         virtual const uci::type::MISP_PackingPlanType::ClassificationComment& getClassificationComment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ClassificationComment.
           *
           * @return The value of the string data type identified by ClassificationComment.
           */
         virtual uci::type::MISP_PackingPlanType::ClassificationComment& getClassificationComment()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationComment to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setClassificationComment(const uci::type::MISP_PackingPlanType::ClassificationComment& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationComment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationComment(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ClassificationComment to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setClassificationComment(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClassificationComment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClassificationComment is emabled or not
           */
         virtual bool hasClassificationComment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClassificationComment
           *
           * @param type = uci::type::mISP_PackingPlanType::accessorType::classificationComment This Accessor's accessor type
           */
         virtual void enableClassificationComment(uci::base::accessorType::AccessorType type = uci::type::mISP_PackingPlanType::accessorType::classificationComment)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClassificationComment */
         virtual void clearClassificationComment()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_VideoID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_VideoID.
           */
         virtual const uci::type::UMID_VideoID_Type& getUMID_VideoID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_VideoID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_VideoID.
           */
         virtual uci::type::UMID_VideoID_Type& getUMID_VideoID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UMID_VideoID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UMID_VideoID
           */
         virtual void setUMID_VideoID(const uci::type::UMID_VideoID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UMID_VideoID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UMID_VideoID is emabled or not
           */
         virtual bool hasUMID_VideoID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UMID_VideoID
           *
           * @param type = uci::type::accessorType::uMID_VideoID_Type This Accessor's accessor type
           */
         virtual void enableUMID_VideoID(uci::base::accessorType::AccessorType type = uci::type::accessorType::uMID_VideoID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UMID_VideoID */
         virtual void clearUMID_VideoID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_AudioID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_AudioID.
           */
         virtual const uci::type::UMID_AudioID_Type& getUMID_AudioID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_AudioID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_AudioID.
           */
         virtual uci::type::UMID_AudioID_Type& getUMID_AudioID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UMID_AudioID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UMID_AudioID
           */
         virtual void setUMID_AudioID(const uci::type::UMID_AudioID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UMID_AudioID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UMID_AudioID is emabled or not
           */
         virtual bool hasUMID_AudioID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UMID_AudioID
           *
           * @param type = uci::type::accessorType::uMID_AudioID_Type This Accessor's accessor type
           */
         virtual void enableUMID_AudioID(uci::base::accessorType::AccessorType type = uci::type::accessorType::uMID_AudioID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UMID_AudioID */
         virtual void clearUMID_AudioID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_DataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_DataID.
           */
         virtual const uci::type::UMID_DataID_Type& getUMID_DataID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_DataID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_DataID.
           */
         virtual uci::type::UMID_DataID_Type& getUMID_DataID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UMID_DataID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UMID_DataID
           */
         virtual void setUMID_DataID(const uci::type::UMID_DataID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UMID_DataID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UMID_DataID is emabled or not
           */
         virtual bool hasUMID_DataID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UMID_DataID
           *
           * @param type = uci::type::accessorType::uMID_DataID_Type This Accessor's accessor type
           */
         virtual void enableUMID_DataID(uci::base::accessorType::AccessorType type = uci::type::accessorType::uMID_DataID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UMID_DataID */
         virtual void clearUMID_DataID()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_SystemID.
           */
         virtual const uci::type::UMID_SystemID_Type& getUMID_SystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the UMID_SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by UMID_SystemID.
           */
         virtual uci::type::UMID_SystemID_Type& getUMID_SystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the UMID_SystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by UMID_SystemID
           */
         virtual void setUMID_SystemID(const uci::type::UMID_SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UMID_SystemID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UMID_SystemID is emabled or not
           */
         virtual bool hasUMID_SystemID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UMID_SystemID
           *
           * @param type = uci::type::accessorType::uMID_SystemID_Type This Accessor's accessor type
           */
         virtual void enableUMID_SystemID(uci::base::accessorType::AccessorType type = uci::type::accessorType::uMID_SystemID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UMID_SystemID */
         virtual void clearUMID_SystemID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ItemDesignator.
           *
           * @return The value of the string data type identified by ItemDesignator.
           */
         virtual const uci::type::MISP_PackingPlanType::ItemDesignator& getItemDesignator() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the ItemDesignator.
           *
           * @return The value of the string data type identified by ItemDesignator.
           */
         virtual uci::type::MISP_PackingPlanType::ItemDesignator& getItemDesignator()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ItemDesignator to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setItemDesignator(const uci::type::MISP_PackingPlanType::ItemDesignator& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ItemDesignator to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setItemDesignator(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the ItemDesignator to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setItemDesignator(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ItemDesignator exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ItemDesignator is emabled or not
           */
         virtual bool hasItemDesignator() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ItemDesignator
           *
           * @param type = uci::type::mISP_PackingPlanType::accessorType::itemDesignator This Accessor's accessor type
           */
         virtual void enableItemDesignator(uci::base::accessorType::AccessorType type = uci::type::mISP_PackingPlanType::accessorType::itemDesignator)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ItemDesignator */
         virtual void clearItemDesignator()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         MISP_PackingPlanType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The MISP_PackingPlanType to copy from
           */
         MISP_PackingPlanType(const MISP_PackingPlanType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this MISP_PackingPlanType to the contents of the MISP_PackingPlanType
           * on the right hand side (rhs) of the assignment operator.MISP_PackingPlanType [only available to derived classes]
           *
           * @param rhs The MISP_PackingPlanType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::MISP_PackingPlanType
           * @return A constant reference to this MISP_PackingPlanType.
           */
         const MISP_PackingPlanType& operator=(const MISP_PackingPlanType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // MISP_PackingPlanType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__MISP_PackingPlanType_h

