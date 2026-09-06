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
#ifndef Uci__Type__Link16EntityMetadataType_h
#define Uci__Type__Link16EntityMetadataType_h 1

#if !defined(Uci__Type__EntityMetadataPET_h)
# include "uci/type/EntityMetadataPET.h"
#endif

#if !defined(Uci__Type__Link16MetadataType_h)
# include "uci/type/Link16MetadataType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is a polymorphic extension which allows for the extension of the base EntityMetadataPET. This specifies metadata
        * for Link16.
        */
      class Link16EntityMetadataType : public virtual uci::type::EntityMetadataPET {
      public:

         /** The destructor */
         virtual ~Link16EntityMetadataType()
         { }

         /** Returns this accessor's type constant, i.e. Link16EntityMetadataType
           *
           * @return This accessor's type constant, i.e. Link16EntityMetadataType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::link16EntityMetadataType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const Link16EntityMetadataType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by Data.
           */
         virtual const uci::type::Link16MetadataType& getData() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Data.
           *
           * @return The acecssor that provides access to the complex content that is identified by Data.
           */
         virtual uci::type::Link16MetadataType& getData()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Data to the contents of the complex content that is accessed by
           * the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Data
           */
         virtual void setData(const uci::type::Link16MetadataType& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         Link16EntityMetadataType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The Link16EntityMetadataType to copy from
           */
         Link16EntityMetadataType(const Link16EntityMetadataType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this Link16EntityMetadataType to the contents of the
           * Link16EntityMetadataType on the right hand side (rhs) of the assignment operator.Link16EntityMetadataType [only
           * available to derived classes]
           *
           * @param rhs The Link16EntityMetadataType on the right hand side (rhs) of the assignment operator whose contents are
           *      used to set the contents of this uci::type::Link16EntityMetadataType
           * @return A constant reference to this Link16EntityMetadataType.
           */
         const Link16EntityMetadataType& operator=(const Link16EntityMetadataType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // Link16EntityMetadataType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__Link16EntityMetadataType_h

