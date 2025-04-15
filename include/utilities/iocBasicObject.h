/**
 * @file iocBasicObject.h
 * @brief Declaration of the IocBasicObject class.
 * 
 * This file defines the basic object used to replicate IOCs in the node manager of the OPC UA Server.
 * 
 * @author Pablo Del Río López
 * @date 2025-06-01
 */
#ifndef __IOCBASICOBJECT_H__
#define __IOCBASICOBJECT_H__

#include <uaobjecttypes.h>
#include <methodmanager.h>
#include <methodhandleuanode.h>

#include <myNodeIOEventManager.h>

/**
 * @class IocBasicObject
 * @brief Basic object to replicate IOC in OPC UA Server.
 * 
 * This class allows the creation of objects representing the IOC to be handled by the node manager.
 * Extends the class [UaObjectBase](https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classUaObjectBase.html)
 * Disables the copying constructor and the assignment operator to avoid misuses of this class.
 */
class IocBasicObject : public UaObjectBase
{
    UA_DISABLE_COPY(IocBasicObject);

private:
    /**
     * @brief A pointer to the node manager,
     * @see MyNodeIOEventManager
     */
    MyNodeIOEventManager* m_pNodeManager;

    /**
     * @brief Identifier for the ObjectType that defines this object.
     * An identifier to determinate the ObjectType ("Class in OOP") of this instance.
     */
    int m_typeId;

public:
    /**
     * @brief Constructor.
     * 
     * @param name Name of the new object.
     * @param newNodeId NodeId of the new object.
     * @param defaultLocaleId Default LocaleId for the node manager.
     * @param pNodeManager A pointer to the node manager.
     * @param typeId Identifier to determinate the ObjectType ("Class in OPP") of this instance.
     */
    IocBasicObject(
        const UaString & name,
        const UaNodeId & newNodeId,
        const UaString & defaultLocaleId,
        MyNodeIOEventManager* pNodeManager,
        const int typeId);

    /**
     * @brief Destructor.
     */
    virtual ~IocBasicObject(void);

    /**
     * 
     */
    UaStatus addVariable(const int typeIdVar);

    /**
     * @brief Determines the UaNodeId representing the ObjectType associated with this instance.
     * @return UaNodeId representing the ObjectType of this instance.
     */
    inline UaNodeId typeDefinitionId() const { return UaNodeId(m_typeId, m_pNodeManager->getNameSpaceIndex()); };

    // Implement UaObject interface
    OpcUa_Byte eventNotifier() const;


};

#endif