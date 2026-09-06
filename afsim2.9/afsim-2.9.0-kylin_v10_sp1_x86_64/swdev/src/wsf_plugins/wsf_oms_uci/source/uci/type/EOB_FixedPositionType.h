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
#ifndef Uci__Type__EOB_FixedPositionType_h
#define Uci__Type__EOB_FixedPositionType_h 1

#if !defined(Uci__Type__FixedPositionType_h)
# include "uci/type/FixedPositionType.h"
#endif

#if !defined(Uci__Type__DistanceType_h)
# include "uci/type/DistanceType.h"
#endif

#if !defined(Xs__Type__StringXmlSchemaPrimitives_h)
# include "xs/type/stringXmlSchemaPrimitives.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EOB_FixedPositionType sequence accessor class */
      class EOB_FixedPositionType : public virtual uci::type::FixedPositionType {
      public:

         /** The destructor */
         virtual ~EOB_FixedPositionType()
         { }

         /** Returns this accessor's type constant, i.e. EOB_FixedPositionType
           *
           * @return This accessor's type constant, i.e. EOB_FixedPositionType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::eOB_FixedPositionType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EOB_FixedPositionType& accessor)
            throw(uci::base::UCIException) = 0;


         /** [Minimum length: 14] [Maximum length: 15] [Pattern: "\d{1,2}[^\d\sABIOYZabioyz][A-Za-z]{2}([0-9][0-9]){5}"] */
         typedef xs::String MilitaryGrid;

         /** Returns the value of the SimplePrimitive data type that is identified by the RadiusOfAccuracy.
           *
           * @return The value of the simple primitive data type identified by RadiusOfAccuracy.
           */
         virtual uci::type::DistanceTypeValue getRadiusOfAccuracy() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the RadiusOfAccuracy.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setRadiusOfAccuracy(uci::type::DistanceTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by RadiusOfAccuracy exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by RadiusOfAccuracy is emabled or not
           */
         virtual bool hasRadiusOfAccuracy() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by RadiusOfAccuracy
           *
           * @param type = uci::type::accessorType::distanceType This Accessor's accessor type
           */
         virtual void enableRadiusOfAccuracy(uci::base::accessorType::AccessorType type = uci::type::accessorType::distanceType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by RadiusOfAccuracy */
         virtual void clearRadiusOfAccuracy()
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MilitaryGrid.
           *
           * @return The value of the string data type identified by MilitaryGrid.
           */
         virtual const uci::type::EOB_FixedPositionType::MilitaryGrid& getMilitaryGrid() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the MilitaryGrid.
           *
           * @return The value of the string data type identified by MilitaryGrid.
           */
         virtual uci::type::EOB_FixedPositionType::MilitaryGrid& getMilitaryGrid()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MilitaryGrid to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setMilitaryGrid(const uci::type::EOB_FixedPositionType::MilitaryGrid& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MilitaryGrid to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMilitaryGrid(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the MilitaryGrid to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setMilitaryGrid(const char* value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by MilitaryGrid exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by MilitaryGrid is emabled or not
           */
         virtual bool hasMilitaryGrid() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by MilitaryGrid
           *
           * @param type = uci::type::eOB_FixedPositionType::accessorType::militaryGrid This Accessor's accessor type
           */
         virtual void enableMilitaryGrid(uci::base::accessorType::AccessorType type = uci::type::eOB_FixedPositionType::accessorType::militaryGrid)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by MilitaryGrid */
         virtual void clearMilitaryGrid()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EOB_FixedPositionType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EOB_FixedPositionType to copy from
           */
         EOB_FixedPositionType(const EOB_FixedPositionType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EOB_FixedPositionType to the contents of the EOB_FixedPositionType
           * on the right hand side (rhs) of the assignment operator.EOB_FixedPositionType [only available to derived classes]
           *
           * @param rhs The EOB_FixedPositionType on the right hand side (rhs) of the assignment operator whose contents are used
           *      to set the contents of this uci::type::EOB_FixedPositionType
           * @return A constant reference to this EOB_FixedPositionType.
           */
         const EOB_FixedPositionType& operator=(const EOB_FixedPositionType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EOB_FixedPositionType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EOB_FixedPositionType_h

