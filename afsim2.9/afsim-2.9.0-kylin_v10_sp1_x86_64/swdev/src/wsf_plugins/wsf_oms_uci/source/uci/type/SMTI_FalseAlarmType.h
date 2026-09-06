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
#ifndef Uci__Type__SMTI_FalseAlarmType_h
#define Uci__Type__SMTI_FalseAlarmType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DecibelType_h)
# include "uci/type/DecibelType.h"
#endif

#if !defined(Uci__Type__CapabilityFalseAlarmLevelEnum_h)
# include "uci/type/CapabilityFalseAlarmLevelEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SMTI_FalseAlarmType sequence accessor class */
      class SMTI_FalseAlarmType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~SMTI_FalseAlarmType()
         { }

         /** Returns this accessor's type constant, i.e. SMTI_FalseAlarmType
           *
           * @return This accessor's type constant, i.e. SMTI_FalseAlarmType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sMTI_FalseAlarmType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SMTI_FalseAlarmType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CFAR_Threshold.
           *
           * @return The value of the simple primitive data type identified by CFAR_Threshold.
           */
         virtual uci::type::DecibelTypeValue getCFAR_Threshold() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CFAR_Threshold.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCFAR_Threshold(uci::type::DecibelTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CFAR_Threshold exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CFAR_Threshold is emabled or not
           */
         virtual bool hasCFAR_Threshold() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CFAR_Threshold
           *
           * @param type = uci::type::accessorType::decibelType This Accessor's accessor type
           */
         virtual void enableCFAR_Threshold(uci::base::accessorType::AccessorType type = uci::type::accessorType::decibelType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CFAR_Threshold */
         virtual void clearCFAR_Threshold()
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
         SMTI_FalseAlarmType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SMTI_FalseAlarmType to copy from
           */
         SMTI_FalseAlarmType(const SMTI_FalseAlarmType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SMTI_FalseAlarmType to the contents of the SMTI_FalseAlarmType on
           * the right hand side (rhs) of the assignment operator.SMTI_FalseAlarmType [only available to derived classes]
           *
           * @param rhs The SMTI_FalseAlarmType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::SMTI_FalseAlarmType
           * @return A constant reference to this SMTI_FalseAlarmType.
           */
         const SMTI_FalseAlarmType& operator=(const SMTI_FalseAlarmType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SMTI_FalseAlarmType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SMTI_FalseAlarmType_h

