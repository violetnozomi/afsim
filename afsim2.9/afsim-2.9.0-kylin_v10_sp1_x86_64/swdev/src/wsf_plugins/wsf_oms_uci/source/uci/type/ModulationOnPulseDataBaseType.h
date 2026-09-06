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
#ifndef Uci__Type__ModulationOnPulseDataBaseType_h
#define Uci__Type__ModulationOnPulseDataBaseType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ModulationEnum_h)
# include "uci/type/ModulationEnum.h"
#endif

#if !defined(Uci__Type__FMOP_Enum_h)
# include "uci/type/FMOP_Enum.h"
#endif

#if !defined(Uci__Type__PMOP_Enum_h)
# include "uci/type/PMOP_Enum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ModulationOnPulseDataBaseType sequence accessor class */
      class ModulationOnPulseDataBaseType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ModulationOnPulseDataBaseType()
         { }

         /** Returns this accessor's type constant, i.e. ModulationOnPulseDataBaseType
           *
           * @return This accessor's type constant, i.e. ModulationOnPulseDataBaseType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::modulationOnPulseDataBaseType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ModulationOnPulseDataBaseType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MOP.
           *
           * @return The value of the enumeration identified by MOP.
           */
         virtual const uci::type::ModulationEnum& getMOP() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the MOP.
           *
           * @return The value of the enumeration identified by MOP.
           */
         virtual uci::type::ModulationEnum& getMOP()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMOP(const uci::type::ModulationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the MOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setMOP(uci::type::ModulationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FMOP.
           *
           * @return The value of the enumeration identified by FMOP.
           */
         virtual const uci::type::FMOP_Enum& getFMOP() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the FMOP.
           *
           * @return The value of the enumeration identified by FMOP.
           */
         virtual uci::type::FMOP_Enum& getFMOP()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FMOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFMOP(const uci::type::FMOP_Enum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the FMOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setFMOP(uci::type::FMOP_Enum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield FMOPis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasFMOP() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getFMOP will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::fMOP_Enum This Accessor's accessor type
           */
         virtual void enableFMOP(uci::base::accessorType::AccessorType type = uci::type::accessorType::fMOP_Enum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearFMOP()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PMOP.
           *
           * @return The value of the enumeration identified by PMOP.
           */
         virtual const uci::type::PMOP_Enum& getPMOP() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the PMOP.
           *
           * @return The value of the enumeration identified by PMOP.
           */
         virtual uci::type::PMOP_Enum& getPMOP()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PMOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPMOP(const uci::type::PMOP_Enum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the PMOP.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setPMOP(uci::type::PMOP_Enum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield PMOPis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasPMOP() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getPMOP will return the optional field and not throw an exception when invoked.
           *
           * @param type = uci::type::accessorType::pMOP_Enum This Accessor's accessor type
           */
         virtual void enablePMOP(uci::base::accessorType::AccessorType type = uci::type::accessorType::pMOP_Enum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearPMOP()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ModulationOnPulseDataBaseType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ModulationOnPulseDataBaseType to copy from
           */
         ModulationOnPulseDataBaseType(const ModulationOnPulseDataBaseType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ModulationOnPulseDataBaseType to the contents of the
           * ModulationOnPulseDataBaseType on the right hand side (rhs) of the assignment operator.ModulationOnPulseDataBaseType
           * [only available to derived classes]
           *
           * @param rhs The ModulationOnPulseDataBaseType on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::type::ModulationOnPulseDataBaseType
           * @return A constant reference to this ModulationOnPulseDataBaseType.
           */
         const ModulationOnPulseDataBaseType& operator=(const ModulationOnPulseDataBaseType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ModulationOnPulseDataBaseType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ModulationOnPulseDataBaseType_h

