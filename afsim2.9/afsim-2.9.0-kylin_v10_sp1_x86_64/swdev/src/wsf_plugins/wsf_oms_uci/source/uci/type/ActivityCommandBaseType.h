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
#ifndef Uci__Type__ActivityCommandBaseType_h
#define Uci__Type__ActivityCommandBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__CapabilityCommandID_Type_h)
# include "uci/type/CapabilityCommandID_Type.h"
#endif

#if !defined(Uci__Type__MessageStateEnum_h)
# include "uci/type/MessageStateEnum.h"
#endif

#if !defined(Uci__Type__ActivityID_Type_h)
# include "uci/type/ActivityID_Type.h"
#endif

#if !defined(Uci__Type__RankingType_h)
# include "uci/type/RankingType.h"
#endif

#if !defined(Uci__Type__CapabilityStateCommandEnum_h)
# include "uci/type/CapabilityStateCommandEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ActivityCommandBaseType sequence accessor class */
      class ActivityCommandBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ActivityCommandBaseType()
         { }

         /** Returns this accessor's type constant, i.e. ActivityCommandBaseType
           *
           * @return This accessor's type constant, i.e. ActivityCommandBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::activityCommandBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ActivityCommandBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual const uci::type::CapabilityCommandID_Type& getCommandID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CommandID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CommandID.
           */
         virtual uci::type::CapabilityCommandID_Type& getCommandID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CommandID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CommandID
           */
         virtual void setCommandID(const uci::type::CapabilityCommandID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual const uci::type::MessageStateEnum& getCommandState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the CommandState.
           *
           * @return The value of the enumeration identified by CommandState.
           */
         virtual uci::type::MessageStateEnum& getCommandState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(const uci::type::MessageStateEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the CommandState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setCommandState(uci::type::MessageStateEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual const uci::type::ActivityID_Type& getActivityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ActivityID.
           *
           * @return The acecssor that provides access to the complex content that is identified by ActivityID.
           */
         virtual uci::type::ActivityID_Type& getActivityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ActivityID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ActivityID
           */
         virtual void setActivityID(const uci::type::ActivityID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeActivityRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeActivityRank.
           */
         virtual const uci::type::RankingType& getChangeActivityRank() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChangeActivityRank.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChangeActivityRank.
           */
         virtual uci::type::RankingType& getChangeActivityRank()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChangeActivityRank to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChangeActivityRank
           */
         virtual void setChangeActivityRank(const uci::type::RankingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChangeActivityRank exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChangeActivityRank is emabled or not
           */
         virtual bool hasChangeActivityRank() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChangeActivityRank
           *
           * @param type = uci::type::accessorType::rankingType This Accessor's accessor type
           */
         virtual void enableChangeActivityRank(uci::base::accessorType::AccessorType type = uci::type::accessorType::rankingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChangeActivityRank */
         virtual void clearChangeActivityRank()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ChangeActivityState.
           *
           * @return The value of the enumeration identified by ChangeActivityState.
           */
         virtual const uci::type::CapabilityStateCommandEnum& getChangeActivityState() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the ChangeActivityState.
           *
           * @return The value of the enumeration identified by ChangeActivityState.
           */
         virtual uci::type::CapabilityStateCommandEnum& getChangeActivityState()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeActivityState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeActivityState(const uci::type::CapabilityStateCommandEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the ChangeActivityState.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setChangeActivityState(uci::type::CapabilityStateCommandEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield ChangeActivityStateis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasChangeActivityState() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getChangeActivityState will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::capabilityStateCommandEnum This Accessor's accessor type
           */
         virtual void enableChangeActivityState(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityStateCommandEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearChangeActivityState()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DeleteActivity.
           *
           * @return The value of the simple primitive data type identified by DeleteActivity.
           */
         virtual xs::Boolean getDeleteActivity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DeleteActivity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDeleteActivity(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DeleteActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DeleteActivity is emabled or not
           */
         virtual bool hasDeleteActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DeleteActivity
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableDeleteActivity(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DeleteActivity */
         virtual void clearDeleteActivity()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the UnassignActivity.
           *
           * @return The value of the simple primitive data type identified by UnassignActivity.
           */
         virtual xs::Boolean getUnassignActivity() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the UnassignActivity.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setUnassignActivity(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by UnassignActivity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by UnassignActivity is emabled or not
           */
         virtual bool hasUnassignActivity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by UnassignActivity
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableUnassignActivity(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by UnassignActivity */
         virtual void clearUnassignActivity()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ActivityCommandBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ActivityCommandBaseType to copy from
           */
         ActivityCommandBaseType(const ActivityCommandBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ActivityCommandBaseType to the contents of the
           * ActivityCommandBaseType on the right hand side (rhs) of the assignment operator.ActivityCommandBaseType [only
           * available to derived classes]
           *
           * @param rhs The ActivityCommandBaseType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::ActivityCommandBaseType
           * @return A constant reference to this ActivityCommandBaseType.
           */
         const ActivityCommandBaseType& operator=(const ActivityCommandBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ActivityCommandBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ActivityCommandBaseType_h

