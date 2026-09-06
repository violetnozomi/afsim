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
#ifndef Uci__Type__ServiceLifecycleRequestMDT_h
#define Uci__Type__ServiceLifecycleRequestMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ServiceLifecycleRequestID_Type_h)
# include "uci/type/ServiceLifecycleRequestID_Type.h"
#endif

#if !defined(Uci__Type__MissionID_Type_h)
# include "uci/type/MissionID_Type.h"
#endif

#if !defined(Uci__Type__ServiceID_Type_h)
# include "uci/type/ServiceID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__ServiceLifecycleStateEnum_h)
# include "uci/type/ServiceLifecycleStateEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ServiceLifecycleRequestMDT sequence accessor class */
      class ServiceLifecycleRequestMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ServiceLifecycleRequestMDT()
         { }

         /** Returns this accessor's type constant, i.e. ServiceLifecycleRequestMDT
           *
           * @return This accessor's type constant, i.e. ServiceLifecycleRequestMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::serviceLifecycleRequestMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ServiceLifecycleRequestMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Represents the ID of the services that should be affected by this request. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ServiceID_Type, uci::type::accessorType::serviceID_Type> AffectedServiceID;

         /** Returns the accessor that provides access to the complex content that is identified by the ServiceLifecycleRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceLifecycleRequestID.
           */
         virtual const uci::type::ServiceLifecycleRequestID_Type& getServiceLifecycleRequestID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ServiceLifecycleRequestID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ServiceLifecycleRequestID.
           */
         virtual uci::type::ServiceLifecycleRequestID_Type& getServiceLifecycleRequestID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ServiceLifecycleRequestID to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ServiceLifecycleRequestID
           */
         virtual void setServiceLifecycleRequestID(const uci::type::ServiceLifecycleRequestID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual const uci::type::MissionID_Type& getMissionID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the MissionID.
           *
           * @return The acecssor that provides access to the complex content that is identified by MissionID.
           */
         virtual uci::type::MissionID_Type& getMissionID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the MissionID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by MissionID
           */
         virtual void setMissionID(const uci::type::MissionID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EffectingServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EffectingServiceID.
           */
         virtual const uci::type::ServiceID_Type& getEffectingServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EffectingServiceID.
           *
           * @return The acecssor that provides access to the complex content that is identified by EffectingServiceID.
           */
         virtual uci::type::ServiceID_Type& getEffectingServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EffectingServiceID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EffectingServiceID
           */
         virtual void setEffectingServiceID(const uci::type::ServiceID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EffectingServiceID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EffectingServiceID is emabled or not
           */
         virtual bool hasEffectingServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EffectingServiceID
           *
           * @param type = uci::type::accessorType::serviceID_Type This Accessor's accessor type
           */
         virtual void enableEffectingServiceID(uci::base::accessorType::AccessorType type = uci::type::accessorType::serviceID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EffectingServiceID */
         virtual void clearEffectingServiceID()
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AffectedServiceID.
           *
           * @return The bounded list identified by AffectedServiceID.
           */
         virtual const uci::type::ServiceLifecycleRequestMDT::AffectedServiceID& getAffectedServiceID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AffectedServiceID.
           *
           * @return The bounded list identified by AffectedServiceID.
           */
         virtual uci::type::ServiceLifecycleRequestMDT::AffectedServiceID& getAffectedServiceID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AffectedServiceID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAffectedServiceID(const uci::type::ServiceLifecycleRequestMDT::AffectedServiceID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LifecycleState.
           *
           * @return The value of the enumeration identified by LifecycleState.
           */
         virtual const uci::type::ServiceLifecycleStateEnum& getLifecycleState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LifecycleState.
           *
           * @return The value of the enumeration identified by LifecycleState.
           */
         virtual uci::type::ServiceLifecycleStateEnum& getLifecycleState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LifecycleState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLifecycleState(const uci::type::ServiceLifecycleStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LifecycleState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLifecycleState(uci::type::ServiceLifecycleStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ServiceLifecycleRequestMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ServiceLifecycleRequestMDT to copy from
           */
         ServiceLifecycleRequestMDT(const ServiceLifecycleRequestMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ServiceLifecycleRequestMDT to the contents of the
           * ServiceLifecycleRequestMDT on the right hand side (rhs) of the assignment operator.ServiceLifecycleRequestMDT [only
           * available to derived classes]
           *
           * @param rhs The ServiceLifecycleRequestMDT on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ServiceLifecycleRequestMDT
           * @return A constant reference to this ServiceLifecycleRequestMDT.
           */
         const ServiceLifecycleRequestMDT& operator=(const ServiceLifecycleRequestMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ServiceLifecycleRequestMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ServiceLifecycleRequestMDT_h

