/*
 * mod_compare - compares apache requests
 *
 * Copyright (C) 2013 Orange
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <apr_pools.h>
#include <apr_hooks.h>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <httpd.h>
#include <http_config.h>
#include <http_request.h>
#include <http_protocol.h>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <list>
#include <fstream>
#include <ios>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <unordered_map>
#include <boost/regex.hpp>
#include <tuple>

#include "Log.hh"
#include "RequestInfo.hh"
#include "RequestProcessor.hh"

#include <libws_diff/stringCompare.hh>
#include <libws_diff/mapCompare.hh>

extern module AP_DECLARE_DATA migrate_module;

namespace MigrateModule {

extern std::ofstream gFile;
extern const char* gFilePath;
extern bool gWriteInFile;
extern std::string gLogFacility;

boost::interprocess::named_mutex &getGlobalMutex();


/**
 * A class that holds the configuration specific to the location
 */
class MigrateConf {
public:
    struct MigrateEnv {
        std::string mVarName;
        boost::regex mMatchRegex;
        std::string mSetValue;
        ApplicationScope::eApplicationScope mApplicationScope;
    };

    /// MigrateEnv directive elements list
    //std::list<MigrateEnv> mEnvList;

    char* mDirName;

    ApplicationScope::eApplicationScope mCurrentApplicationScope;

    /// Map with Location as key and a list of MigrateEnv structure as value
    std::unordered_map<std::string, std::list<MigrateEnv>> mEnvLists;
    /// Map with Location as key and a list of tuple consisting of (field, regex filter, scope)
    std::unordered_map<std::string, std::list<std::tuple<std::string, std::string, ApplicationScope::eApplicationScope>>> mInputFilters;

    MigrateConf() : mDirName(NULL),mCurrentApplicationScope(ApplicationScope::ALL) {}
};

///**
// * @brief allocate a pointer to a string which will hold the path for the dir config if mod_dup is active on it
// * @param pPool the apache pool on which to allocate data
// * @param pDirName the directory name for which to create data
// * @return a void pointer to newly allocated object
// */
//void *
//createDirConfig(apr_pool_t *pPool, char *pDirName);
//
///**
// * @brief Initialize logging post-config
// * @param pPool the apache pool
// * @param pServer the corresponding server record
// * @return Always OK
// */
//int
//postConfig(apr_pool_t * pPool, apr_pool_t * pLog, apr_pool_t * pTemp, server_rec * pServer);
//
///**
// * @brief Clean up before the child exits
// */
//apr_status_t
//cleanUp(void *);

/*
 * Read the request body ans stores it in a RequestInfo object in the request context
 * Enrich the request context for mod_rewrite
 */
int translateHook(request_rec *r);

/**
 * @brief register hooks in apache
 * @param pPool the apache pool
 */
void registerHooks(apr_pool_t *pPool);

/**
 * @brief the input filter callback
 */
apr_status_t inputFilterHandler(ap_filter_t *pFilter, apr_bucket_brigade *pBrigade, ap_input_mode_t pMode, apr_read_type_e pBlock, apr_off_t pReadbytes);

/**
 * @brief the source input filter callback
 * This filter is placed first in the chain and serves the body stored in a RequestInfo object in the request context
 * to the other filters
 */
apr_status_t inputFilterBody2Brigade(ap_filter_t *pF, apr_bucket_brigade *pB, ap_input_mode_t pMode, apr_read_type_e pBlock, apr_off_t pReadbytes);

///** @brief the output filter callback
// * Plugged only in REQUEST_WITH_ANSWER mode
// */
//apr_status_t
//outputFilterHandler(ap_filter_t *pFilter, apr_bucket_brigade *pBrigade);
//
//apr_status_t
//outputFilterHandler2(ap_filter_t *pFilter, apr_bucket_brigade *pBrigade);
//
///**
// * @brief Set the list of errors to ignore in the comparison
// * @param pParams miscellaneous data
// * @param pCfg user data for the directory/location
// * @param pListType the type of list (Header or Body)
// * @param pValue the value to insert in the list
// * @return NULL if parameters are valid, otherwise a string describing the error
// */
//const char* setHeaderList(cmd_parms* pParams, void* pCfg, const char* pListType, const char* pAffectedKey, const char* pValue);
//
///**
// * @brief Set the list of errors who stop the comparison
// * @param pParams miscellaneous data
// * @param pCfg user data for the directory/location
// * @param pListType the type of list (Header or Body)
// * @param pValue the value to insert in the list
// * @return NULL if parameters are valid, otherwise a string describing the error
// */
//const char* setBodyList(cmd_parms* pParams, void* pCfg, const char* pListType, const char* pValue);
//
//void
//printRequest(request_rec *pRequest, std::string pBody);
//
//void writeCassandraDiff(const std::string &pUniqueID, std::stringstream &diffStr);
//
//void writeSerializedRequest(const DupModule::RequestInfo& req);
//
//size_t getLength(const std::string pString, const size_t pFirst );
//
//apr_status_t deserializeBody(DupModule::RequestInfo &pReqInfo);
//apr_status_t deserializeHeader(DupModule::RequestInfo &pReqInfo,const std::string& header);
//
//void childInit(apr_pool_t *pPool, server_rec *pServer);
//
//void writeInFacility(std::string pDiffLog);
//
//void writeDifferences(const DupModule::RequestInfo &pReqInfo,const std::string& myDiffHeader , const std::string& myDiffBody, double time);
//
//void map2string(const std::map< std::string, std::string> &pMap, std::string &pString);
//
//int iterateOverHeadersCallBack(void *d, const char *key, const char *value);
//
//apr_status_t closeLogFile(void *);
//
//apr_status_t openLogFile(const char* filepath,std::ios_base::openmode mode=std::ios_base::out);
//
//const char* setFilePath(cmd_parms* pParams, void* pCfg, const char* pPath);
//
//const char* setDisableLibwsdiff(cmd_parms* pParams, void* pCfg, const char* pValue);
//
//const char* setLogFacility(cmd_parms* pParams, void* pCfg, const char* pValue);
//
//bool checkCassandraDiff(const std::string &pUniqueID);

/*
 * Method that calls the destructor of an object which type is templated
 */
template <class T>
apr_status_t
cleaner(void *self) {
    if (self) {
        T *elt = reinterpret_cast<T *>(self);
        assert(elt);
        elt->~T();
    }
    return 0;
}

}

