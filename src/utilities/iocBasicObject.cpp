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

    std::cout << "Objeto creado" << std::endl;
    
}

IocBasicObject::~IocBasicObject(void) {}

UaStatus IocBasicObject::addAnalogVariable( const int typeIdVar) {
    
    UaVariable* pInstanceDeclaration;
    OpcUa::AnalogItemType* pAnalogVar;

    pInstanceDeclaration = m_pNodeManager->getInstanceDeclarationVariable(typeIdVar);
    UA_ASSERT(pInstanceDeclaration!=NULL);
    pAnalogVar = new OpcUa::AnalogItemType(
        this,
        pInstanceDeclaration,
        m_pNodeManager,
        m_pSharedMutex
    );

    OpcUa::BaseAnalogType * pAnalogType = dynamic_cast<OpcUa::BaseAnalogType*>(pInstanceDeclaration);
    pAnalogVar->setEngineeringUnits(pAnalogType->getEngineeringUnits());
    pAnalogVar->setEURange(pAnalogType->getEURange());
    pAnalogVar->setInstrumentRange(pAnalogType->getInstrumentRange());
    

    return m_pNodeManager->addNodeAndReference(this, pAnalogVar, OpcUaId_HasComponent);
    
}

UaStatus IocBasicObject::addTwoStateVariable(const int typeIdVar) {
    
    UaVariable* pInstanceDeclaration;
    OpcUa::TwoStateDiscreteType* pTwoStateVar;

    pInstanceDeclaration = m_pNodeManager->getInstanceDeclarationVariable(typeIdVar);
    UA_ASSERT(pInstanceDeclaration!=NULL);
    pTwoStateVar = new OpcUa::TwoStateDiscreteType(
        this,
        pInstanceDeclaration,
        m_pNodeManager,
        m_pSharedMutex
    );

    OpcUa::TwoStateDiscreteType * pTwoStateType = dynamic_cast<OpcUa::TwoStateDiscreteType*>(pInstanceDeclaration);
    pTwoStateVar->setFalseState(pTwoStateType->getFalseState(NULL));
    pTwoStateVar->setTrueState(pTwoStateType->getTrueState(NULL));

    return m_pNodeManager->addNodeAndReference(this, pTwoStateVar, OpcUaId_HasComponent);
}

UaStatus IocBasicObject::addMultiStateVariable(const int typeIdVar) {
    
    UaVariable* pInstanceDeclaration;
    OpcUa::MultiStateDiscreteType* pMultiStateVar;

    pInstanceDeclaration = m_pNodeManager->getInstanceDeclarationVariable(typeIdVar);
    UA_ASSERT(pInstanceDeclaration!=NULL);
    pMultiStateVar = new OpcUa::MultiStateDiscreteType(
        this,
        pInstanceDeclaration,
        m_pNodeManager,
        m_pSharedMutex
    );

    OpcUa::MultiStateDiscreteType * pMultiStateType = dynamic_cast<OpcUa::MultiStateDiscreteType*>(pInstanceDeclaration);
    UaLocalizedTextArray enumStrings;
    pMultiStateType->getEnumStrings(enumStrings);
    pMultiStateVar->setEnumStrings(enumStrings);

    return m_pNodeManager->addNodeAndReference(this, pMultiStateVar, OpcUaId_HasComponent);
}

OpcUa_Byte IocBasicObject::eventNotifier() const { return OpcUa_Byte(); }
