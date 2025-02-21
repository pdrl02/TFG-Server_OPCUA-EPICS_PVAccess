#ifndef __MYNODEANDIOMANAGER_H__
#define __MYNODEANDIOMANAGER_H__

#include "nodemanagerbase.h"

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

#endif // __MYNODEANDIOMANAGER_H__