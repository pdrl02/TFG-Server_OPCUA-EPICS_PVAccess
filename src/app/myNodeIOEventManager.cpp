#include "myNodeIOEventManager.h"

#include <opcua_dataitemtype.h>
#include <opcua_analogitemtype.h>
#include <opcua_twostatediscretetype.h>
#include <opcua_multistatediscretetype.h>


#include <iostream>

#include <typeIDs.h>
#include <iocBasicObject.h>

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
    : NodeManagerBase("TFG:OPCUA_EPICS", OpcUa_True) {

    std::cout << "Constructor del servidor..." << std::endl;
    //m_defaultLocaleId = "en";

}

MyNodeIOEventManager::~MyNodeIOEventManager(){
    
}

UaStatus MyNodeIOEventManager::createObjectType(const UaString & name, const int typeId, OpcUa_Boolean abstract, const UaNodeId & sourceNode) {
   
    UaObjectTypeSimple * pType;
    UaStatus result;

    pType = new UaObjectTypeSimple(
        name,                                             // String used in browse name and display name
        UaNodeId(typeId, getNameSpaceIndex()),            // Numeric NodeId for types
        m_defaultLocaleId,                                // Defaul LocaleId for UaLocalizedText strings
        abstract                                          // Abstract object -> Can not be instantiated
    );

    result = addNodeAndReference(sourceNode, pType, OpcUaId_HasSubtype);
    //std::cout << result.toString().toUtf8() << std::endl;
    UA_ASSERT(result.isGood());

    return result; 
}

UaStatus MyNodeIOEventManager::createAnalogVariableType(
    const UaString & name,
    const OpcUa_Double value,
    const int typeId,
    const UaNodeId & sourceNode,
    const bool writable,
    const bool mandatory,
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
        UaNodeId( typeId, getNameSpaceIndex()),
        name,
        getNameSpaceIndex(),
        defaultValue,
        (writable ? (Ua_AccessLevel_CurrentRead | Ua_AccessLevel_CurrentWrite) : Ua_AccessLevel_CurrentRead),
        this);
    pBaseAnalogType->setModellingRuleId(mandatory ? OpcUaId_ModellingRule_Mandatory : OpcUaId_ModellingRule_Optional);
    result = addNodeAndReference(sourceNode, pBaseAnalogType, OpcUaId_HasComponent);
    UA_ASSERT(result.isGood());

    // Set property values
    pBaseAnalogType->setEngineeringUnits(EngineeringUnits);
    pBaseAnalogType->setEURange(EURange);
    pBaseAnalogType->setInstrumentRange(InstrumentRange);

    return result;

}

// in or out cambia UaAccessLevel

UaStatus MyNodeIOEventManager::createTwoStateVariableType(
    const UaString &name, 
    const OpcUa_Boolean value, 
    const int typeId,
    const UaNodeId &sourceNode,
    const bool writable,
    const bool mandatory,
    const UaLocalizedText & falseText,
    const UaLocalizedText & trueText
    ) {

    UaVariant defaultValue;
    OpcUa::TwoStateDiscreteType* pTwoStateDiscreteType;
    UaStatus result;

    defaultValue.setBool(value);
    pTwoStateDiscreteType = new OpcUa::TwoStateDiscreteType(
        UaNodeId(typeId, getNameSpaceIndex()),
        name,
        getNameSpaceIndex(),
        defaultValue,
        (writable ? (Ua_AccessLevel_CurrentRead | Ua_AccessLevel_CurrentWrite) : Ua_AccessLevel_CurrentRead),
        this);

    pTwoStateDiscreteType->setModellingRuleId( mandatory ? OpcUaId_ModellingRule_Mandatory : OpcUaId_ModellingRule_Optional);
    pTwoStateDiscreteType->setFalseState(falseText);
    pTwoStateDiscreteType->setTrueState(trueText);

    result = addNodeAndReference(sourceNode, pTwoStateDiscreteType, OpcUaId_HasComponent);
    
    return result;
    
}

UaStatus MyNodeIOEventManager::createMultiStatateVariableType(
    const UaString &name, 
    const OpcUa_Int64 value, 
    const int typeId,
    const UaNodeId &sourceNode, 
    const bool writable,
    const bool mandatory,
    const UaLocalizedTextArray & enumStrings 
    ) {
    
    UaVariant defaultValue;
    OpcUa::MultiStateDiscreteType* pMultiStateDiscreteType;
    UaStatus result;

    defaultValue.setInt64(value);
    pMultiStateDiscreteType = new OpcUa::MultiStateDiscreteType(
        UaNodeId(typeId, getNameSpaceIndex()),
        name,
        getNameSpaceIndex(),
        defaultValue,
        (writable ? (Ua_AccessLevel_CurrentRead | Ua_AccessLevel_CurrentWrite) : Ua_AccessLevel_CurrentRead),
        this);

    pMultiStateDiscreteType->setModellingRuleId( mandatory ? OpcUaId_ModellingRule_Mandatory : OpcUaId_ModellingRule_Optional);
    pMultiStateDiscreteType->setEnumStrings(enumStrings);
    result = addNodeAndReference(sourceNode, pMultiStateDiscreteType, OpcUaId_HasComponent);
    
    return result;
}

UaStatus MyNodeIOEventManager::createObject(
    const UaString & objectName,         // Name of the object
    const int typeId,                    // Type of the object
    const UaNodeId & objectId,           // NodeId of the new object
    const UaNodeId & sourceNodeId        // Node of the parent
) {
    //std::cout << "AA" << m_defaultLocaleId << "AA" << std::endl;
    UaStatus result;

    IocBasicObject * pObject = new IocBasicObject(objectName, objectId, m_defaultLocaleId, this, typeId);

    m_mutexNodes.lock();
    std::vector<UaVariable*> variables = getInstanceDeclarationVariableArray(typeId);
    for(auto* it : variables){
        pObject->addVariable(it->nodeId().identifierNumeric());
    }

    m_mutexNodes.unlock();
    result = addNodeAndReference(sourceNodeId, pObject, OpcUaId_Organizes); 
    
    return result;
}

// Esta función se llama cuando el NodeManager está creado e inicializado.
// Aquí creamos nuestros UaNodes. Nosotros llamamos a createTypeNodes para crear nuestro Type Model
UaStatus MyNodeIOEventManager::afterStartUp(){

    // Create IOCBasicType
    createObjectType("IOCBasicType", TFG_IOC_Basic_Type, OpcUa_True, OpcUaId_BaseObjectType);
    // Create 3 IOC types
    createObjectType("Ejemplo1Type", TFG_IOC_Ejemplo1, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));
    createObjectType("Ejemplo2Type", TFG_IOC_Ejemplo2, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));
    createObjectType("Ejemplo3Type", TFG_IOC_Ejemplo3, OpcUa_False, UaNodeId(TFG_IOC_Basic_Type, getNameSpaceIndex()));

    // Create variables for Ejemplo1 
    UaEUInformation EUCelsius(
        getNameSpaceUri(), 
        4408652,                                    // UnitId for Celsius
        UaLocalizedText("en", "\xC2\xB0\x43"),      // "°C" en UTF-8
        UaLocalizedText("en", "Degrees Celsius")
    );
    UaRange temperatureRange(-50, 100);

    createAnalogVariableType("Temperature", 20, TFG_IOC_Ejemplo1_Temperature, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         false, true, EUCelsius, temperatureRange);

    UaEUInformation EUPercentage(
        getNameSpaceUri(), 
        20529,                                       // UnitId for percentage
        UaLocalizedText("en", "\x25"),               // "%" en UTF-8
        UaLocalizedText("en", "Percentage")
    );

    UaRange fanSpeedRange(0, 100);

    createAnalogVariableType("FanSpeed", 20, TFG_IOC_Ejemplo1_FanSpeed, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         true, true, EUPercentage, fanSpeedRange);


    // Create Variables for Ejemplo2
    createTwoStateVariableType("OpenCmd", OpcUa_False, TFG_IOC_Ejemplo2_OpenCmd, UaNodeId(TFG_IOC_Ejemplo2, getNameSpaceIndex()),
                                true, true, UaLocalizedText("en", "Closed"), UaLocalizedText("en", "Open"));
    createTwoStateVariableType("Status", OpcUa_True, TFG_IOC_Ejemplo2_Status, UaNodeId(TFG_IOC_Ejemplo2, getNameSpaceIndex()),
                                false, true, UaLocalizedText("en", "Stopped"), UaLocalizedText("en", "Running"));

    // Create Variables for Ejemplo3
    UaEUInformation EUCounter(
        getNameSpaceUri(), 
        0,                                  
        UaLocalizedText("en", "counts"),     
        UaLocalizedText("en", "Counter Steps")
    );
    UaRange counterRange(-2000, 100000);
    createAnalogVariableType("int64in", 100, TFG_IOC_Ejemplo3_int64in, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             false, true, EUCounter, counterRange);
    
    UaEUInformation EUJoules(
        getNameSpaceUri(), 
        5426159,                                          
        UaLocalizedText("en", "J"),       
        UaLocalizedText("en", "Joules")
    );
    UaRange joulesRange(1000, 5000);
    createAnalogVariableType("int64out", 2000, TFG_IOC_Ejemplo3_int64out, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             true, true, EUJoules, joulesRange);

    UaEUInformation EUSteps(
        getNameSpaceUri(), 
        0,                                          
        UaLocalizedText("en", "steps"),       
        UaLocalizedText("en", "Steps")
    );
    UaRange stepsRange(0, 100000);
    createAnalogVariableType("longin", 27182, TFG_IOC_Ejemplo3_longin, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             false, true, EUSteps, stepsRange);

    UaEUInformation EUCalories(
        getNameSpaceUri(), 
        5442241,                                        
        UaLocalizedText("en", "Cal"),      
        UaLocalizedText("en", "Calories")
    );
    UaRange caloriesRange(3000000, 4000000);
    createAnalogVariableType("longout", 3141592, TFG_IOC_Ejemplo3_longout, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             true, true, EUCalories, caloriesRange);

    OpcUa_Int32 size = 4;
    UaLocalizedTextArray states;
    states.create(size);
    for(int i = 0; i < size; ++i)
        UaLocalizedText("en", ("State " + std::to_string(i)).c_str()).copyTo(&states[i]);

    createMultiStatateVariableType("mbbi", 0, TFG_IOC_Ejemplo3_mbbi, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                                    false, true, states);

    
    states.create(size * 2);
    for(int i = 0; i < size * 2; ++i)
        UaLocalizedText("en", ("State " + std::to_string(i)).c_str()).copyTo(&states[i]);
    
    createMultiStatateVariableType("mbbo", 7, TFG_IOC_Ejemplo3_mbbo, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                                    true, true, states);

    
    createObject("obj1.1", TFG_IOC_Ejemplo1, UaNodeId("obj1.1", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("obj1.2", TFG_IOC_Ejemplo1, UaNodeId("obj1.2", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("obj2.1", TFG_IOC_Ejemplo2, UaNodeId("obj2.1", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("obj2.2", TFG_IOC_Ejemplo2, UaNodeId("obj2.2", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("obj3.1", TFG_IOC_Ejemplo3, UaNodeId("obj3.1", getNameSpaceIndex()), OpcUaId_ObjectsFolder);

    
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

// Return a vector with all variables of a object type
// Thread unsafe require     m_mutexNodes.lock();
std::vector<UaVariable*> MyNodeIOEventManager::getInstanceDeclarationVariableArray(OpcUa_UInt32 numericIdentifier){
    
    UaNode* pNode = findNode(UaNodeId(numericIdentifier, getNameSpaceIndex()));
    UaReference * pReference = const_cast<UaReference *>(pNode->getUaReferenceLists()->pTargetNodes());
    std::vector<UaVariable*> variables;

    while(pReference != nullptr){
        UaNode * pNode = pReference->pTargetNode();
        if ( (pNode != NULL) && (pNode->nodeClass() == OpcUa_NodeClass_Variable) )
            variables.push_back((UaVariable *)pNode);

        pReference = pReference->pNextForwardReference();
    }

    return variables;

    

}