/**********************************************************************
 * Copyright (C) 2012 Al Niessner
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 *====================================================================
 *
 * Module Description:
 *
 *********************************************************************/

#pragma code

#include "usb.h"
#include "USB/usb.h"
#include "USB/usb_function_generic.h"

void usb_initialize(void)
{
  USBDeviceInit();
  USBDeviceAttach();
}

void usb_process(void)
{
  if ((USBGetDeviceState() < CONFIGURED_STATE) || USBIsDeviceSuspended())
    { /* NO OP */ }
  else
    {
    }
}

void usb_respond(void)
{
}

