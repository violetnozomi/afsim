// This file was generated  on 12/5/2018 at 1:1:47 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__PO_ComponentStatusFocalPlaneArrayCollectionTimeType_h
#define Uci__Type__PO_ComponentStatusFocalPlaneArrayCollectionTimeType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DurationType_h)
# include "uci/type/DurationType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentStatusFocalPlaneArrayCollectionTimeType sequence accessor class */
      class PO_ComponentStatusFocalPlaneArrayCollectionTimeType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentStatusFocalPlaneArrayCollectionTimeType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayCollectionTimeType
           *
           * @return This accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayCollectionTimeType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentStatusFocalPlaneArrayCollectionTimeType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentStatusFocalPlaneArrayCollectionTimeType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CollectionTimeSetting.
           *
           * @return The value of the simple primitive data type identified by CollectionTimeSetting.
           */
         virtual uci::type::DurationTypeValue getCollectionTimeSetting() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CollectionTimeSetting.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCollectionTimeSetting(uci::type::DurationTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoCollectionTime.
           *
           * @return The value of the simple primitive data type identified by AutoCollectionTime.
           */
         virtual xs::Boolean getAutoCollectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoCollectionTime.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoCollectionTime(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AutoCollectionTime exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AutoCollectionTime is emabled or not
           */
         virtual bool hasAutoCollectionTime() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AutoCollectionTime
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAutoCollectionTime(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AutoCollectionTime */
         virtual void clearAutoCollectionTime()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentStatusFocalPlaneArrayCollectionTimeType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayCollectionTimeType to copy from
           */
         PO_ComponentStatusFocalPlaneArrayCollectionTimeType(const PO_ComponentStatusFocalPlaneArrayCollectionTimeType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentStatusFocalPlaneArrayCollectionTimeType to the
           * contents of the PO_ComponentStatusFocalPlaneArrayCollectionTimeType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentStatusFocalPlaneArrayCollectionTimeType [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayCollectionTimeType on the right hand side (rhs) of the assignment
           *      operator whose contents are used to set the contents of this
           *      uci::type::PO_ComponentStatusFocalPlaneArrayCollectionTimeType
           * @return A constant reference to this PO_ComponentStatusFocalPlaneArrayCollectionTimeType.
           */
         const PO_ComponentStatusFocalPlaneArrayCollectionTimeType& operator=(const PO_ComponentStatusFocalPlaneArrayCollectionTimeType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentStatusFocalPlaneArrayCollectionTimeType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentStatusFocalPlaneArrayCollectionTimeType_h

