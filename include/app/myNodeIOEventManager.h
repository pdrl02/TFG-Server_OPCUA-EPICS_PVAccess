#ifndef __MYNODEIOEVENTMANAGER_H__
#define __MYNODEIOEVENTMANAGER_H__

#include "nodemanagerbase.h"
#include "uaeuinformation.h"
#include "uarange.h"

class MyNodeIOEventManager : public NodeManagerBase
{
    // Desabilita el constructor de copia y el operador de asignación
    UA_DISABLE_COPY(MyNodeIOEventManager);


private:
    // Método para crear crear los TypeNode en el espacio de direcciones
    UaStatus createTypeNodes();
    
public:
    MyNodeIOEventManager();
    virtual ~MyNodeIOEventManager();

    UaStatus createTypeNode(const UaString & name, const int typeID, OpcUa_Boolean abstract, const UaNodeId & sourceNode);

    UaStatus createAnalogVariable(
        const UaString & name, 
        const OpcUa_Double value, 
        const int typeID, 
        const UaNodeId & sourceNode, 
        const UaEUInformation & EngineeringUnits =  UaEUInformation(),
        const UaRange & EURange = UaRange(),
        const UaRange & InstrumentRange = UaRange()	);
    
    UaStatus createObject();

    // NodeManagerUaNode implementation
    virtual UaStatus   afterStartUp();
    virtual UaStatus   beforeShutDown();

    // IOManagerUaNode implementation
    virtual UaStatus readValues(const UaVariableArray &arrUaVariables, UaDataValueArray &arrDataValues);
    virtual UaStatus writeValues(const UaVariableArray &arrUaVariables, const PDataValueArray &arrpDataValues, UaStatusCodeArray &arrStatusCodes);
    
    // EventManagerUaNode implementation
    virtual UaStatus OnAcknowledge(const ServiceContext& serviceContext, OpcUa::AcknowledgeableConditionType* pCondition,
                                    const UaByteString& EventId, const UaLocalizedText& Comment);
    

    // Método para obtener los nodos de declaración de instancia
    UaVariable* getInstanceDeclarationVariable(OpcUa_UInt32 numericIdentifier);

;
};



#endif // __MYNODEIOEVENTMANAGER_H__