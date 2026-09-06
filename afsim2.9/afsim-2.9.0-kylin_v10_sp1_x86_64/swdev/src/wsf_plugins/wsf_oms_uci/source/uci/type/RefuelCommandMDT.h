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
#ifndef Uci__Type__RefuelCommandMDT_h
#define Uci__Type__RefuelCommandMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__RefuelCommandType_h)
# include "uci/type/RefuelCommandType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the RefuelCommandMDT sequence accessor class */
      class RefuelCommandMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~RefuelCommandMDT()
         { }

         /** Returns this accessor's type constant, i.e. RefuelCommandMDT
           *
           * @return This accessor's type constant, i.e. RefuelCommandMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::refuelCommandMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const RefuelCommandMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates an instance of an Refuel Capability command. A Capability command message can have multiple command
           * instances to allow a sequence of related commands in a single coherent message. Each command instance is then
           * statused via a separate, independent Capability CommandStatus message. [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::RefuelCommandType, uci::type::accessorType::refuelCommandType> Command;

         /** Returns the bounded list that is identified by the Command.
           *
           * @return The bounded list identified by Command.
           */
         virtual const uci::type::RefuelCommandMDT::Command& getCommand() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Command.
           *
           * @return The bounded list identified by Command.
           */
         virtual uci::type::RefuelCommandMDT::Command& getCommand()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Command.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCommand(const uci::type::RefuelCommandMDT::Command& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         RefuelCommandMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The RefuelCommandMDT to copy from
           */
         RefuelCommandMDT(const RefuelCommandMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this RefuelCommandMDT to the contents of the RefuelCommandMDT on the
           * right hand side (rhs) of the assignment operator.RefuelCommandMDT [only available to derived classes]
           *
           * @param rhs The RefuelCommandMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::RefuelCommandMDT
           * @return A constant reference to this RefuelCommandMDT.
           */
         const RefuelCommandMDT& operator=(const RefuelCommandMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // RefuelCommandMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__RefuelCommandMDT_h

