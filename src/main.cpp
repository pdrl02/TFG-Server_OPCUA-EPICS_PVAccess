#include "uaplatformlayer.h"
#include "shutdown.h"
#include "xmldocument.h"
#include "opcServer.h"
#include "uathread.h"
#include <iostream>
#include <string.h>
#include <myNodeIOEventManager.h>
#include <typeIDs.h>
#include <thread>



int OpcServerMain(const char* szAppPath)
{
    int ret = 0;
    //- Initialize the environment --------------
    
    // Initialize the XML Parser si se utilizan los xml para la configuración del servidor
    UaXmlDocument::initParser();

    // Initialize the UA Stack platform layer
    ret = UaPlatformLayer::init();


    //-------------------------------------------

    if ( ret == 0 ){
        // Create configuration file name
        UaString sConfigFileName(szAppPath);
        sConfigFileName += "/ServerConfig.xml";

        //- Start up OPC server ---------------------
        //-------------------------------------------
        // Create and initialize server object
        unique_ptr<OpcServer> pServer = make_unique<OpcServer>();
        ret = pServer->setServerConfig(sConfigFileName, szAppPath);

        // Add NodeManager for the server specific nodes
        MyNodeIOEventManager *pMyNodeIOEventManager = new MyNodeIOEventManager();
        ret = pServer->setMyNodeManager(pMyNodeIOEventManager);
        

        // Start server object
        ret = pServer->start();
        if ( ret != 0 ){
            std::cout << "Error starting the server: " << ret << std::endl;
        }

        if ( ret == 0 ){
            // Add Gateway to the server
            EPICStoOPCUAGateway * pGateway = new EPICStoOPCUAGateway (pMyNodeIOEventManager, 1);
            pServer->addEPICSGateway(pGateway);

            printf("***************************************************\n");
            printf(" Press %s to shut down server\n", SHUTDOWN_SEQUENCE);
            printf("***************************************************\n");
            // Wait for user command to terminate the server thread.
            const char animation[] = { '#', '#', '#', ' '}; 
            int index = 0;
            while (ShutDownFlag() == 0)
            {
                std::cout << "\r" << animation[index % 4] << animation[(index + 1) % 4] << animation[(index + 2) % 4] << std::flush;
                UaThread::msleep(500);
                index++;
            }
            printf("***************************************************\n");
            printf(" Shutting down server\n");
            printf("***************************************************\n");
            //- Stop OPC server -------------------------
            // Stop the server and wait three seconds if clients are connected
            // to allow them to disconnect after they received the shutdown signal
            pServer->stop(3, UaLocalizedText("", "User shutdown"));
            //-------------------------------------------
        }
    }
    
    
    //- Clean up the environment --------------
    
        // Clean up the UA Stack platform layer
    UaPlatformLayer::cleanup();
    
        // Clean up the XML Parser
    UaXmlDocument::cleanupParser();
    //-------------------------------------------
    return ret;
}

int main(int, char*[])
{
    // Servidor OPC UA
    int ret = 0;
    RegisterSignalHandler();
    // Extract application path
    char* pszAppPath = getAppPath();
    //-------------------------------------------
    // Call the OPC server main method
    ret = OpcServerMain(pszAppPath);
    //-------------------------------------------
    if ( pszAppPath ) 
        delete [] pszAppPath;

    return ret;

    return 0;
    
}