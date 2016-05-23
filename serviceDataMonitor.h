/*
 * Copyright (C) 2016 Chuck McParland All rights reserved.
 */

/* 
 * File:   serviceInstanceTemplateApp2.h
 * Author: mcp
 *
 * Created on January 24, 2016, 5:01 PM
 */

#ifndef SERVICEDATAMONITOR_H
#define SERVICEDATAMONITOR_H

namespace serviceDataMonitor {
using namespace pqxx;
using namespace serviceCommon;
    
#define CMD_QUEUE_LEN 32
#define DATA_QUEUE_LEN 16

class serviceDataMonitor : public serviceInstanceApi {
private:
    std::string serviceName;
    boost::thread* m_thread; // This thread runs this object
    /* Variable that indicates to stop and the mutex to synchronise access */
    bool m_mustStop;
    boost::mutex m_mustStopMutex;
    /* serviceDataMonitor methods */
    bool initService();
    void processCmd(SERVICE_COMMAND *);
    void processData(std::shared_ptr<serviceDataBuffer>);
    boost::mutex m_dataQueueMutex;
    shared_ptr<serviceDataBuffer> curDataBuf;
    shared_ptr<serviceDataBuffer> capturedDataBuf;
    shared_ptr<serviceDataBuffer> dataPtr;
    
public:
    serviceDataMonitor();
    ~serviceDataMonitor();
    void start();
    void stop();
    dataBufferOffer_t dataBufferOffer(const std::shared_ptr<serviceDataBuffer>&);
    void operator () ();
    void setServiceName(std::string s) { serviceName = s;};
    std::string getServiceName() { return serviceName;};
    
    dbConnection * dbConn;
};

}

#endif /* SERVICEDATAMONITOR_H */

