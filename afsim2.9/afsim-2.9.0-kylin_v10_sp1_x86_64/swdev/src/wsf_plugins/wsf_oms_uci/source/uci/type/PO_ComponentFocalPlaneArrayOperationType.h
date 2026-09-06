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
#ifndef Uci__Type__PO_ComponentFocalPlaneArrayOperationType_h
#define Uci__Type__PO_ComponentFocalPlaneArrayOperationType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_FPA_ChannelOperationEnum_h)
# include "uci/type/PO_FPA_ChannelOperationEnum.h"
#endif

#if !defined(Uci__Type__ComponentControllabilityA_Type_h)
# include "uci/type/ComponentControllabilityA_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentFocalPlaneArrayOperationType sequence accessor class */
      class PO_ComponentFocalPlaneArrayOperationType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentFocalPlaneArrayOperationType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentFocalPlaneArrayOperationType
           *
           * @return This accessor's type constant, i.e. PO_ComponentFocalPlaneArrayOperationType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentFocalPlaneArrayOperationType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentFocalPlaneArrayOperationType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationType.
           *
           * @return The value of the enumeration identified by OperationType.
           */
         virtual const uci::type::PO_FPA_ChannelOperationEnum& getOperationType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the OperationType.
           *
           * @return The value of the enumeration identified by OperationType.
           */
         virtual uci::type::PO_FPA_ChannelOperationEnum& getOperationType()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationType(const uci::type::PO_FPA_ChannelOperationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the OperationType.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setOperationType(uci::type::PO_FPA_ChannelOperationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual const uci::type::ComponentControllabilityA_Type& getControllability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Controllability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Controllability.
           */
         virtual uci::type::ComponentControllabilityA_Type& getControllability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Controllability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Controllability
           */
         virtual void setControllability(const uci::type::ComponentControllabilityA_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Controllability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Controllability is emabled or not
           */
         virtual bool hasControllability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Controllability
           *
           * @param type = uci::type::accessorType::componentControllabilityA_Type This Accessor's accessor type
           */
         virtual void enableControllability(uci::base::accessorType::AccessorType type = uci::type::accessorType::componentControllabilityA_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Controllability */
         virtual void clearControllability()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentFocalPlaneArrayOperationType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentFocalPlaneArrayOperationType to copy from
           */
         PO_ComponentFocalPlaneArrayOperationType(const PO_ComponentFocalPlaneArrayOperationType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentFocalPlaneArrayOperationType to the contents of the
           * PO_ComponentFocalPlaneArrayOperationType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentFocalPlaneArrayOperationType [only available to derived classes]
           *
           * @param rhs The PO_ComponentFocalPlaneArrayOperationType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::PO_ComponentFocalPlaneArrayOperationType
           * @return A constant reference to this PO_ComponentFocalPlaneArrayOperationType.
           */
         const PO_ComponentFocalPlaneArrayOperationType& operator=(const PO_ComponentFocalPlaneArrayOperationType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentFocalPlaneArrayOperationType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentFocalPlaneArrayOperationType_h

