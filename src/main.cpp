#include "uaplatformlayer.h"
#include "shutdown.h"
#include "xmldocument.h"
#include "opcserver.h"
#include "uathread.h"
#include <iostream>
#include <string.h>

#include <EPICSClient.h>
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
        pServer->setServerConfig(sConfigFileName, szAppPath);

        // Add NodeManager for the server specific nodes
        MyNodeIOEventManager *pMyNodeIOEventManager = new MyNodeIOEventManager();
        pServer->addNodeManager(pMyNodeIOEventManager);

        // Start server object
        ret = pServer->start();
        if ( ret != 0 ){
            std::cout << "Error: " << ret << std::endl;
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


class MyMonitor : public epics::pvaClient::PvaClientMonitorRequester {

    void monitorConnect(Status const & status, PvaClientMonitorPtr const & monitor, StructureConstPtr const & structure) override {

        std::cout << "Monitor conectado: " << monitor->getPvaClientChannel()->getChannelName() << std::endl;
        

    }

    void event(PvaClientMonitorPtr const & monitor ) override {

        std::cout << "Entro en event" << std::endl;
        if(monitor->getPvaClientChannel()->getChannelName() == "ejemplo1:Temperature")
            std::this_thread::sleep_for(std::chrono::seconds(3));
        else
            std:: cout << "Monitor 2: " << monitor->getPvaClientChannel()->getChannelName() << std::endl;
        while (monitor->poll()) {  // Verifica si hay datos nuevos
            auto pvStruct = monitor->getData()->getPVStructure();
            auto pvValue = pvStruct->getSubField<PVDouble>("value");
            if (pvValue) {
                std::cout << "Nuevo valor recibido: " << pvValue->get() << std::endl;
            }
            monitor->releaseEvent();
        }

    }

    void unlisten() override {

        std::cout << "Monitor desconectado" << std::endl;
    }
};


int main(int, char*[])
{
    // // Servidor OPC UA
    // int ret = 0;
    // RegisterSignalHandler();
    // // Extract application path
    // char* pszAppPath = getAppPath();
    // //-------------------------------------------
    // // Call the OPC server main method
    // ret = OpcServerMain(pszAppPath);
    // //-------------------------------------------
    // if ( pszAppPath ) 
    //     delete [] pszAppPath;

    // return ret;

    // Prueba de pvaccess
    
    PvaClientPtr pClient = PvaClient::get("pva"); 
    PvaClientChannelPtr pChannel = pClient->channel("ejemplo1:Temperature");

    // Monitorea solo value
    PvaClientMonitorRequesterPtr pRequester = std::make_shared<MyMonitor>();
    PvaClientMonitorPtr pMonitor = pChannel->monitor("field(value)", pRequester);


    PvaClientChannelPtr pChannel2 = pClient->channel("ejemplo1:FanSpeed");
    PvaClientMonitorRequesterPtr pRequester2 = std::make_shared<MyMonitor>();
    // Monitorea solo value
    PvaClientMonitorPtr pMonitor2 = pChannel2->monitor("field(value)", pRequester2);

    while (true) { std::this_thread::sleep_for(std::chrono::seconds(1)); }


}