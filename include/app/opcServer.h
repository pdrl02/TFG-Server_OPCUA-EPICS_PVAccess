/**
 * @file opcServer.h
 * @brief Declaration of the OpcServer class. 
 * 
 * This file defines the main OPC UA Server object used to manage all server SDK modules.
 * It extends [UaServerApplication](https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classUaServerApplication.html)
 * to provide specific methods for this project.
 * 
 * @author Pablo Del Río López
 * @date 2025-06-01
 */

#ifndef MAIN_OPCSERVER_H
#define MAIN_OPCSERVER_H

#include "uaserverapplication.h"
#include "myNodeIOEventManager.h"
#include <memory>
#include <EPICStoOPCUAGateway.h>
class UaServer;

/**
 * @class OpcServer
 * @brief Main object of the OpcUa Server 
 * 
 * This class is an utility class that manages all server SDK modules.
 * It extends [UaServerApplication](https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classUaServerApplication.html)
 * to provide specific methods and behaviour for this project.
 */
class OpcServer: public UaServerApplication
{

    UA_DISABLE_COPY(OpcServer);

public:
    
    /**
     * @brief Default constructor. 
     */
    OpcServer();

    /**
     * @brief Destructor.
     * 
     * Ensures that beforeShutdown() is called before destroying itself.
     */
    virtual ~OpcServer();

    /**
     * @brief Get the build date from the static compiled-in string.
     * 
     * Function based on the Unified Automation SDK sample code.
     * Original source: opcserver.cpp © 2006–2024 Unified Automation GmbH.
     * See SLA 2.8: https://www.unified-automation.com/License/SLA/2.8/
     * 
     * @return The build date from the static compiled-in string.
     */
    virtual OpcUa_DateTime getBuildDate() const;

    /**
     * @brief Get a pointer to the custom implementation of NodeManagerBase, MyNodeIOEventManager.
     * @return  Pointer to MyNodeIOEventManager.
     * @see MyNodeIOEventManager
     */
    virtual MyNodeIOEventManager * getMyNodeIOEventManager();

    /**
     * @brief Adds an EPICS-to-OPC_UA Gateway to the server.
     * This class will delete the gateway when this class destroy itself.
     * This method will start the gateway. 
     * @param gate Pointer to the EPICS-to-OPC_UA gateway object.
     * @see EPICStoOPCUAGateway
     */
    virtual void addEPICSGateway(EPICStoOPCUAGateway * gate);

    /**
     * @brief Sets the custom node manager for this server instance.
     * It can be setted with addNodeManager() too, but then the pointer to the node manager will not be initialized.
     * This method use addNodeManager() from 
     * [UaCoreServerApplication](https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classUaCoreServerApplication.html)
     * to set the node manager.
     * @return Success code for the operation. Return 0 if adding the node manager succedded and
     *  -1 if adding the node manager failed.
     */
    virtual int setMyNodeManager(MyNodeIOEventManager * pMyNodeManager);

protected:

    /**
     * @brief Post-startup initialization routine.
     * 
     * Copied and adapted from the Unified Automation SDK sample code.
     * Original file: opcserver.cpp (c) 2006–2024 Unified Automation GmbH.
     * License: SLA v2.8 – https://www.unified-automation.com/License/SLA/2.8/
     * 
     * This method logs all opened and failed endpoint URLs after server startup.
     * 
     * @return UaStatus indicating success or failure.
     */
    virtual UaStatus afterStartUp();

private:
    /**
     * @brief Pointer to the EPICS-to-OPC_UA Gateway of the server.
     * @see EPICStoOPCUAGateway
     */
    EPICStoOPCUAGateway * m_pGateway = nullptr;

    /**
     * @brief Pointer to the custom node manager created for this project.
     * @see MyNodeIOEventManager
     */
    MyNodeIOEventManager * m_pMyNodeManager = nullptr;
};


#endif // MAIN_OPCSERVER_H


