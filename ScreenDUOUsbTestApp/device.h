//
// Define below GUIDs
//
#include <initguid.h>

//
// Device Interface GUID.
// Used by all WinUsb devices that this application talks to.
// Must match "DeviceInterfaceGUIDs" registry value specified in the INF file.
// 13e57103-081b-4cc6-9631-f95d03d13947 0x13e57103,0x081b,0x4cc6,0x96,0x31,0xf9,0x5d,0x03,0xd1,0x39,0x47
//test WinUSB 88bae032-5a81-49f0-bc3d-a4ff138216d6
//
DEFINE_GUID(GUID_DEVINTERFACE_ScreenDUOUsbTestApp,
	0x37f0720c, 0xc1f2, 0x4043, 0xaa, 0x89, 0x3b, 0x17, 0x18, 0xad, 0xa1, 0xdf);

//DEFINE_GUID(GUID_DEVINTERFACE_ScreenDUOUsbTestApp,
//	0x88bae032, 0x5a81, 0x49f0, 0xaa, 0x89, 0x3b, 0x17, 0x18, 0xad, 0xa1, 0xdf);


typedef struct _DEVICE_DATA {

    BOOL                    HandlesOpen;
    WINUSB_INTERFACE_HANDLE WinusbHandle;
    HANDLE                  DeviceHandle;
    TCHAR                   DevicePath[MAX_PATH];

} DEVICE_DATA, *PDEVICE_DATA;

HRESULT
OpenDevice(
    _Out_     PDEVICE_DATA DeviceData,
    _Out_opt_ PBOOL        FailureDeviceNotFound
    );

VOID
CloseDevice(
    _Inout_ PDEVICE_DATA DeviceData
    );
