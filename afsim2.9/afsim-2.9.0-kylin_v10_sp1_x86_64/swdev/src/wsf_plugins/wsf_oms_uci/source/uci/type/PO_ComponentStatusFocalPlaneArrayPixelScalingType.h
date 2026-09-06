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
#ifndef Uci__Type__PO_ComponentStatusFocalPlaneArrayPixelScalingType_h
#define Uci__Type__PO_ComponentStatusFocalPlaneArrayPixelScalingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentStatusFocalPlaneArrayPixelScalingType sequence accessor class */
      class PO_ComponentStatusFocalPlaneArrayPixelScalingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentStatusFocalPlaneArrayPixelScalingType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayPixelScalingType
           *
           * @return This accessor's type constant, i.e. PO_ComponentStatusFocalPlaneArrayPixelScalingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentStatusFocalPlaneArrayPixelScalingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentStatusFocalPlaneArrayPixelScalingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Gain.
           *
           * @return The value of the simple primitive data type identified by Gain.
           */
         virtual xs::Double getGain() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Gain.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGain(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Offset.
           *
           * @return The value of the simple primitive data type identified by Offset.
           */
         virtual xs::Double getOffset() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Offset.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffset(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AutoScaling.
           *
           * @return The value of the simple primitive data type identified by AutoScaling.
           */
         virtual xs::Boolean getAutoScaling() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AutoScaling.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAutoScaling(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AutoScaling exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AutoScaling is emabled or not
           */
         virtual bool hasAutoScaling() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AutoScaling
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableAutoScaling(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AutoScaling */
         virtual void clearAutoScaling()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentStatusFocalPlaneArrayPixelScalingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayPixelScalingType to copy from
           */
         PO_ComponentStatusFocalPlaneArrayPixelScalingType(const PO_ComponentStatusFocalPlaneArrayPixelScalingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentStatusFocalPlaneArrayPixelScalingType to the contents
           * of the PO_ComponentStatusFocalPlaneArrayPixelScalingType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentStatusFocalPlaneArrayPixelScalingType [only available to derived classes]
           *
           * @param rhs The PO_ComponentStatusFocalPlaneArrayPixelScalingType on the right hand side (rhs) of the assignment
           *      operator whose contents are used to set the contents of this
           *      uci::type::PO_ComponentStatusFocalPlaneArrayPixelScalingType
           * @return A constant reference to this PO_ComponentStatusFocalPlaneArrayPixelScalingType.
           */
         const PO_ComponentStatusFocalPlaneArrayPixelScalingType& operator=(const PO_ComponentStatusFocalPlaneArrayPixelScalingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentStatusFocalPlaneArrayPixelScalingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentStatusFocalPlaneArrayPixelScalingType_h

