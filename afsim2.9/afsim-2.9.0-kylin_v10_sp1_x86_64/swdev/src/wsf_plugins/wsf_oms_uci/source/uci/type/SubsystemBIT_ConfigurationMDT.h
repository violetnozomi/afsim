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
#ifndef Uci__Type__SubsystemBIT_ConfigurationMDT_h
#define Uci__Type__SubsystemBIT_ConfigurationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__SubsystemBIT_Type_h)
# include "uci/type/SubsystemBIT_Type.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SubsystemBIT_ConfigurationMDT sequence accessor class */
      class SubsystemBIT_ConfigurationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SubsystemBIT_ConfigurationMDT()
         { }

         /** Returns this accessor's type constant, i.e. SubsystemBIT_ConfigurationMDT
           *
           * @return This accessor's type constant, i.e. SubsystemBIT_ConfigurationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::subsystemBIT_ConfigurationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SubsystemBIT_ConfigurationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a BIT supported by the Subsystem. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SubsystemBIT_Type, uci::type::accessorType::subsystemBIT_Type> BIT;

         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual const uci::type::SubsystemID_Type& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SubsystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SubsystemID.
           */
         virtual uci::type::SubsystemID_Type& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SubsystemID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SubsystemID
           */
         virtual void setSubsystemID(const uci::type::SubsystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BIT.
           *
           * @return The bounded list identified by BIT.
           */
         virtual const uci::type::SubsystemBIT_ConfigurationMDT::BIT& getBIT() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the BIT.
           *
           * @return The bounded list identified by BIT.
           */
         virtual uci::type::SubsystemBIT_ConfigurationMDT::BIT& getBIT()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the BIT.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setBIT(const uci::type::SubsystemBIT_ConfigurationMDT::BIT& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SubsystemBIT_ConfigurationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_ConfigurationMDT to copy from
           */
         SubsystemBIT_ConfigurationMDT(const SubsystemBIT_ConfigurationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SubsystemBIT_ConfigurationMDT to the contents of the
           * SubsystemBIT_ConfigurationMDT on the right hand side (rhs) of the assignment operator.SubsystemBIT_ConfigurationMDT
           * [only available to derived classes]
           *
           * @param rhs The SubsystemBIT_ConfigurationMDT on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::SubsystemBIT_ConfigurationMDT
           * @return A constant reference to this SubsystemBIT_ConfigurationMDT.
           */
         const SubsystemBIT_ConfigurationMDT& operator=(const SubsystemBIT_ConfigurationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SubsystemBIT_ConfigurationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SubsystemBIT_ConfigurationMDT_h

