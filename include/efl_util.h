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

#ifndef __TIZEN_UI_EFL_UTIL_H__
#define __TIZEN_UI_EFL_UTIL_H__

#include <tizen.h>
#include <Evas.h>
#include <tbm_surface.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
# if __GNUC__ >= 4
#  ifndef API
#   define API __attribute__ ((visibility("default")))
#  endif
# endif
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
   EFL_UTIL_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
   EFL_UTIL_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
   EFL_UTIL_ERROR_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY, /**< Out of memory */
   EFL_UTIL_ERROR_PERMISSION_DENIED = TIZEN_ERROR_PERMISSION_DENIED, /**< Permisson denied */
   EFL_UTIL_ERROR_NO_SUCH_DEVICE = TIZEN_ERROR_NO_SUCH_DEVICE, /**< @platform No such device or address (@b Since: 2.4) */
   EFL_UTIL_ERROR_INVALID_OPERATION = TIZEN_ERROR_INVALID_OPERATION, /**< @platform Function not implemented (@b Since: 2.4) */
   EFL_UTIL_ERROR_NOT_SUPPORTED = TIZEN_ERROR_NOT_SUPPORTED, /**< @platform Not supported (@b Since: 2.4) */
   EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE = TIZEN_ERROR_EFL_UTIL | 0x01, /**< Window type not supported */
   EFL_UTIL_ERROR_SCREENSHOT_INIT_FAIL = TIZEN_ERROR_EFL_UTIL | 0x02,  /**< @platform Screenshot initialization fail (@b Since: 2.4) */
   EFL_UTIL_ERROR_SCREENSHOT_EXECUTION_FAIL = TIZEN_ERROR_EFL_UTIL | 0x03  /**< @platform Screenshot execution fail (@b Since: 2.4) */
} efl_util_error_e;

/**
 * @brief Enumeration of notification window's priority level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
   EFL_UTIL_NOTIFICATION_LEVEL_1, /**< Default notification level. (Deprecated since 2.4. Use EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT instead.) */
   EFL_UTIL_NOTIFICATION_LEVEL_2, /**< Higher notification level than default. (Deprecated since 2.4. Use EFL_UTIL_NOTIFICATION_LEVEL_MEDIUM instead.) */
   EFL_UTIL_NOTIFICATION_LEVEL_3, /**< The highest notification level. (Deprecated since 2.4. Use EFL_UTIL_NOTIFICATION_LEVEL_TOP instead.) */
   EFL_UTIL_NOTIFICATION_LEVEL_NONE    = -1, /**< No (reset) notification level. This value makes the window place in normal layer. (@b Since: 2.4) */
   EFL_UTIL_NOTIFICATION_LEVEL_DEFAULT = 10, /**< Default notification level. (@b Since: 2.4) */
   EFL_UTIL_NOTIFICATION_LEVEL_MEDIUM  = 20, /**< Higher notification level than default. (@b Since: 2.4) */
   EFL_UTIL_NOTIFICATION_LEVEL_HIGH    = 30, /**< Higher notification level than medium. (@b Since: 2.4) */
   EFL_UTIL_NOTIFICATION_LEVEL_TOP     = 40  /**< The highest notification level. (@b Since: 2.4) */
} efl_util_notification_level_e;

/**
 * @brief Enumeration of screen mode.
 * @since_tizen 2.4
 */
typedef enum
{
   EFL_UTIL_SCREEN_MODE_DEFAULT, /**< The mode which turns the screen off after a timeout. */
   EFL_UTIL_SCREEN_MODE_ALWAYS_ON, /**< The mode which keeps the screen turned on. */
} efl_util_screen_mode_e;

/**
 * @brief Sets the priority level for the specified notification window, asynchronously.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/window.priority.set
 * @remarks This API can be used for a notification type window only.
 * @param[in] window The EFL window
 * @param[in] level The notification window level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Window type not supported
 */
API int efl_util_set_notification_window_level(Evas_Object *window, efl_util_notification_level_e level);

/**
 * @brief Gets the priority level for the specified notification window, asynchronously.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks This API can be used for a notification type window only.
 * @param[in] window The EFL window
 * @param[out] level The notification window level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE Window type not supported
 */
API int efl_util_get_notification_window_level(Evas_Object *window, efl_util_notification_level_e *level);

/**
 * @brief Called when an error occurs for setting notification window level
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] window The EFL window
 * @param[in] error_code The error code (#EFL_UTIL_ERROR_PERMISSION_DENIED)
 * @param[in] user_data The user data passed from the callback registration function
 * @see efl_util_set_notification_window_level_error_cb()
 * @see efl_util_unset_notification_window_level_error_cb()
 */
typedef void (*efl_util_notification_window_level_error_cb)(Evas_Object *window, int error_code, void *user_data);

/**
 * @brief Registers a callback function to be invoked when an error which set the notification level occurs.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] window The EFL window
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_OUT_OF_MEMORY Out of memory
 * @post efl_util_notification_window_level_error_cb() will be invoked.
 * @see efl_util_unset_notification_window_level_error_cb()
 * @see efl_util_notification_window_level_error_cb()
 */
API int efl_util_set_notification_window_level_error_cb(Evas_Object *window, efl_util_notification_window_level_error_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] window The EFL window
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @see efl_util_set_notification_window_level_error_cb()
 */
API int efl_util_unset_notification_window_level_error_cb(Evas_Object *window);

/**
 * @brief Sets the alpha window's visual state to opaque state
 * @details This API sets the alpha window's visual state to opaque state.
 *          If the alpha window sets the visual state to the opaque,
 *          then the window manager could handle it as the opaque window while calculating visibility.
 *          This API will have no effect when used by a non-alpha window.
 * @since_tizen 2.4
 * @param[in] window The EFL window
 * @param[in] opaque The value that indicates whether the window has set a visual state to opaque (0: unset, 1: set)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 */
API int efl_util_set_window_opaque_state(Evas_Object *window, int opaque);

/**
 * @brief Sets the window's screen mode.
 * @details This API is useful when the application need to keep the display turned on.
 *          If the application set the mode to #EFL_UTIL_SCREEN_MODE_ALWAYS_ON to its window and the window is shown wholly or partially,
 *          the window manager requests the display system to keep the display on as long as the window is shown.
 *          If the window is no longer shown, then the window manger request the display system to go back to normal operation.
 *          Default screen mode of window is #EFL_UTIL_SCREEN_MODE_DEFAULT.
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/display
 * @remarks This API needs the privilege.
 *          If the application which is not get the privilege use this API, the window manager generates the permission deny error.
 *          The application can notice this error if it set the callback function using the efl_util_set_window_screen_mode_error_cb().
 * @param[in] window The EFL window
 * @param[in] mode The screen mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 */
API int efl_util_set_window_screen_mode(Evas_Object *window, efl_util_screen_mode_e mode);

/**
 * @brief Gets the screen mode of the specified window.
 * @since_tizen 2.4
 * @param[in] window The EFL window
 * @param[out] mode The screen mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 */
API int efl_util_get_window_screen_mode(Evas_Object *window, efl_util_screen_mode_e *mode);

/**
 * @brief Called when an error occurs for setting window's screen mode
 * @since_tizen 2.4
 * @param[in] window The EFL window
 * @param[in] error_code The error code (#EFL_UTIL_ERROR_PERMISSION_DENIED)
 * @param[in] user_data The user data passed from the callback registration function
 * @see efl_util_set_window_screen_mode_error_cb()
 * @see efl_util_unset_window_screen_mode_error_cb()
 */
typedef void (*efl_util_window_screen_mode_error_cb)(Evas_Object *window, int error_code, void *user_data);

/**
 * @brief Registers a callback function to be invoked when an error which set the screen mode.
 * @since_tizen 2.4
 * @param[in] window The EFL window
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_OUT_OF_MEMORY Out of memory
 * @post  efl_util_window_screen_mode_error_cb() will be invoked.
 * @see efl_util_unset_window_screen_mode_error_cb()
 * @see efl_util_window_screen_mode_error_cb()
 */
API int efl_util_set_window_screen_mode_error_cb(Evas_Object *window, efl_util_window_screen_mode_error_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen 2.4
 * @param[in] window The EFL window
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @see efl_util_set_window_screen_mode_error_cb()
 */
API int efl_util_unset_window_screen_mode_error_cb(Evas_Object *window);

/**
 * @}
 */

/**
 * @addtogroup CAPI_EFL_UTIL_INPUT_MODULE
 * @{
 */

 /**
 * @platform
 * @brief Definition for the input generator handle.
 * @since_tizen 2.4
 */

typedef struct _efl_util_inputgen_h *efl_util_inputgen_h;

/**
 * @platform
 * @brief Enumeration of device type generated events.
 * @since_tizen 2.4
 */
typedef enum
{
   EFL_UTIL_INPUT_DEVTYPE_NONE = 0x0,
   EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN = (1 << 0), /**< Touch screen device */
   EFL_UTIL_INPUT_DEVTYPE_KEYBOARD = (1 << 1), /**< Keyboard device */
   EFL_UTIL_INPUT_DEVTYPE_ALL = EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN |
                                EFL_UTIL_INPUT_DEVTYPE_KEYBOARD, /**< Both of touch screen and keyboard device */
   EFL_UTIL_INPUT_DEVTYPE_MAX = (1 << 10)
} efl_util_input_device_type_e;

/**
 * @platform
 * @brief Enumeration of touch event types.
 * @since_tizen 2.4
 */
typedef enum
{
   EFL_UTIL_INPUT_TOUCH_NONE,
   EFL_UTIL_INPUT_TOUCH_BEGIN, /**< Finger press. It is same a behavior put your finger on touch screen */
   EFL_UTIL_INPUT_TOUCH_UPDATE, /**< Finger move. It is same a behavior move your finger on touch screen */
   EFL_UTIL_INPUT_TOUCH_END, /**< Finger release. It is same a behavior release your finger on touch screen */
   EFL_UTIL_INPUT_TOUCH_MAX = 10
} efl_util_input_touch_type_e;

/**
 * @platform
 * @brief Initializes system and check input generate functions are supported, open devices generated events.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/inputgenerator
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] dev_type The device type want to generate events (ex> EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN, EFL_UTIL_INPUT_DEVTYPE_KEYBOARD, EFL_UTIL_INPUT_DEVTYPE_ALL)
 * @return #efl_util_inputgen_h on success, otherwise @c NULL
 * @retval #efl_util_inputgen_h The input generator handle
 * @exception #EFL_UTIL_ERROR_NONE Successful
 * @exception #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #EFL_UTIL_ERROR_NO_SUCH_DEVICE No such device or address
 * @exception #EFL_UTIL_ERROR_INVALID_OPERATION Function not implemented
 * @exception #EFL_UTIL_ERROR_OUT_OF_MEMORY Memory allocation failure
 * @see efl_util_input_deinitialize_generator()
 */
API efl_util_inputgen_h efl_util_input_initialize_generator(efl_util_input_device_type_e dev_type);

/**
 * @platform
 * @brief Deinitializes system and close opened devices.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/inputgenerator
 * @param[in] inputgen_h The efl_util_inputgen_h handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @see efl_util_input_initialize_generator()
 */
API int efl_util_input_deinitialize_generator(efl_util_inputgen_h inputgen_h);

/**
 * @platform
 * @brief Generates all of key events using a opened device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/inputgenerator
 * @param[in] key_name The key name want to generate
 * @param[in] pressed The value that select key press or release (0: release, 1: press)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_PERMISSION_DENIED Has no permission to generate key
 */
API int efl_util_input_generate_key(efl_util_inputgen_h inputgen_h, const char *key_name, int pressed);

/**
 * @platform
 * @brief Generates a touch event using a opened device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/inputgenerator
 * @param[in] idx The index of touched finger
 * @param[in] efl_util_input_touch_type_e The touch type (ex> EFL_UTIL_INPUT_TOUCH_BEGIN, EFL_UTIL_INPUT_TOUCH_UPDATE, EFL_UTIL_INPUT_TOUCH_END)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #EFL_UTIL_ERROR_PERMISSION_DENIED Has no permission to generate touch
 */
API int efl_util_input_generate_touch(efl_util_inputgen_h inputgen_h, int idx, efl_util_input_touch_type_e touch_type, int x, int y);

/**
 * @}
 */

/**
 * @addtogroup CAPI_EFL_UTIL_SCREENSHOT_MODULE
 * @{
 */

/**
 * @platform
 * @brief Definition for the screenshot handle.
 * @since_tizen 2.4
 */
typedef struct _efl_util_screenshot_h * efl_util_screenshot_h;

/**
 * @platform
 * @brief Initializes the screenshot.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/screenshot
 * @remarks The specific error code can be obtained using the get_last_result().
 * method. Error codes are dedescribed in Exception section.
 * @param[in] width The width of the screenshot surface
 * @param[in] height The height of the screenshot surface
 * @return #efl_util_screenshot_h on success, otherwise @c NULL
 * @retval #efl_util_screenshot_h  The screenshot handle
 * @exception #EFL_UTIL_ERROR_NONE Successful
 * @exception #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #EFL_UTIL_ERROR_OUT_OF_MEMORY Memory allocation failure
 * @exception #EFL_UTIL_ERROR_SCREENSHOT_INIT_FAIL Initialization failure
 * @see efl_util_screenshot_deinitialize()
 */
API efl_util_screenshot_h efl_util_screenshot_initialize(int width, int height);

/**
 * @platform
 * @brief Takes a screenshot and get a tbm_surface handle.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/screenshot
 * @remarks The specific error code can be obtained using the get_last_result().
 *          The tbm_surface_h must be free by caller.
 * @param[in] screenshot The efl_util_screenshot_h handle
 * @return #tbm_surface_h on success, otherwise @c NULL
 * @retval #tbm_surface_h The TBM surface handle
 * @exception #EFL_UTIL_ERROR_NONE Successful
 * @exception #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #EFL_UTIL_ERROR_SCREENSHOT_EXECUTION_FAIL Execution failure
 * @exception #EFL_UTIL_ERROR_PERMISSION_DENIED Permission denied
 * @see efl_util_screenshot_initialize()
 * @see efl_util_screenshot_deinitialize()
 */
API tbm_surface_h efl_util_screenshot_take_tbm_surface(efl_util_screenshot_h screenshot);

/**
 * @platform
 * @brief Deinitializes the screenshot.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/screenshot
 * @param[in] screenshot The efl_util_screenshot_h handle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #EFL_UTIL_ERROR_NONE Successful
 * @retval #EFL_UTIL_ERROR_INVALID_PARAMETER Invalid parameter
 * @see efl_util_screenshot_initialize()
 */
API int efl_util_screenshot_deinitialize(efl_util_screenshot_h screenshot);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif
#endif /* __TIZEN_UI_EFL_UTIL_H__ */
