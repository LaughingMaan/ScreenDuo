/*++

Module Name:

	TextMsg.cpp

Abstract:

	This file contains the vendor-specific methods.

Environment:

	User-mode Driver Framework 2

--*/


#include "Driver.h"

int x2d(char x) {
	if ((x == 'a') || (x == 'A')) return 10;
	if ((x == 'b') || (x == 'B')) return 11;
	if ((x == 'c') || (x == 'C')) return 12;
	if ((x == 'd') || (x == 'D')) return 13;
	if ((x == 'e') || (x == 'E')) return 14;
	if ((x == 'f') || (x == 'F')) return 15;
	else
		return (x - 48);
}
uint32_t flip32(uint32_t v) {
	return ((v & 0xff000000) >> 24) | ((v & 0xff0000) >> 8) | ((v & 0xff00) << 8) | ((v & 0xff) << 24);
}
void putpixelxl(uint8_t* data, int x, int y, int r, int g, int b) {
	if (r != 0)
	{
		data[(x * 2 + y * 2 * 320) * 3] = r;
		data[(x * 2 + y * 2 * 320) * 3 + 3] = r;
		data[(x * 2 + (y * 2 + 1) * 320) * 3] = r;
		data[(x * 2 + (y * 2 + 1) * 320) * 3 + 3] = r;
	}

	if (g != 0)
	{
		data[(x * 2 + y * 2 * 320) * 3 + 1] = g;
		data[(x * 2 + y * 2 * 320) * 3 + 4] = g;
		data[(x * 2 + (y * 2 + 1) * 320) * 3 + 1] = g;
		data[(x * 2 + (y * 2 + 1) * 320) * 3 + 4] = g;
	}

	if (b != 0)
	{
		data[(x * 2 + y * 2 * 320) * 3 + 2] = b;
		data[(x * 2 + y * 2 * 320) * 3 + 5] = b;
		data[(x * 2 + (y * 2 + 1) * 320) * 3 + 2] = b;
		data[(x * 2 + (y * 2 + 1) * 320) * 3 + 5] = b;
	}
}
size_t MsgEntry(char* entryBuf, size_t length) {
	memset(image, 0, sizeof(image));
	image_t* header = (image_t*)image;
	header->u1 = 0x02;
	header->u2 = 0xf0;
	header->u3 = 0x20; /* bits per pixel? */
	header->x = 0;
	header->y = 0;
	header->w = 320;
	header->h = 240;
	header->length = sizeof(image_t) + (230400);
	header->u5 = 0x01; /* no idea */

	memset(data, 0x00, sizeof(image) - sizeof(image_t));
	//original textmode ancient variables
	int x, y;
	int set;
	int mask;
	int c;
	int cc;
	int color = 1; // 1-white 2-red 3-blue 4-green 5-cyan 6-yellow 7-magenta 8-black
	int red = 255;
	int green = 255;
	int blue = 255;
	int xpos = 0;
	int ypos = 0;
	int scale = 1;
	int ra = 0;  // 6 variables for arbitrary colors
	int rb = 0;
	int ga = 0;
	int gb = 0;
	int ba = 0;
	int bb = 0;
	int xa = 0;  // 6 variables for box
	int xb = 0;
	int ya = 0;
	int yb = 0;
	int lnx = 0;
	int lny = 0;
	int line = 0;
	int nl = 0;
	int nc = 0;
	int maxlines = 13; /* maximum number of lines for the device (starting at 0) with linespace at 10 */
	int cx = 0;
	int linespace = 8;  /* line spacing.  8 is small, 10 looks ok */
	
	if (length > 0) {
		for (c = 0; c < length; c++) {
			cc = entryBuf[c];
			red = 255; green = 255; blue = 255;
			char* bitmap = font8x8_extended[entryBuf[c]];
			for (y = 0; y < 8; y++) {
				for (x = 0; x < 8; x++) {
					set = bitmap[y] & 1 << x;
					putpixelxl(data, x + cx * 8, y + line, set ? red : 0, set ? green : 0, set ? blue : 0);
				}
			}

			cx++;
			if ((cx % 20) == 0) line += linespace;  /* text end-of-line crlf */
			if (line > (linespace * maxlines)) line = 0; /* bottom of screen screenwrap */
			if (nc) { nc = 0; c--; cx--; }
			if (nl) { nl = 0; cx = 0; c--; }
		
		}


	}
	return sizeof(image);
}

void MsgSend(
	 WDFQUEUE   Queue,
	 WDFREQUEST Request,
	 size_t     length_income) {

	 //WDFUSBPIPE                 pipe; pDeviceContext->BulkWritePipe
	 WDFMEMORY                  reqMemory;
	 PDEVICE_CONTEXT            pDeviceContext;
	 WDFREQUEST					myReq;
	 WDFMEMORY					myMem;
	 PVOID						myBufPointer;
	 NTSTATUS					status;
	 size_t						bufferLength = 0;
	 char*						entryBuf;
	 bool test;
	 uint8_t*					imagePtr = image;
	pDeviceContext = DeviceGetContext(WdfIoQueueGetDevice(Queue));
	status = WdfRequestRetrieveInputMemory(Request, &reqMemory);
	entryBuf = (char*)WdfMemoryGetBuffer(reqMemory, &bufferLength);

	size_t length = MsgEntry(entryBuf, bufferLength);


	int r, wrote;
	int pos = 0;
	uint8_t tmp[0xff];

	cbw_t* cbw;
	cb_header_t* header;
	cb_footer_t* footer;

	uint8_t h[sizeof(cbw_t) + sizeof(cb_header_t)];
	uint8_t f[sizeof(cbw_t) + sizeof(cb_footer_t)];
	cbw = (cbw_t*)& h[0];
	header = (cb_header_t*)& h[sizeof(cbw_t)];
	footer = (cb_footer_t*)& f[sizeof(cbw_t)];

	cbw->dCBWSignature = 0x43425355;
	cbw->dCBWTag = 0x843d84a0;
	cbw->bmCBWFlags = 0x00;
	cbw->bCBWLUN = 0x00;
	cbw->bCBWCBLength = 0x0c;
	header->u1 = 0x02e6;
	header->totalSize = flip32(length);
	header->index = 0;

	footer->u1 = 0x02e6;
	footer->totalSize = header->totalSize;
	memset(header->unused, 0, sizeof(header->unused));
	memset(footer->unused, 0, sizeof(footer->unused));

	int block = 0;
	while (length > 0) {
		int copy = length > 0x10000 ? 0x10000 : length; //if length > 0x10000 set copy = 0x10000 else set copy = length

		cbw->dCBWDataTransferLength = copy;
		header->blockSize = flip32(copy);
		header->index = block;
		header->u2 = 0x0;

		// send header
		status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myReq);
		status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 0, sizeof(h), &myMem, &myBufPointer);
		status = WdfMemoryCopyFromBuffer(myMem, 0, &h, sizeof(h));
		status = WdfUsbTargetPipeFormatRequestForWrite(pDeviceContext->BulkWritePipe, myReq, myMem, NULL);
		WDF_REQUEST_SEND_OPTIONS myOpt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&myOpt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS | WDF_REQUEST_SEND_OPTION_TIMEOUT);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&myOpt, WDF_REL_TIMEOUT_IN_SEC(10));
		status = WdfRequestAllocateTimer(myReq);
		if (WdfRequestSend(myReq, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myOpt) == FALSE) {
			test = false;
		}
		PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
		WDF_REQUEST_COMPLETION_PARAMS CompletionParams;
		WDF_REQUEST_COMPLETION_PARAMS_INIT(&CompletionParams);
		WdfRequestGetCompletionParams(myReq, &CompletionParams);
		status = CompletionParams.IoStatus.Status;
		usbCompletionParams = CompletionParams.Parameters.Usb.Completion;
		if (usbCompletionParams->Parameters.PipeWrite.Length == sizeof(h)) {
			test = true;
		}

		WdfObjectDelete(myMem);
		WdfObjectDelete(myReq);
		//send data

		status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myReq);
		status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 0, copy, &myMem, &myBufPointer);
		status = WdfMemoryCopyFromBuffer(myMem, 0, imagePtr, copy); //dx* (uint_8t(*)[230432])data   /dx* (PVOID(*)[65536])myBufPointer 
		status = WdfUsbTargetPipeFormatRequestForWrite(pDeviceContext->BulkWritePipe, myReq, myMem, NULL);
		//WDF_REQUEST_SEND_OPTIONS myOpt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&myOpt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS | WDF_REQUEST_SEND_OPTION_TIMEOUT);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&myOpt, WDF_REL_TIMEOUT_IN_SEC(10));
		status = WdfRequestAllocateTimer(myReq);
		if (WdfRequestSend(myReq, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myOpt) == FALSE) {
			test = false;
		}
		//PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
		//WDF_REQUEST_COMPLETION_PARAMS CompletionParams;
		WDF_REQUEST_COMPLETION_PARAMS_INIT(&CompletionParams);
		WdfRequestGetCompletionParams(myReq, &CompletionParams);
		status = CompletionParams.IoStatus.Status;
		usbCompletionParams = CompletionParams.Parameters.Usb.Completion;
		if (((int)usbCompletionParams->Parameters.PipeWrite.Length) == copy) {
			test = true;
		}
		wrote = copy;

		WdfObjectDelete(myMem);
		WdfObjectDelete(myReq);


		//read

		status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkReadPipe), &myReq);
		status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 0, 0xd, &myMem, &myBufPointer);
		//status = WdfRequestRetrieveOutputMemory(myReq, &myMem);
		status = WdfUsbTargetPipeFormatRequestForRead(pDeviceContext->BulkReadPipe,
			myReq,
			myMem,
			NULL // Offsets
			);
		WDF_REQUEST_SEND_OPTIONS_INIT(&myOpt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS | WDF_REQUEST_SEND_OPTION_TIMEOUT);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&myOpt, WDF_REL_TIMEOUT_IN_SEC(10));
		status = WdfRequestAllocateTimer(myReq);
		if (WdfRequestSend(myReq, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkReadPipe), &myOpt) == FALSE) {
			test = false;
		}
		WDF_REQUEST_COMPLETION_PARAMS_INIT(&CompletionParams);
		WdfRequestGetCompletionParams(myReq, &CompletionParams);
		status = CompletionParams.IoStatus.Status;
		usbCompletionParams = CompletionParams.Parameters.Usb.Completion;
		if (((int)usbCompletionParams->Parameters.PipeRead.Length) == 0xd) {
			test = true;
		}

		WdfObjectDelete(myMem);
		WdfObjectDelete(myReq);

		imagePtr += wrote;
		pos += wrote;
		length -= wrote;

		/* the footer cbw is the same as the header */
		memcpy(f, cbw, sizeof(cbw_t));
		footer->blockSize = flip32(copy);
		footer->index = block;
		footer->u2 = 0x0;

		//sendFooter
		status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myReq);
		status = WdfMemoryCreate(WDF_NO_OBJECT_ATTRIBUTES, PagedPool, 0, sizeof(f), &myMem, &myBufPointer);
		status = WdfMemoryCopyFromBuffer(myMem, 0, &f, sizeof(f));
		status = WdfUsbTargetPipeFormatRequestForWrite(pDeviceContext->BulkWritePipe, myReq, myMem, NULL);
		//WDF_REQUEST_SEND_OPTIONS myOpt;
		WDF_REQUEST_SEND_OPTIONS_INIT(&myOpt, WDF_REQUEST_SEND_OPTION_SYNCHRONOUS | WDF_REQUEST_SEND_OPTION_TIMEOUT);
		WDF_REQUEST_SEND_OPTIONS_SET_TIMEOUT(&myOpt, WDF_REL_TIMEOUT_IN_SEC(10));
		status = WdfRequestAllocateTimer(myReq);
		if (WdfRequestSend(myReq, WdfUsbTargetPipeGetIoTarget(pDeviceContext->BulkWritePipe), &myOpt) == FALSE) {
			test = false;
		}
		//PWDF_USB_REQUEST_COMPLETION_PARAMS usbCompletionParams;
		//WDF_REQUEST_COMPLETION_PARAMS CompletionParams;
		WDF_REQUEST_COMPLETION_PARAMS_INIT(&CompletionParams);
		WdfRequestGetCompletionParams(myReq, &CompletionParams);
		status = CompletionParams.IoStatus.Status;
		usbCompletionParams = CompletionParams.Parameters.Usb.Completion;
		if (((int)usbCompletionParams->Parameters.PipeWrite.Length) == sizeof(f)) {
			test = true;
		}

		WdfObjectDelete(myMem);
		WdfObjectDelete(myReq);
		++block;
	}
	WdfRequestCompleteWithInformation(Request, status, length_income);
	return;
}