/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef QL_TIPC_SMCALL_H_
#define QL_TIPC_SMCALL_H_

#define SMC_NUM_ENTITIES	64
#define SMC_NUM_ARGS		4
#define SMC_NUM_PARAMS		(SMC_NUM_ARGS - 1)

#define SMC_IS_FASTCALL(smc_nr)	((smc_nr) & 0x80000000)
#define SMC_IS_SMC64(smc_nr)	((smc_nr) & 0x40000000)
#define SMC_ENTITY(smc_nr)	(((smc_nr) & 0x3F000000) >> 24)
#define SMC_FUNCTION(smc_nr)	((smc_nr) & 0x0000FFFF)

#define SMC_NR(entity, fn, fastcall, smc64) ((((fastcall) & 0x1) << 31) | \
					     (((smc64) & 0x1) << 30) | \
					     (((entity) & 0x3F) << 24) | \
					     ((fn) & 0xFFFF) \
					    )

#define SMC_FASTCALL_NR(entity, fn)	SMC_NR((entity), (fn), 1, 0)
#define SMC_STDCALL_NR(entity, fn)	SMC_NR((entity), (fn), 0, 0)
#define SMC_FASTCALL64_NR(entity, fn)	SMC_NR((entity), (fn), 1, 1)
#define SMC_STDCALL64_NR(entity, fn)	SMC_NR((entity), (fn), 0, 1)

#define	SMC_ENTITY_ARCH			0	/* ARM Architecture calls */
#define	SMC_ENTITY_CPU			1	/* CPU Service calls */
#define	SMC_ENTITY_SIP			2	/* SIP Service calls */
#define	SMC_ENTITY_OEM			3	/* OEM Service calls */
#define	SMC_ENTITY_STD			4	/* Standard Service calls */
#define	SMC_ENTITY_RESERVED		5	/* Reserved for future use */
#define	SMC_ENTITY_TRUSTED_APP		48	/* Trusted Application calls */
#define	SMC_ENTITY_TRUSTED_OS		50	/* Trusted OS calls */
#define	SMC_ENTITY_LOGGING		51	/* Used for secure -> nonsecure logging */
#define	SMC_ENTITY_SECURE_MONITOR	60	/* Trusted OS calls internal to secure monitor */

/* FC = Fast call, SC = Standard call */
#define SMC_SC_RESTART_LAST	SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 0)
#define SMC_SC_LOCKED_NOP	SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 1)

/**
 * SMC_SC_RESTART_FIQ - Re-enter trusty after it was interrupted by an fiq
 *
 * No arguments, no return value.
 *
 * Re-enter trusty after returning to ns to process an fiq. Must be called iff
 * trusty returns SM_ERR_FIQ_INTERRUPTED.
 *
 * Enable by selecting api version TRUSTY_API_VERSION_RESTART_FIQ (1) or later.
 */
#define SMC_SC_RESTART_FIQ	SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 2)

/**
 * SMC_SC_NOP - Enter trusty to run pending work.
 *
 * No arguments.
 *
 * Returns SM_ERR_NOP_INTERRUPTED or SM_ERR_NOP_DONE.
 * If SM_ERR_NOP_INTERRUPTED is returned, the call must be repeated.
 *
 * Enable by selecting api version TRUSTY_API_VERSION_SMP (2) or later.
 */
#define SMC_SC_NOP		SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 3)

/*
 * Return from secure os to non-secure os with return value in r1
 */
#define SMC_SC_NS_RETURN	SMC_STDCALL_NR  (SMC_ENTITY_SECURE_MONITOR, 0)

#define SMC_FC_RESERVED		SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 0)
#define SMC_FC_FIQ_EXIT		SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 1)
#define SMC_FC_REQUEST_FIQ	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 2)
#define SMC_FC_GET_NEXT_IRQ	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 3)
#define SMC_FC_FIQ_ENTER	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 4)

#define SMC_FC64_SET_FIQ_HANDLER SMC_FASTCALL64_NR(SMC_ENTITY_SECURE_MONITOR, 5)
#define SMC_FC64_GET_FIQ_REGS	SMC_FASTCALL64_NR (SMC_ENTITY_SECURE_MONITOR, 6)

#define SMC_FC_CPU_SUSPEND	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 7)
#define SMC_FC_CPU_RESUME	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 8)

#define SMC_FC_AARCH_SWITCH	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 9)
#define SMC_FC_GET_VERSION_STR	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 10)

/**
 * SMC_FC_API_VERSION - Find and select supported API version.
 *
 * @r1: Version supported by client.
 *
 * Returns version supported by trusty.
 *
 * If multiple versions are supported, the client should start by calling
 * SMC_FC_API_VERSION with the largest version it supports. Trusty will then
 * return a version it supports. If the client does not support the version
 * returned by trusty and the version returned is less than the version
 * requested, repeat the call with the largest supported version less than the
 * last returned version.
 *
 * This call must be made before any calls that are affected by the api version.
 */
#define TRUSTY_API_VERSION_RESTART_FIQ	(1)
#define TRUSTY_API_VERSION_SMP		(2)
#define TRUSTY_API_VERSION_SMP_NOP	(3)
#define TRUSTY_API_VERSION_CURRENT	(3)
#define SMC_FC_API_VERSION	SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 11)

/* TRUSTED_OS entity calls */
#define SMC_SC_VIRTIO_GET_DESCR	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 20)
#define SMC_SC_VIRTIO_START	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 21)
#define SMC_SC_VIRTIO_STOP	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 22)

#define SMC_SC_VDEV_RESET	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 23)
#define SMC_SC_VDEV_KICK_VQ	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 24)
#define SMC_NC_VDEV_KICK_VQ	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 25)

/* Queueless Trusty IPC Interface */
#define SMC_SC_TRUSTY_IPC_CREATE_QL_DEV		SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 30)
#define SMC_SC_TRUSTY_IPC_SHUTDOWN_QL_DEV	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 31)
#define SMC_SC_TRUSTY_IPC_HANDLE_QL_DEV_CMD	SMC_STDCALL_NR(SMC_ENTITY_TRUSTED_OS, 32)

#define SMC_FC_GET_VERSION_STR SMC_FASTCALL_NR (SMC_ENTITY_SECURE_MONITOR, 10)

#define SMC_ARG0                "x0"
#define SMC_ARG1                "x1"
#define SMC_ARG2                "x2"
#define SMC_ARG3                "x3"
#define SMC_ARCH_EXTENSION      ""
#define SMC_REGISTERS_TRASHED   "x4","x5","x6","x7","x8","x9","x10","x11", \
                                "x12","x13","x14","x15","x16","x17"

static inline u64 trusty_smc(u64 r0, u64 r1, u64 r2, u64 r3)
{
        register u64 _r0 asm(SMC_ARG0) = r0;
        register u64 _r1 asm(SMC_ARG1) = r1;
        register u64 _r2 asm(SMC_ARG2) = r2;
        register u64 _r3 asm(SMC_ARG3) = r3;

        asm volatile(
                __asmeq("%0", SMC_ARG0)
                __asmeq("%1", SMC_ARG1)
                __asmeq("%2", SMC_ARG2)
                __asmeq("%3", SMC_ARG3)
                __asmeq("%4", SMC_ARG0)
                __asmeq("%5", SMC_ARG1)
                __asmeq("%6", SMC_ARG2)
                __asmeq("%7", SMC_ARG3)
                SMC_ARCH_EXTENSION
                "b .\r\n"
                "smc    #0"     /* switch to secure world */
                : "=r" (_r0), "=r" (_r1), "=r" (_r2), "=r" (_r3)
                : "r" (_r0), "r" (_r1), "r" (_r2), "r" (_r3)
                : SMC_REGISTERS_TRASHED);
        return _r0;
}


static inline void mytest(void)
{
	trusty_smc(SMC_FC_GET_VERSION_STR, -1, 0, 0);
}

#endif /* QL_TIPC_SMCALL_H_ */
