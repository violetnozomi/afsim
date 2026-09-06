// This file was generated  on 12/5/2018 at 1:1:48 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Type__ThumbnailType_h
#define Uci__Type__ThumbnailType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Type__ProductOutputType_h)
# include "uci/type/ProductOutputType.h"
#endif

#if !defined(Uci__Type__FileNameType_h)
# include "uci/type/FileNameType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the ThumbnailType sequence accessor class */
      class ThumbnailType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~ThumbnailType()
         { }

         /** Returns this accessor's type constant, i.e. ThumbnailType
           *
           * @return This accessor's type constant, i.e. ThumbnailType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::thumbnailType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const ThumbnailType& accessor)
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Description.
           *
           * @return The acecssor that provides access to the complex content that is identified by Description.
           */
         virtual const uci::type::ProductOutputType& getDescription() const
            throw(uci::base::UCIException) = 0;


         /** Returns the accessor that provides access to the complex content that is identified by the Description.
           *
           * @return The acecssor that provides access to the complex content that is identified by Description.
           */
         virtual uci::type::ProductOutputType& getDescription()
            throw(uci::base::UCIException) = 0;


         /** Sets the complex content that is identified by the Description to the contents of the complex content that is
           * accessed by the specified accessor.
           *
           * @param value The accessor that provides access to the sequence whose contents are to be used to set the contents of
           *      the sequence identified by Description
           */
         virtual void setDescription(const uci::type::ProductOutputType& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual const uci::type::FileNameType& getFileName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the value of the string data type that is identified by the FileName.
           *
           * @return The value of the string data type identified by FileName.
           */
         virtual uci::type::FileNameType& getFileName()
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the string accessed by the specified
           * String.
           *
           * @param value The String whose contents are used to set the string data type.
           */
         virtual void setFileName(const uci::type::FileNameType& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const std::string& value)
            throw(uci::base::UCIException) = 0;


         /** Sets the value of the string data type that is identified by the FileName to the specified string.
           *
           * @param value The string used to set the string data type.
           */
         virtual void setFileName(const char* value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         ThumbnailType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The ThumbnailType to copy from
           */
         ThumbnailType(const ThumbnailType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this ThumbnailType to the contents of the ThumbnailType on the right
           * hand side (rhs) of the assignment operator.ThumbnailType [only available to derived classes]
           *
           * @param rhs The ThumbnailType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::ThumbnailType
           * @return A constant reference to this ThumbnailType.
           */
         const ThumbnailType& operator=(const ThumbnailType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // ThumbnailType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__ThumbnailType_h

