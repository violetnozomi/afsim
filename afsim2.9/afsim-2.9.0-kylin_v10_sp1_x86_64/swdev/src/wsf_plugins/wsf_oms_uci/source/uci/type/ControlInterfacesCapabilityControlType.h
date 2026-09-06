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
#ifndef Uci__Type__ControlInterfacesCapabilityControlType_h
#define Uci__Type__ControlInterfacesCapabilityControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityControlInterfacesEnum_h)
# include "uci/type/CapabilityControlInterfacesEnum.h"
#endif

#if !defined(Uci__Type__CapabilityManagerType_h)
# include "uci/type/CapabilityManagerType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlInterfacesCapabilityControlType sequence accessor class */
      class ControlInterfacesCapabilityControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlInterfacesCapabilityControlType()
         { }

         /** Returns this accessor's type constant, i.e. ControlInterfacesCapabilityControlType
           *
           * @return This accessor's type constant, i.e. ControlInterfacesCapabilityControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlInterfacesCapabilityControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlInterfacesCapabilityControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a form of Capability control that shall be accepted for the Capability. If omitted, then no control
           * interfaces are accepted. See enumeration annotations and ControlStatus for further details. [Minimum occurrences: 0]
           * [Maximum occurrences: 7]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityControlInterfacesEnum, uci::type::accessorType::capabilityControlInterfacesEnum> AcceptedInterface;

         /** Indicates a Capability Manager (an automation service, also known as a Resource Manager) that is allowed to
           * autonomously command the Capability. See ControlStatus and ControlTypeEnum annotations for further details. [Minimum
           * occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityManagerType, uci::type::accessorType::capabilityManagerType> CapabilityManager;

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


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::ControlInterfacesCapabilityControlType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::ControlInterfacesCapabilityControlType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::ControlInterfacesCapabilityControlType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityManager.
           *
           * @return The bounded list identified by CapabilityManager.
           */
         virtual const uci::type::ControlInterfacesCapabilityControlType::CapabilityManager& getCapabilityManager() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityManager.
           *
           * @return The bounded list identified by CapabilityManager.
           */
         virtual uci::type::ControlInterfacesCapabilityControlType::CapabilityManager& getCapabilityManager()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityManager.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityManager(const uci::type::ControlInterfacesCapabilityControlType::CapabilityManager& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlInterfacesCapabilityControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlInterfacesCapabilityControlType to copy from
           */
         ControlInterfacesCapabilityControlType(const ControlInterfacesCapabilityControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlInterfacesCapabilityControlType to the contents of the
           * ControlInterfacesCapabilityControlType on the right hand side (rhs) of the assignment
           * operator.ControlInterfacesCapabilityControlType [only available to derived classes]
           *
           * @param rhs The ControlInterfacesCapabilityControlType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ControlInterfacesCapabilityControlType
           * @return A constant reference to this ControlInterfacesCapabilityControlType.
           */
         const ControlInterfacesCapabilityControlType& operator=(const ControlInterfacesCapabilityControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlInterfacesCapabilityControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlInterfacesCapabilityControlType_h

