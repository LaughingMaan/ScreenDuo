/*++

Module Name:

    device.h

Abstract:

    This file contains the device definitions.

Environment:

    User-mode Driver Framework 2

--*/

#include "public.h"

EXTERN_C_START

//
// The device context performs the same job as
// a WDM device extension in the driver frameworks
//
typedef struct _DEVICE_CONTEXT
{
    ULONG PrivateDeviceData;  // just a placeholder

	WDFUSBDEVICE                    UsbDevice;

	WDFUSBINTERFACE                 UsbInterface;

	WDFUSBPIPE                      BulkReadPipe;

	WDFUSBPIPE                      BulkWritePipe;

	WDFUSBPIPE                      InterruptPipe;

	WDFWAITLOCK                     ResetDeviceWaitLock;

	UCHAR                           CurrentSwitchState;

	WDFQUEUE                        InterruptMsgQueue;

	ULONG                           UsbDeviceTraits;

	//
	// The following fields are used during event logging to 
	// report the events relative to this specific instance 
	// of the device.
	//

	WDFMEMORY                       DeviceNameMemory;
	PCWSTR                          DeviceName;

	WDFMEMORY                       LocationMemory;
	PCWSTR                          Location;

} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

//
// This macro will generate an inline function called DeviceGetContext
// which will be used to get a pointer to the device context memory
// in a type safe manner.
//
WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)

//
// Function to initialize the device and its callbacks
//
NTSTATUS
ScreenDuoUMDFCreateDevice(
    _Inout_ PWDFDEVICE_INIT DeviceInit
    );
typedef
_Function_class_(EVT_WDF_DEVICE_PREPARE_HARDWARE)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_DEVICE_PREPARE_HARDWARE(
	_In_
	WDFDEVICE Device,
	_In_
	WDFCMRESLIST ResourcesRaw,
	_In_
	WDFCMRESLIST ResourcesTranslated
);

EVT_WDF_DEVICE_PREPARE_HARDWARE ScreenDuoUMDFEvtDevicePrepareHardware;


_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
SelectInterfaces(
	_In_ WDFDEVICE Device
);
EVT_WDF_DEVICE_D0_ENTRY ScreenDuoUMDFEvtDeviceD0Entry;
EXTERN_C_END
