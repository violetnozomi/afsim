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
#ifndef Uci__Type__EntityExternalType_h
#define Uci__Type__EntityExternalType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ForeignKeyType_h)
# include "uci/type/ForeignKeyType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the EntityExternalType sequence accessor class */
      class EntityExternalType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~EntityExternalType()
         { }

         /** Returns this accessor's type constant, i.e. EntityExternalType
           *
           * @return This accessor's type constant, i.e. EntityExternalType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::entityExternalType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const EntityExternalType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ForeignSpecificTrackType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ForeignSpecificTrackType.
           */
         virtual const uci::type::ForeignKeyType& getForeignSpecificTrackType() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ForeignSpecificTrackType.
           *
           * @return The acecssor that provides access to the complex content that is identified by ForeignSpecificTrackType.
           */
         virtual uci::type::ForeignKeyType& getForeignSpecificTrackType()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ForeignSpecificTrackType to the contents of the complex content
           * that is accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ForeignSpecificTrackType
           */
         virtual void setForeignSpecificTrackType(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ForeignSpecificTrackType exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ForeignSpecificTrackType is emabled or not
           */
         virtual bool hasForeignSpecificTrackType() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ForeignSpecificTrackType
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableForeignSpecificTrackType(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ForeignSpecificTrackType */
         virtual void clearForeignSpecificTrackType()
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ForeignTrackNumber.
           *
           * @return The acecssor that provides access to the complex content that is identified by ForeignTrackNumber.
           */
         virtual const uci::type::ForeignKeyType& getForeignTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the ForeignTrackNumber.
           *
           * @return The acecssor that provides access to the complex content that is identified by ForeignTrackNumber.
           */
         virtual uci::type::ForeignKeyType& getForeignTrackNumber()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the ForeignTrackNumber to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by ForeignTrackNumber
           */
         virtual void setForeignTrackNumber(const uci::type::ForeignKeyType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns whether the Element that is identified by ForeignTrackNumber exists (is enabled) or not.
           *
           * @return A boolean indicating whether the Element identified by ForeignTrackNumber is emabled or not
           */
         virtual bool hasForeignTrackNumber() const
            throw(uci::base::UCIException) = 0;


         /** Enables the Element that is identified by ForeignTrackNumber
           *
           * @param type = uci::type::accessorType::foreignKeyType This Accessor's accessor type
           */
         virtual void enableForeignTrackNumber(uci::base::accessorType::AccessorType type = uci::type::accessorType::foreignKeyType)
            throw(uci::base::UCIException) = 0;


         /** Clears (disabled) the Element that is identified by ForeignTrackNumber */
         virtual void clearForeignTrackNumber()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         EntityExternalType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The EntityExternalType to copy from
           */
         EntityExternalType(const EntityExternalType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this EntityExternalType to the contents of the EntityExternalType on
           * the right hand side (rhs) of the assignment operator.EntityExternalType [only available to derived classes]
           *
           * @param rhs The EntityExternalType on the right hand side (rhs) of the assignment operator whose contents are used to
           *      set the contents of this uci::type::EntityExternalType
           * @return A constant reference to this EntityExternalType.
           */
         const EntityExternalType& operator=(const EntityExternalType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // EntityExternalType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__EntityExternalType_h

