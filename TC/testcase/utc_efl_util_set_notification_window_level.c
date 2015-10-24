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

#include <tet_api.h>
#include <efl_util.h>
#include <Elementary.h>
#include <Ecore_X.h>

#define API_SET_NOTIFICATION_WINDOW_LEVEL "efl_util_set_notification_window_level"
#define API_GET_NOTIFICATION_WINDOW_LEVEL "efl_util_get_notification_window_level"


static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_efl_util_set_notification_window_level_negative_1(void);
static void utc_efl_util_set_notification_window_level_negative_2(void);
static void utc_efl_util_set_notification_window_level_negative_3(void);

static void utc_efl_util_get_notification_window_level_negative_1(void);
static void utc_efl_util_get_notification_window_level_negative_2(void);

static void utc_efl_util_set_notification_window_level_positive_1(void);
static void utc_efl_util_set_notification_window_level_positive_2(void);
static void utc_efl_util_set_notification_window_level_positive_3(void);

static void utc_efl_util_get_notification_window_level_positive_1(void);
static void utc_efl_util_get_notification_window_level_positive_2(void);
static void utc_efl_util_get_notification_window_level_positive_3(void);


struct tet_testlist tet_testlist[] = {
	{ utc_efl_util_set_notification_window_level_negative_1, 1 },
	{ utc_efl_util_set_notification_window_level_negative_2, 1 },
	{ utc_efl_util_set_notification_window_level_negative_3, 1 },
	{ utc_efl_util_get_notification_window_level_negative_1, 1 },
	{ utc_efl_util_get_notification_window_level_negative_2, 1 },
	{ utc_efl_util_set_notification_window_level_positive_1, 1 },
	{ utc_efl_util_set_notification_window_level_positive_2, 1 },
	{ utc_efl_util_set_notification_window_level_positive_3, 1 },
	{ utc_efl_util_get_notification_window_level_positive_1, 1 },
	{ utc_efl_util_get_notification_window_level_positive_2, 1 },
	{ utc_efl_util_get_notification_window_level_positive_3, 1 },
	{ NULL, 0 },
};


static void startup(void)
{
	/* start of TC */
	elm_init(0, NULL);
}


static void cleanup(void)
{
	/* end of TC */
	elm_shutdown();
}


static void win_del(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}


static Evas_Object* create_normal_win(const char *name)
{
	Evas_Object *eo;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (eo)
	{
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request",
				win_del, NULL);
		elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
	}

	return eo;
}


static Evas_Object* create_notification_win(const char *name)
{
	Evas_Object *eo;

	eo = elm_win_add(NULL, name, ELM_WIN_NOTIFICATION);
	if (eo)
	{
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request",
				win_del, NULL);
		elm_win_indicator_mode_set(eo, ELM_WIN_INDICATOR_SHOW);
	}

	return eo;
}


/**
 * @brief Negative test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_negative_1(void)
{
	int ret;

	ret = efl_util_set_notification_window_level(NULL,  EFL_UTIL_NOTIFICATION_LEVEL_2);

	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Negative test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_negative_2(void)
{
	Evas_Object *win;
	int ret;

	win = create_normal_win("Normal window");
	if (!win)
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret = efl_util_set_notification_window_level(win,  EFL_UTIL_NOTIFICATION_LEVEL_2);

	if (ret == EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Negative test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_negative_3(void)
{
	Evas_Object *win;
	int ret;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret = efl_util_set_notification_window_level(win, 100);

	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Negative test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_get_notification_window_level_negative_1(void)
{
	int ret;
	int level;

	ret = efl_util_get_notification_window_level(NULL,  &level);

	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_pass(API_GET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Negative test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_get_notification_window_level_negative_2(void)
{
	Evas_Object *win;
	int ret;
	int level;

	win = create_normal_win("Normal Type Window");
	if (!win)
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret = efl_util_get_notification_window_level(win,  &level);

	if (ret == EFL_UTIL_ERROR_NOT_SUPPORTED_WINDOW_TYPE)
	{
		dts_pass(API_GET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_positive_1(void)
{
	Evas_Object *win;
	int ret;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);

	if (ret == EFL_UTIL_ERROR_NONE)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_positive_2(void)
{
	Evas_Object *win;
	int ret1, ret2;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret1 = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);
	ret2 = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_2);

	if (ret2 == EFL_UTIL_ERROR_NONE)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_set_notification_window_level_positive_3(void)
{
	Evas_Object *win;
	int ret1, ret2, ret3;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}

	ret1 = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);
	ret2 = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_2);
	ret3 = efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_3);

	if (ret3 == EFL_UTIL_ERROR_NONE)
	{
		dts_pass(API_SET_NOTIFICATION_WINDOW_LEVEL, "passed");
	}
	else
	{
		dts_fail(API_SET_NOTIFICATION_WINDOW_LEVEL, "failed");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_get_notification_window_level_positive_1(void)
{
	Evas_Object *win;
	int ret;
	int level;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}
	efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);

	level = -1;
	ret = efl_util_get_notification_window_level(win,  &level);

	if (ret == EFL_UTIL_ERROR_NONE)
	{
		if (level == EFL_UTIL_NOTIFICATION_LEVEL_1)
		{
			dts_pass(API_GET_NOTIFICATION_WINDOW_LEVEL, "passed");
		}
		else
		{
			dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - level is wrong");
		}
	}
	else
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - return value is wrong");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_get_notification_window_level_positive_2(void)
{
	Evas_Object *win;
	int ret;
	int level;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}
	efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);
	efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_2);

	level = -1;
	ret = efl_util_get_notification_window_level(win,  &level);

	if (ret == EFL_UTIL_ERROR_NONE)
	{
		if (level == EFL_UTIL_NOTIFICATION_LEVEL_2)
		{
			dts_pass(API_GET_NOTIFICATION_WINDOW_LEVEL, "passed");
		}
		else
		{
			dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - level is wrong");
		}
	}
	else
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - return value is wrong");
	}
}


/**
 * @brief Positive test case of efl_util_set_notification_window_level()
 */
static void utc_efl_util_get_notification_window_level_positive_3(void)
{
	Evas_Object *win;
	int ret;
	int level;

	win = create_notification_win("Notification Type Window");
	if (!win)
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed to create window");
	}
	efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_1);
	efl_util_set_notification_window_level(win, EFL_UTIL_NOTIFICATION_LEVEL_3);

	level = -1;
	ret = efl_util_get_notification_window_level(win,  &level);

	if (ret == EFL_UTIL_ERROR_NONE)
	{
		if (level == EFL_UTIL_NOTIFICATION_LEVEL_3)
		{
			dts_pass(API_GET_NOTIFICATION_WINDOW_LEVEL, "passed");
		}
		else
		{
			dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - level is wrong");
		}
	}
	else
	{
		dts_fail(API_GET_NOTIFICATION_WINDOW_LEVEL, "failed - return value is wrong");
	}
}

