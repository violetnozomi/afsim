// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__SubCapabilityDetailsType_h
#define Uci__Type__SubCapabilityDetailsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SelectAntennaType_h)
# include "uci/type/SelectAntennaType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__ESM_TargetType_h)
# include "uci/type/ESM_TargetType.h"
#endif

#if !defined(Uci__Type__ESM_LocationType_h)
# include "uci/type/ESM_LocationType.h"
#endif

#if !defined(Uci__Type__PulseDataCollectCommandType_h)
# include "uci/type/PulseDataCollectCommandType.h"
#endif

#if !defined(Uci__Type__ESM_SubcapabilityGeolocationType_h)
# include "uci/type/ESM_SubcapabilityGeolocationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubCapabilityDetailsType sequence accessor class */
      class SubCapabilityDetailsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubCapabilityDetailsType()
         { }

         /** Returns this accessor's type constant, i.e. SubCapabilityDetailsType
           *
           * @return This accessor's type constant, i.e. SubCapabilityDetailsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subCapabilityDetailsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubCapabilityDetailsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a specific frequency to be used for the Acquisition command. If omitted, the command defers to the active
           * MDF. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> FrequencyRestriction;

         /** Indicates the source of or explicit values for emitter characteristics of the target of the ESM command. If omitted,
           * the command defers to the active MDF. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ESM_TargetType, uci::type::accessorType::eSM_TargetType> TargetEmitterData;

         /** Returns the accessor that provides access to the complex content that is identified by the SelectAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectAntenna.
           */
         virtual const uci::type::SelectAntennaType& getSelectAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SelectAntenna.
           *
           * @return The acecssor that provides access to the complex content that is identified by SelectAntenna.
           */
         virtual uci::type::SelectAntennaType& getSelectAntenna()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SelectAntenna to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SelectAntenna
           */
         virtual void setSelectAntenna(const uci::type::SelectAntennaType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SelectAntenna exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SelectAntenna is emabled or not
           */
         virtual bool hasSelectAntenna() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SelectAntenna
           *
           * @param type = uci::type::accessorType::selectAntennaType This Accessor's accessor type
           */
         virtual void enableSelectAntenna(uci::base::accessorType::AccessorType type = uci::type::accessorType::selectAntennaType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SelectAntenna */
         virtual void clearSelectAntenna()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FrequencyRestriction.
           *
           * @return The bounded list identified by FrequencyRestriction.
           */
         virtual const uci::type::SubCapabilityDetailsType::FrequencyRestriction& getFrequencyRestriction() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FrequencyRestriction.
           *
           * @return The bounded list identified by FrequencyRestriction.
           */
         virtual uci::type::SubCapabilityDetailsType::FrequencyRestriction& getFrequencyRestriction()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FrequencyRestriction.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequencyRestriction(const uci::type::SubCapabilityDetailsType::FrequencyRestriction& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetEmitterData.
           *
           * @return The bounded list identified by TargetEmitterData.
           */
         virtual const uci::type::SubCapabilityDetailsType::TargetEmitterData& getTargetEmitterData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TargetEmitterData.
           *
           * @return The bounded list identified by TargetEmitterData.
           */
         virtual uci::type::SubCapabilityDetailsType::TargetEmitterData& getTargetEmitterData()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TargetEmitterData.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTargetEmitterData(const uci::type::SubCapabilityDetailsType::TargetEmitterData& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ESM_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM_Location.
           */
         virtual const uci::type::ESM_LocationType& getESM_Location() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ESM_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by ESM_Location.
           */
         virtual uci::type::ESM_LocationType& getESM_Location()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ESM_Location to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ESM_Location
           */
         virtual void setESM_Location(const uci::type::ESM_LocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ESM_Location exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ESM_Location is emabled or not
           */
         virtual bool hasESM_Location() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ESM_Location
           *
           * @param type = uci::type::accessorType::eSM_LocationType This Accessor's accessor type
           */
         virtual void enableESM_Location(uci::base::accessorType::AccessorType type = uci::type::accessorType::eSM_LocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ESM_Location */
         virtual void clearESM_Location()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseDataCollection.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseDataCollection.
           */
         virtual const uci::type::PulseDataCollectCommandType& getPulseDataCollection() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseDataCollection.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseDataCollection.
           */
         virtual uci::type::PulseDataCollectCommandType& getPulseDataCollection()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PulseDataCollection to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PulseDataCollection
           */
         virtual void setPulseDataCollection(const uci::type::PulseDataCollectCommandType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseDataCollection exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseDataCollection is emabled or not
           */
         virtual bool hasPulseDataCollection() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseDataCollection
           *
           * @param type = uci::type::accessorType::pulseDataCollectCommandType This Accessor's accessor type
           */
         virtual void enablePulseDataCollection(uci::base::accessorType::AccessorType type = uci::type::accessorType::pulseDataCollectCommandType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseDataCollection */
         virtual void clearPulseDataCollection()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geolocation.
           */
         virtual const uci::type::ESM_SubcapabilityGeolocationType& getGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Geolocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Geolocation.
           */
         virtual uci::type::ESM_SubcapabilityGeolocationType& getGeolocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Geolocation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Geolocation
           */
         virtual void setGeolocation(const uci::type::ESM_SubcapabilityGeolocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Geolocation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Geolocation is emabled or not
           */
         virtual bool hasGeolocation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Geolocation
           *
           * @param type = uci::type::accessorType::eSM_SubcapabilityGeolocationType This Accessor's accessor type
           */
         virtual void enableGeolocation(uci::base::accessorType::AccessorType type = uci::type::accessorType::eSM_SubcapabilityGeolocationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Geolocation */
         virtual void clearGeolocation()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubCapabilityDetailsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubCapabilityDetailsType to copy from
           */
         SubCapabilityDetailsType(const SubCapabilityDetailsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubCapabilityDetailsType to the contents of the
           * SubCapabilityDetailsType on the right hand side (rhs) of the assignment operator.SubCapabilityDetailsType [only
           * available to derived classes]
           *
           * @param rhs The SubCapabilityDetailsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SubCapabilityDetailsType
           * @return A constant reference to this SubCapabilityDetailsType.
           */
         const SubCapabilityDetailsType& operator=(const SubCapabilityDetailsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubCapabilityDetailsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubCapabilityDetailsType_h

