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
 * @file efl_util.h
 */

/**
 * @addtogroup CAPI_EFL_UTIL_MODULE
 * @{
 */


/**
 * @brief Enumeration for EFL UTIL ERROR.
 * @since_tizen 2.3
 */
typedef enum
{
	EFL_UTIL_ERROR_NONE = TIZEN_ERROR_NONE,	/**< Successful */
	EFL_UTIL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	EFL_UTIL_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
	EFL_UTIL_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permisson denied */
	EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE = TIZEN_ERROR_EFL_UTIL | 0x01  /**< Window type not supported */
} efl_util_error_e;


/**
 * @brief Enumeration of notification window's priority level.
 * @since_tizen 2.3
 */
typedef enum
{
	EFL_UTIL_NOTIFICATION_LEVEL_1, /**< Default notification level */
	EFL_UTIL_NOTIFICATION_LEVEL_2, /**< Higher notification level than default */
	EFL_UTIL_NOTIFICATION_LEVEL_3, /**< The highest notification level */
} efl_util_notification_level_e;



/**
 * @brief Sets the priority level for the specified notification window, asynchronously.
 * @since_tizen 2.3
 * @privlevel public
 * @privilege %http://tizen.org/privilege/window.priority.set
 * @remarks This API can be used for a notification type window only.
 * @param[in] window The EFL window
 * @param[in] level The notification window level
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Window type not supported
 */
int efl_util_set_notification_window_level (Evas_Object *window, efl_util_notification_level_e level);


/**
 * @brief Gets the priority level for the specified notification window, asynchronously.
 * @since_tizen 2.3
 *
 * @remarks This API can be used for a notification type window only.
 * @param[in] window The EFL window
 * @param[out] level The notification window level
 * @return @c 0 on success,
 *         otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Window type not supported
 */
int efl_util_get_notification_window_level (Evas_Object *window, efl_util_notification_level_e* level);


/**
 * @brief Called when an error occurs for setting notification window level
 * @since_tizen 2.3
 * @param[in]	window	The EFL window
 * @param[in]	error_code	The error code (#EFL_UTIL_ERROR_PERMISSION_DENIED)
 * @param[in]	user_data	The user data passed from the callback registration function
 * @see efl_util_set_notification_window_level_error_cb()
 * @see efl_util_unset_notification_window_level_error_cb()
 */
typedef void (*efl_util_notification_window_level_error_cb)(Evas_Object *window, int error_code, void *user_data);


/**
 * @brief Registers a callback function to be invoked when an error which set the notification level occurs.
 * @since_tizen 2.3
 * @param[in] window	The EFL window
 * @param[in] callback	The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success,
 *		   otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_OUT_OF_MEMORY Out of memory
 * @post  efl_util_notification_window_level_error_cb() will be invoked.
 * @see efl_util_unset_notification_window_level_error_cb()
 * @see efl_util_notification_window_level_error_cb()
 */
int efl_util_set_notification_window_level_error_cb(Evas_Object *window, efl_util_notification_window_level_error_cb callback, void *user_data);


/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.3
 * @param[in] window The EFL window
 * @return @c 0 on success,
 *		   otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @see efl_util_set_notification_window_level_error_cb()
 */
int efl_util_unset_notification_window_level_error_cb(Evas_Object *window);


/**
 * @}
 */


#ifdef __cplusplus
}
#endif
#endif				/* __TIZEN_UI_EFL_UTIL_H__ */
