/**
 * @file myNodeIOEventManager.h
 * @brief Declaration of the MyNodeIOEventManager class.
 * 
 * This file defines the custom node manager used in the server of this project.
 * It extends NodeManagerBase class to implement specific OPC UA node creation and value updates.
 * It do not implements specific behaviour for events.
 * It is designed to allow integration with EPICS.
 * 
 * @author Pablo Del Río López
 * @date 2025-06-01
 */
#ifndef __MYNODEIOEVENTMANAGER_H__
#define __MYNODEIOEVENTMANAGER_H__

#include "nodemanagerbase.h"
#include "uaeuinformation.h"
#include "uarange.h"
#include <memory>

class EPICStoOPCUAGateway;

/**
 * @class MyNodeIOEventManager
 * @brief Custon Node Manager for handling variable creation and I/O operations.
 * 
 * Extends the class [NodeManagerBase](https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classNodeManagerBase.html)
 * to handle:
 * - Creation of custom ObjectTypes, Variables and their instances.
 * - Integration with an EPICStoOPCUAGateway for data exchange.
 * - Update values in external EPICS IOCs.
 * - Receive and apply values update from external EPICS IOCs.
 * 
 * This class disables the copying constructor and the assignment operator to avoid misuses of this class.
 * 
 */
class MyNodeIOEventManager : public NodeManagerBase
{
    
    UA_DISABLE_COPY(MyNodeIOEventManager);


private:

    /**
     * @brief Pointer to the EPICS-OPC_UA gateway.
     * 
     */
    EPICStoOPCUAGateway * m_pEPICSGateway; 
    
public:
    /**
     * @brief Construct a new MyNodeIOEventManager object.
     * 
     */
    MyNodeIOEventManager();

    /**
     * @brief Destructor of MyNodeIOEventManager object.
     * 
     */
    virtual ~MyNodeIOEventManager();

    /**
     * @brief Create a custom ObjectType node in the namespace of this node manager. 
     * 
     * @param name Name of the custom ObjectType.
     * @param typeId Identifier used to create the UaNodeId of this ObjectType.
     * @param abstract Boolean that determines whether the ObjectType is abstract, i.e. can be instantiated.
     * @param sourceNode Node from which this type derives.
     * @return UaStatus with error code of the operation.
     */
    UaStatus createObjectType(
        const UaString & name,
        const int typeId, 
        OpcUa_Boolean abstract,
        const UaNodeId & sourceNode
    );

    /**
     * @brief Create a AnalogVariableType node in the namespace of this node manager.
     * 
     * @param name Name of the AnalogVariableType.
     * @param value Default value.
     * @param typeId Identifier used to create the UaNodeId of this variable.
     * @param sourceNode The parent node in the address space (source of the HasComponent reference).
     * @param writable Whether the variable is writable.
     * @param mandatory Whether the variable is mandatory for the instances of the sourceNode
     * @param EngineeringUnits Data describing engineering units.
     * @param EURange Data describing the range of values that this variable can take. 
     * @param InstrumentRange Data describing the limits of the instrument.
     * @return UaStatus with error code of the operation.
     */
    UaStatus createAnalogVariableType(
        const UaString & name, 
        const OpcUa_Double value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaEUInformation & EngineeringUnits =  UaEUInformation(),
        const UaRange & EURange = UaRange(),
        const UaRange & InstrumentRange = UaRange()	
    );

    /**
     * @brief Create a TwoStateVariableType node in the namespace of this node manager.
     * 
     * @param name Name of the TwoStateVariableType.
     * @param value Default value.
     * @param typeId Identifier used to create the UaNodeId of this variable.
     * @param sourceNode The parent node in the address space (source of the HasComponent reference).
     * @param writable Whether the variable is writable.
     * @param mandatory Whether the variable is mandatory for the instances of the sourceNode.
     * @param falseText String to be associated with this variable when it is FALSE.
     * @param trueText String to be associated with this variable when it is TRUE.
     * @return UaStatus with error code of the operation. 
     */
    UaStatus createTwoStateVariableType(
        const UaString & name, 
        const OpcUa_Boolean value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaLocalizedText & falseText = UaLocalizedText("en", "False"),
        const UaLocalizedText & trueText = UaLocalizedText("en", "True")
    );

    /**
     * @brief Create a MultiStatateVariableType node in the namespace of this node manager.
     * 
     * @param name Name of the MultiStateVariableType.
     * @param value Default value.
     * @param typeId Identifier used to create the UaNodeId of this variable.
     * @param sourceNode The parent node in the address space (source of the HasComponent reference).
     * @param writable Whether the variable is writable.
     * @param mandatory Whether the variable is mandatory for the instances of the sourceNode.
     * @param EnumStrings Strings to be associated with the values of the variable.
     * @return UaStatus with error code of the operation. 
     */
    UaStatus createMultiStatateVariableType(
        const UaString & name, 
        const OpcUa_Int64 value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaLocalizedTextArray & EnumStrings = UaLocalizedTextArray()
    );

    /**
     * @brief Create a Object that "inherits" from the ObjectType that is definied by the typeId.
     * It would be as if in OOP we created an instance of a class that is definied by the typeId.
     * 
     * @param objectName Name of the object.
     * @param typeId ObjectType of this object. In OOP, the class.
     * @param objectId UaNodeId of the new object.
     * @param parentNodeId The parent node in the address space (source of the Organizes reference).
     * @return UaStatus with error code of the operation. 
     */
    UaStatus createObject(
        const UaString & objectName,         // Name of the object
        const int typeId,                   // Type of the object
        const UaNodeId & objectId,           // NodeId of the new object
        const UaNodeId & parentNodeId               // Node of the parent;
    );

    /**
     * @brief Update a variable node value.
     * 
     * @param nodeId UaNodeId of the variable to be updated.
     * @param variant Value to update with.
     * @return UaStatus with error code of the operation. 
     *      Return OpcUa_BadNodeIdUnknown if the nodeId do not exist.
     *      Return OpcUa_BadNodeIdRejected if the nodeId is not a variable. 
     */
    UaStatus updateVariable(UaNodeId & nodeId, UaVariant & variant);

    /**
     * @brief Set pointer to EPICS-to-OPCUA gateway,
     * 
     * @param pEPICSGateway Pointer to EPICStoOPCUAGateway instance.
     */
    void setEPICSGateway(EPICStoOPCUAGateway * pEPICSGateway);

    // NodeManagerUaNode implementation https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classNodeManagerUaNode.html

    /**
     * @brief Method that is called to finish the startup of the node manager.
     * 
     * @return UaStatus with error code of the operation. 
     */
    virtual UaStatus   afterStartUp();

    /**
     * @brief Method that is called to start the shutdown of the node manager.
     * 
     * @return UaStatus with error code of the operation. 
     */
    virtual UaStatus   beforeShutDown();

    // IOManagerUaNode implementation https://documentation.unified-automation.com/uasdkcpp/1.8.6/html/classIOManagerUaNode.html

    /**
     * @brief 
     * 
     * @param pSession 
     * @param pNode 
     * @param attributeId 
     * @param dataValue 
     * @param checkWriteMask 
     * @return OpcUa_Boolean 
     */
    OpcUa_Boolean beforeSetAttributeValue(
        Session * pSession, 
        UaNode * pNode, 
        OpcUa_Int32 attributeId, 
        const UaDataValue & dataValue,
        OpcUa_Boolean & checkWriteMask
    );	
    
    // EventManagerUaNode implementation
    virtual UaStatus OnAcknowledge(const ServiceContext& serviceContext, OpcUa::AcknowledgeableConditionType* pCondition,
                                    const UaByteString& EventId, const UaLocalizedText& Comment);
    

    // Métodos para obtener los nodos de declaración de instancia
    UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);
    std::vector<UaVariable*> getInstanceDeclarationVariableArray(OpcUa_UInt32 numericIdentifier);

;
};



#endif // __MYNODEIOEVENTMANAGER_H__