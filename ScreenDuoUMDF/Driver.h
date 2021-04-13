/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    User-mode Driver Framework 2

--*/

#include <windows.h>
#include <winioctl.h>
#pragma warning( disable: 4201 )    // nonstandard extension used : nameless struct/union

#include <initguid.h>
#include <ntstatus.h>
#include <assert.h>
#include <strsafe.h>
#include <devpropdef.h>
//#include <wudfwdm.h>

#include <wdf.h>
#include <usb.h>
#include <wdfusb.h>
//#include <stdlib.h>
//#include <iostream>
//#include <vector>
//#include <usbdlib.h>


#include "TextMsg.h"
#include "device.h"
#include "queue.h"
#include "trace.h"


#define SCREENDUO_TRANSFER_BUFFER_SIZE (64*1024)
EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD ScreenDuoUMDFEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP ScreenDuoUMDFEvtDriverContextCleanup;

EXTERN_C_END
