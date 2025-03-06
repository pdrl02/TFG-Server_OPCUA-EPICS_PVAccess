#ifndef __IOCBASICOBJECT_H__
#define __IOCBASICOBJECT_H__

#include <uaobjecttypes.h>
#include <methodmanager.h>
#include <methodhandleuanode.h>

#include <myNodeIOEventManager.h>

class IocBasicObject :
    public UaObjectBase
{
    UA_DISABLE_COPY(IocBasicObject);

private:
    MyNodeIOEventManager* m_pNodeManager;
    int m_typeId;

public:
    IocBasicObject(
        const UaString & name,
        const UaNodeId & newNodeId,
        const UaString & defaultLocaleId,
        MyNodeIOEventManager* pNodeManager,
        const int typeId);

    virtual ~IocBasicObject(void);

    UaStatus addAnalogVariable(const int typeId);
    UaStatus addTwoStateVariable(const int typeId);
    UaStatus addMultiStateVariable(const int typeId);

    inline UaNodeId typeDefinitionId() const { return UaNodeId(m_typeId, m_pNodeManager->getNameSpaceIndex()); };

    // Implement UaObject interface
    OpcUa_Byte eventNotifier() const;

    inline virtual UaNodeId getTypeId() const { return UaNodeId(m_typeId, browseName().namespaceIndex()); };


};

#endif