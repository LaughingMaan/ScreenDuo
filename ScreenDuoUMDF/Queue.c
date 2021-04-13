/*++

Module Name:

    queue.c

Abstract:

    This file contains the queue entry points and callbacks.

Environment:

    User-mode Driver Framework 2

--*/

#include "driver.h"
#include "queue.tmh"
#define VER_2

NTSTATUS
ScreenDuoUMDFQueueInitialize(
    _In_ WDFDEVICE Device
    )
/*++

Routine Description:

     The I/O dispatch callbacks for the frameworks device object
     are configured in this function.

     A single default I/O Queue is configured for parallel request
     processing, and a driver context memory allocation is created
     to hold our structure QUEUE_CONTEXT.

Arguments:

    Device - Handle to a framework device object.

Return Value:

    VOID

--*/
{
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Entry");
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KdPrintEx %s", "ScreenDuoUMDFQueueInitialize"));
    WDFQUEUE queue;
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;

    //
    // Configure a default queue so that requests that are not
    // configure-fowarded using WdfDeviceConfigureRequestDispatching to goto
    // other queues get dispatched here.
    //
    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(
         &queueConfig,
        WdfIoQueueDispatchParallel
        );

    queueConfig.EvtIoDeviceControl = ScreenDuoUMDFEvtIoDeviceControl;
    queueConfig.EvtIoStop = ScreenDuoUMDFEvtIoStop;
	queueConfig.EvtIoDefault = ScreenDuoUMDFEvtIoDefault;
	queueConfig.EvtIoWrite = ScreenDuoUMDFEvtIoWrite;
	queueConfig.EvtIoRead = ScreenDuoUMDFOsrFxEvtIoRead;
    status = WdfIoQueueCreate(
                 Device,
                 &queueConfig,
                 WDF_NO_OBJECT_ATTRIBUTES,
                 &queue
                 );

    if(!NT_SUCCESS(status)) {
        TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfIoQueueCreate failed %!STATUS!", status);
        return status;
    }
	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_DRIVER, "%!FUNC! Exit");
    return status;
}

VOID
ScreenDuoUMDFEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
    )
/*++

Routine Description:

    This event is invoked when the framework receives IRP_MJ_DEVICE_CONTROL request.

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    OutputBufferLength - Size of the output buffer in bytes

    InputBufferLength - Size of the input buffer in bytes

    IoControlCode - I/O control code.

Return Value:

    VOID

--*/
{
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KdPrintEx %s", "ScreenDuoUMDFEvtIoDeviceControl"));
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p OutputBufferLength %d InputBufferLength %d IoControlCode %d", 
                Queue, Request, (int) OutputBufferLength, (int) InputBufferLength, IoControlCode);

    WdfRequestComplete(Request, STATUS_SUCCESS);

    return;
}

VOID
ScreenDuoUMDFEvtIoStop(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ ULONG ActionFlags
)
/*++

Routine Description:

    This event is invoked for a power-managed queue before the device leaves the working state (D0).

Arguments:

    Queue -  Handle to the framework queue object that is associated with the
             I/O request.

    Request - Handle to a framework request object.

    ActionFlags - A bitwise OR of one or more WDF_REQUEST_STOP_ACTION_FLAGS-typed flags
                  that identify the reason that the callback function is being called
                  and whether the request is cancelable.

Return Value:

    VOID

--*/
{
	KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KdPrintEx %s", "ScreenDuoUMDFEvtIoStop"));
    TraceEvents(TRACE_LEVEL_INFORMATION, 
                TRACE_QUEUE, 
                "%!FUNC! Queue 0x%p, Request 0x%p ActionFlags %d", 
                Queue, Request, ActionFlags);

    //
    // In most cases, the EvtIoStop callback function completes, cancels, or postpones
    // further processing of the I/O request.
    //
    // Typically, the driver uses the following rules:
    //
    // - If the driver owns the I/O request, it calls WdfRequestUnmarkCancelable
    //   (if the request is cancelable) and either calls WdfRequestStopAcknowledge
    //   with a Requeue value of TRUE, or it calls WdfRequestComplete with a
    //   completion status value of STATUS_SUCCESS or STATUS_CANCELLED.
    //
    //   Before it can call these methods safely, the driver must make sure that
    //   its implementation of EvtIoStop has exclusive access to the request.
    //
    //   In order to do that, the driver must synchronize access to the request
    //   to prevent other threads from manipulating the request concurrently.
    //   The synchronization method you choose will depend on your driver's design.
    //
    //   For example, if the request is held in a shared context, the EvtIoStop callback
    //   might acquire an internal driver lock, take the request from the shared context,
    //   and then release the lock. At this point, the EvtIoStop callback owns the request
    //   and can safely complete or requeue the request.
    //
    // - If the driver has forwarded the I/O request to an I/O target, it either calls
    //   WdfRequestCancelSentRequest to attempt to cancel the request, or it postpones
    //   further processing of the request and calls WdfRequestStopAcknowledge with
    //   a Requeue value of FALSE.
    //
    // A driver might choose to take no action in EvtIoStop for requests that are
    // guaranteed to complete in a small amount of time.
    //
    // In this case, the framework waits until the specified request is complete
    // before moving the device (or system) to a lower power state or removing the device.
    // Potentially, this inaction can prevent a system from entering its hibernation state
    // or another low system power state. In extreme cases, it can cause the system
    // to crash with bugcheck code 9F.
    //

    return;
}

#ifdef VER_2
void ScreenDuoUMDFEvtIoWrite(
	_In_ WDFQUEUE   Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t     length
) 

{
	NTSTATUS                    status;
	WDFUSBPIPE                  pipe;
	WDFMEMORY                   reqMemory;
	PDEVICE_CONTEXT             pDeviceContext;
	WDFREQUEST					myReq;
	WDFMEMORY					myMem;
	PVOID						myBufPointer;


	UNREFERENCED_PARAMETER(Queue);
	MsgSend(Queue, Request, length);
//	TraceEvents(TRACE_LEVEL_INFORMATION,
//		TRACE_QUEUE,
//		"%!FUNC! Queue 0x%p, Request 0x%p Length %Iu",
//		Queue, Request, length);
//
//		if (length > SCREENDUO_TRANSFER_BUFFER_SIZE) {
//			TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Transfer exceeds %d\n",
//				SCREENDUO_TRANSFER_BUFFER_SIZE);
//			status = STATUS_INVALID_PARAMETER;
//			goto Exit;
//		}
//		pDeviceContext = DeviceGetContext(WdfIoQueueGetDevice(Queue));
//
//		pipe = pDeviceContext->BulkWritePipe;
//
//		status = WdfRequestRetrieveInputMemory(Request, &reqMemory);
//		if (!NT_SUCCESS(status)) {
//			TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveInputBuffer failed\n");
//			goto Exit;
//		}
//		
//		if (length > 0) {
//			status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myReq);
//			if (!NT_SUCCESS(status)) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfRequestCreate failed 0x%x\n", status);
//				goto Exit;
//			}
//			status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 0, length, &myMem, &myBufPointer);
//			if (!NT_SUCCESS(status)) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfMemoryCreate failed 0x%x\n", status);
//				goto Exit;
//			}
//			status = WdfMemoryCopyToBuffer(reqMemory, 0, myBufPointer, length);
//			if (!NT_SUCCESS(status)) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfMemoryCreate failed 0x%x\n", status);
//				goto Exit;
//			}
//			status = WdfUsbTargetPipeFormatRequestForWrite(pDeviceContext->BulkWritePipe, myReq, myMem, NULL);
//			if (!NT_SUCCESS(status)) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfMemoryCreate failed 0x%x\n", status);
//				goto Exit;
//			}
//
//			WDF_REQUEST_SEND_OPTIONS myOpt;
//			WDF_REQUEST_SEND_OPTIONS_INIT(&myOpt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS | WDF_REQUEST_SEND_OPTION_TIMEOUT);
//			WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&myOpt, WDF_REL_TIMEOUT_IN_SEC(10));
//			status = WdfRequestAllocateTimer(myReq);
//			if (!NT_SUCCESS(status)) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfRequestAllocateTimer failed 0x%x\n", status);
//				goto Exit;
//			}
//			if (WdfRequestSend(myReq, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myOpt) == FALSE) {
//				TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//					"WdfRequestSend failed 0x%x\n", WdfRequestGetStatus(myReq));
//				goto Exit;
//			}
//			else {
//				PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
//				WDF_REQUEST_COMPLETION_PARAMS CompletionParams;
//				WDF_REQUEST_COMPLETION_PARAMS_INIT(&CompletionParams);
//				WdfRequestGetCompletionParams(myReq, &CompletionParams);
//				status = CompletionParams.IoStatus.Status;
//				//
//			   // For usb devices, we should look at the Usb.Completion param.
//			   //
//				usbCompletionParams = CompletionParams.Parameters.Usb.Completion;
//
//				//size_t bytesWritten = usbCompletionParams->Parameters.PipeWrite.Length;
//				if (NT_SUCCESS(status)) {
//					TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE,
//						"Number of bytes written: %I64d\n", (INT64)usbCompletionParams->Parameters.PipeWrite.Length);
//				}
//				else {
//					TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
//						"Write failed: request Status 0x%x UsbdStatus 0x%x\n",
//						status, usbCompletionParams->UsbdStatus);
//					goto Exit;
//				}
//
//				WdfRequestCompleteWithInformation(Request, status, usbCompletionParams->Parameters.PipeWrite.Length);
//				WdfObjectDelete(myMem);
//				WdfObjectDelete(myReq);
//
//				TraceEvents(TRACE_LEVEL_INFORMATION,
//					TRACE_QUEUE,
//					"%!FUNC! Completing request...");
//				char* entrybuf = "AA";
//				//MsgSend(entrybuf, (size_t)2);
//				MsgSend(Queue, Request, length);
//				return;
//			}
//		}
//		//////////_Analysis_assume_(Length > 0);
//		
//Exit:
//		//if (!NT_SUCCESS(status)) {
//
//			WdfRequestCompleteWithInformation(Request, status, 0);
//			TraceEvents(TRACE_LEVEL_INFORMATION,
//				TRACE_QUEUE,
//				"%!FUNC! Complete request ERROR!!!!!!!!!!!");
//			return;
//		//}
	
}

VOID
ScreenDuoUMDFEvtRequestWriteCompletionRoutine(
	_In_ WDFREQUEST                  Request,
	_In_ WDFIOTARGET                 Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
	_In_ WDFCONTEXT                  Context
)
/*++

Routine Description:

	This is the completion routine for writes
	If the irp completes with success, we check if we
	need to recirculate this irp for another stage of
	transfer.

Arguments:

	Context - Driver supplied context
	Device - Device handle
	Request - Request handle
	Params - request completion params

Return Value:
	None

--*/
{
	NTSTATUS    status;
	size_t      bytesWritten = 0;
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;

	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Context);

	status = CompletionParams->IoStatus.Status;
	//
   // For usb devices, we should look at the Usb.Completion param.
   //
	usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

	bytesWritten = usbCompletionParams->Parameters.PipeWrite.Length;

	if (NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE,
			"Number of bytes written: %I64d\n", (INT64)bytesWritten);
	}
	else {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"Write failed: request Status 0x%x UsbdStatus 0x%x\n",
			status, usbCompletionParams->UsbdStatus);
	}

	WdfRequestCompleteWithInformation(Context, status, bytesWritten);
	WdfObjectDelete(Request);
	//WDFMEMORY reqMemory;
	//status = WdfRequestRetrieveInputMemory(Request, &reqMemory);
	//WdfObjectDelete(reqMemory);
	
	return;
}
#endif
#ifdef VER_1
void ScreenDuoUMDFEvtIoWrite(
	_In_ WDFQUEUE   Queue,
	_In_ WDFREQUEST Request,
	_In_ size_t     length
)

{
	NTSTATUS                    status;
	WDFUSBPIPE                  pipe;
	WDFMEMORY                   reqMemory;
	PDEVICE_CONTEXT             pDeviceContext;

	UNREFERENCED_PARAMETER(Queue);
	TraceEvents(TRACE_LEVEL_INFORMATION,
		TRACE_QUEUE,
		"%!FUNC! Queue 0x%p, Request 0x%p Length %Iu",
		Queue, Request, length);

	if (length > SCREENDUO_TRANSFER_BUFFER_SIZE) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Transfer exceeds %d\n",
			SCREENDUO_TRANSFER_BUFFER_SIZE);
		status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}
	pDeviceContext = DeviceGetContext(WdfIoQueueGetDevice(Queue));

	pipe = pDeviceContext->BulkWritePipe;

	status = WdfRequestRetrieveInputMemory(Request, &reqMemory);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestRetrieveInputBuffer failed\n");
		goto Exit;
	}

	status = WdfUsbTargetPipeFormatRequestForWrite(pipe,
		Request,
		reqMemory,
		NULL); // Offset
	//////dx *(UCHAR(*)[65536])switchState

	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"WdfUsbTargetPipeFormatRequestForWrite failed 0x%x\n", status);
		goto Exit;
	}

	WdfRequestSetCompletionRoutine(
		Request,
		ScreenDuoUMDFEvtRequestWriteCompletionRoutine,
		pipe);

	//
	// Send the request asynchronously.
	//
	if (WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS) == FALSE) {
		//
		// Framework couldn't send the request for some reason.
		//
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestSend failed\n");
		status = WdfRequestGetStatus(Request);
		goto Exit;
	}


Exit:
	if (!NT_SUCCESS(status)) {

		WdfRequestCompleteWithInformation(Request, status, 0);
		TraceEvents(TRACE_LEVEL_INFORMATION,
			TRACE_QUEUE,
			"%!FUNC! Complete request ERROR!!!!!!!!!!!");
	}
	TraceEvents(TRACE_LEVEL_INFORMATION,
		TRACE_QUEUE,
		"%!FUNC! Completing request...");
	//WdfRequestCompleteWithInformation(Request, STATUS_SUCCESS, length);

	return;
}

VOID
ScreenDuoUMDFEvtRequestWriteCompletionRoutine(
	_In_ WDFREQUEST                  Request,
	_In_ WDFIOTARGET                 Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
	_In_ WDFCONTEXT                  Context
)
/*++

Routine Description:

	This is the completion routine for writes
	If the irp completes with success, we check if we
	need to recirculate this irp for another stage of
	transfer.

Arguments:

	Context - Driver supplied context
	Device - Device handle
	Request - Request handle
	Params - request completion params

Return Value:
	None

--*/
{
	NTSTATUS    status;
	size_t      bytesWritten = 0;
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;

	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Context);

	status = CompletionParams->IoStatus.Status;
	//
   // For usb devices, we should look at the Usb.Completion param.
   //
	usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

	bytesWritten = usbCompletionParams->Parameters.PipeWrite.Length;

	if (NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE,
			"Number of bytes written: %I64d\n", (INT64)bytesWritten);
	}
	else {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"Write failed: request Status 0x%x UsbdStatus 0x%x\n",
			status, usbCompletionParams->UsbdStatus);
	}

	WdfRequestCompleteWithInformation(Request, status, bytesWritten);

	return;
}


#endif
void ScreenDuoUMDFEvtIoDefault(
	_In_
	WDFQUEUE Queue,
	_In_
	WDFREQUEST Request
)
{
	TraceEvents(TRACE_LEVEL_INFORMATION,
		TRACE_QUEUE,
		"%!FUNC! Queue 0x%p, Request 0x%p",
		Queue, Request);

	return;
}



VOID
ScreenDuoUMDFOsrFxEvtIoRead(
	_In_ WDFQUEUE         Queue,
	_In_ WDFREQUEST       Request,
	_In_ size_t           Length
)
/*++

Routine Description:

	Called by the framework when it receives Read or Write requests.

Arguments:

	Queue - Default queue handle
	Request - Handle to the read/write request
	Lenght - Length of the data buffer associated with the request.
				 The default property of the queue is to not dispatch
				 zero lenght read & write requests to the driver and
				 complete is with status success. So we will never get
				 a zero length request.

Return Value:


--*/
{
	WDFUSBPIPE                  pipe;
	NTSTATUS                    status;
	WDFMEMORY                   reqMemory;
	PDEVICE_CONTEXT             pDeviceContext;

	UNREFERENCED_PARAMETER(Queue);

	// 
	// Log read start event, using IRP activity ID if available or request
	// handle otherwise.
	//

	//EventWriteReadStart(WdfIoQueueGetDevice(Queue), (ULONG)Length);

	TraceEvents(TRACE_LEVEL_VERBOSE, TRACE_QUEUE, "-->OsrFxEvtIoRead\n");

	//
	// First validate input parameters.
	//
	if (Length > SCREENDUO_TRANSFER_BUFFER_SIZE) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "Transfer exceeds %d\n",
			SCREENDUO_TRANSFER_BUFFER_SIZE);
		status = STATUS_INVALID_PARAMETER;
		goto Exit;
	}

	pDeviceContext = DeviceGetContext(WdfIoQueueGetDevice(Queue));

	pipe = pDeviceContext->BulkReadPipe;

	status = WdfRequestRetrieveOutputMemory(Request, &reqMemory);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"WdfRequestRetrieveOutputMemory failed %!STATUS!\n", status);
		goto Exit;
	}

	//
	// The format call validates to make sure that you are reading or
	// writing to the right pipe type, sets the appropriate transfer flags,
	// creates an URB and initializes the request.
	//
	status = WdfUsbTargetPipeFormatRequestForRead(pipe,
		Request,
		reqMemory,
		NULL // Offsets
	);
	if (!NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"WdfUsbTargetPipeFormatRequestForRead failed 0x%x\n", status);
		goto Exit;
	}

	WdfRequestSetCompletionRoutine(
		Request,
		ScreenDuoUMDFEvtRequestReadCompletionRoutine,
		pipe);
	//
	// Send the request asynchronously.
	//
	if (WdfRequestSend(Request, WdfUsbTargetPipeGetIoTarget(pipe), WDF_NO_SEND_OPTIONS) == FALSE) {
		//
		// Framework couldn't send the request for some reason.
		//
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE, "WdfRequestSend failed\n");
		status = WdfRequestGetStatus(Request);
		goto Exit;
	}


Exit:
	if (!NT_SUCCESS(status)) {
		//
		// log event read failed
		//
		//EventWriteReadFail(WdfIoQueueGetDevice(Queue), status);
		WdfRequestCompleteWithInformation(Request, status, 0);
	}

	TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE, "<-- OsrFxEvtIoRead\n");

	return;
}

VOID
ScreenDuoUMDFEvtRequestReadCompletionRoutine(
	_In_ WDFREQUEST                  Request,
	_In_ WDFIOTARGET                 Target,
	_In_ PWDF_REQUEST_COMPLETION_PARAMS CompletionParams,
	_In_ WDFCONTEXT                  Context
)
/*++

Routine Description:

	This is the completion routine for reads
	If the irp completes with success, we check if we
	need to recirculate this irp for another stage of
	transfer.

Arguments:

	Context - Driver supplied context
	Device - Device handle
	Request - Request handle
	Params - request completion params

Return Value:
	None

--*/
{
	NTSTATUS    status;
	size_t      bytesRead = 0;
	PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;

	UNREFERENCED_PARAMETER(Target);
	UNREFERENCED_PARAMETER(Context);

	status = CompletionParams->IoStatus.Status;

	usbCompletionParams = CompletionParams->Parameters.Usb.Completion;

	bytesRead = usbCompletionParams->Parameters.PipeRead.Length;

	if (NT_SUCCESS(status)) {
		TraceEvents(TRACE_LEVEL_INFORMATION, TRACE_QUEUE,
			"Number of bytes read: %I64d\n", (INT64)bytesRead);
	}
	else {
		TraceEvents(TRACE_LEVEL_ERROR, TRACE_QUEUE,
			"Read failed - request status 0x%x UsbdStatus 0x%x\n",
			status, usbCompletionParams->UsbdStatus);

	}

	//
	// Log read stop event, using IRP activity ID if available or request
	// handle otherwise.
	//

	//EventWriteReadStop(WdfIoQueueGetDevice(WdfRequestGetIoQueue(Request)),
	//	bytesRead,
	//	status,
	//	usbCompletionParams->UsbdStatus);

	WdfRequestCompleteWithInformation(Request, status, bytesRead);

	return;
}