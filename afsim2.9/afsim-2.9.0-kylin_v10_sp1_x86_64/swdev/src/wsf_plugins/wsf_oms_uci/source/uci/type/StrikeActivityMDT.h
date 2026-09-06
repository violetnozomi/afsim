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
#ifndef Uci__Type__StrikeActivityMDT_h
#define Uci__Type__StrikeActivityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__StrikeActivityType_h)
# include "uci/type/StrikeActivityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the StrikeActivityMDT sequence accessor class */
      class StrikeActivityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~StrikeActivityMDT()
         { }

         /** Returns this accessor's type constant, i.e. StrikeActivityMDT
           *
           * @return This accessor's type constant, i.e. StrikeActivityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::strikeActivityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const StrikeActivityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the dynamic operational characteristics of an Activity of a Strike Capability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::StrikeActivityType, uci::type::accessorType::strikeActivityType> Activity;

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


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual const uci::type::StrikeActivityMDT::Activity& getActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual uci::type::StrikeActivityMDT::Activity& getActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Activity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivity(const uci::type::StrikeActivityMDT::Activity& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         StrikeActivityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The StrikeActivityMDT to copy from
           */
         StrikeActivityMDT(const StrikeActivityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this StrikeActivityMDT to the contents of the StrikeActivityMDT on the
           * right hand side (rhs) of the assignment operator.StrikeActivityMDT [only available to derived classes]
           *
           * @param rhs The StrikeActivityMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::StrikeActivityMDT
           * @return A constant reference to this StrikeActivityMDT.
           */
         const StrikeActivityMDT& operator=(const StrikeActivityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // StrikeActivityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__StrikeActivityMDT_h

