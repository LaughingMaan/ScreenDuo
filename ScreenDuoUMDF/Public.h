/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    driver and application

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//
/*
// {6DDA3D57-D687-4900-A48D-05F486A83B8D}
DEFINE_GUID(<<name>>,
0x6dda3d57, 0xd687, 0x4900, 0xa4, 0x8d, 0x5, 0xf4, 0x86, 0xa8, 0x3b, 0x8d);

*/
DEFINE_GUID (GUID_DEVINTERFACE_ScreenDuoUMDF,
    0x37f0720c,0xc1f2,0x4043,0xaa,0x89,0x3b,0x17,0x18,0xad,0xa1,0xdf);
// {37f0720c-c1f2-4043-aa89-3b1718ada1df}
