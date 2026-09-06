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
#ifndef Uci__Type__AMTI_ActivityQFT_h
#define Uci__Type__AMTI_ActivityQFT_h 1

#if !defined(Uci__Type__QueryFilterPET_h)
# include "uci/type/QueryFilterPET.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__AMTI_CapabilityEnum_h)
# include "uci/type/AMTI_CapabilityEnum.h"
#endif

#if !defined(Uci__Type__AMTI_SubCapabilityEnum_h)
# include "uci/type/AMTI_SubCapabilityEnum.h"
#endif

#if !defined(Uci__Type__SubsystemID_Type_h)
# include "uci/type/SubsystemID_Type.h"
#endif

#if !defined(Uci__Type__ActivitySourceType_h)
# include "uci/type/ActivitySourceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AMTI_ActivityQFT sequence accessor class */
      class AMTI_ActivityQFT : public virtual uci::type::QueryFilterPET {
      public:

         /** The destructor */
         virtual ~AMTI_ActivityQFT()
         { }

         /** Returns this accessor's type constant, i.e. AMTI_ActivityQFT
           *
           * @return This accessor's type constant, i.e. AMTI_ActivityQFT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::aMTI_ActivityQFT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AMTI_ActivityQFT& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ActivityID_Type, uci::type::accessorType::activityID_Type> ActivityID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 2] */
         typedef uci::base::BoundedList<uci::type::AMTI_CapabilityEnum, uci::type::accessorType::aMTI_CapabilityEnum> CapabilityType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 3] */
         typedef uci::base::BoundedList<uci::type::AMTI_SubCapabilityEnum, uci::type::accessorType::aMTI_SubCapabilityEnum> SubCapabilityType;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::SubsystemID_Type, uci::type::accessorType::subsystemID_Type> SubsystemID;

         /** [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ActivitySourceType, uci::type::accessorType::activitySourceType> Source;

         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual const uci::type::AMTI_ActivityQFT::ActivityID& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the ActivityID.
           *
           * @return The bounded list identified by ActivityID.
           */
         virtual uci::type::AMTI_ActivityQFT::ActivityID& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the ActivityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setActivityID(const uci::type::AMTI_ActivityQFT::ActivityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::AMTI_ActivityQFT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::AMTI_ActivityQFT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::AMTI_ActivityQFT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityType.
           *
           * @return The bounded list identified by CapabilityType.
           */
         virtual const uci::type::AMTI_ActivityQFT::CapabilityType& getCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityType.
           *
           * @return The bounded list identified by CapabilityType.
           */
         virtual uci::type::AMTI_ActivityQFT::CapabilityType& getCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityType(const uci::type::AMTI_ActivityQFT::CapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual const uci::type::AMTI_ActivityQFT::SubCapabilityType& getSubCapabilityType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubCapabilityType.
           *
           * @return The bounded list identified by SubCapabilityType.
           */
         virtual uci::type::AMTI_ActivityQFT::SubCapabilityType& getSubCapabilityType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubCapabilityType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubCapabilityType(const uci::type::AMTI_ActivityQFT::SubCapabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual const uci::type::AMTI_ActivityQFT::SubsystemID& getSubsystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the SubsystemID.
           *
           * @return The bounded list identified by SubsystemID.
           */
         virtual uci::type::AMTI_ActivityQFT::SubsystemID& getSubsystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the SubsystemID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSubsystemID(const uci::type::AMTI_ActivityQFT::SubsystemID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual const uci::type::AMTI_ActivityQFT::Source& getSource() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Source.
           *
           * @return The bounded list identified by Source.
           */
         virtual uci::type::AMTI_ActivityQFT::Source& getSource()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Source.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setSource(const uci::type::AMTI_ActivityQFT::Source& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AMTI_ActivityQFT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AMTI_ActivityQFT to copy from
           */
         AMTI_ActivityQFT(const AMTI_ActivityQFT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AMTI_ActivityQFT to the contents of the AMTI_ActivityQFT on the
           * right hand side (rhs) of the assignment operator.AMTI_ActivityQFT [only available to derived classes]
           *
           * @param rhs The AMTI_ActivityQFT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AMTI_ActivityQFT
           * @return A constant reference to this AMTI_ActivityQFT.
           */
         const AMTI_ActivityQFT& operator=(const AMTI_ActivityQFT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AMTI_ActivityQFT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AMTI_ActivityQFT_h

