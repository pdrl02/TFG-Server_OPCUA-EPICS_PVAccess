#include "myNodeIOEventManager.h"

#include "opcua_dataitemtype.h"
#include "opcua_analogitemtype.h"

#include <iostream>

#include <typeIDs.h>

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
    : NodeManagerBase("TFG:OPCUA_EPICS", OpcUa_True){

    std::cout << "Constructor del servidor..." << std::endl;
    

}

MyNodeIOEventManager::~MyNodeIOEventManager(){
    
}

UaStatus MyNodeIOEventManager::createTypeNode(const UaString & name, const int typeID, OpcUa_Boolean abstract, const UaNodeId & sourceNode) {
   
    UaObjectTypeSimple * pType;
    UaStatus result;

    pType = new UaObjectTypeSimple(
        name,                                             // String used in browse name and display name
        UaNodeId(typeID, getNameSpaceIndex()),            // Numeric NodeID for types
        m_defaultLocaleId,                                // Defaul LocaleId for UaLocalizedText strings
        abstract                                          // Abstract object -> Can not be instantiated
    );

    result = addNodeAndReference(sourceNode, pType, OpcUaId_HasSubtype);
    std::cout << result.toString().toUtf8() << std::endl;
    UA_ASSERT(result.isGood());

    return result; 
}

UaStatus MyNodeIOEventManager::createAnalogVariable(
    const UaString & name,
    const OpcUa_Double value,
    const int typeID,
    const UaNodeId & sourceNode,
    const UaEUInformation & EngineeringUnits,
    const UaRange & EURange,
    const UaRange & InstrumentRange	
    ) { 

    UaVariant defaultValue;                                 // Union for all data types
    OpcUa::BaseAnalogType * pBaseAnalogType;                // Base type for analog data
    UaStatus result;

    // Add Variable "Name" as BaseAnalogType
    defaultValue.setDouble(value);
    pBaseAnalogType = new OpcUa::BaseAnalogType(
        UaNodeId( typeID, getNameSpaceIndex()),
        name,
        getNameSpaceIndex(),
        defaultValue,
        Ua_AccessLevel_CurrentRead,
        this);
    pBaseAnalogType->setModellingRuleId(OpcUaId_ModellingRule_Mandatory);
    result = addNodeAndReference(sourceNode, pBaseAnalogType, OpcUaId_HasComponent);
    UA_ASSERT(result.isGood());

    // Set property values
    pBaseAnalogType->setEngineeringUnits(EngineeringUnits);
    pBaseAnalogType->setEURange(EURange);
    pBaseAnalogType->setInstrumentRange(InstrumentRange);

    return result;

}

// Esta función se llama cuando el NodeManager está creado e inicializado.
// Aquí creamos nuestros UaNodes. Nosotros llamamos a createTypeNodes para crear nuestro Type Model
UaStatus MyNodeIOEventManager::afterStartUp(){

    // Create IOCBasicType
    createTypeNode("IOCBasicType", TFG_IOC_Basic_Type, OpcUa_True, OpcUaId_BaseObjectType);
    // Create 3 IOC types
    createTypeNode("Ejemplo1Type", TFG_IOC_Ejemplo1, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));
    createTypeNode("Ejemplo2Type", TFG_IOC_Ejemplo2, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));
    createTypeNode("Ejemplo3Type", TFG_IOC_Ejemplo3, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));

    UaEUInformation EUCelsius(
        getNameSpaceUri(), 
        4408652,                                    // UnitId for Celsius
        UaLocalizedText("en", "\xC2\xB0\x43"),      // "°C" en UTF-8
        UaLocalizedText("en", "Degrees Celsius")
    );
    UaRange temperatureRange(-50, 100);

    createAnalogVariable("Temperature", 20, TFG_IOC_Ejemplo1_Temperature, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         EUCelsius, temperatureRange);

    UaEUInformation EUPercentage(
        getNameSpaceUri(), 
        20529,                                       // UnitId for percentage
        UaLocalizedText("en", "\x25"),               // "%" en UTF-8
        UaLocalizedText("en", "Percentage")
    );

    UaRange fanSpeedRange(0, 100);

    createAnalogVariable("FanSpeed", 20, TFG_IOC_Ejemplo1_FanSpeed, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         EUPercentage, fanSpeedRange);
    
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