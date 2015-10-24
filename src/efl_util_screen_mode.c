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


#define LOG_TAG "TIZEN_N_EFL_UTIL"

#include <efl_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Elementary.h>
#include <Ecore_X.h>

#define _STR_CMP(a, b) (strncmp(a, b, strlen(b)) == 0)

typedef struct _screen_mode_error_cb_info
{
   Evas_Object *window;
   efl_util_window_screen_mode_error_cb err_cb;
   void *user_data;
} screen_mode_error_cb_info;

Eina_List *_g_screen_mode_error_cb_info_list;
static Ecore_Event_Handler* _screen_mode_access_result_handler = NULL;
static int _screen_mode_handler_count = 0;
static Ecore_X_Atom _screen_mode_access_result_atom;

static Eina_Bool _efl_util_client_message(void *data, int type, void *event);
static screen_mode_error_cb_info *_screen_mode_error_cb_info_find_by_xwin(Ecore_X_Window xwin);
static screen_mode_error_cb_info *_screen_mode_error_cb_info_find(Evas_Object *window);
static Eina_Bool _efl_util_screen_mode_info_add(Evas_Object *window, efl_util_window_screen_mode_error_cb callback, void *user_data);
static Eina_Bool _efl_util_screen_mode_info_del(Evas_Object *window);


API int efl_util_set_window_screen_mode(Evas_Object *window, efl_util_screen_mode_e mode)
{
   Evas *e;
   Ecore_Evas *ee;
   int id;

   if(window == NULL)
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   e = evas_object_evas_get(window);
   if (!e) return EFL_UTIL_ERROR_INVALID_PARAMETER;
   ee = ecore_evas_ecore_evas_get(e);
   if (!ee) return EFL_UTIL_ERROR_INVALID_PARAMETER;

   id = ecore_evas_aux_hint_id_get(ee, "wm.policy.win.lcd.lock");
   if (mode == EFL_UTIL_SCREEN_MODE_ALWAYS_ON)
     {
        if (id == -1) ecore_evas_aux_hint_add(ee, "wm.policy.win.lcd.lock", "1");
        else ecore_evas_aux_hint_val_set(ee, id, "1");
     }
   else if (mode == EFL_UTIL_SCREEN_MODE_DEFAULT)
     {
        if (id == -1) ecore_evas_aux_hint_add(ee, "wm.policy.win.lcd.lock", "0");
        else ecore_evas_aux_hint_val_set(ee, id, "0");
     }
   else
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   return EFL_UTIL_ERROR_NONE;
}



API int efl_util_get_window_screen_mode(Evas_Object *window, efl_util_screen_mode_e *mode)
{
   Evas *e;
   Ecore_Evas *ee;
   const char *value_str;
   int id;

   if(window == NULL || mode == NULL)
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   e = evas_object_evas_get(window);
   if (!e) return EFL_UTIL_ERROR_INVALID_PARAMETER;
   ee = ecore_evas_ecore_evas_get(e);
   if (!ee) return EFL_UTIL_ERROR_INVALID_PARAMETER;

   id = ecore_evas_aux_hint_id_get(ee, "wm.policy.win.lcd.lock");
   if (id == -1)
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   value_str = ecore_evas_aux_hint_val_get(ee, id);
   if (value_str == NULL)
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   if (_STR_CMP(value_str, "1"))
     {
        *mode = EFL_UTIL_SCREEN_MODE_ALWAYS_ON;
     }
   else
     {
        *mode = EFL_UTIL_SCREEN_MODE_DEFAULT;
     }

   return EFL_UTIL_ERROR_NONE;
}



API int efl_util_set_window_screen_mode_error_cb(Evas_Object *window, efl_util_window_screen_mode_error_cb callback, void *user_data)
{
   Eina_Bool ret = EINA_FALSE;

   if (!window) return EFL_UTIL_ERROR_INVALID_PARAMETER;

   ret = _efl_util_screen_mode_info_add(window, callback, user_data);
   if (ret)
     {
        if (!_screen_mode_access_result_atom)
          _screen_mode_access_result_atom = ecore_x_atom_get("_E_SCREEN_MODE_ACCESS_RESULT");

        if (!_screen_mode_access_result_handler)
          _screen_mode_access_result_handler = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, _efl_util_client_message, NULL);
        _screen_mode_handler_count++;

        return EFL_UTIL_ERROR_NONE;
     }
   else
     {
        return EFL_UTIL_ERROR_OUT_OF_MEMORY;
     }
}



API int efl_util_unset_window_screen_mode_error_cb(Evas_Object *window)
{
   Eina_Bool ret = EINA_FALSE;

   if (!window) return EFL_UTIL_ERROR_INVALID_PARAMETER;

   ret = _efl_util_screen_mode_info_del(window);
   if (ret)
     {
        _screen_mode_handler_count--;
        if (_screen_mode_handler_count == 0)
          {
             if (_screen_mode_access_result_handler)
               {
                  ecore_event_handler_del(_screen_mode_access_result_handler);
                  _screen_mode_access_result_handler = NULL;
               }
          }
        return EFL_UTIL_ERROR_NONE;
     }
   else
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }
}



static Eina_Bool _efl_util_client_message(void *data, int type, void *event)
{
   Ecore_X_Event_Client_Message *ev;

   ev = event;
   if (!ev) return ECORE_CALLBACK_PASS_ON;

   if (ev->message_type == _screen_mode_access_result_atom)
     {
        Ecore_X_Window xwin;
        xwin = ev->win;

        screen_mode_error_cb_info *cb_info = NULL;
        cb_info = _screen_mode_error_cb_info_find_by_xwin(xwin);
        if (cb_info)
          {
             int access = ev->data.l[1];
             if (access == 0) // permission denied
               {
                  if (cb_info->err_cb)
                    {
                       cb_info->err_cb(cb_info->window, EFL_UTIL_ERROR_PERMISSION_DENIED, cb_info->user_data);
                    }
               }
          }
     }

   return ECORE_CALLBACK_PASS_ON;
}



static screen_mode_error_cb_info *_screen_mode_error_cb_info_find_by_xwin(Ecore_X_Window xwin)
{
   Eina_List *l;
   screen_mode_error_cb_info* temp;
   Ecore_X_Window temp_xwin;

   EINA_LIST_FOREACH(_g_screen_mode_error_cb_info_list, l, temp)
     {
        if (temp->window)
          {
             temp_xwin = elm_win_xwindow_get(temp->window);
             if (xwin == temp_xwin)
               {
                  return temp;
               }
          }
     }

   return NULL;
}



static screen_mode_error_cb_info *_screen_mode_error_cb_info_find(Evas_Object *window)
{
   Eina_List *l;
   screen_mode_error_cb_info* temp;

   EINA_LIST_FOREACH(_g_screen_mode_error_cb_info_list, l, temp)
     {
        if (temp->window == window)
          {
             return temp;
          }
     }

   return NULL;
}



static Eina_Bool _efl_util_screen_mode_info_add(Evas_Object *window, efl_util_window_screen_mode_error_cb callback, void *user_data)
{
   screen_mode_error_cb_info* _err_info = _screen_mode_error_cb_info_find(window);

   if (_err_info)
     {
        _g_screen_mode_error_cb_info_list = eina_list_remove(_g_screen_mode_error_cb_info_list, _err_info);
        free(_err_info);
        _err_info = NULL;
     }

   _err_info = (screen_mode_error_cb_info*)calloc(1, sizeof(screen_mode_error_cb_info));
   if (!_err_info)
     {
        return EINA_FALSE;
     }
   _err_info->window = window;
   _err_info->err_cb = callback;
   _err_info->user_data = user_data;

   _g_screen_mode_error_cb_info_list = eina_list_append(_g_screen_mode_error_cb_info_list, _err_info);

   return EINA_TRUE;
}



static Eina_Bool _efl_util_screen_mode_info_del(Evas_Object *window)
{
   screen_mode_error_cb_info* _err_info = _screen_mode_error_cb_info_find(window);
   if (!_err_info)
     {
        return EINA_FALSE;
     }

   _g_screen_mode_error_cb_info_list = eina_list_remove(_g_screen_mode_error_cb_info_list, _err_info);
   free(_err_info);

   return EINA_TRUE;
}

