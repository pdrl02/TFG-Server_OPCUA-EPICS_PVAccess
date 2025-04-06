#ifndef __EPICSTOOPCUAGATEWAY_H__
#define __EPICSTOOPCUAGATEWAY_H__

#include <iostream>
#include <pvxs/client.h>
#include <pvxs/util.h>
#include <string>
#include <uanodeid.h>
#include <myNodeIOEventManager.h>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <variant>
#include <uabasenodes.h>
#include <uadatavalue.h>
#include <pvxs/nt.h>

using namespace pvxs;
using namespace pvxs::client;
using namespace std;

struct PutRequest {
    UaVariable * variable;
    UaDataValue value;

    PutRequest() = default;

    PutRequest(UaVariable * var, UaDataValue val)
    : variable(var), value(val) {}

};

using GatewayEvent = std::variant<shared_ptr<pvxs::client::Subscription>, PutRequest>;

struct PVMapping {
    string epicsName;
    UaNodeId nodeId;

    PVMapping() = default;

    PVMapping(string name, UaNodeId node) 
    : epicsName(name), nodeId(node) {}
};

class EPICStoOPCUAGateway {

private:
    
    MPMCFIFO<shared_ptr<GatewayEvent>> m_workQueue{100};

    Context m_pvxsContext;

    vector<shared_ptr<Subscription>> m_subcriptions;

    MyNodeIOEventManager * m_pNodeManager;

    unordered_map<string, PVMapping> m_pvMap_Name;

    map<UaNodeId, PVMapping> m_pvMap_UaNode;

    const int m_numThreads = 1;

    vector<thread> m_workerThreads;

    atomic<bool> m_running{false};

    void processQueue();

    UaVariant convertValueToVariant(const Value & value);

    Value convertUaDataValueToPvxsValue(const UaDataValue & dataValue);


public:
    EPICStoOPCUAGateway(MyNodeIOEventManager * pNodeManager);
    ~EPICStoOPCUAGateway();

    void start();

    void stop();

    void enqueuePutTask(const UaVariable * variable, const UaDataValue& value);

    bool addMapping(const string & name, const PVMapping & pvMapping);


    class GatewayHandler {
        private:
            EPICStoOPCUAGateway* m_self;

        public:
            explicit GatewayHandler(EPICStoOPCUAGateway* ptr) : m_self(ptr) {}

            void operator()(shared_ptr<Subscription> & subscription) const;

            void operator()(PutRequest & putRequest) const;
    };

};

#endif  // __EPICSTOOPCUAGATEWAY_H__
