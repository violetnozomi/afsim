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
#ifndef Uci__Type__POST_ComponentSettingsProcessingStageProcessingSettingsType_h
#define Uci__Type__POST_ComponentSettingsProcessingStageProcessingSettingsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PO_ComponentSettingsProcessingStagePixelScalingType_h)
# include "uci/type/PO_ComponentSettingsProcessingStagePixelScalingType.h"
#endif

#if !defined(Uci__Type__HSB_ColorControlType_h)
# include "uci/type/HSB_ColorControlType.h"
#endif

#if !defined(Uci__Type__RGB_ColorControlType_h)
# include "uci/type/RGB_ColorControlType.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

#if !defined(Uci__Type__PercentType_h)
# include "uci/type/PercentType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_ComponentSettingsProcessingStageProcessingSettingsType sequence accessor class */
      class POST_ComponentSettingsProcessingStageProcessingSettingsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_ComponentSettingsProcessingStageProcessingSettingsType()
         { }

         /** Returns this accessor's type constant, i.e. POST_ComponentSettingsProcessingStageProcessingSettingsType
           *
           * @return This accessor's type constant, i.e. POST_ComponentSettingsProcessingStageProcessingSettingsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_ComponentSettingsProcessingStageProcessingSettingsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_ComponentSettingsProcessingStageProcessingSettingsType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the HSB_ColorControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by HSB_ColorControl.
           */
         virtual const uci::type::HSB_ColorControlType& getHSB_ColorControl() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the HSB_ColorControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by HSB_ColorControl.
           */
         virtual uci::type::HSB_ColorControlType& getHSB_ColorControl()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the HSB_ColorControl to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by HSB_ColorControl
           */
         virtual void setHSB_ColorControl(const uci::type::HSB_ColorControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HSB_ColorControl exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HSB_ColorControl is emabled or not
           */
         virtual bool hasHSB_ColorControl() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HSB_ColorControl
           *
           * @param type = uci::type::accessorType::hSB_ColorControlType This Accessor's accessor type
           */
         virtual void enableHSB_ColorControl(uci::base::accessorType::AccessorType type = uci::type::accessorType::hSB_ColorControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HSB_ColorControl */
         virtual void clearHSB_ColorControl()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RGB_ColorControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by RGB_ColorControl.
           */
         virtual const uci::type::RGB_ColorControlType& getRGB_ColorControl() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RGB_ColorControl.
           *
           * @return The acecssor that provides access to the complex content that is identified by RGB_ColorControl.
           */
         virtual uci::type::RGB_ColorControlType& getRGB_ColorControl()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RGB_ColorControl to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RGB_ColorControl
           */
         virtual void setRGB_ColorControl(const uci::type::RGB_ColorControlType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RGB_ColorControl exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RGB_ColorControl is emabled or not
           */
         virtual bool hasRGB_ColorControl() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RGB_ColorControl
           *
           * @param type = uci::type::accessorType::rGB_ColorControlType This Accessor's accessor type
           */
         virtual void enableRGB_ColorControl(uci::base::accessorType::AccessorType type = uci::type::accessorType::rGB_ColorControlType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RGB_ColorControl */
         virtual void clearRGB_ColorControl()
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
         POST_ComponentSettingsProcessingStageProcessingSettingsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsProcessingStageProcessingSettingsType to copy from
           */
         POST_ComponentSettingsProcessingStageProcessingSettingsType(const POST_ComponentSettingsProcessingStageProcessingSettingsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_ComponentSettingsProcessingStageProcessingSettingsType to the
           * contents of the POST_ComponentSettingsProcessingStageProcessingSettingsType on the right hand side (rhs) of the
           * assignment operator.POST_ComponentSettingsProcessingStageProcessingSettingsType [only available to derived classes]
           *
           * @param rhs The POST_ComponentSettingsProcessingStageProcessingSettingsType on the right hand side (rhs) of the
           *      assignment operator whose contents are used to set the contents of this
           *      uci::type::POST_ComponentSettingsProcessingStageProcessingSettingsType
           * @return A constant reference to this POST_ComponentSettingsProcessingStageProcessingSettingsType.
           */
         const POST_ComponentSettingsProcessingStageProcessingSettingsType& operator=(const POST_ComponentSettingsProcessingStageProcessingSettingsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_ComponentSettingsProcessingStageProcessingSettingsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_ComponentSettingsProcessingStageProcessingSettingsType_h

