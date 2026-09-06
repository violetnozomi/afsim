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
#ifndef Uci__Type__Velocity2D_Type_h
#define Uci__Type__Velocity2D_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__SpeedType_h)
# include "uci/type/SpeedType.h"
#endif

#if !defined(Uci__Type__DateTimeType_h)
# include "uci/type/DateTimeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** Velocity relative to WGS-84 ellipsoid. */
      class Velocity2D_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~Velocity2D_Type()
         { }

         /** Returns this accessor's type constant, i.e. Velocity2D_Type
           *
           * @return This accessor's type constant, i.e. Velocity2D_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::velocity2D_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Velocity2D_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the NorthSpeed.
           *
           * @return The value of the simple primitive data type identified by NorthSpeed.
           */
         virtual uci::type::SpeedTypeValue getNorthSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the NorthSpeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNorthSpeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EastSpeed.
           *
           * @return The value of the simple primitive data type identified by EastSpeed.
           */
         virtual uci::type::SpeedTypeValue getEastSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EastSpeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEastSpeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the DownSpeed.
           *
           * @return The value of the simple primitive data type identified by DownSpeed.
           */
         virtual uci::type::SpeedTypeValue getDownSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the DownSpeed.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setDownSpeed(uci::type::SpeedTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by DownSpeed exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by DownSpeed is emabled or not
           */
         virtual bool hasDownSpeed() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by DownSpeed
           *
           * @param type = uci::type::accessorType::speedType This Accessor's accessor type
           */
         virtual void enableDownSpeed(uci::base::accessorType::AccessorType type = uci::type::accessorType::speedType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by DownSpeed */
         virtual void clearDownSpeed()
            throw(uci::base::UCIException) = 0;


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


         /** Returns whether the Element that is identified by Timestamp exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Timestamp is emabled or not
           */
         virtual bool hasTimestamp() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Timestamp
           *
           * @param type = uci::type::accessorType::dateTimeType This Accessor's accessor type
           */
         virtual void enableTimestamp(uci::base::accessorType::AccessorType type = uci::type::accessorType::dateTimeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Timestamp */
         virtual void clearTimestamp()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Velocity2D_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Velocity2D_Type to copy from
           */
         Velocity2D_Type(const Velocity2D_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Velocity2D_Type to the contents of the Velocity2D_Type on the
           * right hand side (rhs) of the assignment operator.Velocity2D_Type [only available to derived classes]
           *
           * @param rhs The Velocity2D_Type on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::Velocity2D_Type
           * @return A constant reference to this Velocity2D_Type.
           */
         const Velocity2D_Type& operator=(const Velocity2D_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Velocity2D_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Velocity2D_Type_h

