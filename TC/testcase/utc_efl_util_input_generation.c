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

#define API_INPUT_GENERATE_INIT "efl_util_input_generate_init"
#define API_INPUT_GENERATE_KEY "efl_util_input_generate_key"
#define API_INPUT_GENERATE_TOUCH "efl_util_input_generate_touch"


static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_efl_util_input_initialize_generator_negative(void);
static void utc_efl_util_input_initialize_generator_positive(void);
static void utc_efl_util_input_generate_key_negative(void);
static void utc_efl_util_input_generate_key_positive(void);
static void utc_efl_util_input_generate_touch_negative(void);
static void utc_efl_util_input_generate_touch_positive(void);

struct tet_testlist tet_testlist[] = {
	{ utc_efl_util_input_initialize_generator_negative, 1 },
	{ utc_efl_util_input_initialize_generator_positive, 1 },
	{ utc_efl_util_input_generate_key_negative, 1 },
	{ utc_efl_util_input_generate_key_positive, 1 },
	{ utc_efl_util_input_generate_touch_negative, 1 },
	{ utc_efl_util_input_generate_touch_positive, 1 },
	{ NULL, 0 },
};


static void startup(void)
{
}


static void cleanup(void)
{
}

/**
 * @brief Negative test case of efl_util_input_initialize_generator()
 */
static void utc_efl_util_input_initialize_generator_negative(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_NONE);
	if (ret != EFL_UTIL_ERROR_NONE)
	{
		dts_pass(API_INPUT_GENERATE_INIT, "passed");
	}
	else
	{
		dts_fail(API_INPUT_GENERATE_INIT, "failed");
		efl_util_input_deinitialize_generator();
	}
}

/**
 * @brief Positive test case of efl_util_input_initialize_generator()
 */
static void utc_efl_util_input_initialize_generator_positive(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_ALL);
	if (ret == EFL_UTIL_ERROR_NONE)
	{
		dts_pass(API_INPUT_GENERATE_INIT, "passed");
		efl_util_input_deinitialize_generator();
	}
	else
	{
		dts_fail(API_INPUT_GENERATE_INIT, "failed");
	}
}


/**
 * @brief Negative test case of efl_util_input_generate_key()
 */
static void utc_efl_util_input_generate_key_negative(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_KEYBOARD);
	if (ret != EFL_UTIL_ERROR_NONE)
	{
		dts_fail(API_INPUT_GENERATE_KEY, "failed to find keyboard device");
		return;
	}
	ret = efl_util_input_generate_key("None", 1);
	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_pass(API_INPUT_GENERATE_KEY, "passed");
	}
	else
	{
		dts_fail(API_INPUT_GENERATE_KEY, "failed");
	}
	efl_util_input_deinitialize_generator();
}

/**
 * @brief Positive test case of efl_util_input_generate_key()
 */
static void utc_efl_util_input_generate_key_positive(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_KEYBOARD);
	if (ret != EFL_UTIL_ERROR_NONE)
	{
		dts_fail(API_INPUT_GENERATE_KEY, "failed to find keyboard device");
		return;
	}
	ret = efl_util_input_generate_key("XF86Back", 1);
	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_fail(API_INPUT_GENERATE_KEY, "failed");
	}
	else
	{
		dts_pass(API_INPUT_GENERATE_KEY, "passed");
		ret = efl_util_input_generate_key("XF86Back", 0);
	}
	efl_util_input_deinitialize_generator();
}


/**
 * @brief Negative test case of efl_util_input_generate_touch()
 */
static void utc_efl_util_input_generate_touch_negative(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN);
	if (ret != EFL_UTIL_ERROR_NONE)
	{
		dts_fail(API_INPUT_GENERATE_TOUCH, "failed to find keyboard device");
		return;
	}
	ret = efl_util_input_generate_touch(0, EFL_UTIL_INPUT_TOUCH_NONE, -1, -1);
	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_pass(API_INPUT_GENERATE_TOUCH, "passed");
	}
	else
	{
		dts_fail(API_INPUT_GENERATE_TOUCH, "failed");
	}
	efl_util_input_deinitialize_generator();
}

/**
 * @brief Positive test case of efl_util_input_generate_touch()
 */
static void utc_efl_util_input_generate_touch_positive(void)
{
	int ret;

	ret = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN);
	if (ret != EFL_UTIL_ERROR_NONE)
	{
		dts_fail(API_INPUT_GENERATE_TOUCH, "failed to find keyboard device");
		return;
	}
	ret = efl_util_input_generate_touch(0, EFL_UTIL_INPUT_TOUCH_BEGIN, 100, 150);
	if (ret == EFL_UTIL_ERROR_INVALID_PARAMETER)
	{
		dts_fail(API_INPUT_GENERATE_TOUCH, "failed");
	}
	else
	{
		dts_pass(API_INPUT_GENERATE_TOUCH, "passed");
		ret = efl_util_input_generate_touch(0, EFL_UTIL_INPUT_TOUCH_END, 100, 150);
	}
	efl_util_input_deinitialize_generator();
}
