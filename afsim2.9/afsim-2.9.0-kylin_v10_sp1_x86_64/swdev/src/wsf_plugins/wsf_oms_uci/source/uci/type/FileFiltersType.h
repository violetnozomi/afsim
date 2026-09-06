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
#ifndef Uci__Type__FileFiltersType_h
#define Uci__Type__FileFiltersType_h 1

#if !defined(Uci__Base__Accessor_h)
# include "uci/base/Accessor.h"
#endif

#if !defined(Uci__Base__BoundedList_h)
# include "uci/base/BoundedList.h"
#endif

#if !defined(Uci__Type__VisibleString256Type_h)
# include "uci/type/VisibleString256Type.h"
#endif

#if !defined(Uci__Type__MIME_Type_h)
# include "uci/type/MIME_Type.h"
#endif

#if !defined(Uci__Type__ProductLocationType_h)
# include "uci/type/ProductLocationType.h"
#endif

//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all generated data types are declared
   namespace type {

      /** This is the FileFiltersType sequence accessor class */
      class FileFiltersType : public virtual uci::base::Accessor {
      public:

         /** The destructor */
         virtual ~FileFiltersType()
         { }

         /** Returns this accessor's type constant, i.e. FileFiltersType
           *
           * @return This accessor's type constant, i.e. FileFiltersType
           */
         virtual uci::base::accessorType::AccessorType getAccessorType() const
            throw()
         {
            return uci::type::accessorType::fileFiltersType;
         }


         /** Initializes the contents of this accessor by copying the contents of the specified accessor
           *
           * @param accessor The accessor whose contents are to be used to initialize the contents of this accessor
           */
         virtual void copy(const FileFiltersType& accessor)
            throw(uci::base::UCIException) = 0;


         /** A filter on the FileName. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::VisibleString256Type, uci::type::accessorType::visibleString256Type> FileName;

         /** A filter on the file type/format. [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::MIME_Type, uci::type::accessorType::mIME_Type> FileFormat;

         /** A filter on the location of the file [Minimum occurrences: 0] [Maximum occurrences: 9223372036854775807] */
         typedef uci::base::BoundedList<uci::type::ProductLocationType, uci::type::accessorType::productLocationType> Location;

         /** Returns the bounded list that is identified by the FileName.
           *
           * @return The bounded list identified by FileName.
           */
         virtual const uci::type::FileFiltersType::FileName& getFileName() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileName.
           *
           * @return The bounded list identified by FileName.
           */
         virtual uci::type::FileFiltersType::FileName& getFileName()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileName.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileName(const uci::type::FileFiltersType::FileName& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileFormat.
           *
           * @return The bounded list identified by FileFormat.
           */
         virtual const uci::type::FileFiltersType::FileFormat& getFileFormat() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the FileFormat.
           *
           * @return The bounded list identified by FileFormat.
           */
         virtual uci::type::FileFiltersType::FileFormat& getFileFormat()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the FileFormat.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setFileFormat(const uci::type::FileFiltersType::FileFormat& value)
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Location.
           *
           * @return The bounded list identified by Location.
           */
         virtual const uci::type::FileFiltersType::Location& getLocation() const
            throw(uci::base::UCIException) = 0;


         /** Returns the bounded list that is identified by the Location.
           *
           * @return The bounded list identified by Location.
           */
         virtual uci::type::FileFiltersType::Location& getLocation()
            throw(uci::base::UCIException) = 0;


         /** Sets the bounded list that is identified by the Location.
           *
           * @param value The bounded list whose contents are to be used to set the value of the bounded list accessed by this
           *      accessor
           */
         virtual void setLocation(const uci::type::FileFiltersType::Location& value)
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         FileFiltersType()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The FileFiltersType to copy from
           */
         FileFiltersType(const FileFiltersType& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this FileFiltersType to the contents of the FileFiltersType on the
           * right hand side (rhs) of the assignment operator.FileFiltersType [only available to derived classes]
           *
           * @param rhs The FileFiltersType on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::type::FileFiltersType
           * @return A constant reference to this FileFiltersType.
           */
         const FileFiltersType& operator=(const FileFiltersType& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // FileFiltersType


   } // Namespace: type
} // Namespace: uci

#endif // Uci__Type__FileFiltersType_h

