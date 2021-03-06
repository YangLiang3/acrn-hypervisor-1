/*-
 * Copyright (c) 2013 Tycho Nightingale <tycho.nightingale@pluribusnetworks.com>
 * Copyright (c) 2013 Neel Natu <neel@freebsd.org>
 * Copyright (c) 2017 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETAPP, INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NETAPP, INC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef VIOAPIC_H
#define VIOAPIC_H

/**
 * @file vioapic.h
 *
 * @brief public APIs for virtual I/O APIC
 */

#include <apicreg.h>

#define	VIOAPIC_BASE	0xFEC00000UL
#define	VIOAPIC_SIZE	4096UL
#define	VIOAPIC_MAX_PIN	 256U

#define REDIR_ENTRIES_HW	120U /* SOS align with native ioapic */
#define STATE_BITMAP_SIZE	INT_DIV_ROUNDUP(REDIR_ENTRIES_HW, 64U)

#define IOAPIC_RTE_LOW_INTVEC	((uint32_t)IOAPIC_RTE_INTVEC)

struct acrn_vioapic {
	struct acrn_vm	*vm;
	spinlock_t	mtx;
	uint32_t	id;
	uint32_t	ioregsel;
	union ioapic_rte rtbl[REDIR_ENTRIES_HW];
	/* pin_state status bitmap: 1 - high, 0 - low */
	uint64_t pin_state[STATE_BITMAP_SIZE];
	struct ptirq_remapping_info *vpin_to_pt_entry[VIOAPIC_MAX_PIN];
};

void    vioapic_init(struct acrn_vm *vm);
void	vioapic_reset(struct acrn_vioapic *vioapic);


/**
 * @brief virtual I/O APIC
 *
 * @addtogroup acrn_vioapic ACRN vIOAPIC
 * @{
 */

/**
 * @brief Set vIOAPIC IRQ line status.
 *
 * @param[in] vm        Pointer to target VM
 * @param[in] irq       Target IRQ number
 * @param[in] operation Action options: GSI_SET_HIGH/GSI_SET_LOW/
 *			GSI_RAISING_PULSE/GSI_FALLING_PULSE
 *
 * @pre irq < vioapic_pincount(vm)
 *
 * @return None
 */
void	vioapic_set_irq(struct acrn_vm *vm, uint32_t irq, uint32_t operation);

/**
 * @brief Set vIOAPIC IRQ line status.
 *
 * Similar with vioapic_set_irq(),but would not make sure
 * operation be done with ioapic lock.
 *
 * @param[in] vm        Pointer to target VM
 * @param[in] irq       Target IRQ number
 * @param[in] operation Action options: GSI_SET_HIGH/GSI_SET_LOW/
 *			GSI_RAISING_PULSE/GSI_FALLING_PULSE
 *
 * @pre irq < vioapic_pincount(vm)
 * @return None
 */
void	vioapic_set_irq_nolock(struct acrn_vm *vm, uint32_t irq, uint32_t operation);
void	vioapic_update_tmr(struct acrn_vcpu *vcpu);

uint32_t	vioapic_pincount(const struct acrn_vm *vm);
void	vioapic_process_eoi(struct acrn_vm *vm, uint32_t vector);
void	vioapic_get_rte(struct acrn_vm *vm, uint32_t pin, union ioapic_rte *rte);
int32_t	vioapic_mmio_access_handler(struct io_request *io_req, void *handler_private_data);

/**
 * @}
 */
/* End of acrn_vioapic */
#endif /* VIOAPIC_H */
