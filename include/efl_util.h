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


#ifndef __TIZEN_UI_EFL_UTIL_H__
#define __TIZEN_UI_EFL_UTIL_H__

#include <tizen.h>
#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup CAPI_EFL_UTIL_MODULE
 * @{
 */


/**
 * @brief Enumerations of error code for EFL UTIL
 */
typedef enum
{
	EFL_UTIL_ERROR_NONE = TIZEN_ERROR_NONE,	/**< Successful */
	EFL_UTIL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE = TIZEN_ERROR_APPLICATION_CLASS | 0x08  /**< Not supported window type */
} efl_util_error_e;


/**                                                 
 * @brief Enumeration of notification window's priority level
 * 
 */
typedef enum
{
	EFL_UTIL_NOTIFICATION_LEVEL_1, /**< Default notification level*/
	EFL_UTIL_NOTIFICATION_LEVEL_2, /**< Higher notification level than default*/
} efl_util_notification_level_e; 



/**
 * @brief Sets the priority level for the specified notification window, asynchronously.
 *
 * @remark This API can be used for notification type window only
 * @param [in] window EFL window 
 * @param [in] level The notification window level
 * @return 0 on success, otherwise a negative error value.
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Not supported window type
 */
int efl_util_set_notification_window_level (Evas_Object *window, efl_util_notification_level_e level);


/**
 * @brief Gets the priority level for the specified notification window, asynchronously.
 *
 * @remark This API can be used for notification type window only
 * @param [in] window EFL window 
 * @param [out] level The notification window level
 * @return 0 on success, otherwise a negative error value.
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Not supported window type
 */
int efl_util_get_notification_window_level (Evas_Object *window, efl_util_notification_level_e* level);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif
#endif				/* __TIZEN_UI_EFL_UTIL_H__ */
