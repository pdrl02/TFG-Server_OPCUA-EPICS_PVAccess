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

using namespace pvxs;
using namespace pvxs::client;
using namespace std;


struct PVMapping {
    string epicsName;
    UaNodeId nodeId;

    PVMapping() = default;

    PVMapping(string name, UaNodeId node) 
    : epicsName(name), nodeId(node) {}
};

class EPICStoOPCUAGateway {

private:
    
    MPMCFIFO<shared_ptr<Subscription>> m_workQueue{100};

    Context m_pvxsContext;

    vector<shared_ptr<Subscription>> m_subcriptions;

    MyNodeIOEventManager * m_pNodeManager;

    unordered_map<string, PVMapping> m_pvMap;

    const int m_numThreads = 1;

    vector<thread> m_workerThreads;

    atomic<bool> m_running{false};

    void processQueue();

    UaVariant convertValueToVariant(const Value & value);


public:
    EPICStoOPCUAGateway(MyNodeIOEventManager * pNodeManager);
    ~EPICStoOPCUAGateway();

    void addMapping(const string & pvName, const UaNodeId & nodeId );

    void start();

    void stop();

};

#endif  // __EPICSTOOPCUAGATEWAY_H__
