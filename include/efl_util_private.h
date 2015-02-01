/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
#include <Elementary.h>
#include <Ecore_X.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _notification_error_cb_info
{
	Evas_Object *window;
	efl_util_notification_window_level_error_cb err_cb;
	void *user_data;
} notification_error_cb_info;

Eina_List *_g_notification_error_cb_info_list;
static Ecore_Event_Handler* _noti_level_access_result_handler = NULL;
static int _noti_handler_count = 0;
static Ecore_X_Atom _noti_level_access_result_atom;

static Eina_Bool _efl_util_client_message(void *data, int type, void *event);
static notification_error_cb_info *_notification_error_cb_info_find_by_xwin(Ecore_X_Window xwin);
static notification_error_cb_info *_notification_error_cb_info_find(Evas_Object *window);
static Eina_Bool _efl_util_notification_info_add(Evas_Object *window, efl_util_notification_window_level_error_cb callback, void *user_data);
static Eina_Bool _efl_util_notification_info_del(Evas_Object *window);


#ifdef __cplusplus
}
#endif

#endif // __TIZEN_UI_EFL_UTIL_PRIVATE_H__
