// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__AirSampleActivityMDT_h
#define Uci__Type__AirSampleActivityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AirSampleActivityType_h)
# include "uci/type/AirSampleActivityType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AirSampleActivityMDT sequence accessor class */
      class AirSampleActivityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AirSampleActivityMDT()
         { }

         /** Returns this accessor's type constant, i.e. AirSampleActivityMDT
           *
           * @return This accessor's type constant, i.e. AirSampleActivityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::airSampleActivityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AirSampleActivityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the dynamic operational characteristics of an Activity of an AirSample Capability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AirSampleActivityType, uci::type::accessorType::airSampleActivityType> Activity;

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
         virtual const uci::type::AirSampleActivityMDT::Activity& getActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual uci::type::AirSampleActivityMDT::Activity& getActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Activity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivity(const uci::type::AirSampleActivityMDT::Activity& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AirSampleActivityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AirSampleActivityMDT to copy from
           */
         AirSampleActivityMDT(const AirSampleActivityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AirSampleActivityMDT to the contents of the AirSampleActivityMDT
           * on the right hand side (rhs) of the assignment operator.AirSampleActivityMDT [only available to derived classes]
           *
           * @param rhs The AirSampleActivityMDT on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::AirSampleActivityMDT
           * @return A constant reference to this AirSampleActivityMDT.
           */
         const AirSampleActivityMDT& operator=(const AirSampleActivityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AirSampleActivityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AirSampleActivityMDT_h

