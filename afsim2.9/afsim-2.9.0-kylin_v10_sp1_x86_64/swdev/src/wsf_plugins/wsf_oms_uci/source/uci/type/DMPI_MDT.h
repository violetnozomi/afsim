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
#ifndef Uci__Type__DMPI_MDT_h
#define Uci__Type__DMPI_MDT_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__DMPI_LocationType_h)
# include "uci/type/DMPI_LocationType.h"
#endif

#if !defined(Uci__Type__DMPI_TraceabilityType_h)
# include "uci/type/DMPI_TraceabilityType.h"
#endif

#if !defined(Uci__Type__VisibleString1024Type_h)
# include "uci/type/VisibleString1024Type.h"
#endif

#if !defined(Uci__Type__WeaponeeringType_h)
# include "uci/type/WeaponeeringType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_MDT sequence accessor class */
      class DMPI_MDT : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_MDT()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_MDT
           *
           * @return This accessor's type constant, i.e. DMPI_MDT
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_MDT;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_MDT& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_ID to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_ID
           */
         virtual void setDMPI_ID(const uci::type::DMPI_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_Location.
           */
         virtual const uci::type::DMPI_LocationType& getDMPI_Location() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the DMPI_Location.
           *
           * @return The acecssor that provides access to the complex content that is identified by DMPI_Location.
           */
         virtual uci::type::DMPI_LocationType& getDMPI_Location()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the DMPI_Location to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by DMPI_Location
           */
         virtual void setDMPI_Location(const uci::type::DMPI_LocationType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual const uci::type::DMPI_TraceabilityType& getTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Traceability.
           *
           * @return The acecssor that provides access to the complex content that is identified by Traceability.
           */
         virtual uci::type::DMPI_TraceabilityType& getTraceability()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Traceability to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Traceability
           */
         virtual void setTraceability(const uci::type::DMPI_TraceabilityType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Traceability exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Traceability is emabled or not
           */
         virtual bool hasTraceability() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Traceability
           *
           * @param type = uci::type::accessorType::dMPI_TraceabilityType This Accessor's accessor type
           */
         virtual void enableTraceability(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_TraceabilityType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Traceability */
         virtual void clearTraceability()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual const uci::type::VisibleString1024Type& getRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Remarks.
           *
           * @return The value of the string data type identified by Remarks.
           */
         virtual uci::type::VisibleString1024Type& getRemarks()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setRemarks(const uci::type::VisibleString1024Type& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Remarks to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setRemarks(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Remarks exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Remarks is emabled or not
           */
         virtual bool hasRemarks() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Remarks
           *
           * @param type = uci::type::accessorType::visibleString1024Type This Accessor's accessor type
           */
         virtual void enableRemarks(uci::base::accessorType::AccessorType type = uci::type::accessorType::visibleString1024Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Remarks */
         virtual void clearRemarks()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weaponeering.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weaponeering.
           */
         virtual const uci::type::WeaponeeringType& getWeaponeering() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Weaponeering.
           *
           * @return The acecssor that provides access to the complex content that is identified by Weaponeering.
           */
         virtual uci::type::WeaponeeringType& getWeaponeering()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Weaponeering to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Weaponeering
           */
         virtual void setWeaponeering(const uci::type::WeaponeeringType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by Weaponeering exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by Weaponeering is emabled or not
           */
         virtual bool hasWeaponeering() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by Weaponeering
           *
           * @param type = uci::type::accessorType::weaponeeringType This Accessor's accessor type
           */
         virtual void enableWeaponeering(uci::base::accessorType::AccessorType type = uci::type::accessorType::weaponeeringType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by Weaponeering */
         virtual void clearWeaponeering()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_MDT()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_MDT to copy from
           */
         DMPI_MDT(const DMPI_MDT& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_MDT to the contents of the DMPI_MDT on the right hand side
           * (rhs) of the assignment operator.DMPI_MDT [only available to derived classes]
           *
           * @param rhs The DMPI_MDT on the right hand side (rhs) of the assignment operator whose contents are used to set the
           *      contents of this uci::type::DMPI_MDT
           * @return A constant reference to this DMPI_MDT.
           */
         const DMPI_MDT& operator=(const DMPI_MDT& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_MDT


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_MDT_h

