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


#include <efl_util.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <Elementary.h>
#include <Ecore_X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/Xvproto.h>
#include <X11/extensions/Xdamage.h>
#include <xf86drm.h>
#include <tbm_bufmgr.h>
#include <tbm_surface.h>
#include <tbm_surface_internal.h>

#ifdef USE_DRI2
#include <dri2.h>
#else
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/dri3.h>
#include <xcb/xcbext.h>
#endif

struct _efl_util_screenshot_h
{
   tbm_surface_h surface;
   int width;
   int height;

   Ecore_X_Display *dpy;
   int internal_display;
   int screen;
   Window root;
   Pixmap pixmap;
   GC gc;
   Atom atom_capture;

   /* port */
   int port;

   /* damage */
   Damage   damage;
   int      damage_base;

#ifdef USE_DRI2
   /* dri2 */
   int eventBase, errorBase;
   int dri2Major, dri2Minor;
   char *driver_name, *device_name;
   drm_magic_t magic;
#endif

   /* drm */
   int drm_fd;

   /* tbm bufmgr */
   tbm_bufmgr bufmgr;
};

#define FOURCC(a,b,c,d) (((unsigned)d&0xff)<<24 | ((unsigned)c&0xff)<<16 | ((unsigned)b&0xff)<<8 | ((unsigned)a&0xff))

#define FOURCC_RGB32    FOURCC('R','G','B','4')
#define TIMEOUT_CAPTURE 3

/* scrrenshot handle */
static efl_util_screenshot_h g_screenshot;


/* x error handling */
static Bool g_efl_util_x_error_caught;
static Bool g_efl_util_x_error_permission;

static int _efl_util_screenshot_x_error_handle(Display *dpy, XErrorEvent *ev)
{
   if (!g_screenshot || (dpy != g_screenshot->dpy))
     return 0;

   g_efl_util_x_error_caught = True;
   if (ev->error_code == BadAccess)
     g_efl_util_x_error_permission = True;

   return 0;
}

static int _efl_util_screenshot_get_port(Display *dpy, unsigned int id, Window win)
{
   unsigned int ver, rev, req_base, evt_base, err_base;
   unsigned int adaptors;
   XvAdaptorInfo *ai = NULL;
   XvImageFormatValues *fo = NULL;
   int formats;
   int i, j, p;

   if (XvQueryExtension(dpy, &ver, &rev, &req_base, &evt_base, &err_base) != Success)
     {
        fprintf (stderr, "[EFL_UTIL] no XV extension. \n");
        return -1;
     }

   if (XvQueryAdaptors(dpy, win, &adaptors, &ai) != Success)
     {
        fprintf(stderr, "[EFL_UTIL] fail : query adaptors. \n");
        return -1;
     }

   if (!ai)
     {
        fprintf(stderr, "[EFL_UTIL] fail : get adaptor info. \n");
        return -1;
     }

   for (i = 0; i < adaptors; i++)
     {
        int support_format = False;

        if (!(ai[i].type & XvInputMask) ||
            !(ai[i].type & XvStillMask))
          continue;

        p = ai[i].base_id;

        fo = XvListImageFormats (dpy, p, &formats);
        for (j = 0; j < formats; j++)
          if (fo[j].id == (int)id)
            support_format = True;

        if (fo)
          XFree(fo);

        if (!support_format)
          continue;

        for (; p < ai[i].base_id + ai[i].num_ports; p++)
          {
             if (XvGrabPort(dpy, p, 0) == Success)
               {
                  XvFreeAdaptorInfo(ai);
                  return p;
               }
          }

        fprintf(stderr, "[EFL_UTIL] fail : grab port. \n");
     }

   XvFreeAdaptorInfo(ai);

   XSync(dpy, False);

   return -1;
}

static int _efl_util_screenshot_get_best_size(Display *dpy, int port, int width, int height, unsigned int *best_width, unsigned int *best_height)
{
   XErrorHandler old_handler = NULL;

   Atom atom_capture = XInternAtom(dpy, "_USER_WM_PORT_ATTRIBUTE_CAPTURE", False);

   g_efl_util_x_error_caught = False;
   old_handler = XSetErrorHandler(_efl_util_screenshot_x_error_handle);

   XvSetPortAttribute(dpy, port, atom_capture, 1);
   XSync(dpy, False);

   g_efl_util_x_error_caught = False;
   XSetErrorHandler(old_handler);

   XvQueryBestSize(dpy, port, 0, 0, 0, width, height, best_width, best_height);
   if (*best_width <= 0 || *best_height <= 0)
     return 0;

   return 1;
}

static Bool _efl_util_screenshot_predicate_proc(Display *dpy, XEvent *event, char *arg)
{
    efl_util_screenshot_h screenshot = (efl_util_screenshot_h)arg;

    if(event->type == (screenshot->damage_base + XDamageNotify))
        return True;
    else
        return False;
}

API efl_util_screenshot_h efl_util_screenshot_initialize(int width, int height)
{
   efl_util_screenshot_h screenshot = NULL;
   int depth = 0;
   int damage_err_base = 0;
   unsigned int best_width = 0;
   unsigned int best_height = 0;

   if (width < 1 || height < 1)
     {
        printf("invalid parameter.\n");
        set_last_result(EFL_UTIL_ERROR_INVALID_PARAMETER);
        return NULL;
     }

   if (g_screenshot != NULL)
     {
        if (g_screenshot->width != width || g_screenshot->height != height)
          {
             // TODO: recreate pixmap and update information
             if (!_efl_util_screenshot_get_best_size(g_screenshot->dpy, g_screenshot->port, width, height, &best_width, &best_height))
               {
                  set_last_result(EFL_UTIL_ERROR_SCREENSHOT_INIT_FAIL);
                  return NULL;
               }

             g_screenshot->width = width;
             g_screenshot->height = height;
          }

        return g_screenshot;
     }

   screenshot = calloc(1, sizeof(struct _efl_util_screenshot_h));
   if (!screenshot)
     {
        printf("fails screenshot allocation.\n");
        set_last_result(EFL_UTIL_ERROR_OUT_OF_MEMORY);
        return NULL;
     }

   /* set dpy */
   screenshot->dpy = ecore_x_display_get();
   if (!screenshot->dpy)
     {
        screenshot->dpy = XOpenDisplay(0);
        if (!screenshot->dpy)
          {
             printf("fails XOpenDisplay.\n");
             goto fail;
          }
        /* for XCloseDisplay at denitialization */
        screenshot->internal_display = 1;
     }

   /* set screen */
   screenshot->screen = DefaultScreen(screenshot->dpy);

   /* set root window */
   screenshot->root = DefaultRootWindow(screenshot->dpy);

   /* initialize capture adaptor */
   screenshot->port = _efl_util_screenshot_get_port(screenshot->dpy, FOURCC_RGB32, screenshot->root);
   if (screenshot->port <= 0)
     {
        printf("fails to get a port.\n");
        goto fail;
     }

   /* get the best size */
   if (!_efl_util_screenshot_get_best_size(screenshot->dpy, screenshot->port, width, height, &best_width, &best_height))
     {
        printf("fails to get a best size.\n");
        goto fail;
     }

   /* set the width and the height */
   screenshot->width = best_width;
   screenshot->height = best_height;

   /* create a pixmap */
   depth = DefaultDepth(screenshot->dpy, screenshot->screen);
   screenshot->pixmap = XCreatePixmap(screenshot->dpy, screenshot->root, screenshot->width, screenshot->height, depth);
   if (!screenshot->pixmap)
     {
        printf("fails XCreatePixmap.\n");
        goto fail;
     }

   screenshot->gc = XCreateGC(screenshot->dpy, screenshot->pixmap, 0, 0);
   if (!screenshot->gc)
     {
        printf("fails XCreateGC.\n");
        goto fail;
     }
   XSetForeground(screenshot->dpy, screenshot->gc, 0xFF000000);
   XFillRectangle(screenshot->dpy, screenshot->pixmap, screenshot->gc, 0, 0, width, height);

   /* initialize damage */
   if (!XDamageQueryExtension(screenshot->dpy, &screenshot->damage_base, &damage_err_base))
     goto fail;
   screenshot->damage = XDamageCreate(screenshot->dpy, screenshot->pixmap, XDamageReportNonEmpty);
   if (screenshot->damage <= 0)
     {
        printf("fails XDamageCreate.\n");
        goto fail;
     }

   /* initialize dri3 and dri2 */

#ifdef USE_DRI2
   if (!DRI2QueryExtension(screenshot->dpy, &screenshot->eventBase, &screenshot->errorBase))
     {
        printf("fails DRI2QueryExtention\n");
        goto fail;
     }

   if (!DRI2QueryVersion(screenshot->dpy, &screenshot->dri2Major, &screenshot->dri2Minor))
     {
        printf("fails DRI2QueryVersion\n");
        goto fail;
     }

   if (!DRI2Connect(screenshot->dpy, screenshot->root, &screenshot->driver_name, &screenshot->device_name))
     {
        printf("fails DRI2Connect\n");
        goto fail;
     }

   screenshot->drm_fd = open(screenshot->device_name, O_RDWR);
   if (screenshot->drm_fd < 0)
     {
        printf("cannot open drm device (%s)\n", screenshot->device_name);
        goto fail;
     }

   if (drmGetMagic(screenshot->drm_fd, &screenshot->magic))
     {
        printf("fails drmGetMagic\n");
        goto fail;
     }

   if (!DRI2Authenticate(screenshot->dpy, screenshot->root, screenshot->magic))
     {
        printf("fails DRI2Authenticate\n");
        goto fail;
     }

   if (!drmAuthMagic(screenshot->drm_fd, screenshot->magic))
     {
        printf("fails drmAuthMagic\n");
        goto fail;
     }

   DRI2CreateDrawable(screenshot->dpy, screenshot->pixmap);
#else
   xcb_connection_t *c = XGetXCBConnection(screenshot->dpy);
   xcb_generic_error_t *error = NULL;

   xcb_dri3_query_version_cookie_t dri3_cookie;
   xcb_dri3_query_version_reply_t *dri3_reply = NULL;
   const xcb_query_extension_reply_t *extension = NULL;
   xcb_extension_t xcb_dri3_id = { "DRI3", 0 };

   xcb_prefetch_extension_data(c, &xcb_dri3_id);
   extension = xcb_get_extension_data(c, &xcb_dri3_id);
   if (!(extension && extension->present))
     {
        printf("fails DRI3 xcb_get_extension_data\n");
        goto fail;
     }

   dri3_cookie = xcb_dri3_query_version(c, XCB_DRI3_MAJOR_VERSION, XCB_DRI3_MINOR_VERSION);
   dri3_reply = xcb_dri3_query_version_reply(c, dri3_cookie, &error);
   if (!dri3_reply)
     {
        printf("fails xcb_dri3_query_version\n");
        free(error);
        goto fail;
     }
   free(dri3_reply);

   xcb_dri3_open_cookie_t cookie;
   xcb_dri3_open_reply_t *reply = NULL;
   int drm_fd;

   cookie = xcb_dri3_open(c, screenshot->root, 0);
   reply = xcb_dri3_open_reply(c, cookie, NULL);
   if (!reply)
     {
        printf("fails xcb_dri3_open\n");
        goto fail;
     }

   if (reply->nfd != 1)
     {
        printf("fails get drm fd\n");
        free(reply);
        goto fail;
     }

   drm_fd = xcb_dri3_open_reply_fds(c, reply)[0];
   fcntl(drm_fd, F_SETFD, FD_CLOEXEC);
   if (drm_fd < 0)
     {
        printf("fails open drm fd\n");
        free(reply);
        goto fail;
     }
   free(reply);

   screenshot->drm_fd = drm_fd;
#endif

   /* tbm bufmgr */
   screenshot->bufmgr = tbm_bufmgr_init(screenshot->drm_fd);
   if (!screenshot->bufmgr)
     {
        printf("fails tbm_bufmgr_init\n");
        goto fail;
     }

   XFlush(screenshot->dpy);

   g_screenshot = screenshot;
   set_last_result(EFL_UTIL_ERROR_NONE);

   return g_screenshot;
fail:
   if (screenshot)
     efl_util_screenshot_deinitialize(screenshot);
   set_last_result(EFL_UTIL_ERROR_SCREENSHOT_INIT_FAIL);

   return NULL;
}

API int efl_util_screenshot_deinitialize(efl_util_screenshot_h screenshot)
{
   if (!screenshot)
     return EFL_UTIL_ERROR_INVALID_PARAMETER;

   if (!screenshot->dpy)
     return EFL_UTIL_ERROR_INVALID_PARAMETER;

   /* tbm bufmgr */
   if (screenshot->bufmgr)
     tbm_bufmgr_deinit(screenshot->bufmgr);

#ifdef USE_DRI2
   /* dri2 */
   DRI2DestroyDrawable(screenshot->dpy, screenshot->pixmap);

   if (screenshot->driver_name)
     free (screenshot->driver_name);
   if (screenshot->device_name)
     free (screenshot->device_name);
#endif

   if (screenshot->drm_fd >= 0)
     close(screenshot->drm_fd);

   /* xv */
   if (screenshot->port > 0 && screenshot->pixmap > 0)
     XvStopVideo(screenshot->dpy, screenshot->port, screenshot->pixmap);

   /* damage */
   if (screenshot->damage)
     XDamageDestroy(screenshot->dpy, screenshot->damage);

   /* gc */
   if (screenshot->gc)
     XFreeGC(screenshot->dpy, screenshot->gc);

   /* pixmap */
   if (screenshot->pixmap > 0)
     XFreePixmap(screenshot->dpy, screenshot->pixmap);

   /* port */
   if (screenshot->port > 0)
     XvUngrabPort(screenshot->dpy, screenshot->port, 0);

   XSync(screenshot->dpy, False);

   /* dpy */
   if (screenshot->internal_display == 1)
     XCloseDisplay(screenshot->dpy);

   free(screenshot);
   g_screenshot = NULL;

   return EFL_UTIL_ERROR_NONE;
}


API tbm_surface_h efl_util_screenshot_take_tbm_surface(efl_util_screenshot_h screenshot)
{
   XEvent ev = {0,};
   XErrorHandler old_handler = NULL;;
   tbm_bo t_bo = NULL;
   tbm_surface_h t_surface = NULL;
   int buf_width = 0;
   int buf_height = 0;
   int buf_pitch = 0;
   tbm_surface_info_s surf_info;
   int i;
#ifdef USE_DRI2
   unsigned int attachment = DRI2BufferFrontLeft;
   int nbufs = 0;
   DRI2Buffer *bufs = NULL;
#endif

   if (screenshot != g_screenshot)
     {
        set_last_result(EFL_UTIL_ERROR_INVALID_PARAMETER);
        return NULL;
     }

   /* for flush other pending requests and pending events */
   XSync(screenshot->dpy, 0);

   g_efl_util_x_error_caught = False;
   old_handler = XSetErrorHandler(_efl_util_screenshot_x_error_handle);

   /* dump here */
   XvPutStill(screenshot->dpy, screenshot->port, screenshot->pixmap, screenshot->gc,
              0, 0, screenshot->width, screenshot->height,
              0, 0, screenshot->width, screenshot->height);

   XSync(screenshot->dpy, 0);

   if (g_efl_util_x_error_caught)
     {
        g_efl_util_x_error_caught = False;
        XSetErrorHandler(old_handler);
        goto fail;
     }

   g_efl_util_x_error_caught = False;
   XSetErrorHandler(old_handler);

   if (!XCheckIfEvent(screenshot->dpy, &ev, _efl_util_screenshot_predicate_proc, (char *)screenshot))
     {
        int fd = ConnectionNumber(screenshot->dpy);
        fd_set mask;
        struct timeval tv;
        int ret;

        FD_ZERO(&mask);
        FD_SET(fd, &mask);

        tv.tv_usec = 0;
        tv.tv_sec = TIMEOUT_CAPTURE;

        ret = select(fd + 1, &mask, 0, 0, &tv);
        if (ret < 0)
          fprintf(stderr, "[UTILX] fail: select.\n");
        else if (ret == 0)
          fprintf(stderr, "[UTILX] timeout(%d sec)!\n", TIMEOUT_CAPTURE);
        else if (XPending(screenshot->dpy))
          XNextEvent(screenshot->dpy, &ev);
        else
          fprintf(stderr, "[UTILX] fail: not passed a event!\n");
     }

   /* check if the capture is done by xserver and pixmap has got the captured image */
   if (ev.type == (screenshot->damage_base + XDamageNotify))
     {
        XDamageNotifyEvent *damage_ev = (XDamageNotifyEvent *)&ev;
        if (damage_ev->drawable == screenshot->pixmap)
          {
#ifdef USE_DRI2
             /* Get DRI2 FrontLeft buffer of the pixmap */
             bufs = DRI2GetBuffers(screenshot->dpy, screenshot->pixmap, &buf_width, &buf_height, &attachment, 1, &nbufs);
             if (!bufs)
               {
                  fprintf(stderr, "[UTILX] fail to get dri2 buffers!\n");
                  goto fail;
               }

             buf_pitch = bufs->pitch;

             t_bo = tbm_bo_import(screenshot->bufmgr, bufs[0].name);
             if (!t_bo)
               {
                  fprintf(stderr, "[UTILX] fail to get dri2 buffers!\n");
                  goto fail;
               }

             free(bufs);
             bufs = NULL;
#else
             /* Get DRI3 Front buffer of the pixmap */

             xcb_connection_t *c = XGetXCBConnection(screenshot->dpy);
             xcb_generic_error_t *error = NULL;
             xcb_dri3_buffer_from_pixmap_cookie_t bp_cookie;
             xcb_dri3_buffer_from_pixmap_reply_t *bp_reply = NULL;
             int *fds;

             bp_cookie = xcb_dri3_buffer_from_pixmap(c, screenshot->pixmap);
             bp_reply = xcb_dri3_buffer_from_pixmap_reply(c, bp_cookie, &error);
             if (!bp_reply)
               {
                  fprintf(stderr, "[UTILX] fail to xcb_dri3_buffer_from_pixmap error:%d!\n", error->error_code);
                  free(error);
                  goto fail;
               }
             fds = xcb_dri3_buffer_from_pixmap_reply_fds(c, bp_reply);

             buf_pitch = bp_reply->stride;
             buf_width = bp_reply->width;
             buf_height = bp_reply->height;

             t_bo = tbm_bo_import_fd(screenshot->bufmgr, fds[0]);
             if (!t_bo)
               {
                  fprintf(stderr, "[UTILX] fail to get tbm_bo from fd!\n");
                  close(fds[0]);
                  free (bp_reply);
                  goto fail;
               }

             close(fds[0]);
             free(bp_reply);
#endif

             surf_info.width = buf_width;
             surf_info.height = buf_height;
             surf_info.format = TBM_FORMAT_XRGB8888;
             surf_info.bpp = 32;
             surf_info.size = buf_pitch * surf_info.height;
             surf_info.num_planes = 1;
             for (i = 0; i < surf_info.num_planes; i++)
               {
                  surf_info.planes[i].size = buf_pitch * surf_info.height;
                  surf_info.planes[i].stride = buf_pitch;
                  surf_info.planes[i].offset = 0;
               }

             t_surface = tbm_surface_internal_create_with_bos(&surf_info, &t_bo, 1);
             if (!t_surface)
               {
                  fprintf(stderr, "[UTILX] fail to get tbm_surface!\n");
                  goto fail;
               }

             tbm_bo_unref(t_bo);

             XDamageSubtract(screenshot->dpy, screenshot->damage, None, None);

             set_last_result(EFL_UTIL_ERROR_NONE);

             return t_surface;
          }

        XDamageSubtract(screenshot->dpy, screenshot->damage, None, None);
     }

fail:

   if (t_bo)
     tbm_bo_unref(t_bo);

#ifdef USE_DRI2
   if (bufs)
     free(bufs);
#endif

   if (g_efl_util_x_error_permission)
     {
        g_efl_util_x_error_permission = False;
        set_last_result(EFL_UTIL_ERROR_PERMISSION_DENIED);
     }
   else
     set_last_result(EFL_UTIL_ERROR_SCREENSHOT_EXECUTION_FAIL);

   return NULL;
}


