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
#ifndef Uci__Type__PO_ComponentSettingsProcessingStagePixelScalingType_h
#define Uci__Type__PO_ComponentSettingsProcessingStagePixelScalingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__IntAccessor_h)
# include "uci/base/IntAccessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentSettingsProcessingStagePixelScalingType sequence accessor class */
      class PO_ComponentSettingsProcessingStagePixelScalingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsProcessingStagePixelScalingType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentSettingsProcessingStagePixelScalingType
           *
           * @return This accessor's type constant, i.e. PO_ComponentSettingsProcessingStagePixelScalingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentSettingsProcessingStagePixelScalingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentSettingsProcessingStagePixelScalingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Offset.
           *
           * @return The value of the simple primitive data type identified by Offset.
           */
         virtual xs::Int getOffset() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Offset.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setOffset(xs::Int value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Offset exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Offset is emabled or not
           */
         virtual bool hasOffset() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Offset
           *
           * @param type = uci::base::accessorType::intAccessor This Accessor's accessor type
           */
         virtual void enableOffset(uci::base::accessorType::AccessorType type = uci::base::accessorType::intAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Offset */
         virtual void clearOffset()
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


         /** Returns whether the Element that is identified by Gain exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Gain is emabled or not
           */
         virtual bool hasGain() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Gain
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableGain(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Gain */
         virtual void clearGain()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentSettingsProcessingStagePixelScalingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStagePixelScalingType to copy from
           */
         PO_ComponentSettingsProcessingStagePixelScalingType(const PO_ComponentSettingsProcessingStagePixelScalingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsProcessingStagePixelScalingType to the
           * contents of the PO_ComponentSettingsProcessingStagePixelScalingType on the right hand side (rhs) of the assignment
           * operator.PO_ComponentSettingsProcessingStagePixelScalingType [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStagePixelScalingType on the right hand side (rhs) of the assignment
           *      operator whose contents are used to set the contents of this
           *      uci::type::PO_ComponentSettingsProcessingStagePixelScalingType
           * @return A constant reference to this PO_ComponentSettingsProcessingStagePixelScalingType.
           */
         const PO_ComponentSettingsProcessingStagePixelScalingType& operator=(const PO_ComponentSettingsProcessingStagePixelScalingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsProcessingStagePixelScalingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentSettingsProcessingStagePixelScalingType_h

