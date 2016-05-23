/*
 * Copyright (C) 2016 Chuck McParland All rights reserved.
 */

/* 
 * File:   serviceInstanceTemplateApp2.cpp
 * Author: mcp
 * 
 * Created on January 24, 2016, 5:01 PM
 */
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <set>
#include <pqxx/pqxx>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>
#include <boost/ref.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/dll/alias.hpp>
#include <boost/thread/condition.hpp>
#include <boost/any.hpp>

#include "dbConnection.h"
#include "serviceCommand.h"
#include "serviceDataBuffer.h"

#include "serviceCommandQueueTemplate.h"
#include "serviceInstanceApi.h"
#include "serviceStatus.h"
#include "upmuStructKeyValueListProtobuf.pb.h"
#include "serviceCommonStats.h"
#include "serviceSpecificStats.h" 


#include "serviceDataMonitor.h"

namespace serviceDataMonitor {
using namespace boost;
using namespace std;
using namespace serviceCommon;

serviceDataMonitor::serviceDataMonitor() {
    std::cout<< "Creating serviceDataMonitor" << std::endl;
    cmdQueue = new SynchronisedCommandQueue<SERVICE_COMMAND *>(CMD_QUEUE_LEN);
    curDataBuf = nullptr;
    capturedDataBuf = nullptr;
    m_thread=NULL;
    m_mustStop=false;
    serviceStatus = NOT_STARTED;
}

serviceDataMonitor::~serviceDataMonitor() {
    std::cout<< "serviceDataMonitor::~serviceDataQA" << std::endl;
    if (m_thread!=NULL) delete m_thread;
}

// Start the thread 
void serviceDataMonitor::start() {
    std::cout<< "serviceDataMonitor::start" << std::endl;// Create thread and start it with myself as argument.
    // Pass myself as reference since I don't want a copy
    m_mustStop = false;  
    /* start thread.  operator() () is the function executed */
    m_thread=new boost::thread(boost::ref(*this));
}
 
// Stop the thread
void serviceDataMonitor::stop() {
    std::cout<< "serviceDataQA::stop" << std::endl;
    // Signal the thread to stop (thread-safe)
    serviceStatus = STOPPING;
    m_mustStopMutex.lock();
    m_mustStop=true;
    m_mustStopMutex.unlock();
    // notify running thread
    wake_up.notify_one();
 
    // Wait for the thread to finish.
    if (m_thread!=NULL) m_thread->join();
}
 
dataBufferOffer_t serviceDataMonitor::dataBufferOffer(
    const std::shared_ptr<serviceDataBuffer>& offeredBuf) {    
    std::cout<< "serviceDataMonitor::dataBufferOffer" << std::endl;
   
    serviceCmnStatsMutex.lock();
    unsigned int * cnt = any_cast<unsigned int *>
        ((*serviceCmnStatsValues)[INDX_UINT_NUM_DATA_BUF_SEEN]);
    *cnt++;
    serviceCmnStatsMutex.unlock();
    
    /* copy shared_ptr for possible latching by request */
    m_dataQueueMutex.lock(); 
    if(curDataBuf != nullptr) {
        /* release currently held buffer */
        curDataBuf.reset();
    }
    /* grab new buffer */ 
    curDataBuf = offeredBuf;
    m_dataQueueMutex.unlock();
            
    serviceCmnStatsMutex.lock();
    cnt = any_cast<unsigned int *>
        ((*serviceCmnStatsValues)[INDX_UINT_NUM_DATA_BUF_ACCEPTED]);
    *cnt++;
    serviceCmnStatsMutex.unlock();
    
    return BUFFER_ACCEPTED;
}

// Thread function
void serviceDataMonitor::operator () () {
    std::cout<< "serviceDataMonitor::operator" << std::endl;
    
    bool localMustStop = false;
    SERVICE_COMMAND * cmd;
    int data;
    serviceStatus = RUNNING;
    
    
    if(initService()) {
        /* log error and exit*/
    }
    
    int testCnt = 0;
    try {
        while(!localMustStop) {
            testCnt++;
            /* always check for possible command */
            cmd = cmdQueue->Dequeue();
            if(cmd != 0) {
                processCmd(cmd);
            }
            /* sleep for a bit */
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // Get the "must stop" state (thread-safe)
            m_mustStopMutex.lock();
            localMustStop=m_mustStop;
            m_mustStopMutex.unlock();
        }
        // exiting function kills thread
        if(curDataBuf != nullptr) {
            /* release currently held buffer */
            curDataBuf.reset();
        }
        if(capturedDataBuf != nullptr) {
            /* release currently held buffer */
            capturedDataBuf.reset();
        }
        serviceStatus = NORMAL_STOP;
        std::cout<< "Normal stop serviceDataMonitor::operator" << std::endl;
        return;
    }
    catch (const std::exception &e) {
        serviceStatus = ERROR_STOP;
        std::cout<< "Error stop serviceDataMonitor::operator" << std::endl;
        return;
    }
};

void serviceDataMonitor::processCmd(SERVICE_COMMAND * cmd) {
    std::cout<< "serviceDataMonitor::processCmd" << std::endl;
    
}

void serviceDataMonitor::processData(std::shared_ptr<serviceDataBuffer> dataBuf) {
    
}

bool serviceDataMonitor::initService() {
    std::cout<< "serviceDataMonitor::initService" << std::endl;
    /* init service data structures */
    createServiceCommonStats(this);
    createServiceSpecificStats(this);
    /* init local values */
    dataPtr = nullptr;
    /* get a db connection for local database */
    dbConnection * dbConn = dbConnection::getInstance();
    if(dbConn->conn == NULL) {
        /* log an error and exit. */
        return false;
    }
    return true;
}



serviceDataMonitor plugin;
BOOST_DLL_AUTO_ALIAS(plugin)

}
