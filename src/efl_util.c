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
#include <Elementary.h>
#include <Ecore_X.h>
#include <utilX.h>

API int efl_util_set_window_opaque_state(Evas_Object *window, int opaque)
{
   int ret;
   Ecore_X_Window xwin;

   if(window == NULL)
     {
        return EFL_UTIL_ERROR_INVALID_PARAMETER;
     }

   xwin = elm_win_xwindow_get(window);

   if (opaque == 0)
     ret = utilx_set_window_opaque_state(ecore_x_display_get(), xwin, UTILX_OPAQUE_STATE_OFF);
   else if (opaque == 1)
     ret = utilx_set_window_opaque_state(ecore_x_display_get(), xwin, UTILX_OPAQUE_STATE_ON);
   else
     return EFL_UTIL_ERROR_INVALID_PARAMETER;

   if (!ret)
     return EFL_UTIL_ERROR_INVALID_PARAMETER;
   else
     return EFL_UTIL_ERROR_NONE;
}

