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
        OpcServer* pServer = new OpcServer;
        ret = pServer->setServerConfig(sConfigFileName, szAppPath);

        // Add NodeManager for the server specific nodes
        MyNodeIOEventManager *pMyNodeIOEventManager = new MyNodeIOEventManager();
        ret = pServer->setMyNodeManager(pMyNodeIOEventManager);

        // Start server object
        ret = pServer->start();
        if ( ret != 0 ){
            std::cout << "Error al iniciar el servidor: " << ret << std::endl;
            delete pServer;
            pServer = 0;
        }

        //- Add variable to address space -----------------------------------------------------------------------------
        // Get the default node manager for server specific nodes from the SDK
        NodeManagerConfig *pNodeConfig = pServer->getDefaultNodeManager();
        // Create a variable node with a string data type
        UaVariant defaultValue;
        defaultValue.setString("Hello World");
        OpcUa::BaseDataVariableType *pVariable = new OpcUa::BaseDataVariableType(
            UaNodeId("HelloWorld", pNodeConfig->getNameSpaceIndex()), // NodeId of the node with string identifier "HelloWorld" and the namespace index of the default node manager which is 1
            "HelloWorld",                                             // Name of the node used for display name and browse name
            pNodeConfig->getNameSpaceIndex(),                         // The same namespace index is also used for the browse name
            defaultValue,                                             // Setting the default value and the data type of the variable
            OpcUa_AccessLevels_CurrentRead,                           // Setting the access level to read only
            pNodeConfig);                                             // The node manager config interface used for this node
        // Add the node to the node manager using the objects folder as source node and the reference type HasComponent
        pNodeConfig->addNodeAndReference(UaNodeId(OpcUaId_ObjectsFolder, 0), pVariable, OpcUaId_HasComponent);
        //--------------------------------------------------------------------------------------------------------------

        if ( ret == 0 ){
            EPICStoOPCUAGateway * pGateway = new EPICStoOPCUAGateway (pMyNodeIOEventManager);
            pServer->addEPICSGateway(pGateway);

            printf("***************************************************\n");
            printf(" Press %s to shut down server\n", SHUTDOWN_SEQUENCE);
            printf("***************************************************\n");
            // Wait for user command to terminate the server thread.
            // Simulate data

            // UaString name = "Obj1";
            // pServer->getMyNodeIOEventManager()->
            //     createObject(name, TFG_IOC_Ejemplo1, UaNodeId(name, pNodeConfig->getNameSpaceIndex()), OpcUaId_ObjectsFolder);


            
            const char animation[] = { '#', '#', '#', ' '}; // Secuencia de puntos
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
            delete pServer;
            pServer = NULL;
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

#include <pvxs/client.h>

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

    // // Prueba de pvxs
    // using namespace pvxs;

    // // Configure client using $EPICS_PVA_*
    // auto ctxt(client::Context::fromEnv());

    // // fetch PV "some:pv:name" and wait up to 5 seconds for a reply.
    // // (throws an exception on error, including timeout)
    // //Value reply = ctxt.get("ejemplo1:Temperature").exec()->wait(5.0);
    
    // MPMCFIFO<std::shared_ptr<Subscription>> workqueue(42u);

    // auto sub = ctxt.monitor("ejemplo1:Temperature")
    //             .event([&workqueue](Subscription& sub) {
    //                 // Subscription queue becomes not empty.
    //                 // Avoid I/O on PVXS worker thread,
    //                 // delegate to application thread
    //                 workqueue.push(sub.shared_from_this());
    //             })
    //             .exec();

    // while(auto sub = workqueue.pop()) { // could workqueue.push(nullptr) to break
    //     try {
    //         Value update = sub->pop();
    //         cout << sub->name() << endl;
    //         if(!update)
    //             continue; // Subscription queue empty, wait for another event callback
    //         std::cout<<update<<"\n";
    //     } catch(std::exception& e) {
    //         // may be Connected(), Disconnect(), Finished(), or RemoteError()
    //         std::cerr<<"Error "<<e.what()<<"\n";
    //     }
    //     // queue not empty, reschedule
    //     workqueue.push(sub);
    // }
    // // store op until completion

    return 0;
    


}