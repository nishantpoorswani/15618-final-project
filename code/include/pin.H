/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

// Multiple inclusion is an error in pin sources, but ok for pintool
// writers
#if !defined(PIN_H)
#define PIN_H

#include <string>
#include <cstdarg>
#include <vector>
#include <cstdint>
extern "C"
{
#include "xed-interface.h"
#include "xed-iclass-enum.h"
#include "xed-category-enum.h"
}
#include "pin_version.h"

#include "types_marker.h"
#include "pin-errtype.h"

#include "util/intel-fp.hpp"
#include "atomic.hpp"
#include "gen/pinsync.hpp"

// BASE
// foundation
namespace LEVEL_BASE
{
#include "gen/types_foundation.PH"
#include "gen/types_base.PH"
#include "gen/message.PH"
#include "gen/util.PH"
#include "gen/knob.PH"
#include "gen/lock.PH"

#if defined(TARGET_IA32)
#include "gen/fpstate_ia32.PH"
#endif
#if defined(TARGET_IA32E)
#include "gen/fpstate_ia32e.PH"
#endif

#include "gen/reginfo_ia32.PH"
#include "gen/reg_ia32.PH"
#include "gen/reg_partial_ia32.PH"
#include "gen/base_ia32.PH"
#include "gen/tls.PH"
#include "gen/exception.PH"
#include "gen/child_process.PH"
#include "gen/sym_undecorate.PH"

#if defined(TARGET_LINUX)
#include "gen/sysfuncs_linux.PH"
#else
#error "no target OS defined"
#endif

} // namespace LEVEL_BASE

// CORE
namespace LEVEL_CORE
{
using namespace LEVEL_BASE;

#include "gen/types_core.PH"
#include "gen/val.PH"
#include "gen/sec.PH"
#include "gen/img.PH"
#include "gen/bbl.PH"
#include "gen/edg.PH"
#include "gen/ins.PH"
#include "gen/regset.PH"

#include "gen/ins_ia32.PH"
#include "gen/regset_ia32.PH"

#if defined(TARGET_LINUX) || defined(TARGET_MAC)
#include "gen/ins_syscall_ia32_unix.PH"
#else
#error "No target os"
#endif

#include "gen/ins_api_xed_ia32.PH"
#include "gen/ins_xed_ia32.PH"
#include "gen/operand_ia32.PH"

} // namespace LEVEL_CORE

// vmapi
namespace LEVEL_VM
{
using namespace LEVEL_CORE;

#include "gen/types_vmapi.PH"
#include "gen/ins_iarg.PH"

} // namespace LEVEL_VM

// pinclient

namespace LEVEL_PINCLIENT
{
// This includes the subset of vm types that pintools must access
using namespace LEVEL_VM;

#include "gen/pin_client.PH"
#include "gen/versions_client.PH"
#include "gen/pin_client_replay.PH"
#include "gen/thread_client.PH"
#include "gen/debugger_client.PH"
#include "gen/stopped_thread_client.PH"
#include "gen/deprecated_client.PH"
#include "gen/opin_client.PH"
#include "gen/tls_client.PH"
#include "gen/child_process_client.PH"
#include "gen/image.PH"
#include "gen/debug.PH"
#include "gen/internal_exception_client.PH"
#include "gen/probe_instrument.PH"

#if defined(TARGET_LINUX)
#include "gen/image_elf.PH"
#include "gen/symbol_linux.PH"
#include "gen/backtrace_u.PH"
#else
#error "Unknown OS"
#endif

} // namespace LEVEL_PINCLIENT

using namespace LEVEL_PINCLIENT;

#endif
