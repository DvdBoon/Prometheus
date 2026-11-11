#ifndef LIBRARIES_MEDIATORPCI_H
#define LIBRARIES_MEDIATORPCI_H
/*
** $VER: mediatorpci.h 2.3 (10.09.2010)
** Definitions for the pci.library ver.9.0.
**
** (C) Copyright 2001-2010 Elbox Computer.
**     All Rights Reserved
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif   /* EXEC_TYPES_H */

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif   /* EXEC_LIBRARIES_H */

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif   /* EXEC_SEMAPHORES_H */

   
/* The memory attribute for allocation of the PCI DMA memory for function by function AllocMem, etc ... */
#define MEMF_MEDIATORPCIDMA   (1UL<< 13) /* DMAable memory on MediatorPCI*/


struct  PciBase {
    struct Library  pb_LibNode;
    UBYTE  pb_Flags; /* bit 0: 1 - MMU, 0 - NO_MMU, do not change it, for Mediator 4000 this bit always 1 */ 
    UBYTE  pb_Reserved;
    UWORD  pb_MemOffset; /* only for use by pci.library */
    ULONG  pb_BoardAddress; /* only for use by pci.library */
    ULONG  pb_MemAddress; /* address of memory window */
    ULONG  pb_MemSize; /* size of memory window */ 
    ULONG  pb_IOAddress; /* address of IO space */
    struct SignalSemaphore  pb_MemSemaphore;
    struct MinList  pb_DevicesList; /* list of PciDevice structures on bus 0*/
    struct MinList  pb_MemAlloc; /* list of MEMORY space allocated */
    struct MinList  pb_IOAlloc; /* list of IO space allocated */
    struct MinList  pb_IntA; /* list of interrupts connected to line INT A*/
    struct MinList  pb_IntB; /* list of interrupts connected to line INT B*/
    struct MinList  pb_IntC; /* list of interrupts connected to line INT C*/
    struct MinList  pb_IntD; /* list of interrupts connected to line INT D*/
	ULONG  pb_Reserved1[9];
	ULONG  pb_DMAMemAddres;  /* address in PCI space of DMA buffers for Elbox drivers - size 1MB*/ 
};

/* for more information about this structure see PCI Specification */
/* (PCI Local Bus Specification Rev.2.2 Dec 18, 1998)*/
struct PciDevice {
    struct  Node  pd_Node; /* ln_Pri = DevFuncNumber */ 
    UWORD  pd_VendorID;
    UWORD  pd_DeviceID;
    ULONG  pd_ClassCode;
    UBYTE  pd_RevisionID;
    UBYTE  pd_InterruptPin;
    UWORD  pd_SubsystemVendorID;
    UWORD  pd_SubsystemID;
    ULONG  pd_ABaseAddress0;
       /* for Mem Space: pd_ABaseAddressX = BaseAddressX */
       /* for I/O Space: pd_AbaseAddressX = BaseAddressX + pd_IOAddress */
    ULONG  pd_Size0;
    ULONG  pd_ABaseAddress1;
    ULONG  pd_Size1;
    ULONG  pd_ABaseAddress2;
    ULONG  pd_Size2;
    ULONG  pd_ABaseAddress3;
    ULONG  pd_Size3;
    ULONG  pd_ABaseAddress4;
    ULONG  pd_Size4;
    ULONG  pd_ABaseAddress5;
    ULONG  pd_Size5;
    ULONG  pd_ARomBaseAddress;
       /*                pd_ARomBaseAddress = RomBaseAddress */
    ULONG  pd_RomSize;
    ULONG  pd_CardBusCISPointer;
    char  *pd_VendorName;
    char  *pd_DeviceName;
    char  *pd_SubName;
    UWORD  pd_DevFunc; /* bits 0-2 function number, bits 3-7 device number, */
                       /* bits 8-15 bus number */
    UBYTE  pd_HeaderType;
    UBYTE  pd_Reserved;
};

#endif   /* LIBRARIES_PCI_H */
