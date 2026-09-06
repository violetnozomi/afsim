// This file was generated  on 12/5/2018 at 1:1:45 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__DamageSummaryType_h
#define Uci__Type__DamageSummaryType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__UnsignedIntAccessor_h)
# include "uci/base/UnsignedIntAccessor.h"
#endif

#if !defined(Uci__Type__DamageSignificanceEnum_h)
# include "uci/type/DamageSignificanceEnum.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DamageSummaryType sequence accessor class */
      class DamageSummaryType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DamageSummaryType()
         { }

         /** Returns this accessor's type constant, i.e. DamageSummaryType
           *
           * @return This accessor's type constant, i.e. DamageSummaryType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::damageSummaryType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DamageSummaryType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CivilianCasualties.
           *
           * @return The value of the simple primitive data type identified by CivilianCasualties.
           */
         virtual xs::UnsignedInt getCivilianCasualties() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CivilianCasualties.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCivilianCasualties(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CivilianCasualties exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CivilianCasualties is emabled or not
           */
         virtual bool hasCivilianCasualties() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CivilianCasualties
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableCivilianCasualties(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CivilianCasualties */
         virtual void clearCivilianCasualties()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the CivilianInjuries.
           *
           * @return The value of the simple primitive data type identified by CivilianInjuries.
           */
         virtual xs::UnsignedInt getCivilianInjuries() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the CivilianInjuries.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setCivilianInjuries(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CivilianInjuries exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CivilianInjuries is emabled or not
           */
         virtual bool hasCivilianInjuries() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CivilianInjuries
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableCivilianInjuries(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CivilianInjuries */
         virtual void clearCivilianInjuries()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MilitaryCasualties.
           *
           * @return The value of the simple primitive data type identified by MilitaryCasualties.
           */
         virtual xs::UnsignedInt getMilitaryCasualties() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MilitaryCasualties.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMilitaryCasualties(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MilitaryCasualties exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MilitaryCasualties is emabled or not
           */
         virtual bool hasMilitaryCasualties() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MilitaryCasualties
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableMilitaryCasualties(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MilitaryCasualties */
         virtual void clearMilitaryCasualties()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the MilitaryInjuries.
           *
           * @return The value of the simple primitive data type identified by MilitaryInjuries.
           */
         virtual xs::UnsignedInt getMilitaryInjuries() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the MilitaryInjuries.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setMilitaryInjuries(xs::UnsignedInt value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MilitaryInjuries exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MilitaryInjuries is emabled or not
           */
         virtual bool hasMilitaryInjuries() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MilitaryInjuries
           *
           * @param type = uci::base::accessorType::unsignedIntAccessor This Accessor's accessor type
           */
         virtual void enableMilitaryInjuries(uci::base::accessorType::AccessorType type = uci::base::accessorType::unsignedIntAccessor)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MilitaryInjuries */
         virtual void clearMilitaryInjuries()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Significance.
           *
           * @return The value of the enumeration identified by Significance.
           */
         virtual const uci::type::DamageSignificanceEnum& getSignificance() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Significance.
           *
           * @return The value of the enumeration identified by Significance.
           */
         virtual uci::type::DamageSignificanceEnum& getSignificance()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Significance.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSignificance(const uci::type::DamageSignificanceEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Significance.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setSignificance(uci::type::DamageSignificanceEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether this optionalfield Significanceis available
           *
           * @return Whether the optional field is available (true) or not (false).
           */
         virtual bool hasSignificance() const
            throw(uci::base::UCIException) = 0;


         /** Enables the optional field such that getSignificance will return the optional field and not throw an exception when
           * invoked.
           *
           * @param type = uci::type::accessorType::damageSignificanceEnum This Accessor's accessor type
           */
         virtual void enableSignificance(uci::base::accessorType::AccessorType type = uci::type::accessorType::damageSignificanceEnum)
            throw(uci::base::UCIException) = 0;


         /** Clears the optional item marking it as not being set. */
         virtual void clearSignificance()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DamageSummaryType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DamageSummaryType to copy from
           */
         DamageSummaryType(const DamageSummaryType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DamageSummaryType to the contents of the DamageSummaryType on the
           * right hand side (rhs) of the assignment operator.DamageSummaryType [only available to derived classes]
           *
           * @param rhs The DamageSummaryType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DamageSummaryType
           * @return A constant reference to this DamageSummaryType.
           */
         const DamageSummaryType& operator=(const DamageSummaryType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DamageSummaryType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DamageSummaryType_h

