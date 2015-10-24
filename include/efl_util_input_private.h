/*
 * Copyright (c) 2011-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_UI_EFL_UTIL_PRIVATE_H__
#define __TIZEN_UI_EFL_UTIL_PRIVATE_H__

#include <efl_util.h>
#include <Ecore_X.h>
#include <X11/Xlib.h>
#include <X11/extensions/XInput.h>

#define TOUCH_DEVICE_NAME "Virtual core XTEST touch"
#define HWKEY_DEVICE_NAME "Virtual core XTEST functionkeys"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
	Ecore_X_Display *dpy;
	int devid_key;
	int devid_touch;
	XDevice *devInfo_key;
	XDevice *devInfo_touch;
}_input_generate_info;

struct _efl_util_inputgen_h
{
   _input_generate_info input_generate_info;
};

static int _efl_util_input_initialize_xi2_system(Ecore_X_Display *dpy, int *opcode, int *event, int *error);
static int _efl_util_input_initialize_input_generator(efl_util_inputgen_h inputgen_h);
static void _efl_util_input_get_device_id(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e devtype);
static int _efl_util_input_open_generate_device(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e devtype);
static void _efl_util_input_close_generate_device(efl_util_inputgen_h inputgen_h);
static int _efl_util_input_keyname_to_keycode(Ecore_X_Display *dpy, const char * name);
static int _efl_util_input_generate_key_event(Ecore_X_Display *dpy, XDevice *key_device_info, int keycode, int pressed);
static int _efl_util_input_generate_touch_event(Ecore_X_Display *dpy, XDevice *touch_device_info, int touchid, efl_util_input_touch_type_e type, int x, int y);
static int _efl_util_input_find_all_request_devices(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e request_type);
static void _efl_util_input_deinitialize_input_generator(efl_util_inputgen_h inputgen_h);

#ifdef __cplusplus
}
#endif

#endif // __TIZEN_UI_EFL_UTIL_PRIVATE_H__
