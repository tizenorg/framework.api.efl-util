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


#define LOG_TAG "TIZEN_N_EFL_UTIL_INPUT"

#include <efl_util.h>
#include <efl_util_input_private.h>
#include <string.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput2.h>

API efl_util_inputgen_h
efl_util_input_initialize_generator(efl_util_input_device_type_e dev_type)
{
   int res = EFL_UTIL_ERROR_NONE;
   efl_util_inputgen_h inputgen_h;

   inputgen_h = (efl_util_inputgen_h)calloc(1, sizeof(struct _efl_util_inputgen_h));
   if (!inputgen_h)
     {
        fprintf(stderr, "Failed to allocate memory for efl_util_inputgen_h\n");
        set_last_result(EFL_UTIL_ERROR_OUT_OF_MEMORY);
        goto out;
     }

   /* initialize system */
   res = _efl_util_input_initialize_input_generator(inputgen_h);
   if (EFL_UTIL_ERROR_NONE != res)
     {
        fprintf(stderr, "Failed to initialize input generator system\n");
        set_last_result(res);
        free(inputgen_h);
        inputgen_h = NULL;
        goto out;
     }

   /* open a touch device */
   _efl_util_input_get_device_id(inputgen_h, dev_type);
   res = _efl_util_input_open_generate_device(inputgen_h, dev_type);
   if (EFL_UTIL_ERROR_NONE != res)
     {
        fprintf(stderr, "Failed to find device\n");
        set_last_result(res);
        free(inputgen_h);
        inputgen_h = NULL;
        goto out;
     }

   set_last_result(res);

out:
   return inputgen_h;
}

API int
efl_util_input_deinitialize_generator(efl_util_inputgen_h inputgen_h)
{
   if (!inputgen_h)
     {
         fprintf(stderr, "No initialized efl_util_inputgen_h\n");
         return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   _efl_util_input_deinitialize_input_generator(inputgen_h);

   return EFL_UTIL_ERROR_NONE;
}

API int
efl_util_input_generate_key(efl_util_inputgen_h inputgen_h, const char *key_name, int pressed)
{
   int keycode = 0;

   if (!inputgen_h)
     {
         fprintf(stderr, "No initialized efl_util_inputgen_h\n");
         return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   /* get a keycode using keysym */
   keycode = _efl_util_input_keyname_to_keycode(inputgen_h->input_generate_info.dpy, key_name);
   if (0 >= keycode)
     {
        fprintf(stderr, "Invalide keyname(%s) keycode(%d)\n", key_name, keycode);
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   /* send a key event */
   return _efl_util_input_generate_key_event(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devInfo_key, keycode, pressed);
}

API int
efl_util_input_generate_touch(efl_util_inputgen_h inputgen_h, int idx, efl_util_input_touch_type_e touch_type, int x, int y)
{
   if (!inputgen_h)
     {
         fprintf(stderr, "No initialized efl_util_inputgen_h\n");
         return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   if (idx < 0 || x < 0 || y < 0)
     return EFL_UTIL_ERROR_INVALID_PARAMETER;
   if (touch_type <= EFL_UTIL_INPUT_TOUCH_NONE || touch_type >= EFL_UTIL_INPUT_TOUCH_MAX)
     return EFL_UTIL_ERROR_INVALID_PARAMETER;

   return _efl_util_input_generate_touch_event(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devInfo_touch, idx, touch_type, x, y);
}

static int
_efl_util_input_initialize_xi2_system(Ecore_X_Display *dpy, int *opcode, int *event, int *error)
{
   int major=XI_2_Major, minor=XI_2_Minor;

   /* X Input Extension available? */
   if(!XQueryExtension((Display *)dpy, "XInputExtension", opcode, event, error))
     {
        fprintf(stderr, "X Input extension not available\n");
        return 0;
     }

   /* Which version of XI2? We support 2.0 */
   if (XIQueryVersion((Display *)dpy, &major, &minor) == BadRequest)
     {
        fprintf(stderr, "XI2 not available. Server supports %d.%d\n", major, minor);
        return 0;
     }

   return 1;
}

static int
_efl_util_input_initialize_input_generator(efl_util_inputgen_h inputgen_h)
{
   int opcode=0, event=0, error=0;
   /* Init the ecore_x system */
   if(!ecore_x_init(NULL)) goto out;

   /* Init global struct valuables */
   inputgen_h->input_generate_info.dpy = NULL;
   inputgen_h->input_generate_info.devInfo_key = NULL;
   inputgen_h->input_generate_info.devInfo_touch = NULL;
   inputgen_h->input_generate_info.devid_key = 0;
   inputgen_h->input_generate_info.devid_touch = 0;

   /* get display id */
   inputgen_h->input_generate_info.dpy= ecore_x_display_get();
   if(!inputgen_h->input_generate_info.dpy)
     {
        fprintf(stderr, "failed to get dpy\n");
        goto out;
     }

   /* initialize xi2 system */
   if(!_efl_util_input_initialize_xi2_system(inputgen_h->input_generate_info.dpy, &opcode, &event, &error))
     {
        fprintf(stderr, "failed to initialize xi2 system\n");
        goto out;
     }

   return EFL_UTIL_ERROR_NONE;
out:
   return EFL_UTIL_ERROR_INVALID_OPERATION;
}

/* query X devices and find wanted device's id (such as a touch device and a hwkey device) */
static void
_efl_util_input_get_device_id(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e devtype)
{
   XIDeviceInfo *info=NULL;
   int i, ndevices;

   if (devtype <= EFL_UTIL_INPUT_DEVTYPE_NONE || EFL_UTIL_INPUT_DEVTYPE_MAX <= devtype)
     {
        fprintf(stderr, "Invalid devtype(%d)\n", devtype);
        return;
     }

   info = XIQueryDevice(inputgen_h->input_generate_info.dpy, XIAllDevices, &ndevices);

   for(i=0; i<ndevices; i++)
     {
        XIDeviceInfo *dev = &info[i];
        switch(dev->use)
          {
           case XISlavePointer:
              if ((0 == inputgen_h->input_generate_info.devid_touch) &&
                 (devtype & EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN) &&
                 !strncmp(dev->name, TOUCH_DEVICE_NAME, strlen(TOUCH_DEVICE_NAME)))
                {
                   fprintf(stderr, "pointer device id: %d, name: %s, attachment: %d\n", dev->deviceid, dev->name, dev->attachment);
                   inputgen_h->input_generate_info.devid_touch = dev->deviceid;

                   if (_efl_util_input_find_all_request_devices(inputgen_h, devtype))
                     {
                        goto finish;
                     }
                }
              break;
           case XISlaveKeyboard:
              if ((devtype & EFL_UTIL_INPUT_DEVTYPE_KEYBOARD) &&
                 !strncmp(dev->name, HWKEY_DEVICE_NAME, strlen(HWKEY_DEVICE_NAME)))
                {
                   fprintf(stderr, "keyboard device id: %d, name: %s, attachment: %d\n", dev->deviceid, dev->name, dev->attachment);
                   inputgen_h->input_generate_info.devid_key = dev->deviceid;

                   if (_efl_util_input_find_all_request_devices(inputgen_h, devtype))
                     {
                        goto finish;
                     }
                }
              break;
           case XIFloatingSlave:
              if ((devtype & EFL_UTIL_INPUT_DEVTYPE_KEYBOARD) &&
                 !strncmp(dev->name, HWKEY_DEVICE_NAME, strlen(HWKEY_DEVICE_NAME)))
                {
                   fprintf(stderr, "floating device id: %d, name: %s, attachment: %d\n", dev->deviceid, dev->name, dev->attachment);
                   inputgen_h->input_generate_info.devid_key = dev->deviceid;

                   if (_efl_util_input_find_all_request_devices(inputgen_h, devtype))
                     {
                        goto finish;
                     }
                }
              break;
           default:
              break;
          }
     }

finish:
   if (info) free(info);
}

static int
_efl_util_input_open_generate_device(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e devtype)
{
   XDevice *tmp_device_key = NULL, *tmp_device_touch = NULL;
   int res = EFL_UTIL_ERROR_NO_SUCH_DEVICE;

   if (devtype & EFL_UTIL_INPUT_DEVTYPE_KEYBOARD)
     {
        tmp_device_key = XOpenDevice(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devid_key);
        if (!tmp_device_key)
          {
             fprintf(stderr, "failed to open device (id: %d)\n", inputgen_h->input_generate_info.devid_key);
             res = EFL_UTIL_ERROR_NO_SUCH_DEVICE;
             goto out;
          }
        else
          {
             inputgen_h->input_generate_info.devInfo_key = tmp_device_key;
             res = EFL_UTIL_ERROR_NONE;
          }
     }

   if (devtype & EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN)
     {
        tmp_device_touch = XOpenDevice(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devid_touch);
        if (!tmp_device_touch)
          {
             fprintf(stderr, "failed to open device (id: %d)\n", inputgen_h->input_generate_info.devid_touch);
             res = EFL_UTIL_ERROR_NO_SUCH_DEVICE;
             goto out;
          }
        else
          {
             inputgen_h->input_generate_info.devInfo_touch = tmp_device_touch;
             res = EFL_UTIL_ERROR_NONE;
          }
     }

out:
   return res;
}

static void
_efl_util_input_close_generate_device(efl_util_inputgen_h inputgen_h)
{
   if (inputgen_h->input_generate_info.devInfo_key)
     {
        XCloseDevice(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devInfo_key);
        inputgen_h->input_generate_info.devInfo_key = NULL;
        inputgen_h->input_generate_info.devid_key = 0;
     }
   if (inputgen_h->input_generate_info.devInfo_touch)
     {
        XCloseDevice(inputgen_h->input_generate_info.dpy, inputgen_h->input_generate_info.devInfo_touch);
        inputgen_h->input_generate_info.devInfo_touch = NULL;
        inputgen_h->input_generate_info.devid_touch = 0;
     }
}

static int
_efl_util_input_keyname_to_keycode(Ecore_X_Display *dpy, const char * name)
{
   int keycode;
   KeySym keysym;

   if (!strncmp(name, "Keycode-", 8))
     {
        keycode = atoi(name + 8);
     }
   else
     {
        keysym = XStringToKeysym(name);
        if (keysym == NoSymbol) return 0;
        keycode = XKeysymToKeycode((Display *)dpy, XStringToKeysym(name));
     }

   return keycode;
}

static int
_efl_util_input_generate_key_event(Ecore_X_Display *dpy, XDevice *key_device_info, int keycode, int pressed)
{
   int res = -1;
   Bool press = (pressed==1)?True:False;

   res = XTestFakeDeviceKeyEvent((Display *)dpy, key_device_info, keycode, press, NULL, 0, CurrentTime);
   XSync((Display *)dpy, False);

   if (res == 1)
     {
        return EFL_UTIL_ERROR_NONE;
     }
   return EFL_UTIL_ERROR_PERMISSION_DENIED;
}

static int
_efl_util_input_generate_touch_event(Ecore_X_Display *dpy, XDevice *touch_device_info, int touchid, efl_util_input_touch_type_e type, int x, int y)
{
   int touch_axis[2]={0,0};
   int res = -1;

   touch_axis[0] = x;
   touch_axis[1] = y;

   switch(type)
     {
      case EFL_UTIL_INPUT_TOUCH_BEGIN:
         res =  XTestFakeDeviceTouchEvent(dpy, touch_device_info, touchid, True, touch_axis, 2, CurrentTime);
         break;
      case EFL_UTIL_INPUT_TOUCH_UPDATE:
         res  = XTestFakeDeviceTouchUpdateEvent(dpy, touch_device_info, touchid, touch_axis, 2, CurrentTime);
         break;
      case EFL_UTIL_INPUT_TOUCH_END:
         res = XTestFakeDeviceTouchEvent(dpy, touch_device_info, touchid, False, touch_axis, 2, CurrentTime);
         break;
      default:
         fprintf(stderr, "Invalid touch type (%d)\n", type);
         break;
     }
  XSync(dpy, False);

  if (res == 1)
    {
       return EFL_UTIL_ERROR_NONE;
    }
  return EFL_UTIL_ERROR_PERMISSION_DENIED;
}

static int
_efl_util_input_find_all_request_devices(efl_util_inputgen_h inputgen_h, efl_util_input_device_type_e request_type)
{
   if (EFL_UTIL_INPUT_DEVTYPE_KEYBOARD & request_type)
     {
        if (inputgen_h->input_generate_info.devid_key == 0)
          {
             return 0;
          }
     }

   if (EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN & request_type)
     {
        if (inputgen_h->input_generate_info.devid_touch == 0)
          {
             return 0;
          }
     }

   return 1;
}

static void
_efl_util_input_deinitialize_input_generator(efl_util_inputgen_h inputgen_h)
{
   _efl_util_input_close_generate_device(inputgen_h);
   free(inputgen_h);
   inputgen_h = NULL;

   ecore_x_shutdown();
}
