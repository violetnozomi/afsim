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
#ifndef Uci__Type__PRI_PW_Type_h
#define Uci__Type__PRI_PW_Type_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__PRI_DataType_h)
# include "uci/type/PRI_DataType.h"
#endif

#if !defined(Uci__Type__SignalPulseWidthType_h)
# include "uci/type/SignalPulseWidthType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the PRI_PW_Type sequence accessor class */
      class PRI_PW_Type : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~PRI_PW_Type()
         { }

         /** Returns this accessor's type constant, i.e. PRI_PW_Type
           *
           * @return This accessor's type constant, i.e. PRI_PW_Type
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::pRI_PW_Type;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const PRI_PW_Type& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PRI_Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by PRI_Data.
           */
         virtual const uci::type::PRI_DataType& getPRI_Data() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PRI_Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by PRI_Data.
           */
         virtual uci::type::PRI_DataType& getPRI_Data()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PRI_Data to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PRI_Data
           */
         virtual void setPRI_Data(const uci::type::PRI_DataType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PRI_Data exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PRI_Data is emabled or not
           */
         virtual bool hasPRI_Data() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PRI_Data
           *
           * @param type = uci::type::accessorType::pRI_DataType This Accessor's accessor type
           */
         virtual void enablePRI_Data(uci::base::accessorType::AccessorType type = uci::type::accessorType::pRI_DataType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PRI_Data */
         virtual void clearPRI_Data()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseWidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseWidth.
           */
         virtual const uci::type::SignalPulseWidthType& getPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PulseWidth.
           *
           * @return The acecssor that provides access to the complex content that is identified by PulseWidth.
           */
         virtual uci::type::SignalPulseWidthType& getPulseWidth()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PulseWidth to the contents of the complex content that is accessed
           * by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PulseWidth
           */
         virtual void setPulseWidth(const uci::type::SignalPulseWidthType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by PulseWidth exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by PulseWidth is emabled or not
           */
         virtual bool hasPulseWidth() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by PulseWidth
           *
           * @param type = uci::type::accessorType::signalPulseWidthType This Accessor's accessor type
           */
         virtual void enablePulseWidth(uci::base::accessorType::AccessorType type = uci::type::accessorType::signalPulseWidthType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by PulseWidth */
         virtual void clearPulseWidth()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         PRI_PW_Type()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The PRI_PW_Type to copy from
           */
         PRI_PW_Type(const PRI_PW_Type& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this PRI_PW_Type to the contents of the PRI_PW_Type on the right hand
           * side (rhs) of the assignment operator.PRI_PW_Type [only available to derived classes]
           *
           * @param rhs The PRI_PW_Type on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::PRI_PW_Type
           * @return A constant reference to this PRI_PW_Type.
           */
         const PRI_PW_Type& operator=(const PRI_PW_Type& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // PRI_PW_Type


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__PRI_PW_Type_h

