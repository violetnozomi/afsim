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
#ifndef Uci__Type__EA_ActivityMDT_h
#define Uci__Type__EA_ActivityMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__EA_ActivityType_h)
# include "uci/type/EA_ActivityType.h"
#endif

#if !defined(Uci__Type__VehicleThreatAssessmentType_h)
# include "uci/type/VehicleThreatAssessmentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EA_ActivityMDT sequence accessor class */
      class EA_ActivityMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EA_ActivityMDT()
         { }

         /** Returns this accessor's type constant, i.e. EA_ActivityMDT
           *
           * @return This accessor's type constant, i.e. EA_ActivityMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eA_ActivityMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EA_ActivityMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the dynamic operational characteristics of an Activity of an EA Capability. [Maximum occurrences:
           * 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::EA_ActivityType, uci::type::accessorType::eA_ActivityType> Activity;

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
         virtual const uci::type::EA_ActivityMDT::Activity& getActivity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Activity.
           *
           * @return The bounded list identified by Activity.
           */
         virtual uci::type::EA_ActivityMDT::Activity& getActivity()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Activity.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivity(const uci::type::EA_ActivityMDT::Activity& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProtectedEntityAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProtectedEntityAssessment.
           */
         virtual const uci::type::VehicleThreatAssessmentType& getProtectedEntityAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ProtectedEntityAssessment.
           *
           * @return The acecssor that provides access to the complex content that is identified by ProtectedEntityAssessment.
           */
         virtual uci::type::VehicleThreatAssessmentType& getProtectedEntityAssessment()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ProtectedEntityAssessment to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ProtectedEntityAssessment
           */
         virtual void setProtectedEntityAssessment(const uci::type::VehicleThreatAssessmentType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ProtectedEntityAssessment exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ProtectedEntityAssessment is emabled or not
           */
         virtual bool hasProtectedEntityAssessment() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ProtectedEntityAssessment
           *
           * @param type = uci::type::accessorType::vehicleThreatAssessmentType This Accessor's accessor type
           */
         virtual void enableProtectedEntityAssessment(uci::base::accessorType::AccessorType type = uci::type::accessorType::vehicleThreatAssessmentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ProtectedEntityAssessment */
         virtual void clearProtectedEntityAssessment()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EA_ActivityMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EA_ActivityMDT to copy from
           */
         EA_ActivityMDT(const EA_ActivityMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EA_ActivityMDT to the contents of the EA_ActivityMDT on the right
           * hand side (rhs) of the assignment operator.EA_ActivityMDT [only available to derived classes]
           *
           * @param rhs The EA_ActivityMDT on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::EA_ActivityMDT
           * @return A constant reference to this EA_ActivityMDT.
           */
         const EA_ActivityMDT& operator=(const EA_ActivityMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EA_ActivityMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EA_ActivityMDT_h

