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
#ifndef Uci__Type__DMPI_PatternType_h
#define Uci__Type__DMPI_PatternType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__DMPI_ID_Type_h)
# include "uci/type/DMPI_ID_Type.h"
#endif

#if !defined(Uci__Type__Point3D_Type_h)
# include "uci/type/Point3D_Type.h"
#endif

#if !defined(Uci__Type__AngleType_h)
# include "uci/type/AngleType.h"
#endif

#if !defined(Uci__Base__BooleanAccessor_h)
# include "uci/base/BooleanAccessor.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the DMPI_PatternType sequence accessor class */
      class DMPI_PatternType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~DMPI_PatternType()
         { }

         /** Returns this accessor's type constant, i.e. DMPI_PatternType
           *
           * @return This accessor's type constant, i.e. DMPI_PatternType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::dMPI_PatternType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const DMPI_PatternType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Indicates a list of unique IDs of DMPIs. [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::DMPI_ID_Type, uci::type::accessorType::dMPI_ID_Type> DMPI_ID;

         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual const uci::type::DMPI_PatternType::DMPI_ID& getDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the DMPI_ID.
           *
           * @return The bounded list identified by DMPI_ID.
           */
         virtual uci::type::DMPI_PatternType::DMPI_ID& getDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the DMPI_ID.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setDMPI_ID(const uci::type::DMPI_PatternType::DMPI_ID& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PatternCenterLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PatternCenterLocation.
           */
         virtual const uci::type::Point3D_Type& getPatternCenterLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the PatternCenterLocation.
           *
           * @return The acecssor that provides access to the complex content that is identified by PatternCenterLocation.
           */
         virtual uci::type::Point3D_Type& getPatternCenterLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the PatternCenterLocation to the contents of the complex content that
           * is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by PatternCenterLocation
           */
         virtual void setPatternCenterLocation(const uci::type::Point3D_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the PatternOrientation.
           *
           * @return The value of the simple primitive data type identified by PatternOrientation.
           */
         virtual uci::type::AngleTypeValue getPatternOrientation() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the PatternOrientation.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setPatternOrientation(uci::type::AngleTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the IncludeDMPI_AtCenter.
           *
           * @return The value of the simple primitive data type identified by IncludeDMPI_AtCenter.
           */
         virtual xs::Boolean getIncludeDMPI_AtCenter() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the IncludeDMPI_AtCenter.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setIncludeDMPI_AtCenter(xs::Boolean value)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CenterDMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterDMPI_ID.
           */
         virtual const uci::type::DMPI_ID_Type& getCenterDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the CenterDMPI_ID.
           *
           * @return The acecssor that provides access to the complex content that is identified by CenterDMPI_ID.
           */
         virtual uci::type::DMPI_ID_Type& getCenterDMPI_ID()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the CenterDMPI_ID to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by CenterDMPI_ID
           */
         virtual void setCenterDMPI_ID(const uci::type::DMPI_ID_Type& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by CenterDMPI_ID exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by CenterDMPI_ID is emabled or not
           */
         virtual bool hasCenterDMPI_ID() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by CenterDMPI_ID
           *
           * @param type = uci::type::accessorType::dMPI_ID_Type This Accessor's accessor type
           */
         virtual void enableCenterDMPI_ID(uci::base::accessorType::AccessorType type = uci::type::accessorType::dMPI_ID_Type)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by CenterDMPI_ID */
         virtual void clearCenterDMPI_ID()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         DMPI_PatternType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The DMPI_PatternType to copy from
           */
         DMPI_PatternType(const DMPI_PatternType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this DMPI_PatternType to the contents of the DMPI_PatternType on the
           * right hand side (rhs) of the assignment operator.DMPI_PatternType [only available to derived classes]
           *
           * @param rhs The DMPI_PatternType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::DMPI_PatternType
           * @return A constant reference to this DMPI_PatternType.
           */
         const DMPI_PatternType& operator=(const DMPI_PatternType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // DMPI_PatternType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__DMPI_PatternType_h

