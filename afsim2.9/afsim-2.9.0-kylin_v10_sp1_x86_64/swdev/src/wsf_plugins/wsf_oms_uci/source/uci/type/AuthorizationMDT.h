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
#ifndef Uci__Type__AuthorizationMDT_h
#define Uci__Type__AuthorizationMDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__AuthorizationID_Type_h)
# include "uci/type/AuthorizationID_Type.h"
#endif

#if !defined(Uci__Type__UserID_Type_h)
# include "uci/type/UserID_Type.h"
#endif

#if !defined(Uci__Type__SystemID_Type_h)
# include "uci/type/SystemID_Type.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__TaskTypeEnum_h)
# include "uci/type/TaskTypeEnum.h"
#endif

#if !defined(Uci__Type__DateTimeRangeType_h)
# include "uci/type/DateTimeRangeType.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the AuthorizationMDT sequence accessor class */
      class AuthorizationMDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~AuthorizationMDT()
         { }

         /** Returns this accessor's type constant, i.e. AuthorizationMDT
           *
           * @return This accessor's type constant, i.e. AuthorizationMDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::authorizationMDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const AuthorizationMDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates which Task type(s) the Authorization applies to. If omitted, the Authorization applies to all task types.
           * [Minimum occurrences: 0] [Maximum occurrences: 22]
           */
         typedef uci::base::BoundedList<uci::type::TaskTypeEnum, uci::type::accessorType::taskTypeEnum> TaskType;

         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizationID.
           */
         virtual const uci::type::AuthorizationID_Type& getAuthorizationID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizationID.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizationID.
           */
         virtual uci::type::AuthorizationID_Type& getAuthorizationID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AuthorizationID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AuthorizationID
           */
         virtual void setAuthorizationID(const uci::type::AuthorizationID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the UserID.
           *
           * @return The value of the string data type identified by UserID.
           */
         virtual const uci::type::UserID_Type& getUserID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the UserID.
           *
           * @return The value of the string data type identified by UserID.
           */
         virtual uci::type::UserID_Type& getUserID()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the UserID to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setUserID(const uci::type::UserID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the UserID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setUserID(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the UserID to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setUserID(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual const uci::type::SystemID_Type& getSystemID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the SystemID.
           *
           * @return The acecssor that provides access to the complex content that is identified by SystemID.
           */
         virtual uci::type::SystemID_Type& getSystemID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the SystemID to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by SystemID
           */
         virtual void setSystemID(const uci::type::SystemID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Precedence.
           *
           * @return The value of the simple primitive data type identified by Precedence.
           */
         virtual xs::UnsignedInt getPrecedence() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Precedence.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPrecedence(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskType.
           *
           * @return The bounded list identified by TaskType.
           */
         virtual const uci::type::AuthorizationMDT::TaskType& getTaskType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the TaskType.
           *
           * @return The bounded list identified by TaskType.
           */
         virtual uci::type::AuthorizationMDT::TaskType& getTaskType()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the TaskType.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setTaskType(const uci::type::AuthorizationMDT::TaskType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizedTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizedTime.
           */
         virtual const uci::type::DateTimeRangeType& getAuthorizedTime() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizedTime.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizedTime.
           */
         virtual uci::type::DateTimeRangeType& getAuthorizedTime()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AuthorizedTime to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AuthorizedTime
           */
         virtual void setAuthorizedTime(const uci::type::DateTimeRangeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AuthorizedTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AuthorizedTime is emabled or not
           */
         virtual bool hasAuthorizedTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AuthorizedTime
           *
           * @param type = uci::type::accessorType::dateTimeRangeType This Accessor's accessor type
           */
         virtual void enableAuthorizedTime(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeRangeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AuthorizedTime */
         virtual void clearAuthorizedTime()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizedArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizedArea.
           */
         virtual const uci::type::ZoneType& getAuthorizedArea() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the AuthorizedArea.
           *
           * @return The acecssor that provides access to the complex content that is identified by AuthorizedArea.
           */
         virtual uci::type::ZoneType& getAuthorizedArea()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the AuthorizedArea to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by AuthorizedArea
           */
         virtual void setAuthorizedArea(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AuthorizedArea exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AuthorizedArea is emabled or not
           */
         virtual bool hasAuthorizedArea() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AuthorizedArea
           *
           * @param type = uci::type::accessorType::zoneType This Accessor's accessor type
           */
         virtual void enableAuthorizedArea(uci::base::accessorType::AccessorType type = uci::type::accessorType::zoneType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AuthorizedArea */
         virtual void clearAuthorizedArea()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AuthorizationMDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AuthorizationMDT to copy from
           */
         AuthorizationMDT(const AuthorizationMDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AuthorizationMDT to the contents of the AuthorizationMDT on the
           * right hand side (rhs) of the assignment operator.AuthorizationMDT [only available to derived classes]
           *
           * @param rhs The AuthorizationMDT on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::AuthorizationMDT
           * @return A constant reference to this AuthorizationMDT.
           */
         const AuthorizationMDT& operator=(const AuthorizationMDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AuthorizationMDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__AuthorizationMDT_h

