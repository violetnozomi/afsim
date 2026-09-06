// This file was generated  on 12/5/2018 at 1:1:44 PM by the Boeing OMS CAL generation tools
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
#ifndef Uci__Base__UCIException_h
#define Uci__Base__UCIException_h 1

#if !defined(SysLibStdint_h)
#  include <stdint.h>
//  @cond
#  define SysLibStdint_h 1
//  @endcond
#endif

#if !defined(StdLibString)
#  include <string>
//  @cond
#  define StdLibString 1
//  @endcond
#endif

#if !defined(StdLibSstream)
#  include <sstream>
//  @cond
#  define StdLibSstream 1
//  @endcond
#endif

#if !defined(StdLibStdexcept)
#  include <stdexcept>
//  @cond
#  define StdLibStdexcept 1
//  @endcond
#endif


//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The UCIException is the base class of all exceptions that are thrown by the CAL methods */
      class UCIException : public virtual std::runtime_error {
      public:

         //  A numerical value that identifies the cause of an exception.
         typedef uint32_t ErrorCode;

         /** The constructor
           *
           * @param reason The reason why the exception was thrown
           * @param errorCode The error code associated with the exception
           */
         explicit UCIException(const std::string& reason, ErrorCode errorCode = 0) :
            std::runtime_error(reason),
            m_errorCode(errorCode)
         { }

         /** The constructor
           *
           * @param reason The reason why the exception was thrown
           * @param errorCode The error code associated with the exception
           */
         explicit UCIException(const char* reason, ErrorCode errorCode = 0) :
            std::runtime_error(reason),
            m_errorCode(errorCode)
         { }

         /** The constructor
           *
           * @param reason The reason why the exception was thrown
           * @param errorCode The error code associated with the exception
           */
         explicit UCIException(const std::ostringstream& reason, ErrorCode errorCode = 0) :
            std::runtime_error(reason.str()),
            m_errorCode(errorCode)
         { }

         /** The destructor */
         virtual ~UCIException() throw()
         { }

         /** Returns this exception's error code
           *
           * @return This exception's error code.
           */
         ErrorCode getErrorCode() const
         {
            return m_errorCode;
         }



      protected:

         //  The error code that identifies the cause of the exception (may not be available for all exceptions)
         long m_errorCode;


      private:

         /** The assignment operator. Sets the contents of this UCIException to the contents of the UCIException on the right hand
           * side (rhs) of the assignment operator.UCIException [only available to derived classes]
           *
           * @param rhs The UCIException on the right hand side (rhs) of the assignment operator whose contents are used to set
           *      the contents of this uci::base::UCIException
           * @return A constant reference to this UCIException.
           */
         const UCIException& operator=(const UCIException& rhs);


      }; // UCIException


   } // Namespace: base
} // Namespace: uci
#define throwUciException(message) \
   do { \
      std::ostringstream oStream; \
      oStream << message; \
      throw uci::base::UCIException(oStream); \
   } while (false)


#endif // Uci__Base__UCIException_h

