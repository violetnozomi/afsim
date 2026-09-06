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
#ifndef Uci__Type__SAR_SubCapabilityType_h
#define Uci__Type__SAR_SubCapabilityType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__RadarPointingTargetType_h)
# include "uci/type/RadarPointingTargetType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__SAR_EllipticityPolicyEnum_h)
# include "uci/type/SAR_EllipticityPolicyEnum.h"
#endif

#if !defined(Uci__Base__FloatAccessor_h)
# include "uci/base/FloatAccessor.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__SAR_SubCapabilityNparDetailsType_h)
# include "uci/type/SAR_SubCapabilityNparDetailsType.h"
#endif

#if !defined(Uci__Type__SAR_SubCapabilityClutterParametersType_h)
# include "uci/type/SAR_SubCapabilityClutterParametersType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_SubCapabilityType sequence accessor class */
      class SAR_SubCapabilityType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SAR_SubCapabilityType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_SubCapabilityType
           *
           * @return This accessor's type constant, i.e. SAR_SubCapabilityType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_SubCapabilityType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_SubCapabilityType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RadarPointingTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by RadarPointingTarget.
           */
         virtual const uci::type::RadarPointingTargetType& getRadarPointingTarget() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the RadarPointingTarget.
           *
           * @return The acecssor that provides access to the complex content that is identified by RadarPointingTarget.
           */
         virtual uci::type::RadarPointingTargetType& getRadarPointingTarget()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the RadarPointingTarget to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by RadarPointingTarget
           */
         virtual void setRadarPointingTarget(const uci::type::RadarPointingTargetType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MultiLook.
           *
           * @return The value of the simple primitive data type identified by MultiLook.
           */
         virtual xs::Boolean getMultiLook() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MultiLook.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMultiLook(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MultiLook exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MultiLook is emabled or not
           */
         virtual bool hasMultiLook() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MultiLook
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableMultiLook(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MultiLook */
         virtual void clearMultiLook()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EllipticityPolicy.
           *
           * @return The value of the enumeration identified by EllipticityPolicy.
           */
         virtual const uci::type::SAR_EllipticityPolicyEnum& getEllipticityPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the EllipticityPolicy.
           *
           * @return The value of the enumeration identified by EllipticityPolicy.
           */
         virtual uci::type::SAR_EllipticityPolicyEnum& getEllipticityPolicy()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EllipticityPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEllipticityPolicy(const uci::type::SAR_EllipticityPolicyEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the EllipticityPolicy.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setEllipticityPolicy(uci::type::SAR_EllipticityPolicyEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield EllipticityPolicyis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasEllipticityPolicy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getEllipticityPolicy will return the optional field and not throw an exception
           * when invoked.
           *
           * @param type = uci::type::accessorType::sAR_EllipticityPolicyEnum This Accessor's accessor type
           */
         virtual void enableEllipticityPolicy(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_EllipticityPolicyEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearEllipticityPolicy()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the AllowableEllipticityMagnitude.
           *
           * @return The value of the simple primitive data type identified by AllowableEllipticityMagnitude.
           */
         virtual xs::Float getAllowableEllipticityMagnitude() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the AllowableEllipticityMagnitude.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setAllowableEllipticityMagnitude(xs::Float value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by AllowableEllipticityMagnitude exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by AllowableEllipticityMagnitude is emabled or not
           */
         virtual bool hasAllowableEllipticityMagnitude() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by AllowableEllipticityMagnitude
           *
           * @param type = uci::base::accessorType::floatAccessor This Accessor's accessor type
           */
         virtual void enableAllowableEllipticityMagnitude(uci::base::accessorType::AccessorType type = uci::base::accessorType::floatAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by AllowableEllipticityMagnitude */
         virtual void clearAllowableEllipticityMagnitude()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MaximumBeamRolloffDB.
           *
           * @return The value of the simple primitive data type identified by MaximumBeamRolloffDB.
           */
         virtual uci::type::DecibelTypeValue getMaximumBeamRolloffDB() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MaximumBeamRolloffDB.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMaximumBeamRolloffDB(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MaximumBeamRolloffDB exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MaximumBeamRolloffDB is emabled or not
           */
         virtual bool hasMaximumBeamRolloffDB() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MaximumBeamRolloffDB
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableMaximumBeamRolloffDB(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MaximumBeamRolloffDB */
         virtual void clearMaximumBeamRolloffDB()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NPAR_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by NPAR_Details.
           */
         virtual const uci::type::SAR_SubCapabilityNparDetailsType& getNPAR_Details() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the NPAR_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by NPAR_Details.
           */
         virtual uci::type::SAR_SubCapabilityNparDetailsType& getNPAR_Details()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the NPAR_Details to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by NPAR_Details
           */
         virtual void setNPAR_Details(const uci::type::SAR_SubCapabilityNparDetailsType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by NPAR_Details exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by NPAR_Details is emabled or not
           */
         virtual bool hasNPAR_Details() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by NPAR_Details
           *
           * @param type = uci::type::accessorType::sAR_SubCapabilityNparDetailsType This Accessor's accessor type
           */
         virtual void enableNPAR_Details(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_SubCapabilityNparDetailsType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by NPAR_Details */
         virtual void clearNPAR_Details()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClutterParameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClutterParameters.
           */
         virtual const uci::type::SAR_SubCapabilityClutterParametersType& getClutterParameters() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ClutterParameters.
           *
           * @return The acecssor that provides access to the complex content that is identified by ClutterParameters.
           */
         virtual uci::type::SAR_SubCapabilityClutterParametersType& getClutterParameters()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ClutterParameters to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ClutterParameters
           */
         virtual void setClutterParameters(const uci::type::SAR_SubCapabilityClutterParametersType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ClutterParameters exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ClutterParameters is emabled or not
           */
         virtual bool hasClutterParameters() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ClutterParameters
           *
           * @param type = uci::type::accessorType::sAR_SubCapabilityClutterParametersType This Accessor's accessor type
           */
         virtual void enableClutterParameters(uci::base::accessorType::AccessorType type = uci::type::accessorType::sAR_SubCapabilityClutterParametersType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ClutterParameters */
         virtual void clearClutterParameters()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_SubCapabilityType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_SubCapabilityType to copy from
           */
         SAR_SubCapabilityType(const SAR_SubCapabilityType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_SubCapabilityType to the contents of the SAR_SubCapabilityType
           * on the right hand side (rhs) of the assignment operator.SAR_SubCapabilityType [only available to derived classes]
           *
           * @param rhs The SAR_SubCapabilityType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::SAR_SubCapabilityType
           * @return A constant reference to this SAR_SubCapabilityType.
           */
         const SAR_SubCapabilityType& operator=(const SAR_SubCapabilityType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_SubCapabilityType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_SubCapabilityType_h

