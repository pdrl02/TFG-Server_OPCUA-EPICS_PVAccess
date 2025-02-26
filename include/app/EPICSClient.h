#ifndef __EPICSCLIENT_H__
#define __EPICSCLIENT_H__

#include <iostream>
#include <pv/pvaClient.h>


using namespace epics::pvaClient;


class EPICSClient {

private:
    PvaClient::shared_pointer client;

public:
    EPICSClient();
    ~EPICSClient();


};

#endif  // __EPICSCLIENT_H__
