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
#ifndef Uci__Type__LAR_MDT_h
#define Uci__Type__LAR_MDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__LAR_ID_Type_h)
# include "uci/type/LAR_ID_Type.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__CapabilityID_Type_h)
# include "uci/type/CapabilityID_Type.h"
#endif

#if !defined(Uci__Type__LAR_RelationEnum_h)
# include "uci/type/LAR_RelationEnum.h"
#endif

#if !defined(Uci__Type__LAR_BasisEnum_h)
# include "uci/type/LAR_BasisEnum.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

#if !defined(Uci__Type__ZoneType_h)
# include "uci/type/ZoneType.h"
#endif

#if !defined(Uci__Type__LAR_DetailsType_h)
# include "uci/type/LAR_DetailsType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the LAR_MDT sequence accessor class */
      class LAR_MDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~LAR_MDT()
         { }

         /** Returns this accessor's type constant, i.e. LAR_MDT
           *
           * @return This accessor's type constant, i.e. LAR_MDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::lAR_MDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const LAR_MDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates the weapon Capability instance that this LAR is associated with. Multiple instances of this element should
           * only be given when each weapon Capability is of the same homogeneous weapon type and multiple weapon instances are
           * being employed on a single DMPI. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807]
           */
         typedef uci::base::BoundedList<uci::type::CapabilityID_Type, uci::type::accessorType::capabilityID_Type> CapabilityID;

         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual const uci::type::LAR_ID_Type& getLAR_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_ID.
           */
         virtual uci::type::LAR_ID_Type& getLAR_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_ID
           */
         virtual void setLAR_ID(const uci::type::LAR_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual const uci::type::LAR_MDT::CapabilityID& getCapabilityID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the CapabilityID.
           *
           * @return The bounded list identified by CapabilityID.
           */
         virtual uci::type::LAR_MDT::CapabilityID& getCapabilityID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the CapabilityID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setCapabilityID(const uci::type::LAR_MDT::CapabilityID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LAR_Type.
           *
           * @return The value of the enumeration identified by LAR_Type.
           */
         virtual const uci::type::LAR_RelationEnum& getLAR_Type() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the LAR_Type.
           *
           * @return The value of the enumeration identified by LAR_Type.
           */
         virtual uci::type::LAR_RelationEnum& getLAR_Type()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LAR_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLAR_Type(const uci::type::LAR_RelationEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the LAR_Type.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setLAR_Type(uci::type::LAR_RelationEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Basis.
           *
           * @return The value of the enumeration identified by Basis.
           */
         virtual const uci::type::LAR_BasisEnum& getBasis() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the enumeration that is identified by the Basis.
           *
           * @return The value of the enumeration identified by Basis.
           */
         virtual uci::type::LAR_BasisEnum& getBasis()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Basis.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBasis(const uci::type::LAR_BasisEnum& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the enumeration that is identified by the Basis.
           *
           * @param value The value to set the enumeration to
           */
         virtual void setBasis(uci::type::LAR_BasisEnum::EnumerationItem value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the GeneratedByWeapon.
           *
           * @return The value of the simple primitive data type identified by GeneratedByWeapon.
           */
         virtual xs::Boolean getGeneratedByWeapon() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the GeneratedByWeapon.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setGeneratedByWeapon(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Region.
           *
           * @return The acecssor that provides access to the complex content that is identified by Region.
           */
         virtual const uci::type::ZoneType& getRegion() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Region.
           *
           * @return The acecssor that provides access to the complex content that is identified by Region.
           */
         virtual uci::type::ZoneType& getRegion()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Region to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Region
           */
         virtual void setRegion(const uci::type::ZoneType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_Details.
           */
         virtual const uci::type::LAR_DetailsType& getLAR_Details() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the LAR_Details.
           *
           * @return The acecssor that provides access to the complex content that is identified by LAR_Details.
           */
         virtual uci::type::LAR_DetailsType& getLAR_Details()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the LAR_Details to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by LAR_Details
           */
         virtual void setLAR_Details(const uci::type::LAR_DetailsType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         LAR_MDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The LAR_MDT to copy from
           */
         LAR_MDT(const LAR_MDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this LAR_MDT to the contents of the LAR_MDT on the right hand side
           * (rhs) of the assignment operator.LAR_MDT [only available to derived classes]
           *
           * @param rhs The LAR_MDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::LAR_MDT
           * @return A constant reference to this LAR_MDT.
           */
         const LAR_MDT& operator=(const LAR_MDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // LAR_MDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__LAR_MDT_h

