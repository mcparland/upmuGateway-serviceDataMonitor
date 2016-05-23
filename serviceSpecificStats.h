/*
 * Copyright (C) 2016 Chuck McParland All rights reserved.
 */

/* 
 * File:   serviceCommonStats.h
 * Author: mcp
 *
 * Created on February 6, 2016, 11:12 AM
 */

#ifndef SERVICESPECIFICSTATS_H
#define SERVICESPECIFICSTATS_H

#define INDX_UINT_NON_INTEG_EVENT_BUF 0

#define LBL_NON_INTEG_EVENT_BUF "Non_Integral_Event_Data_Buffers"

void createServiceSpecificStats(serviceCommon::serviceInstanceApi * servApi);

void destroyServiceSpecificStats(serviceCommon::serviceInstanceApi * servApi);

void clearServiceSpecificStats(serviceCommon::serviceInstanceApi * servApi);

#endif /* SERVICESPECIFICSTATS_H */

