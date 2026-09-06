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
#ifndef Uci__Type__POST_TechniqueSettingType_h
#define Uci__Type__POST_TechniqueSettingType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_TechniqueEnum_h)
# include "uci/type/POST_TechniqueEnum.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__CapabilityFalseAlarmLevelEnum_h)
# include "uci/type/CapabilityFalseAlarmLevelEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_TechniqueSettingType sequence accessor class */
      class POST_TechniqueSettingType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_TechniqueSettingType()
         { }

         /** Returns this accessor's type constant, i.e. POST_TechniqueSettingType
           *
           * @return This accessor's type constant, i.e. POST_TechniqueSettingType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_TechniqueSettingType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_TechniqueSettingType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Technique.
           *
           * @return The value of the enumeration identified by Technique.
           */
         virtual const uci::type::POST_TechniqueEnum& getTechnique() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Technique.
           *
           * @return The value of the enumeration identified by Technique.
           */
         virtual uci::type::POST_TechniqueEnum& getTechnique()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Technique.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTechnique(const uci::type::POST_TechniqueEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Technique.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setTechnique(uci::type::POST_TechniqueEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IntensityValue.
           *
           * @return The value of the simple primitive data type identified by IntensityValue.
           */
         virtual xs::Float getIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IntensityValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIntensityValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by IntensityValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by IntensityValue is emabled or not
           */
         virtual bool hasIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by IntensityValue
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableIntensityValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by IntensityValue */
         virtual void clearIntensityValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ConstellationValue.
           *
           * @return The value of the simple primitive data type identified by ConstellationValue.
           */
         virtual xs::Float getConstellationValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ConstellationValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setConstellationValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ConstellationValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ConstellationValue is emabled or not
           */
         virtual bool hasConstellationValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ConstellationValue
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableConstellationValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ConstellationValue */
         virtual void clearConstellationValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the StrideValue.
           *
           * @return The value of the simple primitive data type identified by StrideValue.
           */
         virtual xs::Float getStrideValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the StrideValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setStrideValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by StrideValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by StrideValue is emabled or not
           */
         virtual bool hasStrideValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by StrideValue
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableStrideValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by StrideValue */
         virtual void clearStrideValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PointIntensityValue.
           *
           * @return The value of the simple primitive data type identified by PointIntensityValue.
           */
         virtual xs::Float getPointIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PointIntensityValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPointIntensityValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PointIntensityValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PointIntensityValue is emabled or not
           */
         virtual bool hasPointIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PointIntensityValue
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enablePointIntensityValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PointIntensityValue */
         virtual void clearPointIntensityValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the EdgeIntensityValue.
           *
           * @return The value of the simple primitive data type identified by EdgeIntensityValue.
           */
         virtual xs::Float getEdgeIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the EdgeIntensityValue.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setEdgeIntensityValue(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EdgeIntensityValue exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EdgeIntensityValue is emabled or not
           */
         virtual bool hasEdgeIntensityValue() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EdgeIntensityValue
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableEdgeIntensityValue(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EdgeIntensityValue */
         virtual void clearEdgeIntensityValue()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FalseAlarmLevel.
           *
           * @return The value of the enumeration identified by FalseAlarmLevel.
           */
         virtual const uci::type::CapabilityFalseAlarmLevelEnum& getFalseAlarmLevel() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FalseAlarmLevel.
           *
           * @return The value of the enumeration identified by FalseAlarmLevel.
           */
         virtual uci::type::CapabilityFalseAlarmLevelEnum& getFalseAlarmLevel()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FalseAlarmLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFalseAlarmLevel(const uci::type::CapabilityFalseAlarmLevelEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FalseAlarmLevel.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFalseAlarmLevel(uci::type::CapabilityFalseAlarmLevelEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FalseAlarmLevelis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFalseAlarmLevel() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFalseAlarmLevel will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::capabilityFalseAlarmLevelEnum This Accessor's accessor type
           */
         virtual void enableFalseAlarmLevel(uci::base::accessorType::AccessorType type = uci::type::accessorType::capabilityFalseAlarmLevelEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFalseAlarmLevel()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         POST_TechniqueSettingType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_TechniqueSettingType to copy from
           */
         POST_TechniqueSettingType(const POST_TechniqueSettingType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_TechniqueSettingType to the contents of the
           * POST_TechniqueSettingType on the right hand side (rhs) of the assignment operator.POST_TechniqueSettingType [only
           * available to derived classes]
           *
           * @param rhs The POST_TechniqueSettingType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::POST_TechniqueSettingType
           * @return A constant reference to this POST_TechniqueSettingType.
           */
         const POST_TechniqueSettingType& operator=(const POST_TechniqueSettingType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_TechniqueSettingType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_TechniqueSettingType_h

