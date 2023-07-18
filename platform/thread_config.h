/* Copyright 2021 Threadpool Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef THREAD_CONFIG_H__
#define THREAD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <signal.h>
#include <sched.h>

struct config {
  int policy_;
  int cancel_state_;
  int cancel_type_;
  cpu_set_t cpuset_;
  struct sched_param sched_;
  pthread_attr_t attribute_;
  sigset_t blockedset_;
};

struct config* config_get_thread_config();
void config_remove_thread_config(struct config* conf);
void config_init(struct config* c);
int config_apply(pthread_t hndl, const struct config* conf);
int config_retrieve(pthread_t hndl, struct config* conf);
struct config* config_set_priority(struct config* conf, int priority);
struct config* config_set_policy(struct config* conf, int policy);
struct config* config_set_affinity(struct config* conf, int* cpuid, int num);
struct config* config_unblock_signals(struct config* conf, int* sig, int num);
const sigset_t* config_get_sigset(struct config* conf);
int config_cancel_thread(pthread_t hndl);

#ifdef __cplusplus
}
#endif

#endif // THREAD_CONFIG_H__
