#include <iocBasicObject.h>
#include <iostream>
#include <opcua_analogitemtype.h>
#include <opcua_twostatediscretetype.h>
#include <opcua_multistatediscretetype.h>

IocBasicObject::IocBasicObject(
    const UaString & name, 
    const UaNodeId & newNodeId,
    const UaString & defaultLocaleId,
    MyNodeIOEventManager* pNodeManager,
    const int typeId) :
    UaObjectBase(name, newNodeId, defaultLocaleId),
    m_typeId(typeId), m_pNodeManager(pNodeManager) {

    std::cout << "Objeto " << name << " creado" << std::endl;
    
}

IocBasicObject::~IocBasicObject(void) {}

UaStatus IocBasicObject::addVariable(UaVariable * pInstanceDeclarationVar) { 
    
    UA_ASSERT(pInstanceDeclarationVar!=NULL);

    UaStatus result;

    // Use dynamic cast to determine the variable type
    // Analog Variable
    if (auto* pAnalogType = dynamic_cast<OpcUa::BaseAnalogType*>(pInstanceDeclarationVar)) {

        OpcUa::AnalogItemType* pAnalogVar = new OpcUa::AnalogItemType(
            this,
            pInstanceDeclarationVar,
            m_pNodeManager,
            m_pSharedMutex
        );
        pAnalogVar->setEngineeringUnits(pAnalogType->getEngineeringUnits());
        pAnalogVar->setEURange(pAnalogType->getEURange());
        pAnalogVar->setInstrumentRange(pAnalogType->getInstrumentRange());

        result = m_pNodeManager->addNodeAndReference(this, pAnalogVar, OpcUaId_HasComponent);
        
    }
    // Two State Variable
    else if (auto* pTwoStateType = dynamic_cast<OpcUa::TwoStateDiscreteType*>(pInstanceDeclarationVar)) {
        
        OpcUa::TwoStateDiscreteType* pTwoStateVar = new OpcUa::TwoStateDiscreteType(
            this,
            pInstanceDeclarationVar,
            m_pNodeManager,
            m_pSharedMutex
        );
        
        pTwoStateVar->setFalseState(pTwoStateType->getFalseState(NULL));
        pTwoStateVar->setTrueState(pTwoStateType->getTrueState(NULL));

        result = m_pNodeManager->addNodeAndReference(this, pTwoStateVar, OpcUaId_HasComponent);

    }
    // Multi State Variable
    else if (auto* pMultiStateType = dynamic_cast<OpcUa::MultiStateDiscreteType*>(pInstanceDeclarationVar)) {
        
        OpcUa::MultiStateDiscreteType * pMultiStateVar = new OpcUa::MultiStateDiscreteType(
            this,
            pInstanceDeclarationVar,
            m_pNodeManager,
            m_pSharedMutex
        );

        UaLocalizedTextArray enumStrings;
        pMultiStateType->getEnumStrings(enumStrings);
        pMultiStateVar->setEnumStrings(enumStrings);

        result = m_pNodeManager->addNodeAndReference(this, pMultiStateVar, OpcUaId_HasComponent);
    }
    // Error, unknown type
    else {

        std::cerr << "Error: Unknown variable type" << std::endl;
        result = OpcUa_BadInvalidArgument;

    }

    return result;
}

OpcUa_Byte IocBasicObject::eventNotifier() const { return Ua_EventNotifier_None; }
