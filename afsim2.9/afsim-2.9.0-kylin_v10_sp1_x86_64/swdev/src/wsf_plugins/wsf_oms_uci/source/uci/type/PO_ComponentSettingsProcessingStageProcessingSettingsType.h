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
#ifndef Uci__Type__PO_ComponentSettingsProcessingStageProcessingSettingsType_h
#define Uci__Type__PO_ComponentSettingsProcessingStageProcessingSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsProcessingStagePixelScalingType_h)
# include "uci/type/PO_ComponentSettingsProcessingStagePixelScalingType.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PO_ComponentSettingsProcessingStageProcessingSettingsType sequence accessor class */
      class PO_ComponentSettingsProcessingStageProcessingSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PO_ComponentSettingsProcessingStageProcessingSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. PO_ComponentSettingsProcessingStageProcessingSettingsType
           *
           * @return This accessor's type constant, i.e. PO_ComponentSettingsProcessingStageProcessingSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pO_ComponentSettingsProcessingStageProcessingSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PO_ComponentSettingsProcessingStageProcessingSettingsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScaling.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScaling.
           */
         virtual const uci::type::PO_ComponentSettingsProcessingStagePixelScalingType& getPixelScaling() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PixelScaling.
           *
           * @return The acecssor that provides access to the complex content that is identified by PixelScaling.
           */
         virtual uci::type::PO_ComponentSettingsProcessingStagePixelScalingType& getPixelScaling()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PixelScaling to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PixelScaling
           */
         virtual void setPixelScaling(const uci::type::PO_ComponentSettingsProcessingStagePixelScalingType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PixelScaling exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PixelScaling is emabled or not
           */
         virtual bool hasPixelScaling() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PixelScaling
           *
           * @param type = uci::type::accessorType::pO_ComponentSettingsProcessingStagePixelScalingType This Accessor's accessor
           *      type
           */
         virtual void enablePixelScaling(uci::base::accessorType::AccessorType type = uci::type::accessorType::pO_ComponentSettingsProcessingStagePixelScalingType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PixelScaling */
         virtual void clearPixelScaling()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Hue.
           *
           * @return The value of the simple primitive data type identified by Hue.
           */
         virtual uci::type::PercentTypeValue getHue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Hue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHue(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Hue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Hue is emabled or not
           */
         virtual bool hasHue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Hue
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableHue(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Hue */
         virtual void clearHue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Saturation.
           *
           * @return The value of the simple primitive data type identified by Saturation.
           */
         virtual uci::type::PercentTypeValue getSaturation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Saturation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSaturation(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Saturation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Saturation is emabled or not
           */
         virtual bool hasSaturation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Saturation
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableSaturation(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Saturation */
         virtual void clearSaturation()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GammaValue.
           *
           * @return The value of the simple primitive data type identified by GammaValue.
           */
         virtual xs::Double getGammaValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GammaValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGammaValue(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by GammaValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by GammaValue is emabled or not
           */
         virtual bool hasGammaValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by GammaValue
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableGammaValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by GammaValue */
         virtual void clearGammaValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the SharpnessValue.
           *
           * @return The value of the simple primitive data type identified by SharpnessValue.
           */
         virtual uci::type::PercentTypeValue getSharpnessValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the SharpnessValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setSharpnessValue(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by SharpnessValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by SharpnessValue is emabled or not
           */
         virtual bool hasSharpnessValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by SharpnessValue
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableSharpnessValue(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by SharpnessValue */
         virtual void clearSharpnessValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EdgeEnhancement.
           *
           * @return The value of the simple primitive data type identified by EdgeEnhancement.
           */
         virtual uci::type::PercentTypeValue getEdgeEnhancement() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EdgeEnhancement.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEdgeEnhancement(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EdgeEnhancement exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EdgeEnhancement is emabled or not
           */
         virtual bool hasEdgeEnhancement() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EdgeEnhancement
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableEdgeEnhancement(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EdgeEnhancement */
         virtual void clearEdgeEnhancement()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ElectronicZoom.
           *
           * @return The value of the simple primitive data type identified by ElectronicZoom.
           */
         virtual uci::type::PercentTypeValue getElectronicZoom() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ElectronicZoom.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setElectronicZoom(uci::type::PercentTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ElectronicZoom exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ElectronicZoom is emabled or not
           */
         virtual bool hasElectronicZoom() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ElectronicZoom
           *
           * @param type = uci::type::accessorType::percentType This Accessor's accessor type
           */
         virtual void enableElectronicZoom(uci::base::accessorType::AccessorType type = uci::type::accessorType::percentType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ElectronicZoom */
         virtual void clearElectronicZoom()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PO_ComponentSettingsProcessingStageProcessingSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStageProcessingSettingsType to copy from
           */
         PO_ComponentSettingsProcessingStageProcessingSettingsType(const PO_ComponentSettingsProcessingStageProcessingSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PO_ComponentSettingsProcessingStageProcessingSettingsType to the
           * contents of the PO_ComponentSettingsProcessingStageProcessingSettingsType on the right hand side (rhs) of the
           * assignment operator.PO_ComponentSettingsProcessingStageProcessingSettingsType [only available to derived classes]
           *
           * @param rhs The PO_ComponentSettingsProcessingStageProcessingSettingsType on the right hand side (rhs) of the
           *      assignment operator whose contents are used to set the contents of this
           *      uci::type::PO_ComponentSettingsProcessingStageProcessingSettingsType
           * @return A constant reference to this PO_ComponentSettingsProcessingStageProcessingSettingsType.
           */
         const PO_ComponentSettingsProcessingStageProcessingSettingsType& operator=(const PO_ComponentSettingsProcessingStageProcessingSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PO_ComponentSettingsProcessingStageProcessingSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PO_ComponentSettingsProcessingStageProcessingSettingsType_h

