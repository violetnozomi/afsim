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
#ifndef Uci__Type__CapabilityCommandFrequencyType_h
#define Uci__Type__CapabilityCommandFrequencyType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RF_ProfileID_Type_h)
# include "uci/type/RF_ProfileID_Type.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the CapabilityCommandFrequencyType sequence accessor class */
      class CapabilityCommandFrequencyType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~CapabilityCommandFrequencyType()
         { }

         /** Returns this accessor's type constant, i.e. CapabilityCommandFrequencyType
           *
           * @return This accessor's type constant, i.e. CapabilityCommandFrequencyType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::capabilityCommandFrequencyType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const CapabilityCommandFrequencyType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an additional RF_Profile which further constrains the allowed frequencies to be used for the Command. The
           * allowed frequencies for the Command are the intersection of the allowed frequencies in this RF_Profile, RF_Profiles
           * imposed by RF_ControlCommand on the associated Capability, MDF settings, threat avoidance, etc. If the sibling
           * RF_ControlOverride is set to TRUE, this command is only constrained by the RF_Profiles listed here. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RF_ProfileID_Type, uci::type::accessorType::rF_ProfileID_Type> RF_ProfileID;

         /** Returns the bounded list that is identified by the RF_ProfileID.
           *
           * @return The bounded list identified by RF_ProfileID.
           */
         virtual const uci::type::CapabilityCommandFrequencyType::RF_ProfileID& getRF_ProfileID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the RF_ProfileID.
           *
           * @return The bounded list identified by RF_ProfileID.
           */
         virtual uci::type::CapabilityCommandFrequencyType::RF_ProfileID& getRF_ProfileID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the RF_ProfileID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setRF_ProfileID(const uci::type::CapabilityCommandFrequencyType::RF_ProfileID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the RF_ControlOverride.
           *
           * @return The value of the simple primitive data type identified by RF_ControlOverride.
           */
         virtual xs::Boolean getRF_ControlOverride() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RF_ControlOverride.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRF_ControlOverride(xs::Boolean value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         CapabilityCommandFrequencyType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The CapabilityCommandFrequencyType to copy from
           */
         CapabilityCommandFrequencyType(const CapabilityCommandFrequencyType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this CapabilityCommandFrequencyType to the contents of the
           * CapabilityCommandFrequencyType on the right hand side (rhs) of the assignment operator.CapabilityCommandFrequencyType
           * [only available to derived classes]
           *
           * @param rhs The CapabilityCommandFrequencyType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::CapabilityCommandFrequencyType
           * @return A constant reference to this CapabilityCommandFrequencyType.
           */
         const CapabilityCommandFrequencyType& operator=(const CapabilityCommandFrequencyType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // CapabilityCommandFrequencyType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__CapabilityCommandFrequencyType_h

