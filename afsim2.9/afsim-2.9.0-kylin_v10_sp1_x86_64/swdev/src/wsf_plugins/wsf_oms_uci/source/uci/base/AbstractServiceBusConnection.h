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
#ifndef Uci__Base__AbstractServiceBusConnection_h
#define Uci__Base__AbstractServiceBusConnection_h 1

#if !defined(SysLibStdint_h)
#  include <stdint.h>
//  @cond
#  define SysLibStdint_h 1
//  @endcond
#endif


#if !defined(StdLibMap)
#  include <map>
//  @cond
#  define StdLibMap 1
//  @endcond
#endif

#if !defined(StdLibString)
#  include <string>
//  @cond
#  define StdLibString 1
//  @endcond
#endif

#if !defined(StdLibStdexcept)
#  include <stdexcept>
//  @cond
#  define StdLibStdexcept 1
//  @endcond
#endif


#if !defined(Uci__Base__UCIException_h)
# include "uci/base/UCIException.h"
#endif

#if !defined(Uci__Base__Export_h)
# include "uci/base/export.h"
#endif

#if !defined(Uci__Base__UUID_h)
# include "uci/base/UUID.h"
#endif



//  The namespace in which all UAS C2 Initiative data types are declared
namespace uci {

   //  The namespace in which all base data types are declared
   namespace base {

      /** The AbstractServiceBusConnection provides the fundamental support for sending and receiving messages. */
      class AbstractServiceBusConnection {
      public:

         /** The destructor */
         virtual ~AbstractServiceBusConnection()
         { }

         /** The shutdown method is used to shutdown this abstract service bus connection. Once shutdown, this abstract service
           * bus connection can no longer be used.In addition, any factory associated with this abstract service bus connection
           * will no longer be usable.
           */
         virtual void shutdown()
            throw(uci::base::UCIException) = 0;


         /** This method will return the system label, i.e. the name of the system.
           *
           * @return The system's label
           */
         virtual const std::string getMySystemLabel() const
            throw(uci::base::UCIException) = 0;


         /** This method will generate a new UUID
           *
           * @return The newly generated UUID
           */
         virtual const uci::base::UUID& generateUUID()
            throw(uci::base::UCIException) = 0;


         /** This method will generate a new NIL UUID. The value of this UUID will be zero, i.e. all bytes contain in the UUID's
           * value will be set to zero. This UUID must be initialized before it can be used by invoking one of the UUID's
           * initialization routines. Such an initialization is usually done using the value of a previously generated UUID that
           * was saved/stored in some format, e.g. a stringified version.
           *
           * @return The newly generated NIL UUID
           */
         virtual uci::base::UUID& generateNilUUID()
            throw(uci::base::UCIException) = 0;


         /** This method will destroy a generated UUID, generated either by invoking generateUUID() or generateNilUUID().
           *
           * @param uuid The UUID to destroy
           */
         virtual void destroyUUID(const uci::base::UUID& uuid)
            throw(uci::base::UCIException) = 0;


         /** This method will return the caller's system UUID
           *
           * @return The caller's system UUID
           */
         virtual const uci::base::UUID& getMySystemUUID()
            throw(uci::base::UCIException) = 0;


         /** This method will return the caller's service UUID
           *
           * @return The caller's service UUID
           */
         virtual const uci::base::UUID& getMyServiceUUID()
            throw(uci::base::UCIException) = 0;


         /** This method will return the caller's subsystem UUID
           *
           * @return The caller's subsystem UUID
           */
         virtual const uci::base::UUID& getMySubsystemUUID()
            throw(uci::base::UCIException) = 0;


         /** This method will return the UUID of the caller's component having the specified name
           *
           * @param componentName The name of the caller's component whose UUID is to be returned.
           * @return The UUID of the caller's named component
           */
         virtual const uci::base::UUID& getMyComponentUUID(const std::string& componentName)
            throw(uci::base::UCIException) = 0;


         /** This method will return the UUID of the caller's capability having the specified name
           *
           * @param capabilityName The name of the caller's capability whose UUID is to be returned.
           * @return The UUID of the caller's named capability
           */
         virtual const uci::base::UUID& getMyCapabilityUUID(const std::string& capabilityName)
            throw(uci::base::UCIException) = 0;


         /** This method will return the UUID of the caller's primary controller
           *
           * @return The UUID of the caller's primary controller
           */
         virtual const uci::base::UUID& getMyPrimaryControllerUUID()
            throw(uci::base::UCIException) = 0;


         /** Returns the version of the OMS schema that was used to generate this AbstractServiceBusConnection class.
           *
           * @return The version of the OMS Schema Compiler specification that the OMS Schema compiler that generated this
           *      AbstractServiceBusConnection class complies with.
           */
         virtual const std::string getOmsSchemaVersion()
         {
            return std::string("075.0");
         }


         /** Returns the version of the OMS Schema Compiler specification that the OMS Schema compiler that generated this
           * AbstractServiceBusConnection class complies with.
           *
           * @return The CAL-GT/API version info
           */
         virtual const std::string getOmsSchemaCompilerVersion()
         {
            return std::string("2.2.1");
         }


         /** Returns the version of abstract service bus connection that this AbstractServiceBusConnection class supports
           *
           * @return The version of the abstract service bus connection
           */
         virtual const std::string getAbstractServiceBusConnectionVersion()
            throw(uci::base::UCIException) = 0;



      protected:

         /** The constructor [only available to derived classes]. */
         AbstractServiceBusConnection()
         { }

         /** The copy constructor [only available to derived classes]
           *
           * @param rhs The AbstractServiceBusConnection to copy from
           */
         AbstractServiceBusConnection(const AbstractServiceBusConnection& rhs)
         {
            (void)rhs;
         }

         /** The assignment operator. Sets the contents of this AbstractServiceBusConnection to the contents of the
           * AbstractServiceBusConnection on the right hand side (rhs) of the assignment operator.AbstractServiceBusConnection
           * [only available to derived classes]
           *
           * @param rhs The AbstractServiceBusConnection on the right hand side (rhs) of the assignment operator whose contents
           *      are used to set the contents of this uci::base::AbstractServiceBusConnection
           * @return A constant reference to this AbstractServiceBusConnection.
           */
         const AbstractServiceBusConnection& operator=(const AbstractServiceBusConnection& rhs)
         {
            (void)rhs;

            return *this;
         }


      }; // AbstractServiceBusConnection


   } // Namespace: base
} // Namespace: uci

#ifdef __cplusplus
extern "C" {
#endif

//  Creates a new abstract service bus connection
//  
//   @returns The newly created abstract service bus connection
//  
UCI_EXPORT uci::base::AbstractServiceBusConnection* uci_getAbstractServiceBusConnection(const std::string& serviceIdentifier, const std::string& typeOfAbstractServiceBusConnection) throw(uci::base::UCIException);

#ifdef __cplusplus
}
#endif

#endif // Uci__Base__AbstractServiceBusConnection_h

