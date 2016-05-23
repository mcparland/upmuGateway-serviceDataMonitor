/*
 * Copyright (C) 2016 Chuck McParland All rights reserved.
 */
#include <mutex>
#include <map>
#include <boost/thread/condition.hpp>
#include <boost/any.hpp>

#include "serviceCommandQueueTemplate.h"
#include "serviceInstanceApi.h"

using namespace boost;
using namespace serviceCommon;

#include "serviceSpecificStats.h"

/* these need to be static */
/* label storage */
static std::string lbl_nonIntegEventBuf(LBL_NON_INTEG_EVENT_BUF);

/* value storage **/
static unsigned int val_nonIntegEventBuf = 0;

void createServiceSpecificStats(serviceInstanceApi * servApi) {
    (servApi->serviceSpecStatsMutex).lock();
    /* first the labels */
    std::map<unsigned int, std::string *> * lblMap = 
        new std::map<unsigned int, std::string *>();
    lblMap->insert(std::make_pair(INDX_UINT_NON_INTEG_EVENT_BUF, &lbl_nonIntegEventBuf));
    
    /* next the values */
    std::map<unsigned int, boost::any> * valMap = 
        new std::map<unsigned int, boost::any>();
    valMap->insert(std::make_pair(INDX_UINT_NON_INTEG_EVENT_BUF, &val_nonIntegEventBuf));
    
    /* init any values */
    val_nonIntegEventBuf = 0;
    servApi->serviceSpecStatsLabels = lblMap;
    servApi->serviceSpecStatsValues = valMap;
    (servApi->serviceSpecStatsMutex).unlock();
}

void destroyServiceSpecificStats(serviceInstanceApi * servApi) {
    (servApi->serviceSpecStatsMutex).lock();
    /* first the labels */
    delete servApi->serviceSpecStatsLabels;
    
    /* next the values */
    delete servApi->serviceSpecStatsValues; 
    
    servApi->serviceSpecStatsLabels = nullptr;
    servApi->serviceSpecStatsValues = nullptr;
    (servApi->serviceSpecStatsMutex).unlock();
}

void clearServiceSpecificStats(serviceInstanceApi * servApi) {
    (servApi->serviceSpecStatsMutex).lock();
    
    /* init any values */
    val_nonIntegEventBuf = 0;
    
    (servApi->serviceSpecStatsMutex).unlock();
}