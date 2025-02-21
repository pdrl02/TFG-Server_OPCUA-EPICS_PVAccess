#include "myNodeIOEventManager.h"

#include "opcua_dataitemtype.h"
#include "opcua_analogitemtype.h"

#include <iostream>

/**
 * Crea un type model especifico para el servidor. Se hace definiendo y añadiendo TypeNode al 
 * espacio de direcciones del servidor. TypeNode representan ObjectTypes, es decir, tipos de objetos,
 * son como clases.
 */
UaStatus MyNodeIOEventManager::createTypeNodes()
{
    return UaStatus();
}

MyNodeIOEventManager::MyNodeIOEventManager()
    : NodeManagerBase("urn:UnifiedAutomation:CppDemoServer:BuildingAutomation", OpcUa_True){

    std::cout << "Constructor del servidor..." << std::endl;
    

}

MyNodeIOEventManager::~MyNodeIOEventManager(){

}

// Esta función se llama cuando el NodeManager está creado e inicializado.
// Aquí creamos nuestros UaNodes. Nosotros llamamos a createTypeNodes para crear nuestro Type Model
UaStatus MyNodeIOEventManager::afterStartUp(){

    return UaStatus();
}

// Se llama cuando se cierra. Los nodos se limpian automaticamente pero podemos poner otro tipo de código.
UaStatus MyNodeIOEventManager::beforeShutDown()
{
    std::cout << "Se cierra el servidor" << std::endl;
    return UaStatus();
}

UaStatus MyNodeIOEventManager::readValues(const UaVariableArray &arrUaVariables, UaDataValueArray &arrDataValues)
{
    return UaStatus();
}

UaStatus MyNodeIOEventManager::writeValues(const UaVariableArray &arrUaVariables, const PDataValueArray &arrpDataValues, UaStatusCodeArray &arrStatusCodes)
{
    return UaStatus();
}

UaStatus MyNodeIOEventManager::OnAcknowledge(
    const ServiceContext &serviceContext,
    OpcUa::AcknowledgeableConditionType *pCondition,
     const UaByteString &EventId,
      const UaLocalizedText &Comment)
{
    return UaStatus();
}

UaVariable * MyNodeIOEventManager::getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier)
{
    // Try to find the instance declaration node with the numeric identifier 
    // and the namespace index of this node manager
    UaNode* pNode = findNode(UaNodeId(numericIdentifier, getNameSpaceIndex()));
    if ( (pNode != NULL) && (pNode->nodeClass() == OpcUa_NodeClass_Variable) )
        // Return the node if valid and a variable
        return (UaVariable*)pNode;
    else
        return NULL;
    
}