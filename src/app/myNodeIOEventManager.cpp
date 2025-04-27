#include "myNodeIOEventManager.h"
#include <opcua_dataitemtype.h>
#include <opcua_analogitemtype.h>
#include <opcua_twostatediscretetype.h>
#include <opcua_multistatediscretetype.h>
#include <iostream>
#include <typeIDs.h>
#include <iocBasicObject.h>
#include <EPICStoOPCUAGateway.h>

MyNodeIOEventManager::MyNodeIOEventManager()
    : NodeManagerBase("TFG:OPCUA_EPICS", OpcUa_False) {

    std::cout << "Constructor del servidor..." << std::endl;

}

MyNodeIOEventManager::~MyNodeIOEventManager(){}

UaStatus MyNodeIOEventManager::createObjectType(
    const UaString & name, 
    const int typeId, 
    OpcUa_Boolean abstract, 
    const UaNodeId & sourceNode) {
   
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
    const OpcUa_Int16 value, 
    const int typeId,
    const UaNodeId &sourceNode, 
    const bool writable,
    const bool mandatory,
    const UaLocalizedTextArray & enumStrings 
    ) {
    
    UaVariant defaultValue;
    OpcUa::MultiStateDiscreteType* pMultiStateDiscreteType;
    UaStatus result;

    defaultValue.setInt16(value);
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
    UaStatus result;

    IocBasicObject * pObject = new IocBasicObject(objectName, objectId, m_defaultLocaleId, this, typeId);

    m_mutexNodes.lock();
    std::vector<UaVariable*> variables = getVariablesFromObjectType(typeId);
    for(auto it : variables){
        pObject->addVariable(it);
    }

    m_mutexNodes.unlock();
    result = addNodeAndReference(sourceNodeId, pObject, OpcUaId_Organizes); 
    
    return result;
}

UaStatus MyNodeIOEventManager::updateVariable(UaNodeId &nodeId, UaVariant &variant) {

    UaNode * pNode = getNode(nodeId);
    if(!pNode){
        return UaStatus(OpcUa_BadNodeIdUnknown);
    }

    UaVariable * pVariable = dynamic_cast<UaVariable*>(pNode);
    if(!pVariable){
        return UaStatus(OpcUa_BadNodeIdRejected);
    }

    UaDateTime sourceTimestamp = UaDateTime::now();
    UaDateTime serverTimestamp = UaDateTime::now();    

    OpcUa_Double val;
    variant.toDouble(val);
    //std::cout << "Variant: " << val << std::endl;

    UaDataValue dataValue(variant, OpcUa_Good, sourceTimestamp, serverTimestamp);
    return pVariable->setValue( NULL /*this->m_pServerManager->getInternalSession()*/, dataValue, OpcUa_False );
}

void MyNodeIOEventManager::setEPICSGateway(EPICStoOPCUAGateway* pEPICSGateway) {
    m_pEPICSGateway = pEPICSGateway;
}

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

    OpcUa_Double doubleValue = 20.0;
    createAnalogVariableType("Temperature", doubleValue, TFG_IOC_Ejemplo1_Temperature, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         false, true, EUCelsius, temperatureRange);

    UaEUInformation EUPercentage(
        getNameSpaceUri(), 
        20529,                                       // UnitId for percentage
        UaLocalizedText("en", "\x25"),               // "%" en UTF-8
        UaLocalizedText("en", "Percentage")
    );

    UaRange fanSpeedRange(0, 100);

    createAnalogVariableType("FanSpeed", doubleValue, TFG_IOC_Ejemplo1_FanSpeed, UaNodeId(TFG_IOC_Ejemplo1, getNameSpaceIndex()),
                         true, true, EUPercentage, fanSpeedRange);


    // Create Variables for Ejemplo2
    OpcUa_Boolean valueBool = OpcUa_False;
    createTwoStateVariableType("OpenCmd", valueBool, TFG_IOC_Ejemplo2_OpenCmd, UaNodeId(TFG_IOC_Ejemplo2, getNameSpaceIndex()),
                                true, true, UaLocalizedText("en", "Closed"), UaLocalizedText("en", "Open"));

    valueBool = OpcUa_True;
    createTwoStateVariableType("Status", valueBool, TFG_IOC_Ejemplo2_Status, UaNodeId(TFG_IOC_Ejemplo2, getNameSpaceIndex()),
                                false, true, UaLocalizedText("en", "Stopped"), UaLocalizedText("en", "Running"));

    // Create Variables for Ejemplo3
    UaEUInformation EUCounter(
        getNameSpaceUri(), 
        0,                                  
        UaLocalizedText("en", "counts"),     
        UaLocalizedText("en", "Counter Steps")
    );
    UaRange counterRange(-2000, 100000);
    OpcUa_Int64 valueInt64 = 100;
    createAnalogVariableType("int64in", valueInt64, TFG_IOC_Ejemplo3_int64in, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             false, true, EUCounter, counterRange);
    
    UaEUInformation EUJoules(
        getNameSpaceUri(), 
        5426159,                                          
        UaLocalizedText("en", "J"),       
        UaLocalizedText("en", "Joules")
    );
    UaRange joulesRange(1000, 5000);
    valueInt64 = 2000;
    createAnalogVariableType("int64out", valueInt64, TFG_IOC_Ejemplo3_int64out, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             true, true, EUJoules, joulesRange);

    UaEUInformation EUSteps(
        getNameSpaceUri(), 
        0,                                          
        UaLocalizedText("en", "steps"),       
        UaLocalizedText("en", "Steps")
    );
    UaRange stepsRange(0, 100000);
    OpcUa_Int32 valueInt32 = 27182;
    createAnalogVariableType("longin", valueInt32, TFG_IOC_Ejemplo3_longin, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             false, true, EUSteps, stepsRange);

    UaEUInformation EUCalories(
        getNameSpaceUri(), 
        5442241,                                        
        UaLocalizedText("en", "Cal"),      
        UaLocalizedText("en", "Calories")
    );
    UaRange caloriesRange(3000000, 4000000);
    valueInt32 = 3141592;
    createAnalogVariableType("longout", valueInt32, TFG_IOC_Ejemplo3_longout, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                             true, true, EUCalories, caloriesRange);

    OpcUa_Int32 size = 4;
    UaLocalizedTextArray states;
    states.create(size);
    for(int i = 0; i < size; ++i)
        UaLocalizedText("en", ("State " + std::to_string(i)).c_str()).copyTo(&states[i]);
    OpcUa_Int16 valueInt16 = 0;
    createMultiStatateVariableType("mbbi", valueInt16, TFG_IOC_Ejemplo3_mbbi, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                                    false, true, states);

    
    states.create(size * 2);
    for(int i = 0; i < size * 2; ++i)
        UaLocalizedText("en", ("State " + std::to_string(i)).c_str()).copyTo(&states[i]);
    
    valueInt16 = 7;
    createMultiStatateVariableType("mbbo", valueInt16, TFG_IOC_Ejemplo3_mbbo, UaNodeId(TFG_IOC_Ejemplo3, getNameSpaceIndex()),
                                    true, true, states);

    
    createObject("Ejemplo1", TFG_IOC_Ejemplo1, UaNodeId("Ejemplo1", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("Ejemplo2", TFG_IOC_Ejemplo2, UaNodeId("Ejemplo2", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
    createObject("Ejemplo3", TFG_IOC_Ejemplo3, UaNodeId("Ejemplo3", getNameSpaceIndex()), OpcUaId_ObjectsFolder);
       
    return UaStatus();  
}

// Se llama cuando se cierra. Los nodos se limpian automaticamente pero podemos poner otro tipo de código.
UaStatus MyNodeIOEventManager::beforeShutDown()
{
    std::cout << "Se cierra el servidor" << std::endl;
    return UaStatus();
}

OpcUa_Boolean MyNodeIOEventManager::beforeSetAttributeValue(
    Session *pSession, 
    UaNode *pNode, 
    OpcUa_Int32 attributeId,
    const UaDataValue &dataValue, 
    OpcUa_Boolean &checkWriteMask
) {
    
    UaVariable* pVariable = nullptr;

    if(m_pEPICSGateway == nullptr)
        return OpcUa_True;

    if((pNode != NULL) && (pNode->nodeClass() == OpcUa_NodeClass_Variable))
        pVariable = (UaVariable*) pNode;

    
    // IOC Variable
    if(m_pEPICSGateway->isMapped(pVariable->nodeId())){

        m_pEPICSGateway->enqueuePutTask(pVariable, dataValue);
        
        return OpcUa_False;

    }

    // Not IOC Variable
    return OpcUa_True;
    
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


std::vector<UaVariable*> MyNodeIOEventManager::getVariablesFromObjectType(OpcUa_UInt32 numericIdentifier){
    
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