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
#include "opcua_baseanalogtype.h"
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
        const OpcUa_Int16 value, 
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
     * @brief Event that is called before the value of an attributte of a Node is set. 
     * If the variable is registered in m_pEPICSGateway, never allows writing, but generates an EPICS put event that will
     * change the value of the attributte.
     * If the variable is not registered, allows writing.
     * 
     * @param pSession Interface of the Session context for the attribute write.
     * @param pNode Interface of the UaNode to update.
     * @param attributeId Attribute id indicating the attribute to set.
     * @param dataValue New value for the attribute.
     * @param checkWriteMask Flag indicating if the write mask of the node attribute or the access level for the value
     * attribute should be checked in UaNode::setAttributeValue.
     * @return OpcUa_Boolean 
     */
    OpcUa_Boolean beforeSetAttributeValue(
        Session * pSession, 
        UaNode * pNode, 
        OpcUa_Int32 attributeId, 
        const UaDataValue & dataValue,
        OpcUa_Boolean & checkWriteMask
    );	

    /**
     * @brief Get the instance declaration node of a variable for a numeric identifier.
     * 
     * @param numericIdentifier Numeric identifier of the node of the variable.
     * @return UaVariable* Pointer to the UaVariable or NULL if it do not exist.
     */
    UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);

    /**
     * @brief Get a vector with a pointer to every variable of an ObjectType.
     * This method is thread unsafe. To use it ensure that m_mutexNodes is blocked, and realease it
     * after this method.
     * 
     * @param numericIdentifier Numeric identifier for the ObjectType.
     * @return std::vector<UaVariable*> Vector with pointers to variables of the ObjectType.
     */
    std::vector<UaVariable*> getVariablesFromObjectType(OpcUa_UInt32 numericIdentifier);

    /**
     * @brief Create a AnalogVariableType node in the namespace of this node manager.
     * 
     * This method creates an AnalogVariableType node in the OPC UA address space using a template type `T`. 
     * The default type value for the variable is set based on the template type.
     * 
     * The template parameter `T` must be one of the following types:
     * - `OpcUa_Int32`
     * - `OpcUa_Int64`
     * - `OpcUa_Double`
     * 
     * @tparam T The type of the value to be set as the default value for the variable. It should be OpcUa_Int32,
     * OpcUa_Int64 o OpcUa_Double.
     * 
     * @param name Name of the AnalogVariableType.
     * @param value Default value of type T.
     * @param typeId Identifier used to create the UaNodeId of this variable.
     * @param sourceNode The parent node in the address space (source of the HasComponent reference).
     * @param writable Whether the variable is writable.
     * @param mandatory Whether the variable is mandatory for the instances of the sourceNode
     * @param EngineeringUnits Data describing engineering units.
     * @param EURange Data describing the range of values that this variable can take. 
     * @param InstrumentRange Data describing the limits of the instrument.
     * @return UaStatus with error code of the operation.
     */
    template<typename T>
    typename std::enable_if<
        std::is_same<T, OpcUa_Int32>::value ||
        std::is_same<T, OpcUa_Int64>::value ||
        std::is_same<T, OpcUa_Double>::value,
        UaStatus
    >::type
    createAnalogVariableType(
        const UaString & name, 
        const T value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaEUInformation & EngineeringUnits =  UaEUInformation(),
        const UaRange & EURange = UaRange(),
        const UaRange & InstrumentRange = UaRange()	
    ){
        UaVariant defaultValue;                                 // Union for all data types
        OpcUa::BaseAnalogType * pBaseAnalogType;                // Base type for analog data
        UaStatus result;
    
        // Decides the type
        if constexpr (std::is_same<T, OpcUa_Int32>::value)
            defaultValue.setInt32(value);
        else if constexpr (std::is_same<T, OpcUa_Int64>::value)
            defaultValue.setInt64(value);
        else if constexpr (std::is_same<T, OpcUa_Double>::value)
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

;
};



#endif // __MYNODEIOEVENTMANAGER_H__