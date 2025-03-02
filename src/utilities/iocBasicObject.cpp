#include <iocBasicObject.h>
#include <iostream>
#include <opcua_analogitemtype.h>

IocBasicObject::IocBasicObject(
    const UaString & name, 
    const UaNodeId & newNodeId,
    const UaString & defaultLocaleId,
    MyNodeIOEventManager* pNodeManager,
    const int typeId) :
    UaObjectBase(name, newNodeId, defaultLocaleId),
    m_typeId(typeId)
    {

    std::cout << "Objeto creado" << std::endl;


    // UaVariable * pInstanceDeclaration;
    // UaReferenceLists* pReferences;

    // pInstanceDeclaration = pNodeManager->getInstanceDeclarationVariable(typeId);
    
}

IocBasicObject::~IocBasicObject(void) {}

UaStatus IocBasicObject::addAnalogVariable(
    const int typeId,
    MyNodeIOEventManager* pNodeManager) {
    
    UaVariable* pInstanceDeclaration;
    OpcUa::AnalogItemType* pAnalogItem;

    pInstanceDeclaration = pNodeManager->getInstanceDeclarationVariable(typeId);
    UA_ASSERT(pInstanceDeclaration!=NULL);
    pAnalogItem = new OpcUa::AnalogItemType(
        this,
        pInstanceDeclaration,
        pNodeManager,
        m_pSharedMutex
    );
    return pNodeManager->addNodeAndReference(this, pAnalogItem, OpcUaId_HasComponent);

}

OpcUa_Byte IocBasicObject::eventNotifier() const { return OpcUa_Byte(); }
