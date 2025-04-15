/**
 * @file opcServer.cpp
 * @brief Implementation of the OpcServer class.
 * 
 * Contains the logic for server startup, initialization,
 * and endpoint configuration. Based on Unified Automation SDK sample code.
 * 
 * @author Pablo Del Río López
 * @date 2025-06-01
 * @copyright
 * This file includes portions of code © 2006–2024 Unified Automation GmbH,
 * used under the SLA 2.8 license: https://www.unified-automation.com/License/SLA/2.8/
 */

#include "opcServer.h"
#include "uamodule.h"
#include "uasession.h"

#ifndef UA_BUILD_DATE_ZONE
#define UA_BUILD_DATE_ZONE 1 // Must match UTC offset and daylight saving time at build date
#endif /* UA_BUILD_DATE_ZONE */

/** Construction. */
OpcServer::OpcServer()
{}

/** Destruction. */
OpcServer::~OpcServer()
{
    if(m_pGateway != nullptr)
        m_pGateway->stop();
    
    if ( isStarted() != OpcUa_False )
    {
        UaLocalizedText reason("en","Application shut down");
        stop(0, reason);
    }
}

UaStatus OpcServer::afterStartUp()
{
    UaStatus ret = UaServerApplication::afterStartUp();
    if ( ret.isGood() )
    {
        UaString        sRejectedCertificateDirectory;
        OpcUa_UInt32    nRejectedCertificatesCount;
        UaEndpointArray uaEndpointArray;
        getServerConfig()->getEndpointConfiguration(
            sRejectedCertificateDirectory,
            nRejectedCertificatesCount,
            uaEndpointArray);
        if ( uaEndpointArray.length() > 0 )
        {
            printf("***************************************************\n");
            printf(" Server opened endpoints for following URLs:\n");
            OpcUa_UInt32 idx;
            bool bError = false;
            for ( idx=0; idx<uaEndpointArray.length(); idx++ )
            {
                if ( uaEndpointArray[idx]->isOpened() )
                {
                    printf("     %s\n", uaEndpointArray[idx]->sEndpointUrl().toUtf8());
                }
                else
                {
                    bError = true;
                }
            }
            if ( bError )
            {
                printf("\n");
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                printf("!!!! The following endpoints URLs failed:\n");
                for ( idx=0; idx<uaEndpointArray.length(); idx++ )
                {
                    if ( uaEndpointArray[idx]->isOpened() == false )
                    {
                        printf("!!!! %s\n", uaEndpointArray[idx]->sEndpointUrl().toUtf8());
                    }
                }
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
                printf("\n");
            }
            printf("***************************************************\n");
        }
    }

    return ret;
}

OpcUa_DateTime OpcServer::getBuildDate() const
{
    static OpcUa_DateTime date;
    static const char szDate[] = __DATE__; /* "Mon DD YYYY" */
    static char szISO[] = "YYYY-MM-DDT" __TIME__ "Z";
    static const char* Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    char mon = 0;

    /* set year */
    szISO[0] = szDate[7];
    szISO[1] = szDate[8];
    szISO[2] = szDate[9];
    szISO[3] = szDate[10];

    /* set month */
    while ( (strncmp(Months[(int)mon], szDate, 3) != 0) && (mon < 11) )
    {
        mon++;
    }
    mon++;
    szISO[5] = '0' + mon / 10;
    szISO[6] = '0' + mon % 10;

    /* set day */
    szISO[8] = szDate[4];
    szISO[9] = szDate[5];

    /* convert to UA time */
    OpcUa_DateTime_GetDateTimeFromString(szISO, &date);

    /* correct time */
    UaDateTime buildDate(date);
    buildDate.addSecs(UA_BUILD_DATE_ZONE * 3600 * -1);

    return buildDate;
}

MyNodeIOEventManager* OpcServer::getMyNodeIOEventManager() { 
    return m_pMyNodeManager;
}

int OpcServer::setMyNodeManager(MyNodeIOEventManager * pMyNodeManager){
    int ret = -1;
    if(pMyNodeManager != nullptr){
        ret = this->addNodeManager(pMyNodeManager);
        m_pMyNodeManager = pMyNodeManager;
    }
    return ret;
}

void OpcServer::addEPICSGateway(EPICStoOPCUAGateway * gate) {
    m_pGateway = gate;
    getMyNodeIOEventManager()->setEPICSGateway(m_pGateway);
    m_pGateway->start();
}
