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
#ifndef Uci__Type__EA_ComponentType_h
#define Uci__Type__EA_ComponentType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__FieldOfRegardType_h)
# include "uci/type/FieldOfRegardType.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

#if !defined(Uci__Type__FrequencyType_h)
# include "uci/type/FrequencyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ComponentType sequence accessor class */
      class EA_ComponentType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_ComponentType()
         { }

         /** Returns this accessor's type constant, i.e. EA_ComponentType
           *
           * @return This accessor's type constant, i.e. EA_ComponentType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ComponentType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ComponentType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the bandwidth of the Component. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::FrequencyRangeType, uci::type::accessorType::frequencyRangeType> Band;

         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual const uci::type::ComponentID_Type& getComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ComponentID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ComponentID.
           */
         virtual uci::type::ComponentID_Type& getComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ComponentID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ComponentID
           */
         virtual void setComponentID(const uci::type::ComponentID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by FieldOfRegard.
           */
         virtual const uci::type::FieldOfRegardType& getFieldOfRegard() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the FieldOfRegard.
           *
           * @return The acecssor that provides access to the complex content that is identified by FieldOfRegard.
           */
         virtual uci::type::FieldOfRegardType& getFieldOfRegard()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the FieldOfRegard to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by FieldOfRegard
           */
         virtual void setFieldOfRegard(const uci::type::FieldOfRegardType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FrequencyChangeTime.
           *
           * @return The value of the simple primitive data type identified by FrequencyChangeTime.
           */
         virtual uci::type::DurationTypeValue getFrequencyChangeTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FrequencyChangeTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFrequencyChangeTime(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DutyFactorLimit.
           *
           * @return The value of the simple primitive data type identified by DutyFactorLimit.
           */
         virtual uci::type::PercentTypeValue getDutyFactorLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DutyFactorLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDutyFactorLimit(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @return The value of the simple primitive data type identified by BeamLimit.
           */
         virtual xs::UnsignedInt getBeamLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the BeamLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setBeamLimit(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CommutationsLimit.
           *
           * @return The value of the simple primitive data type identified by CommutationsLimit.
           */
         virtual xs::UnsignedInt getCommutationsLimit() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CommutationsLimit.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCommutationsLimit(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual const uci::type::EA_ComponentType::Band& getBand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Band.
           *
           * @return The bounded list identified by Band.
           */
         virtual uci::type::EA_ComponentType::Band& getBand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Band.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBand(const uci::type::EA_ComponentType::Band& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the InstantaneousBand.
           *
           * @return The value of the simple primitive data type identified by InstantaneousBand.
           */
         virtual uci::type::FrequencyTypeValue getInstantaneousBand() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the InstantaneousBand.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setInstantaneousBand(uci::type::FrequencyTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by InstantaneousBand exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by InstantaneousBand is emabled or not
           */
         virtual bool hasInstantaneousBand() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by InstantaneousBand
           *
           * @param type = uci::type::accessorType::frequencyType This Accessor's accessor type
           */
         virtual void enableInstantaneousBand(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by InstantaneousBand */
         virtual void clearInstantaneousBand()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ComponentType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ComponentType to copy from
           */
         EA_ComponentType(const EA_ComponentType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ComponentType to the contents of the EA_ComponentType on the
           * right hand side (rhs) of the assignment operator.EA_ComponentType [only available to derived classes]
           *
           * @param rhs The EA_ComponentType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EA_ComponentType
           * @return A constant reference to this EA_ComponentType.
           */
         const EA_ComponentType& operator=(const EA_ComponentType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ComponentType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ComponentType_h

