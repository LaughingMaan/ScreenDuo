#include "pch.h"

#include <stdio.h>
#include "image.h"
#include <vector>

LONG __cdecl
_tmain(
    LONG     Argc,
    LPTSTR * Argv
    )
/*++

Routine description:

    Sample program that communicates with a USB device using WinUSB

--*/
{
    DEVICE_DATA           deviceData;
    HRESULT               hr;
   // USB_DEVICE_DESCRIPTOR deviceDesc;
   // BOOL                  bResult;
    BOOL                  noDevice;
   // ULONG                 lengthReceived;

    UNREFERENCED_PARAMETER(Argc);
    UNREFERENCED_PARAMETER(Argv);

    //
    // Find a device connected to the system that has WinUSB installed using our
    // INF
    //
    hr = OpenDevice(&deviceData, &noDevice);

    if (FAILED(hr)) {

        if (noDevice) {

            wprintf(L"Device not connected or driver not installed\n");

        } else {

            wprintf(L"Failed looking for device, HRESULT 0x%x\n", hr);
        }

        return 0;
    }

    ////
    //// Get device descriptor
    ////
    //bResult = WinUsb_GetDescriptor(deviceData.WinusbHandle,
    //                               USB_DEVICE_DESCRIPTOR_TYPE,
    //                               0,
    //                               0,
    //                               (PBYTE) &deviceDesc,
    //                               sizeof(deviceDesc),
    //                               &lengthReceived);

    //if (FALSE == bResult || lengthReceived != sizeof(deviceDesc)) {

    //    wprintf(L"Error among LastError %d or lengthReceived %d\n",
    //            FALSE == bResult ? GetLastError() : 0,
    //            lengthReceived);
    //    CloseDevice(&deviceData);
    //    return 0;
    //}

    ////
    //// Print a few parts of the device descriptor
    ////
    //wprintf(L"Device found: VID_%04X&PID_%04X; bcdUsb %04X\n",
    //        deviceDesc.idVendor,
    //        deviceDesc.idProduct,
    //        deviceDesc.bcdUSB);
	
	ULONG bytesReturned = 0;
	//if (!WriteFile(deviceData.DeviceHandle,
	//	/*(LPCVOID)testdata*/(LPCVOID)testdata,
	//	(DWORD)/*230432*/230432,
	//	&bytesReturned,
	//	NULL)) {
	//	
	//	//typedef int (*MyFunct_t)();
	//	//auto myDLL = LoadLibrary((LPCWSTR)"ntdll.dll");
	//	//auto MyFunct = (MyFunct_t)GetProcAddress(myDLL, (LPCSTR)"RtlGetLastNtStatus");
	//	////MyFunct();

	//	//printf("PerformWriteReadTest: WriteFile failed: "
	//	//	"Error %d\n", MyFunct());

	//	printf("PerformWriteReadTest: WriteFile failed: "
	//		"Error %d\n", GetLastError());
	//	//ERROR_INVALID_FUNCTION
	//}
	//unsigned tmp[0xff];
	//std::vector<int> lol;
	char* myTest = "Pes Suda!";
		if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)myTest, (DWORD)strlen(myTest), &bytesReturned, NULL)) {
			printf("PerformWriteReadTest: WriteFile cbwh1 failed: "
				"Error %d\n", GetLastError());
			goto Exit;
		}
		else printf("Bytes returned cbwh1: %d\n", bytesReturned);
////Test1:
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwh1, (DWORD)31, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwh1 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwh1: %d\n", bytesReturned);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)data1, (DWORD)65536, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile data1 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned data1: %d\n", bytesReturned);
//	ReadFile(deviceData.DeviceHandle, (LPVOID)tmp, (DWORD)0xd, &bytesReturned, NULL);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwf1, (DWORD)512, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwf1 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwf1: %d\n", bytesReturned);
//	bytesReturned = 0;
//
////Test2:
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwh2, (DWORD)31, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwh2 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwh2: %d\n", bytesReturned);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)data2, (DWORD)65536, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile data2 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned data2: %d\n", bytesReturned);
//	ReadFile(deviceData.DeviceHandle, (LPVOID)tmp, (DWORD)0xd, &bytesReturned, NULL);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwf2, (DWORD)512, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwf2 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwf2: %d\n", bytesReturned);
//	bytesReturned = 0;
//
//
////Test3:
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwh3, (DWORD)31, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwh3 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwh3: %d\n", bytesReturned);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)data3, (DWORD)65536, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile data3 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned data3: %d\n", bytesReturned);
//	ReadFile(deviceData.DeviceHandle, (LPVOID)tmp, (DWORD)0xd, &bytesReturned, NULL);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwf3, (DWORD)512, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwf3 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwf3: %d\n", bytesReturned);
//	bytesReturned = 0;
//
//
////Test4:
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwh4, (DWORD)31, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwh4 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwh4: %d\n", bytesReturned);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)data4, (DWORD)33824, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile data4 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned data4: %d\n", bytesReturned);
//	ReadFile(deviceData.DeviceHandle, (LPVOID)tmp, (DWORD)0xd, &bytesReturned, NULL);
//	bytesReturned = 0;
//	if (!WriteFile(deviceData.DeviceHandle, (LPCVOID)cbwf4, (DWORD)512, &bytesReturned, NULL)) {
//		printf("PerformWriteReadTest: WriteFile cbwf4 failed: "
//			"Error %d\n", GetLastError());
//		goto Exit;
//	}
//	else printf("Bytes returned cbwf4: %d\n", bytesReturned);
//	bytesReturned = 0;
//

	Exit:
    CloseDevice(&deviceData);
    return 0;
}
