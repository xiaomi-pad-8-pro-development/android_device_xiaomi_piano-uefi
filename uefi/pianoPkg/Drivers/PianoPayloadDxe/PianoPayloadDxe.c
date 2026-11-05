/** @file
  Points /chosen initrd at the payload BootShim carries after the firmware
  volume. SEC keeps the registers BootShim enters with in the entry-argument
  HOB: x0 (ABL's device tree), x2 (payload address), x3 (payload size).

  Copyright (c) 2026, MUHAMMAD ALIF DANIEL BIN MOHD HAIRUL HEZZELIN.

  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/FdtLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>

// Writes a cell-sized property in place: ABL already sized it, so the tree
// needs no free space.
STATIC
INT32
SetCells (
  IN VOID         *Fdt,
  IN INT32        Node,
  IN CONST CHAR8  *Name,
  IN UINT64       Value
  )
{
  CONST VOID  *Old;
  INT32       Length;
  UINT32      Value32;
  UINT64      Value64;

  Old = FdtGetProp (Fdt, Node, Name, &Length);
  if ((Old != NULL) && (Length == sizeof (UINT32))) {
    if (Value > MAX_UINT32) {
      return -1;
    }

    Value32 = CpuToFdt32 ((UINT32)Value);
    return FdtSetProp (Fdt, Node, Name, &Value32, sizeof (Value32));
  }

  Value64 = CpuToFdt64 (Value);
  return FdtSetProp (Fdt, Node, Name, &Value64, sizeof (Value64));
}

EFI_STATUS
EFIAPI
PianoPayloadInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HOB_GUID_TYPE     *Hob;
  CONST UINT64          *Args;
  VOID                  *Fdt;
  UINT64                Base;
  UINT64                Size;
  EFI_PHYSICAL_ADDRESS  Pages;
  EFI_STATUS            Status;
  INT32                 Chosen;

  Hob = GetFirstGuidHob (&gSiliciumEntryArgHobGuid);
  if ((Hob == NULL) || (GET_GUID_HOB_DATA_SIZE (Hob) < 3 * sizeof (UINT64))) {
    DEBUG ((DEBUG_ERROR, "PianoPayload: no entry-argument HOB with x2 and x3\n"));
    return EFI_NOT_FOUND;
  }

  Args = GET_GUID_HOB_DATA (Hob);
  Fdt  = (VOID *)(UINTN)ReadUnaligned64 (&Args[0]);
  Base = ReadUnaligned64 (&Args[1]);
  Size = ReadUnaligned64 (&Args[2]);
  DEBUG ((DEBUG_ERROR, "PianoPayload: x0 0x%p x2 0x%lx x3 0x%lx\n", Fdt, Base, Size));
  if ((Size == 0) || (Base == 0)) {
    // A plain UEFI boot: BootShim carried no payload.
    return EFI_NOT_FOUND;
  }

  // The payload sits in memory the UEFI memory map does not know is used.
  // It follows BootShim and the firmware volume, so it need not start on a
  // page.
  Pages  = Base & ~(UINT64)EFI_PAGE_MASK;
  Status = gBS->AllocatePages (AllocateAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (Base + Size - Pages), &Pages);
  if (EFI_ERROR (Status)) {
    // Not Found: the memory map does not hand this range out at all.
    DEBUG ((DEBUG_ERROR, "PianoPayload: not reserving 0x%lx-0x%lx: %r\n", Base, Base + Size, Status));
  }

  if ((Fdt == NULL) || (FdtCheckHeader (Fdt) != 0)) {
    DEBUG ((DEBUG_ERROR, "PianoPayload: no device tree at 0x%p\n", Fdt));
    return EFI_NOT_FOUND;
  }

  Chosen = FdtPathOffset (Fdt, "/chosen");
  if ((Chosen < 0) ||
      (SetCells (Fdt, Chosen, "linux,initrd-start", Base) != 0) ||
      (SetCells (Fdt, Chosen, "linux,initrd-end", Base + Size) != 0))
  {
    DEBUG ((DEBUG_ERROR, "PianoPayload: cannot set /chosen initrd\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((DEBUG_ERROR, "PianoPayload: initrd 0x%lx-0x%lx\n", Base, Base + Size));
  return EFI_SUCCESS;
}
