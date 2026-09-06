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
#ifndef Uci__Type__InternationalDesignatorType_h
#define Uci__Type__InternationalDesignatorType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__YearType_h)
# include "uci/type/YearType.h"
#endif

#if !defined(Uci__Type__LaunchNumberType_h)
# include "uci/type/LaunchNumberType.h"
#endif

#if !defined(Uci__Type__LaunchPieceType_h)
# include "uci/type/LaunchPieceType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the InternationalDesignatorType sequence accessor class */
      class InternationalDesignatorType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~InternationalDesignatorType()
         { }

         /** Returns this accessor's type constant, i.e. InternationalDesignatorType
           *
           * @return This accessor's type constant, i.e. InternationalDesignatorType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::internationalDesignatorType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const InternationalDesignatorType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Year.
           *
           * @return The value of the simple primitive data type identified by Year.
           */
         virtual uci::type::YearTypeValue getYear() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Year.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setYear(uci::type::YearTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the SimplePrimitive data type that is identified by the Number.
           *
           * @return The value of the simple primitive data type identified by Number.
           */
         virtual uci::type::LaunchNumberTypeValue getNumber() const
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the SimplePrimitive data type that is identified by the Number.
           *
           * @param value The value to set the SimplePrimitve data type to
           */
         virtual void setNumber(uci::type::LaunchNumberTypeValue value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Piece.
           *
           * @return The value of the string data type identified by Piece.
           */
         virtual const uci::type::LaunchPieceType& getPiece() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the Piece.
           *
           * @return The value of the string data type identified by Piece.
           */
         virtual uci::type::LaunchPieceType& getPiece()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Piece to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setPiece(const uci::type::LaunchPieceType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Piece to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPiece(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the Piece to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setPiece(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         InternationalDesignatorType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The InternationalDesignatorType to copy from
           */
         InternationalDesignatorType(const InternationalDesignatorType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this InternationalDesignatorType to the contents of the
           * InternationalDesignatorType on the right hand side (rhs) of the assignment operator.InternationalDesignatorType [only
           * available to derived classes]
           *
           * @param rhs The InternationalDesignatorType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::InternationalDesignatorType
           * @return A constant reference to this InternationalDesignatorType.
           */
         const InternationalDesignatorType& operator=(const InternationalDesignatorType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // InternationalDesignatorType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__InternationalDesignatorType_h

