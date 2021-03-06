// Copyright (c) 2018 Baidu, Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "common.h"
#include "region.h"

namespace baikaldb {

struct DMLClosure : public braft::Closure {
    virtual void Run();

    brpc::Controller* cntl = nullptr;
    int op_type;
    pb::StoreRes* response = nullptr;
    google::protobuf::Closure* done = nullptr;
    Region* region = nullptr;
    SmartTransaction transaction = nullptr;
    TimeCost cost;
    std::string remote_side;
};

struct AddPeerClosure : public braft::Closure {
    AddPeerClosure(BthreadCond& cond) : cond(cond) {};
    virtual void Run(); 
    Region* region;
    std::string new_instance;
    TimeCost cost;
    google::protobuf::Closure* done = nullptr;
    pb::StoreRes* response = nullptr;
    BthreadCond& cond;
};

struct SplitClosure : public braft::Closure {
    virtual void Run();
    std::function<void()> next_step;
    Region* region;
    std::string new_instance;
    int64_t split_region_id;
    std::string step_message;
    pb::OpType op_type;
    int ret = 0;
    TimeCost cost;
};

struct ConvertToSyncClosure : public braft::Closure {
    ConvertToSyncClosure(BthreadCond& _sync_sign) : sync_sign(_sync_sign) {};
    virtual void Run();
    BthreadCond& sync_sign;
    TimeCost cost;
};

struct SnapshotClosure : public braft::Closure {
    virtual void Run() {
        if (!status().ok()) {
            DB_WARNING("region_id: %ld  status:%s, snapshot failed.",
                        region->get_region_id(), status().error_cstr());
        }
        cond.decrease_signal();
        delete this;
    }
    SnapshotClosure(BthreadCond& cond, Region* reg) : cond(cond), region(reg) {}
    BthreadCond& cond;
    Region* region = nullptr;
    int ret = 0;
    //int retry = 0;
};
} // end of namespace
