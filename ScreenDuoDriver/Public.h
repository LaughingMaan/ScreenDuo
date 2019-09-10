/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_ScreenDuoDriver,
    0x4368b104,0x7e14,0x4324,0x88,0xc3,0x2a,0x3b,0xe1,0x80,0x84,0x77);
// {4368b104-7e14-4324-88c3-2a3be1808477}
