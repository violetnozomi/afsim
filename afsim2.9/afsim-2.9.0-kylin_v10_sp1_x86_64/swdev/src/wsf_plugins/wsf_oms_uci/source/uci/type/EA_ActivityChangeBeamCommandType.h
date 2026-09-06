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
#ifndef Uci__Type__EA_ActivityChangeBeamCommandType_h
#define Uci__Type__EA_ActivityChangeBeamCommandType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__BeamID_Type_h)
# include "uci/type/BeamID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityStateCommandEnum_h)
# include "uci/type/CapabilityStateCommandEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__FrequencyRangeType_h)
# include "uci/type/FrequencyRangeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ActivityChangeBeamCommandType sequence accessor class */
      class EA_ActivityChangeBeamCommandType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_ActivityChangeBeamCommandType()
         { }

         /** Returns this accessor's type constant, i.e. EA_ActivityChangeBeamCommandType
           *
           * @return This accessor's type constant, i.e. EA_ActivityChangeBeamCommandType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ActivityChangeBeamCommandType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ActivityChangeBeamCommandType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamID.
           */
         virtual const uci::type::BeamID_Type& getBeamID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the BeamID.
           *
           * @return The acecssor that provides access to the complex content that is identified by BeamID.
           */
         virtual uci::type::BeamID_Type& getBeamID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the BeamID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by BeamID
           */
         virtual void setBeamID(const uci::type::BeamID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ChangeBeamState.
           *
           * @return The value of the enumeration identified by ChangeBeamState.
           */
         virtual const uci::type::CapabilityStateCommandEnum& getChangeBeamState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ChangeBeamState.
           *
           * @return The value of the enumeration identified by ChangeBeamState.
           */
         virtual uci::type::CapabilityStateCommandEnum& getChangeBeamState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeBeamState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeBeamState(const uci::type::CapabilityStateCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeBeamState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeBeamState(uci::type::CapabilityStateCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ChangeBeamStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasChangeBeamState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getChangeBeamState will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::capabilityStateCommandEnum This Accessor's accessor type
           */
         virtual void enableChangeBeamState(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityStateCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearChangeBeamState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UnassignBeam.
           *
           * @return The value of the simple primitive data type identified by UnassignBeam.
           */
         virtual xs::Boolean getUnassignBeam() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UnassignBeam.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUnassignBeam(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UnassignBeam exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UnassignBeam is emabled or not
           */
         virtual bool hasUnassignBeam() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UnassignBeam
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableUnassignBeam(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UnassignBeam */
         virtual void clearUnassignBeam()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DeleteBeam.
           *
           * @return The value of the simple primitive data type identified by DeleteBeam.
           */
         virtual xs::Boolean getDeleteBeam() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DeleteBeam.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDeleteBeam(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeleteBeam exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeleteBeam is emabled or not
           */
         virtual bool hasDeleteBeam() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeleteBeam
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDeleteBeam(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeleteBeam */
         virtual void clearDeleteBeam()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeFrequency.
           */
         virtual const uci::type::FrequencyRangeType& getChangeFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeFrequency.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeFrequency.
           */
         virtual uci::type::FrequencyRangeType& getChangeFrequency()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeFrequency to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeFrequency
           */
         virtual void setChangeFrequency(const uci::type::FrequencyRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeFrequency exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeFrequency is emabled or not
           */
         virtual bool hasChangeFrequency() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeFrequency
           *
           * @param type = uci::type::accessorType::frequencyRangeType This Accessor's accessor type
           */
         virtual void enableChangeFrequency(uci::base::accessorType::AccessorType type = uci::type::accessorType::frequencyRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeFrequency */
         virtual void clearChangeFrequency()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ActivityChangeBeamCommandType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ActivityChangeBeamCommandType to copy from
           */
         EA_ActivityChangeBeamCommandType(const EA_ActivityChangeBeamCommandType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ActivityChangeBeamCommandType to the contents of the
           * EA_ActivityChangeBeamCommandType on the right hand side (rhs) of the assignment
           * operator.EA_ActivityChangeBeamCommandType [only available to derived classes]
           *
           * @param rhs The EA_ActivityChangeBeamCommandType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::EA_ActivityChangeBeamCommandType
           * @return A constant reference to this EA_ActivityChangeBeamCommandType.
           */
         const EA_ActivityChangeBeamCommandType& operator=(const EA_ActivityChangeBeamCommandType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ActivityChangeBeamCommandType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ActivityChangeBeamCommandType_h

