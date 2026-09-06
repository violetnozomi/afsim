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
#ifndef Uci__Type__SAR_CollectionOptionsType_h
#define Uci__Type__SAR_CollectionOptionsType_h 1

#if !defined(Uci__Type__RadarCollectionOptionsType_h)
# include "uci/type/RadarCollectionOptionsType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Base__DoubleAccessor_h)
# include "uci/base/DoubleAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the SAR_CollectionOptionsType sequence accessor class */
      class SAR_CollectionOptionsType : public virtual uci::type::RadarCollectionOptionsType {
      public:

         /** The destructor */
         virtual ~SAR_CollectionOptionsType()
         { }

         /** Returns this accessor's type constant, i.e. SAR_CollectionOptionsType
           *
           * @return This accessor's type constant, i.e. SAR_CollectionOptionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::sAR_CollectionOptionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const SAR_CollectionOptionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ATR.
           *
           * @return The value of the simple primitive data type identified by ATR.
           */
         virtual xs::Boolean getATR() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ATR.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setATR(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ATR exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ATR is emabled or not
           */
         virtual bool hasATR() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ATR
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableATR(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ATR */
         virtual void clearATR()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the FTI.
           *
           * @return The value of the simple primitive data type identified by FTI.
           */
         virtual xs::Boolean getFTI() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the FTI.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setFTI(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by FTI exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by FTI is emabled or not
           */
         virtual bool hasFTI() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by FTI
           *
           * @param type = uci::base::accessorType::booleanAccessor This Accessor's accessor type
           */
         virtual void enableFTI(uci::base::accessorType::AccessorType type = uci::base::accessorType::booleanAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by FTI */
         virtual void clearFTI()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the ATR_NumberOfTargets.
           *
           * @return The value of the simple primitive data type identified by ATR_NumberOfTargets.
           */
         virtual xs::UnsignedInt getATR_NumberOfTargets() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the ATR_NumberOfTargets.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setATR_NumberOfTargets(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ATR_NumberOfTargets exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ATR_NumberOfTargets is emabled or not
           */
         virtual bool hasATR_NumberOfTargets() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ATR_NumberOfTargets
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableATR_NumberOfTargets(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ATR_NumberOfTargets */
         virtual void clearATR_NumberOfTargets()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CommandedResolution.
           *
           * @return The value of the simple primitive data type identified by CommandedResolution.
           */
         virtual xs::Double getCommandedResolution() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CommandedResolution.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCommandedResolution(xs::Double value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CommandedResolution exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CommandedResolution is emabled or not
           */
         virtual bool hasCommandedResolution() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CommandedResolution
           *
           * @param type = uci::base::accessorType::doubleAccessor This Accessor's accessor type
           */
         virtual void enableCommandedResolution(uci::base::accessorType::AccessorType type = uci::base::accessorType::doubleAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CommandedResolution */
         virtual void clearCommandedResolution()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         SAR_CollectionOptionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The SAR_CollectionOptionsType to copy from
           */
         SAR_CollectionOptionsType(const SAR_CollectionOptionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this SAR_CollectionOptionsType to the contents of the
           * SAR_CollectionOptionsType on the right hand side (rhs) of the assignment operator.SAR_CollectionOptionsType [only
           * available to derived classes]
           *
           * @param rhs The SAR_CollectionOptionsType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::SAR_CollectionOptionsType
           * @return A constant reference to this SAR_CollectionOptionsType.
           */
         const SAR_CollectionOptionsType& operator=(const SAR_CollectionOptionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // SAR_CollectionOptionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__SAR_CollectionOptionsType_h

