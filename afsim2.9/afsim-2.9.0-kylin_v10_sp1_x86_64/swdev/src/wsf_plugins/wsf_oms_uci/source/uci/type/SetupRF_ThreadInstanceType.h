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
#ifndef Uci__Type__SetupRF_ThreadInstanceType_h
#define Uci__Type__SetupRF_ThreadInstanceType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__SystemThreadIdentifierType_h)
# include "uci/type/SystemThreadIdentifierType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ThreadConfigurationType_h)
# include "uci/type/ThreadConfigurationType.h"
#endif

#if !defined(Uci__Type__RF_GainType_h)
# include "uci/type/RF_GainType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SetupRF_ThreadInstanceType sequence accessor class */
      class SetupRF_ThreadInstanceType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SetupRF_ThreadInstanceType()
         { }

         /** Returns this accessor's type constant, i.e. SetupRF_ThreadInstanceType
           *
           * @return This accessor's type constant, i.e. SetupRF_ThreadInstanceType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::setupRF_ThreadInstanceType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SetupRF_ThreadInstanceType& accessor)
            throw(uci::base::UCIException) = 0;


         /** This must match the resource definition in the resource allocation. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ThreadConfigurationType, uci::type::accessorType::threadConfigurationType> BeamConfiguration;

         /** Gain of the RF path associated with the sibling System Thread. Multiplicity of 2 is to allow the transmit path to be
           * different than the receive path gain [Minimum occurrences: 0] [Maximum occurrences: 2]
           */
         typedef uci::base::BoundedList<uci::type::RF_GainType, uci::type::accessorType::rF_GainType> RF_Gain;

         /** Returns the value of the SimplePrimitive data type that is identified by the RF_ThreadInstanceIndex.
           *
           * @return The value of the simple primitive data type identified by RF_ThreadInstanceIndex.
           */
         virtual xs::UnsignedInt getRF_ThreadInstanceIndex() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RF_ThreadInstanceIndex.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRF_ThreadInstanceIndex(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemThreadIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemThreadIndex.
           */
         virtual const uci::type::SystemThreadIdentifierType& getSystemThreadIndex() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemThreadIndex.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemThreadIndex.
           */
         virtual uci::type::SystemThreadIdentifierType& getSystemThreadIndex()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemThreadIndex to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemThreadIndex
           */
         virtual void setSystemThreadIndex(const uci::type::SystemThreadIdentifierType& value)
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


         /** Returns the bounded list that is identified by the BeamConfiguration.
           *
           * @return The bounded list identified by BeamConfiguration.
           */
         virtual const uci::type::SetupRF_ThreadInstanceType::BeamConfiguration& getBeamConfiguration() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BeamConfiguration.
           *
           * @return The bounded list identified by BeamConfiguration.
           */
         virtual uci::type::SetupRF_ThreadInstanceType::BeamConfiguration& getBeamConfiguration()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BeamConfiguration.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBeamConfiguration(const uci::type::SetupRF_ThreadInstanceType::BeamConfiguration& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Gain.
           *
           * @return The bounded list identified by RF_Gain.
           */
         virtual const uci::type::SetupRF_ThreadInstanceType::RF_Gain& getRF_Gain() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_Gain.
           *
           * @return The bounded list identified by RF_Gain.
           */
         virtual uci::type::SetupRF_ThreadInstanceType::RF_Gain& getRF_Gain()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_Gain.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_Gain(const uci::type::SetupRF_ThreadInstanceType::RF_Gain& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SetupRF_ThreadInstanceType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SetupRF_ThreadInstanceType to copy from
           */
         SetupRF_ThreadInstanceType(const SetupRF_ThreadInstanceType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SetupRF_ThreadInstanceType to the contents of the
           * SetupRF_ThreadInstanceType on the right hand side (rhs) of the assignment operator.SetupRF_ThreadInstanceType [only
           * available to derived classes]
           *
           * @param rhs The SetupRF_ThreadInstanceType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SetupRF_ThreadInstanceType
           * @return A constant reference to this SetupRF_ThreadInstanceType.
           */
         const SetupRF_ThreadInstanceType& operator=(const SetupRF_ThreadInstanceType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SetupRF_ThreadInstanceType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SetupRF_ThreadInstanceType_h

