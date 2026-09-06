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
#ifndef Uci__Type__FileHeaderType_h
#define Uci__Type__FileHeaderType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CountryCodeType_h)
# include "uci/type/CountryCodeType.h"
#endif

#if !defined(Uci__Type__SAR_CollectionReasonEnum_h)
# include "uci/type/SAR_CollectionReasonEnum.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__FileDeclassificationEnum_h)
# include "uci/type/FileDeclassificationEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileHeaderType sequence accessor class */
      class FileHeaderType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FileHeaderType()
         { }

         /** Returns this accessor's type constant, i.e. FileHeaderType
           *
           * @return This accessor's type constant, i.e. FileHeaderType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileHeaderType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileHeaderType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum length: 0] [Maximum length: 11] [Pattern: "[A-Za-z0-9 ]{0,10}[!-~]$"] */
         typedef xs::String OriginatingStationID;

         /** [Minimum length: 0] [Maximum length: 80] [Pattern: "[ -~]{0,80}"] */
         typedef xs::String FileTitle;

         /** [White space: preserve] [Minimum length: 0] [Maximum length: 11] [Pattern: "[A-Za-z0-9 ]{0,11}"] */
         typedef xs::String OriginatorsName;

         /** [Minimum length: 0] [Maximum length: 11] [Pattern: "[0-9 ]{0,11}"] */
         typedef xs::String OriginatorsPhone;

         /** [Minimum length: 0] [Maximum length: 3] [White space: preserve] [Pattern: "[A-Z ]{2}[A-Z ]?"] */
         typedef xs::String FileClassificationSystem;

         /** [Minimum length: 0] [Maximum length: 16] [Pattern: "[ -~]{0,16}"] */
         typedef xs::String FileSecurityControlNumber;

         /** [Minimum length: 0] [Maximum length: 3] [White space: preserve] [Pattern: "[ -~]{0,3}"] */
         typedef xs::String FileControlAndHandling;

         /** [Minimum length: 0] [Maximum length: 2] [Pattern: "[ODM ]{0,2}"] */
         typedef xs::String FileClassificationAuthorityType;

         /** [Minimum length: 0] [Maximum length: 2] [Pattern: "[SCR ]{0,2}"] */
         typedef xs::String FileDowngrade;

         /** [Minimum length: 0] [Maximum length: 9] [Pattern: "[ -~]{0,9}"] */
         typedef xs::String FileSecuritySourceDate;

         /** Returns the value of the string data type that is identified by the CountryCode.
           *
           * @return The value of the string data type identified by CountryCode.
           */
         virtual const uci::type::CountryCodeType& getCountryCode() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the CountryCode.
           *
           * @return The value of the string data type identified by CountryCode.
           */
         virtual uci::type::CountryCodeType& getCountryCode()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setCountryCode(const uci::type::CountryCodeType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCountryCode(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the CountryCode to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setCountryCode(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CountryCode exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CountryCode is emabled or not
           */
         virtual bool hasCountryCode() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CountryCode
           *
           * @param type = uci::type::accessorType::countryCodeType This Accessor's accessor type
           */
         virtual void enableCountryCode(uci::base::accessorType::AccessorType type = uci::type::accessorType::countryCodeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CountryCode */
         virtual void clearCountryCode()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SceneSource.
           *
           * @return The value of the enumeration identified by SceneSource.
           */
         virtual const uci::type::SAR_CollectionReasonEnum& getSceneSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the SceneSource.
           *
           * @return The value of the enumeration identified by SceneSource.
           */
         virtual uci::type::SAR_CollectionReasonEnum& getSceneSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SceneSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSceneSource(const uci::type::SAR_CollectionReasonEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the SceneSource.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSceneSource(uci::type::SAR_CollectionReasonEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield SceneSourceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSceneSource() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSceneSource will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::sAR_CollectionReasonEnum This Accessor's accessor type
           */
         virtual void enableSceneSource(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_CollectionReasonEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSceneSource()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatingStationID.
           *
           * @return The value of the string data type identified by OriginatingStationID.
           */
         virtual const uci::type::FileHeaderType::OriginatingStationID& getOriginatingStationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatingStationID.
           *
           * @return The value of the string data type identified by OriginatingStationID.
           */
         virtual uci::type::FileHeaderType::OriginatingStationID& getOriginatingStationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatingStationID to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setOriginatingStationID(const uci::type::FileHeaderType::OriginatingStationID& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatingStationID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatingStationID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatingStationID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatingStationID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OriginatingStationID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OriginatingStationID is emabled or not
           */
         virtual bool hasOriginatingStationID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OriginatingStationID
           *
           * @param type = uci::type::fileHeaderType::accessorType::originatingStationID This Accessor's accessor type
           */
         virtual void enableOriginatingStationID(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::originatingStationID)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OriginatingStationID */
         virtual void clearOriginatingStationID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CollectionID.
           *
           * @return The value of the simple primitive data type identified by CollectionID.
           */
         virtual xs::UnsignedInt getCollectionID() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CollectionID.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCollectionID(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CollectionID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CollectionID is emabled or not
           */
         virtual bool hasCollectionID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CollectionID
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableCollectionID(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CollectionID */
         virtual void clearCollectionID()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileTitle.
           *
           * @return The value of the string data type identified by FileTitle.
           */
         virtual const uci::type::FileHeaderType::FileTitle& getFileTitle() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileTitle.
           *
           * @return The value of the string data type identified by FileTitle.
           */
         virtual uci::type::FileHeaderType::FileTitle& getFileTitle()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileTitle to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileTitle(const uci::type::FileHeaderType::FileTitle& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileTitle to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileTitle(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileTitle to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileTitle(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileTitle exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileTitle is emabled or not
           */
         virtual bool hasFileTitle() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileTitle
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileTitle This Accessor's accessor type
           */
         virtual void enableFileTitle(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileTitle)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileTitle */
         virtual void clearFileTitle()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorsName.
           *
           * @return The value of the string data type identified by OriginatorsName.
           */
         virtual const uci::type::FileHeaderType::OriginatorsName& getOriginatorsName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorsName.
           *
           * @return The value of the string data type identified by OriginatorsName.
           */
         virtual uci::type::FileHeaderType::OriginatorsName& getOriginatorsName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsName to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setOriginatorsName(const uci::type::FileHeaderType::OriginatorsName& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorsName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorsName(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OriginatorsName exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OriginatorsName is emabled or not
           */
         virtual bool hasOriginatorsName() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OriginatorsName
           *
           * @param type = uci::type::fileHeaderType::accessorType::originatorsName This Accessor's accessor type
           */
         virtual void enableOriginatorsName(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::originatorsName)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OriginatorsName */
         virtual void clearOriginatorsName()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorsPhone.
           *
           * @return The value of the string data type identified by OriginatorsPhone.
           */
         virtual const uci::type::FileHeaderType::OriginatorsPhone& getOriginatorsPhone() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the OriginatorsPhone.
           *
           * @return The value of the string data type identified by OriginatorsPhone.
           */
         virtual uci::type::FileHeaderType::OriginatorsPhone& getOriginatorsPhone()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsPhone to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setOriginatorsPhone(const uci::type::FileHeaderType::OriginatorsPhone& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsPhone to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorsPhone(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the OriginatorsPhone to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setOriginatorsPhone(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by OriginatorsPhone exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by OriginatorsPhone is emabled or not
           */
         virtual bool hasOriginatorsPhone() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by OriginatorsPhone
           *
           * @param type = uci::type::fileHeaderType::accessorType::originatorsPhone This Accessor's accessor type
           */
         virtual void enableOriginatorsPhone(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::originatorsPhone)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by OriginatorsPhone */
         virtual void clearOriginatorsPhone()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileClassificationSystem.
           *
           * @return The value of the string data type identified by FileClassificationSystem.
           */
         virtual const uci::type::FileHeaderType::FileClassificationSystem& getFileClassificationSystem() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileClassificationSystem.
           *
           * @return The value of the string data type identified by FileClassificationSystem.
           */
         virtual uci::type::FileHeaderType::FileClassificationSystem& getFileClassificationSystem()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationSystem to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileClassificationSystem(const uci::type::FileHeaderType::FileClassificationSystem& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileClassificationSystem(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationSystem to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileClassificationSystem(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileClassificationSystem exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileClassificationSystem is emabled or not
           */
         virtual bool hasFileClassificationSystem() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileClassificationSystem
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileClassificationSystem This Accessor's accessor type
           */
         virtual void enableFileClassificationSystem(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileClassificationSystem)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileClassificationSystem */
         virtual void clearFileClassificationSystem()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileSecurityControlNumber.
           *
           * @return The value of the string data type identified by FileSecurityControlNumber.
           */
         virtual const uci::type::FileHeaderType::FileSecurityControlNumber& getFileSecurityControlNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileSecurityControlNumber.
           *
           * @return The value of the string data type identified by FileSecurityControlNumber.
           */
         virtual uci::type::FileHeaderType::FileSecurityControlNumber& getFileSecurityControlNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecurityControlNumber to the string accessed by
           * the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileSecurityControlNumber(const uci::type::FileHeaderType::FileSecurityControlNumber& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecurityControlNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileSecurityControlNumber(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecurityControlNumber to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileSecurityControlNumber(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileSecurityControlNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileSecurityControlNumber is emabled or not
           */
         virtual bool hasFileSecurityControlNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileSecurityControlNumber
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileSecurityControlNumber This Accessor's accessor type
           */
         virtual void enableFileSecurityControlNumber(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileSecurityControlNumber)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileSecurityControlNumber */
         virtual void clearFileSecurityControlNumber()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileControlAndHandling.
           *
           * @return The value of the string data type identified by FileControlAndHandling.
           */
         virtual const uci::type::FileHeaderType::FileControlAndHandling& getFileControlAndHandling() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileControlAndHandling.
           *
           * @return The value of the string data type identified by FileControlAndHandling.
           */
         virtual uci::type::FileHeaderType::FileControlAndHandling& getFileControlAndHandling()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileControlAndHandling to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileControlAndHandling(const uci::type::FileHeaderType::FileControlAndHandling& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileControlAndHandling to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileControlAndHandling(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileControlAndHandling to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileControlAndHandling(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileControlAndHandling exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileControlAndHandling is emabled or not
           */
         virtual bool hasFileControlAndHandling() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileControlAndHandling
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileControlAndHandling This Accessor's accessor type
           */
         virtual void enableFileControlAndHandling(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileControlAndHandling)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileControlAndHandling */
         virtual void clearFileControlAndHandling()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FileDeclassificationType.
           *
           * @return The value of the enumeration identified by FileDeclassificationType.
           */
         virtual const uci::type::FileDeclassificationEnum& getFileDeclassificationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FileDeclassificationType.
           *
           * @return The value of the enumeration identified by FileDeclassificationType.
           */
         virtual uci::type::FileDeclassificationEnum& getFileDeclassificationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileDeclassificationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileDeclassificationType(const uci::type::FileDeclassificationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FileDeclassificationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFileDeclassificationType(uci::type::FileDeclassificationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FileDeclassificationTypeis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFileDeclassificationType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFileDeclassificationType will return the optional field and not throw an
           * exception when invoked.
           *
           * @param type = uci::type::accessorType::fileDeclassificationEnum This Accessor's accessor type
           */
         virtual void enableFileDeclassificationType(uci::base::accessorType::AccessorType type = uci::type::accessorType::fileDeclassificationEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFileDeclassificationType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileClassificationAuthorityType.
           *
           * @return The value of the string data type identified by FileClassificationAuthorityType.
           */
         virtual const uci::type::FileHeaderType::FileClassificationAuthorityType& getFileClassificationAuthorityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileClassificationAuthorityType.
           *
           * @return The value of the string data type identified by FileClassificationAuthorityType.
           */
         virtual uci::type::FileHeaderType::FileClassificationAuthorityType& getFileClassificationAuthorityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationAuthorityType to the string
           * accessed by the specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileClassificationAuthorityType(const uci::type::FileHeaderType::FileClassificationAuthorityType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationAuthorityType to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileClassificationAuthorityType(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileClassificationAuthorityType to the specified
           * string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileClassificationAuthorityType(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileClassificationAuthorityType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileClassificationAuthorityType is emabled or not
           */
         virtual bool hasFileClassificationAuthorityType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileClassificationAuthorityType
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileClassificationAuthorityType This Accessor's accessor type
           */
         virtual void enableFileClassificationAuthorityType(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileClassificationAuthorityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileClassificationAuthorityType */
         virtual void clearFileClassificationAuthorityType()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileDowngrade.
           *
           * @return The value of the string data type identified by FileDowngrade.
           */
         virtual const uci::type::FileHeaderType::FileDowngrade& getFileDowngrade() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileDowngrade.
           *
           * @return The value of the string data type identified by FileDowngrade.
           */
         virtual uci::type::FileHeaderType::FileDowngrade& getFileDowngrade()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileDowngrade to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileDowngrade(const uci::type::FileHeaderType::FileDowngrade& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileDowngrade to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileDowngrade(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileDowngrade to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileDowngrade(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileDowngrade exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileDowngrade is emabled or not
           */
         virtual bool hasFileDowngrade() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileDowngrade
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileDowngrade This Accessor's accessor type
           */
         virtual void enableFileDowngrade(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileDowngrade)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileDowngrade */
         virtual void clearFileDowngrade()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileSecuritySourceDate.
           *
           * @return The value of the string data type identified by FileSecuritySourceDate.
           */
         virtual const uci::type::FileHeaderType::FileSecuritySourceDate& getFileSecuritySourceDate() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileSecuritySourceDate.
           *
           * @return The value of the string data type identified by FileSecuritySourceDate.
           */
         virtual uci::type::FileHeaderType::FileSecuritySourceDate& getFileSecuritySourceDate()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecuritySourceDate to the string accessed by the
           * specified String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileSecuritySourceDate(const uci::type::FileHeaderType::FileSecuritySourceDate& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecuritySourceDate to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileSecuritySourceDate(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileSecuritySourceDate to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileSecuritySourceDate(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FileSecuritySourceDate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FileSecuritySourceDate is emabled or not
           */
         virtual bool hasFileSecuritySourceDate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FileSecuritySourceDate
           *
           * @param type = uci::type::fileHeaderType::accessorType::fileSecuritySourceDate This Accessor's accessor type
           */
         virtual void enableFileSecuritySourceDate(uci::base::accessorType::AccessorType type = uci::type::fileHeaderType::accessorType::fileSecuritySourceDate)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FileSecuritySourceDate */
         virtual void clearFileSecuritySourceDate()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileHeaderType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileHeaderType to copy from
           */
         FileHeaderType(const FileHeaderType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileHeaderType to the contents of the FileHeaderType on the right
           * hand side (rhs) of the assignment operator.FileHeaderType [only available to derived classes]
           *
           * @param rhs The FileHeaderType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileHeaderType
           * @return A constant reference to this FileHeaderType.
           */
         const FileHeaderType& operator=(const FileHeaderType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileHeaderType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileHeaderType_h

