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
#ifndef Uci__Type__ControlInterfacesMissionControlType_h
#define Uci__Type__ControlInterfacesMissionControlType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__MissionControlInterfaceEnum_h)
# include "uci/type/MissionControlInterfaceEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ControlInterfacesMissionControlType sequence accessor class */
      class ControlInterfacesMissionControlType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ControlInterfacesMissionControlType()
         { }

         /** Returns this accessor's type constant, i.e. ControlInterfacesMissionControlType
           *
           * @return This accessor's type constant, i.e. ControlInterfacesMissionControlType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::controlInterfacesMissionControlType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ControlInterfacesMissionControlType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a form of mission control interface that shall be accepted by the System. If omitted, then no control
           * interfaces are accepted. See enumeration annotations and ControlStatus for further details. [Minimum occurrences: 0]
           * [Maximum occurrences: 6]
           */
         typedef uci::base::BoundedList<uci::type::MissionControlInterfaceEnum, uci::type::accessorType::missionControlInterfaceEnum> AcceptedInterface;

         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual const uci::type::ControlInterfacesMissionControlType::AcceptedInterface& getAcceptedInterface() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AcceptedInterface.
           *
           * @return The bounded list identified by AcceptedInterface.
           */
         virtual uci::type::ControlInterfacesMissionControlType::AcceptedInterface& getAcceptedInterface()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AcceptedInterface.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAcceptedInterface(const uci::type::ControlInterfacesMissionControlType::AcceptedInterface& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ControlInterfacesMissionControlType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ControlInterfacesMissionControlType to copy from
           */
         ControlInterfacesMissionControlType(const ControlInterfacesMissionControlType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ControlInterfacesMissionControlType to the contents of the
           * ControlInterfacesMissionControlType on the right hand side (rhs) of the assignment
           * operator.ControlInterfacesMissionControlType [only available to derived classes]
           *
           * @param rhs The ControlInterfacesMissionControlType on the right hand side (rhs) of the assignment operator whose
           *      contents are used to set the contents of this uci::type::ControlInterfacesMissionControlType
           * @return A constant reference to this ControlInterfacesMissionControlType.
           */
         const ControlInterfacesMissionControlType& operator=(const ControlInterfacesMissionControlType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ControlInterfacesMissionControlType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ControlInterfacesMissionControlType_h

