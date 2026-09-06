// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__CommDataType_h
#define Uci__Type__CommDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CommResourceType_h)
# include "uci/type/CommResourceType.h"
#endif

#if !defined(Uci__Type__CommCapabilityEnum_h)
# include "uci/type/CommCapabilityEnum.h"
#endif

#if !defined(Uci__Type__DataRateType_h)
# include "uci/type/DataRateType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

#if !defined(Uci__Type__CommSpecificRequestDataType_h)
# include "uci/type/CommSpecificRequestDataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CommDataType sequence accessor class */
      class CommDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CommDataType()
         { }

         /** Returns this accessor's type constant, i.e. CommDataType
           *
           * @return This accessor's type constant, i.e. CommDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::commDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CommDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Resource.
           *
           * @return The acecssor that provides access to the complex content that is identified by Resource.
           */
         virtual const uci::type::CommResourceType& getResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Resource.
           *
           * @return The acecssor that provides access to the complex content that is identified by Resource.
           */
         virtual uci::type::CommResourceType& getResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Resource to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Resource
           */
         virtual void setResource(const uci::type::CommResourceType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommType.
           *
           * @return The value of the enumeration identified by CommType.
           */
         virtual const uci::type::CommCapabilityEnum& getCommType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommType.
           *
           * @return The value of the enumeration identified by CommType.
           */
         virtual uci::type::CommCapabilityEnum& getCommType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommType(const uci::type::CommCapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommType(uci::type::CommCapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UplinkDataRate.
           *
           * @return The value of the simple primitive data type identified by UplinkDataRate.
           */
         virtual uci::type::DataRateTypeValue getUplinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UplinkDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUplinkDataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UplinkDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UplinkDataRate is emabled or not
           */
         virtual bool hasUplinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UplinkDataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableUplinkDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UplinkDataRate */
         virtual void clearUplinkDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UplinkFrequency.
           *
           * @return The value of the simple primitive data type identified by UplinkFrequency.
           */
         virtual uci::type::FrequencyTypeValue getUplinkFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UplinkFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUplinkFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UplinkFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UplinkFrequency is emabled or not
           */
         virtual bool hasUplinkFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UplinkFrequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableUplinkFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UplinkFrequency */
         virtual void clearUplinkFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DownlinkDataRate.
           *
           * @return The value of the simple primitive data type identified by DownlinkDataRate.
           */
         virtual uci::type::DataRateTypeValue getDownlinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DownlinkDataRate.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDownlinkDataRate(uci::type::DataRateTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DownlinkDataRate exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DownlinkDataRate is emabled or not
           */
         virtual bool hasDownlinkDataRate() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DownlinkDataRate
           *
           * @param type = uci::type::accessorType::dataRateType This Accessor's accessor type
           */
         virtual void enableDownlinkDataRate(uci::base::accessorType::AccessorType type = uci::type::accessorType::dataRateType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DownlinkDataRate */
         virtual void clearDownlinkDataRate()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DownlinkFrequency.
           *
           * @return The value of the simple primitive data type identified by DownlinkFrequency.
           */
         virtual uci::type::FrequencyTypeValue getDownlinkFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DownlinkFrequency.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDownlinkFrequency(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DownlinkFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DownlinkFrequency is emabled or not
           */
         virtual bool hasDownlinkFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DownlinkFrequency
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableDownlinkFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DownlinkFrequency */
         virtual void clearDownlinkFrequency()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSpecificData.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSpecificData.
           */
         virtual const uci::type::CommSpecificRequestDataType& getCommSpecificData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommSpecificData.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommSpecificData.
           */
         virtual uci::type::CommSpecificRequestDataType& getCommSpecificData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommSpecificData to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommSpecificData
           */
         virtual void setCommSpecificData(const uci::type::CommSpecificRequestDataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommSpecificData exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommSpecificData is emabled or not
           */
         virtual bool hasCommSpecificData() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommSpecificData
           *
           * @param type = uci::type::accessorType::commSpecificRequestDataType This Accessor's accessor type
           */
         virtual void enableCommSpecificData(uci::base::accessorType::AccessorType type = uci::type::accessorType::commSpecificRequestDataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommSpecificData */
         virtual void clearCommSpecificData()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CommDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CommDataType to copy from
           */
         CommDataType(const CommDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CommDataType to the contents of the CommDataType on the right hand
           * side (rhs) of the assignment operator.CommDataType [only available to derived classes]
           *
           * @param rhs The CommDataType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::CommDataType
           * @return A constant reference to this CommDataType.
           */
         const CommDataType& operator=(const CommDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CommDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CommDataType_h

