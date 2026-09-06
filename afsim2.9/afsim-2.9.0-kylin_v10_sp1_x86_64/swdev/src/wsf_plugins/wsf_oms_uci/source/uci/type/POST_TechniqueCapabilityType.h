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
#ifndef Uci__Type__POST_TechniqueCapabilityType_h
#define Uci__Type__POST_TechniqueCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__POST_TechniqueEnum_h)
# include "uci/type/POST_TechniqueEnum.h"
#endif

#if !defined(Uci__Type__FloatMinMaxType_h)
# include "uci/type/FloatMinMaxType.h"
#endif

#if !defined(Uci__Type__CapabilityFalseAlarmLevelEnum_h)
# include "uci/type/CapabilityFalseAlarmLevelEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the POST_TechniqueCapabilityType sequence accessor class */
      class POST_TechniqueCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~POST_TechniqueCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. POST_TechniqueCapabilityType
           *
           * @return This accessor's type constant, i.e. POST_TechniqueCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pOST_TechniqueCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const POST_TechniqueCapabilityType& accessor)
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


         /** Returns the accessor that provides access to the complex content that is identified by the Intensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Intensity.
           */
         virtual const uci::type::FloatMinMaxType& getIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Intensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by Intensity.
           */
         virtual uci::type::FloatMinMaxType& getIntensity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Intensity to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Intensity
           */
         virtual void setIntensity(const uci::type::FloatMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Intensity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Intensity is emabled or not
           */
         virtual bool hasIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Intensity
           *
           * @param type = uci::type::accessorType::floatMinMaxType This Accessor's accessor type
           */
         virtual void enableIntensity(uci::base::accessorType::AccessorType type = uci::type::accessorType::floatMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Intensity */
         virtual void clearIntensity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Constellation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Constellation.
           */
         virtual const uci::type::FloatMinMaxType& getConstellation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Constellation.
           *
           * @return The acecssor that provides access to the complex content that is identified by Constellation.
           */
         virtual uci::type::FloatMinMaxType& getConstellation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Constellation to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Constellation
           */
         virtual void setConstellation(const uci::type::FloatMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Constellation exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Constellation is emabled or not
           */
         virtual bool hasConstellation() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Constellation
           *
           * @param type = uci::type::accessorType::floatMinMaxType This Accessor's accessor type
           */
         virtual void enableConstellation(uci::base::accessorType::AccessorType type = uci::type::accessorType::floatMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Constellation */
         virtual void clearConstellation()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Stride.
           *
           * @return The acecssor that provides access to the complex content that is identified by Stride.
           */
         virtual const uci::type::FloatMinMaxType& getStride() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Stride.
           *
           * @return The acecssor that provides access to the complex content that is identified by Stride.
           */
         virtual uci::type::FloatMinMaxType& getStride()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Stride to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Stride
           */
         virtual void setStride(const uci::type::FloatMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Stride exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Stride is emabled or not
           */
         virtual bool hasStride() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Stride
           *
           * @param type = uci::type::accessorType::floatMinMaxType This Accessor's accessor type
           */
         virtual void enableStride(uci::base::accessorType::AccessorType type = uci::type::accessorType::floatMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Stride */
         virtual void clearStride()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointIntensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointIntensity.
           */
         virtual const uci::type::FloatMinMaxType& getPointIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PointIntensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by PointIntensity.
           */
         virtual uci::type::FloatMinMaxType& getPointIntensity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PointIntensity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PointIntensity
           */
         virtual void setPointIntensity(const uci::type::FloatMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PointIntensity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PointIntensity is emabled or not
           */
         virtual bool hasPointIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PointIntensity
           *
           * @param type = uci::type::accessorType::floatMinMaxType This Accessor's accessor type
           */
         virtual void enablePointIntensity(uci::base::accessorType::AccessorType type = uci::type::accessorType::floatMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PointIntensity */
         virtual void clearPointIntensity()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EdgeIntensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by EdgeIntensity.
           */
         virtual const uci::type::FloatMinMaxType& getEdgeIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the EdgeIntensity.
           *
           * @return The acecssor that provides access to the complex content that is identified by EdgeIntensity.
           */
         virtual uci::type::FloatMinMaxType& getEdgeIntensity()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the EdgeIntensity to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by EdgeIntensity
           */
         virtual void setEdgeIntensity(const uci::type::FloatMinMaxType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by EdgeIntensity exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by EdgeIntensity is emabled or not
           */
         virtual bool hasEdgeIntensity() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by EdgeIntensity
           *
           * @param type = uci::type::accessorType::floatMinMaxType This Accessor's accessor type
           */
         virtual void enableEdgeIntensity(uci::base::accessorType::AccessorType type = uci::type::accessorType::floatMinMaxType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by EdgeIntensity */
         virtual void clearEdgeIntensity()
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
         POST_TechniqueCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The POST_TechniqueCapabilityType to copy from
           */
         POST_TechniqueCapabilityType(const POST_TechniqueCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this POST_TechniqueCapabilityType to the contents of the
           * POST_TechniqueCapabilityType on the right hand side (rhs) of the assignment operator.POST_TechniqueCapabilityType
           * [only available to derived classes]
           *
           * @param rhs The POST_TechniqueCapabilityType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::POST_TechniqueCapabilityType
           * @return A constant reference to this POST_TechniqueCapabilityType.
           */
         const POST_TechniqueCapabilityType& operator=(const POST_TechniqueCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // POST_TechniqueCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__POST_TechniqueCapabilityType_h

