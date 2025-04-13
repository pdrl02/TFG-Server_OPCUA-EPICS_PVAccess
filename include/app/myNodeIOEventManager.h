#ifndef __MYNODEIOEVENTMANAGER_H__
#define __MYNODEIOEVENTMANAGER_H__

#include "nodemanagerbase.h"
#include "uaeuinformation.h"
#include "uarange.h"
#include <memory>

class EPICStoOPCUAGateway;

class MyNodeIOEventManager : public NodeManagerBase
{
    // Desabilita el constructor de copia y el operador de asignación
    UA_DISABLE_COPY(MyNodeIOEventManager);


private:

    EPICStoOPCUAGateway * m_pEPICSGateway; 

    // Método para crear crear los TypeNode en el espacio de direcciones
    UaStatus createTypeNodes();
    
public:
    MyNodeIOEventManager();
    virtual ~MyNodeIOEventManager();

    UaStatus createObjectType(const UaString & name, const int typeId, OpcUa_Boolean abstract, const UaNodeId & sourceNode);

    UaStatus createAnalogVariableType(
        const UaString & name, 
        const OpcUa_Double value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaEUInformation & EngineeringUnits =  UaEUInformation(),
        const UaRange & EURange = UaRange(),
        const UaRange & InstrumentRange = UaRange()	);

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

    UaStatus createMultiStatateVariableType(
        const UaString & name, 
        const OpcUa_Int64 value, 
        const int typeId, 
        const UaNodeId & sourceNode,
        const bool writable = false,
        const bool mandatory = true,
        const UaLocalizedTextArray & EnumStrings = UaLocalizedTextArray()
    );
    UaStatus createObject(
        const UaString & objectName,         // Name of the object
        const int typeId,                   // Type of the object
        const UaNodeId & objectId,           // NodeId of the new object
        const UaNodeId & parentNodeId               // Node of the parent;
    );

    UaStatus updateVariable(UaNodeId & nodeId, UaVariant & variant);

    void setEPICSGateway(EPICStoOPCUAGateway * pEPICSGateway);

    // NodeManagerUaNode implementation
    virtual UaStatus   afterStartUp();
    virtual UaStatus   beforeShutDown();

    // IOManagerUaNode implementation
    virtual UaStatus readValues(const UaVariableArray &arrUaVariables, UaDataValueArray &arrDataValues);
    virtual UaStatus writeValues(const UaVariableArray &arrUaVariables, const PDataValueArray &arrpDataValues, UaStatusCodeArray &arrStatusCodes);

    virtual void afterSetAttributeValue(Session * pSession, UaNode * pNode, OpcUa_Int32 attributeId, const UaDataValue & dataValue);
    
    // EventManagerUaNode implementation
    virtual UaStatus OnAcknowledge(const ServiceContext& serviceContext, OpcUa::AcknowledgeableConditionType* pCondition,
                                    const UaByteString& EventId, const UaLocalizedText& Comment);
    

    // Métodos para obtener los nodos de declaración de instancia
    UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);
    std::vector<UaVariable*> getInstanceDeclarationVariableArray(OpcUa_UInt32 numericIdentifier);

;
};



#endif // __MYNODEIOEVENTMANAGER_H__