/*
 * BLELog.h
 *
 *  Created on: Aug 28, 2020
 *      Author: Hongtai Liu
 *       brief: a simple log system
 */

#ifndef COMPONENTS_CPP_UTILS_BLELOG_H_
#define COMPONENTS_CPP_UTILS_BLELOG_H_

#include "rpc_unified_log.h"

#ifdef ENABLE_RPC_VERBOSE
#define log_v RPC_VERBOSE
#endif
#ifdef ENABLE_RPC_DEBUG
#define log_d RPC_DEBUG
#endif
#ifdef ENABLE_RPC_WARN
#define log_w RPC_WARN
#endif
#ifdef ENABLE_RPC_INFO
#define log_i RPC_INFO
#endif
#ifdef ENABLE_RPC_ERROR
#define log_e RPC_ERROR
#endif

#endif /**/