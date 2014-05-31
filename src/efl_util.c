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


#define LOG_TAG "TIZEN_N_EFL_UTIL"

#include <efl_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include <utilX.h>


int efl_util_set_notification_window_level (Evas_Object* window, efl_util_notification_level_e level)
{
	Ecore_X_Window_Type window_type;
	
	if(window == NULL)
	{
		return EFL_UTIL_ERROR_INVALID_PARAMETER;
	}

	if(level < EFL_UTIL_NOTIFICATION_LEVEL_1 || level > EFL_UTIL_NOTIFICATION_LEVEL_2)
	{
		return EFL_UTIL_ERROR_INVALID_PARAMETER;
	}
	
	Ecore_X_Window xwin = elm_win_xwindow_get(window);

	if(ecore_x_netwm_window_type_get(xwin, &window_type) == EINA_TRUE)
	{
		// success to get window type
		if(window_type != ECORE_X_WINDOW_TYPE_NOTIFICATION)
		{
			// given EFL window's type is not notification type.
			return EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE;
		}
	}
	else
	{
		// fail to get window type
		return EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE;
	}
	
	// this api doesn't have return type
	if(level == EFL_UTIL_NOTIFICATION_LEVEL_1) {
		utilx_set_system_notification_level(ecore_x_display_get(), xwin, UTILX_NOTIFICATION_LEVEL_LOW);
	}
	else if(level == EFL_UTIL_NOTIFICATION_LEVEL_2)
	{
		utilx_set_system_notification_level(ecore_x_display_get(), xwin, UTILX_NOTIFICATION_LEVEL_NORMAL);
	}
	
	return EFL_UTIL_ERROR_NONE;
}



int efl_util_get_notification_window_level (Evas_Object* window, efl_util_notification_level_e* level)
{
	Ecore_X_Window_Type window_type;

	 Utilx_Notification_Level utilx_level;
	
	if(window == NULL)
	{
		return EFL_UTIL_ERROR_INVALID_PARAMETER;
	}


	Ecore_X_Window xwin = elm_win_xwindow_get(window);

	if(ecore_x_netwm_window_type_get(xwin, &window_type) == EINA_TRUE)
	{
		// success to get window type
		if(window_type != ECORE_X_WINDOW_TYPE_NOTIFICATION)
		{
			// given EFL window's type is not notification type.
			return EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE;
		}
		
		utilx_level = utilx_get_system_notification_level (ecore_x_display_get(), xwin);

		if(utilx_level == UTILX_NOTIFICATION_LEVEL_LOW)
		{
			*level = EFL_UTIL_NOTIFICATION_LEVEL_1;
		}
		else if(utilx_level == UTILX_NOTIFICATION_LEVEL_NORMAL)
		{
			*level = EFL_UTIL_NOTIFICATION_LEVEL_2;
		}
		else if(utilx_level == UTILX_NOTIFICATION_LEVEL_HIGH)
		{
			*level = EFL_UTIL_NOTIFICATION_LEVEL_2;
		}
		else
		{
			return EFL_UTIL_ERROR_INVALID_PARAMETER;
		}
		
	}
	else
	{
		// fail to get window type
		return EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE;
	}
	
	return EFL_UTIL_ERROR_NONE;
}
