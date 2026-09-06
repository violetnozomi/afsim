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
#ifndef Uci__Type__SignalNavDataType_h
#define Uci__Type__SignalNavDataType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

#if !defined(Uci__Type__KinematicsFixedType_h)
# include "uci/type/KinematicsFixedType.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__AntennaKinematicsType_h)
# include "uci/type/AntennaKinematicsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SignalNavDataType sequence accessor class */
      class SignalNavDataType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SignalNavDataType()
         { }

         /** Returns this accessor's type constant, i.e. SignalNavDataType
           *
           * @return This accessor's type constant, i.e. SignalNavDataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::signalNavDataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SignalNavDataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Data describing the orientation of the antenna and the velocity vector of the antenna face. [Minimum occurrences: 0]
           * [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::AntennaKinematicsType, uci::type::accessorType::antennaKinematicsType> AntennaKinematics;

         /** Returns the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @return The value of the simple primitive data type identified by Timestamp.
           */
         virtual uci::type::DateTimeTypeValue getTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Timestamp.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setTimestamp(uci::type::DateTimeTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformKinematics.
           */
         virtual const uci::type::KinematicsFixedType& getPlatformKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PlatformKinematics.
           *
           * @return The acecssor that provides access to the complex content that is identified by PlatformKinematics.
           */
         virtual uci::type::KinematicsFixedType& getPlatformKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PlatformKinematics to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PlatformKinematics
           */
         virtual void setPlatformKinematics(const uci::type::KinematicsFixedType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaKinematics.
           *
           * @return The bounded list identified by AntennaKinematics.
           */
         virtual const uci::type::SignalNavDataType::AntennaKinematics& getAntennaKinematics() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the AntennaKinematics.
           *
           * @return The bounded list identified by AntennaKinematics.
           */
         virtual uci::type::SignalNavDataType::AntennaKinematics& getAntennaKinematics()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the AntennaKinematics.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setAntennaKinematics(const uci::type::SignalNavDataType::AntennaKinematics& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SignalNavDataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SignalNavDataType to copy from
           */
         SignalNavDataType(const SignalNavDataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SignalNavDataType to the contents of the SignalNavDataType on the
           * right hand side (rhs) of the assignment operator.SignalNavDataType [only available to derived classes]
           *
           * @param rhs The SignalNavDataType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SignalNavDataType
           * @return A constant reference to this SignalNavDataType.
           */
         const SignalNavDataType& operator=(const SignalNavDataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SignalNavDataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SignalNavDataType_h

