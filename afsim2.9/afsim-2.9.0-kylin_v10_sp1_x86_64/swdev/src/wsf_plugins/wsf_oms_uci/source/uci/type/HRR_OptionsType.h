// This file was generated  on 12/5/2018 at 1:1:46 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__HRR_OptionsType_h
#define Uci__Type__HRR_OptionsType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__HRR_ChipSizeType_h)
# include "uci/type/HRR_ChipSizeType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the HRR_OptionsType sequence accessor class */
      class HRR_OptionsType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~HRR_OptionsType()
         { }

         /** Returns this accessor's type constant, i.e. HRR_OptionsType
           *
           * @return This accessor's type constant, i.e. HRR_OptionsType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::hRR_OptionsType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const HRR_OptionsType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Maximum inclusive: 99] */
         typedef uci::base::UnsignedIntAccessor HRR_PeakScatterers;
         //  The type of primitive data accessed by HRR_PeakScatterers
         typedef xs::UnsignedInt HRR_PeakScatterersValue;

         /** Returns the value of the SimplePrimitive data type that is identified by the HRR_PeakScatterers.
           *
           * @return The value of the simple primitive data type identified by HRR_PeakScatterers.
           */
         virtual uci::type::HRR_OptionsType::HRR_PeakScatterersValue getHRR_PeakScatterers() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the HRR_PeakScatterers.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setHRR_PeakScatterers(uci::type::HRR_OptionsType::HRR_PeakScatterersValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by HRR_PeakScatterers exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by HRR_PeakScatterers is emabled or not
           */
         virtual bool hasHRR_PeakScatterers() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by HRR_PeakScatterers
           *
           * @param type = uci::type::hRR_OptionsType::accessorType::hRR_PeakScatterers This Accessor's accessor type
           */
         virtual void enableHRR_PeakScatterers(uci::base::accessorType::AccessorType type = uci::type::hRR_OptionsType::accessorType::hRR_PeakScatterers)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by HRR_PeakScatterers */
         virtual void clearHRR_PeakScatterers()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChipSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChipSize.
           */
         virtual const uci::type::HRR_ChipSizeType& getChipSize() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ChipSize.
           *
           * @return The acecssor that provides access to the complex content that is identified by ChipSize.
           */
         virtual uci::type::HRR_ChipSizeType& getChipSize()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ChipSize to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ChipSize
           */
         virtual void setChipSize(const uci::type::HRR_ChipSizeType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ChipSize exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ChipSize is emabled or not
           */
         virtual bool hasChipSize() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ChipSize
           *
           * @param type = uci::type::accessorType::hRR_ChipSizeType This Accessor's accessor type
           */
         virtual void enableChipSize(uci::base::accessorType::AccessorType type = uci::type::accessorType::hRR_ChipSizeType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ChipSize */
         virtual void clearChipSize()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         HRR_OptionsType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The HRR_OptionsType to copy from
           */
         HRR_OptionsType(const HRR_OptionsType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this HRR_OptionsType to the contents of the HRR_OptionsType on the
           * right hand side (rhs) of the assignment operator.HRR_OptionsType [only available to derived classes]
           *
           * @param rhs The HRR_OptionsType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::HRR_OptionsType
           * @return A constant reference to this HRR_OptionsType.
           */
         const HRR_OptionsType& operator=(const HRR_OptionsType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // HRR_OptionsType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__HRR_OptionsType_h

