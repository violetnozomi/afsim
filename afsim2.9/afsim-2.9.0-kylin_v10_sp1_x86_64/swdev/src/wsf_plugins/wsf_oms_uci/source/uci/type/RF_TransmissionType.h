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
#ifndef Uci__Type__RF_TransmissionType_h
#define Uci__Type__RF_TransmissionType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__AntennaAndResourceInstanceType_h)
# include "uci/type/AntennaAndResourceInstanceType.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__PolarizationType_h)
# include "uci/type/PolarizationType.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__BlankingLimitType_h)
# include "uci/type/BlankingLimitType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RF_TransmissionType sequence accessor class */
      class RF_TransmissionType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RF_TransmissionType()
         { }

         /** Returns this accessor's type constant, i.e. RF_TransmissionType
           *
           * @return This accessor's type constant, i.e. RF_TransmissionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::rF_TransmissionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RF_TransmissionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This grouping of RF_ThreadInstanceIndexs must be of equivalent ResourceAccessPriority. This set of RF_ThreadInstances
           * cannot represent multiple resource allocations. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::base::UnsignedIntAccessor, uci::base::accessorType::unsignedIntAccessor> RF_ThreadInstanceIndex;

         /** A set of antenna resource that will be used for RF transmit [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::AntennaAndResourceInstanceType, uci::type::accessorType::antennaAndResourceInstanceType> AntennaResource;

         /** Indicates finer granularity of frequency chunks that this CIR Index is associated with. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Frequency;

         /** Transmit function report of acceptable amount of blanking. Subsystems that blank are expected not to exceed the
           * TxPercentBlankLimit allocated to their subsystem. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::BlankingLimitType, uci::type::accessorType::blankingLimitType> BlankingLimit;

         /** Returns the bounded list that is identified by the RF_ThreadInstanceIndex.
           *
           * @return The bounded list identified by RF_ThreadInstanceIndex.
           */
         virtual const uci::type::RF_TransmissionType::RF_ThreadInstanceIndex& getRF_ThreadInstanceIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ThreadInstanceIndex.
           *
           * @return The bounded list identified by RF_ThreadInstanceIndex.
           */
         virtual uci::type::RF_TransmissionType::RF_ThreadInstanceIndex& getRF_ThreadInstanceIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_ThreadInstanceIndex.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_ThreadInstanceIndex(const uci::type::RF_TransmissionType::RF_ThreadInstanceIndex& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaResource.
           *
           * @return The bounded list identified by AntennaResource.
           */
         virtual const uci::type::RF_TransmissionType::AntennaResource& getAntennaResource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaResource.
           *
           * @return The bounded list identified by AntennaResource.
           */
         virtual uci::type::RF_TransmissionType::AntennaResource& getAntennaResource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaResource.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaResource(const uci::type::RF_TransmissionType::AntennaResource& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ResourceAccessPriority.
           *
           * @return The value of the simple primitive data type identified by ResourceAccessPriority.
           */
         virtual xs::UnsignedInt getResourceAccessPriority() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ResourceAccessPriority.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setResourceAccessPriority(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual const uci::type::RF_TransmissionType::Frequency& getFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Frequency.
           *
           * @return The bounded list identified by Frequency.
           */
         virtual uci::type::RF_TransmissionType::Frequency& getFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Frequency.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFrequency(const uci::type::RF_TransmissionType::Frequency& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Polarization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Polarization.
           */
         virtual const uci::type::PolarizationType& getPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Polarization.
           *
           * @return The acecssor that provides access to the complex content that is identified by Polarization.
           */
         virtual uci::type::PolarizationType& getPolarization()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Polarization to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Polarization
           */
         virtual void setPolarization(const uci::type::PolarizationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Polarization exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Polarization is emabled or not
           */
         virtual bool hasPolarization() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Polarization
           *
           * @param type = uci::type::accessorType::polarizationType This Accessor's accessor type
           */
         virtual void enablePolarization(uci::base::accessorType::AccessorType type = uci::type::accessorType::polarizationType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Polarization */
         virtual void clearPolarization()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumTransmitERP.
           *
           * @return The value of the simple primitive data type identified by MaximumTransmitERP.
           */
         virtual uci::type::DecibelTypeValue getMaximumTransmitERP() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumTransmitERP.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumTransmitERP(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BlankingLimit.
           *
           * @return The bounded list identified by BlankingLimit.
           */
         virtual const uci::type::RF_TransmissionType::BlankingLimit& getBlankingLimit() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BlankingLimit.
           *
           * @return The bounded list identified by BlankingLimit.
           */
         virtual uci::type::RF_TransmissionType::BlankingLimit& getBlankingLimit()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BlankingLimit.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBlankingLimit(const uci::type::RF_TransmissionType::BlankingLimit& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MinimumReceiveSensitivity.
           *
           * @return The value of the simple primitive data type identified by MinimumReceiveSensitivity.
           */
         virtual uci::type::DecibelTypeValue getMinimumReceiveSensitivity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MinimumReceiveSensitivity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMinimumReceiveSensitivity(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MinimumReceiveSensitivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MinimumReceiveSensitivity is emabled or not
           */
         virtual bool hasMinimumReceiveSensitivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MinimumReceiveSensitivity
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableMinimumReceiveSensitivity(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MinimumReceiveSensitivity */
         virtual void clearMinimumReceiveSensitivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PercentInterruptable.
           *
           * @return The value of the simple primitive data type identified by PercentInterruptable.
           */
         virtual uci::type::PercentTypeValue getPercentInterruptable() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PercentInterruptable.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPercentInterruptable(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PercentInterruptable exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PercentInterruptable is emabled or not
           */
         virtual bool hasPercentInterruptable() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PercentInterruptable
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enablePercentInterruptable(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PercentInterruptable */
         virtual void clearPercentInterruptable()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RF_TransmissionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RF_TransmissionType to copy from
           */
         RF_TransmissionType(const RF_TransmissionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RF_TransmissionType to the contents of the RF_TransmissionType on
           * the right hand side (rhs) of the assignment operator.RF_TransmissionType [only available to derived classes]
           *
           * @param rhs The RF_TransmissionType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RF_TransmissionType
           * @return A constant reference to this RF_TransmissionType.
           */
         const RF_TransmissionType& operator=(const RF_TransmissionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RF_TransmissionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RF_TransmissionType_h

