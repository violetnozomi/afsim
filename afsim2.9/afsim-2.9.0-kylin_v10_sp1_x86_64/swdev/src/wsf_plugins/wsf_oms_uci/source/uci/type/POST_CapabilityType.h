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
#ifndef Uci__Type__POST_CapabilityType_h
#define Uci__Type__POST_CapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__POST_CapabilityEnum_h)
# include "uci/type/POST_CapabilityEnum.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubCapabilityID_Type_h)
# include "uci/type/SubCapabilityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__POST_CapabilityOptionsType_h)
# include "uci/type/POST_CapabilityOptionsType.h"
#endif

#if !defined(Uci__Type__ComponentID_Type_h)
# include "uci/type/ComponentID_Type.h"
#endif

#if !defined(Uci__Type__TrackOptionsID_Type_h)
# include "uci/type/TrackOptionsID_Type.h"
#endif

#if !defined(Uci__Type__ProductOutputCapabilityImageryType_h)
# include "uci/type/ProductOutputCapabilityImageryType.h"
#endif

#if !defined(Uci__Type__POST_MessageOutputsEnum_h)
# include "uci/type/POST_MessageOutputsEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_CapabilityType sequence accessor class */
      class POST_CapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_CapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. POST_CapabilityType
           *
           * @return This accessor's type constant, i.e. POST_CapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_CapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_CapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a command and control relevant Subsystem SubCapability associated with the Capability. A subcapability
           * unique ID can: 1.Associate with Multiple Capabilities. In this case, a change to a subcapability with this unique ID
           * will impact all associated capabilities. 2.Associated with a single capability. In this case, a change to a
           * subcapability with this unique ID will impact only the associated capability. [Minimum occurrences: 0] [Maximum
           * occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::SubCapabilityID_Type, uci::type::accessorType::subCapabilityID_Type> AssociateSubCapabilityID;

         /** Indicates an accepted control interface for the Capability. See enumeration annotations for further details. [Maximum
           * occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a command and control relevant Subsystem Component associated with the Capability. A component unique ID
           * can: 1.Associate with Multiple Capabilities. In this case, a change to a component with this unique ID will impact
           * all associated capabilities. 2.Associated with a single capability. In this case, a change to a component with this
           * unique ID will impact only the associated capability. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ComponentID_Type, uci::type::accessorType::componentID_Type> AssociateComponentID;

         /** Indicates a command and control relevant Subsystem Track Options associated with the Capability. A track option
           * unique ID can: 1.Associate with Multiple Capabilities. In this case, a change to a track option with this unique ID
           * will impact all associated capabilities. 2.Associated with a single capability. In this case, a change to a track
           * option with this unique ID will impact only the associated capability. [Minimum occurrences: 0] [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::TrackOptionsID_Type, uci::type::accessorType::trackOptionsID_Type> AssociateTrackOptionID;

         /** Indicates a Product that is an output of the Capability. This can also be done in the Component level, but this way
           * gives you the ability to generate products from capabilities without getting into component details. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::ProductOutputCapabilityImageryType, uci::type::accessorType::productOutputCapabilityImageryType> ProductOutput;

         /** Indicates a message that is an output of the Capability. See enumerated type annotations for details. [Minimum
           * occurrences: 0] [Maximum occurrences: 3]
           */
         typedef uci::base::BoundedList<uci::type::POST_MessageOutputsEnum, uci::type::accessorType::pOST_MessageOutputsEnum> MessageOutput;

         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual const uci::type::CapabilityID_Type& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityID.
           */
         virtual uci::type::CapabilityID_Type& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityID
           */
         virtual void setCapabilityID(const uci::type::CapabilityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual const uci::type::POST_CapabilityEnum& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CapabilityType.
           *
           * @return The value of the enumeration identified by CapabilityType.
           */
         virtual uci::type::POST_CapabilityEnum& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(const uci::type::POST_CapabilityEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CapabilityType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCapabilityType(uci::type::POST_CapabilityEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateSubCapabilityID.
           *
           * @return The bounded list identified by AssociateSubCapabilityID.
           */
         virtual const uci::type::POST_CapabilityType::AssociateSubCapabilityID& getAssociateSubCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateSubCapabilityID.
           *
           * @return The bounded list identified by AssociateSubCapabilityID.
           */
         virtual uci::type::POST_CapabilityType::AssociateSubCapabilityID& getAssociateSubCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociateSubCapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociateSubCapabilityID(const uci::type::POST_CapabilityType::AssociateSubCapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::POST_CapabilityType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::POST_CapabilityType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::POST_CapabilityType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual const uci::type::POST_CapabilityOptionsType& getCapabilityOptions() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CapabilityOptions.
           *
           * @return The acecssor that provides access to the complex content that is identified by CapabilityOptions.
           */
         virtual uci::type::POST_CapabilityOptionsType& getCapabilityOptions()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CapabilityOptions to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CapabilityOptions
           */
         virtual void setCapabilityOptions(const uci::type::POST_CapabilityOptionsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateComponentID.
           *
           * @return The bounded list identified by AssociateComponentID.
           */
         virtual const uci::type::POST_CapabilityType::AssociateComponentID& getAssociateComponentID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateComponentID.
           *
           * @return The bounded list identified by AssociateComponentID.
           */
         virtual uci::type::POST_CapabilityType::AssociateComponentID& getAssociateComponentID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociateComponentID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociateComponentID(const uci::type::POST_CapabilityType::AssociateComponentID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateTrackOptionID.
           *
           * @return The bounded list identified by AssociateTrackOptionID.
           */
         virtual const uci::type::POST_CapabilityType::AssociateTrackOptionID& getAssociateTrackOptionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AssociateTrackOptionID.
           *
           * @return The bounded list identified by AssociateTrackOptionID.
           */
         virtual uci::type::POST_CapabilityType::AssociateTrackOptionID& getAssociateTrackOptionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AssociateTrackOptionID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAssociateTrackOptionID(const uci::type::POST_CapabilityType::AssociateTrackOptionID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual const uci::type::POST_CapabilityType::ProductOutput& getProductOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ProductOutput.
           *
           * @return The bounded list identified by ProductOutput.
           */
         virtual uci::type::POST_CapabilityType::ProductOutput& getProductOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ProductOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setProductOutput(const uci::type::POST_CapabilityType::ProductOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual const uci::type::POST_CapabilityType::MessageOutput& getMessageOutput() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the MessageOutput.
           *
           * @return The bounded list identified by MessageOutput.
           */
         virtual uci::type::POST_CapabilityType::MessageOutput& getMessageOutput()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the MessageOutput.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setMessageOutput(const uci::type::POST_CapabilityType::MessageOutput& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CommsRequired.
           *
           * @return The value of the simple primitive data type identified by CommsRequired.
           */
         virtual xs::Boolean getCommsRequired() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CommsRequired.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCommsRequired(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommsRequired exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommsRequired is emabled or not
           */
         virtual bool hasCommsRequired() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommsRequired
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableCommsRequired(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommsRequired */
         virtual void clearCommsRequired()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_CapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_CapabilityType to copy from
           */
         POST_CapabilityType(const POST_CapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_CapabilityType to the contents of the POST_CapabilityType on
           * the right hand side (rhs) of the assignment operator.POST_CapabilityType [only available to derived classes]
           *
           * @param rhs The POST_CapabilityType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::POST_CapabilityType
           * @return A constant reference to this POST_CapabilityType.
           */
         const POST_CapabilityType& operator=(const POST_CapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_CapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_CapabilityType_h

