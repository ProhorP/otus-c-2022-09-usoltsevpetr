/* GStreamer
 * Copyright (C) 2022 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstmyaudio
 *
 * The myaudio element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! myaudio ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasesrc.h>
#include "gstmyaudio.h"
#include <stdio.h>
#include <glib/gprintf.h>
#include <unistd.h>
#include <fcntl.h>

GST_DEBUG_CATEGORY_STATIC (gst_myaudio_debug_category);
#define GST_CAT_DEFAULT gst_myaudio_debug_category

/* prototypes */


static void gst_myaudio_set_property (GObject * object,
				      guint property_id, const GValue * value,
				      GParamSpec * pspec);
static void gst_myaudio_get_property (GObject * object, guint property_id,
				      GValue * value, GParamSpec * pspec);
//static void gst_myaudio_dispose (GObject * object);
static void gst_myaudio_finalize (GObject * object);

//static GstCaps *gst_myaudio_get_caps (GstBaseSrc * src, GstCaps * filter);
//static gboolean gst_myaudio_negotiate (GstBaseSrc * src);
//static GstCaps *gst_myaudio_fixate (GstBaseSrc * src, GstCaps * caps);
//static gboolean gst_myaudio_set_caps (GstBaseSrc * src, GstCaps * caps);
//static gboolean gst_myaudio_decide_allocation (GstBaseSrc * src,
//					       GstQuery * query);
static gboolean gst_myaudio_start (GstBaseSrc * src);
static gboolean gst_myaudio_stop (GstBaseSrc * src);
//static void gst_myaudio_get_times (GstBaseSrc * src, GstBuffer * buffer,
//				   GstClockTime * start, GstClockTime * end);
//static gboolean gst_myaudio_get_size (GstBaseSrc * src, guint64 * size);
//static gboolean gst_myaudio_is_seekable (GstBaseSrc * src);
//static gboolean gst_myaudio_prepare_seek_segment (GstBaseSrc * src,
//						  GstEvent * seek,
//						  GstSegment * segment);
//static gboolean gst_myaudio_do_seek (GstBaseSrc * src, GstSegment * segment);
//static gboolean gst_myaudio_unlock (GstBaseSrc * src);
//static gboolean gst_myaudio_unlock_stop (GstBaseSrc * src);
static gboolean gst_myaudio_query (GstBaseSrc * src, GstQuery * query);
//static gboolean gst_myaudio_event (GstBaseSrc * src, GstEvent * event);
static GstFlowReturn gst_myaudio_create (GstBaseSrc * src, guint64 offset,
					 guint size, GstBuffer ** buf);
//static GstFlowReturn gst_myaudio_alloc (GstBaseSrc * src, guint64 offset,
//					guint size, GstBuffer ** buf);
static GstFlowReturn gst_myaudio_fill (GstBaseSrc * src, guint64 offset,
				       guint size, GstBuffer * buf);

enum
{
  PROP_0,
  PROP_LOCATION
};

/* pad templates */

#if 0
static GstStaticPadTemplate gst_myaudio_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
			 GST_PAD_SRC,
			 GST_PAD_ALWAYS,
			 GST_STATIC_CAPS ("application/unknown"));
#endif

#if 1
static GstStaticPadTemplate gst_myaudio_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
			 GST_PAD_SRC,
			 GST_PAD_ALWAYS,
			 GST_STATIC_CAPS
			 ("audio/x-raw,format=S16LE,rate=[1,max],"
			  "channels=[1,max],layout=interleaved"));
#endif

/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstMyaudio, gst_myaudio, GST_TYPE_BASE_SRC,
			 GST_DEBUG_CATEGORY_INIT (gst_myaudio_debug_category,
						  "myaudio", 0,
						  "debug category for myaudio element"));

static void
gst_myaudio_class_init (GstMyaudioClass * klass)
{
  printf ("gst_myaudio_class_init\n");

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseSrcClass *base_src_class = GST_BASE_SRC_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS (klass),
					     &gst_myaudio_src_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
					 "FIXME Long name", "Generic",
					 "FIXME Description",
					 "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_myaudio_set_property;
  gobject_class->get_property = gst_myaudio_get_property;
//  gobject_class->dispose = gst_myaudio_dispose;
  gobject_class->finalize = gst_myaudio_finalize;
//  base_src_class->get_caps = GST_DEBUG_FUNCPTR (gst_myaudio_get_caps);
//  base_src_class->negotiate = GST_DEBUG_FUNCPTR (gst_myaudio_negotiate);
//  base_src_class->fixate = GST_DEBUG_FUNCPTR (gst_myaudio_fixate);
//  base_src_class->set_caps = GST_DEBUG_FUNCPTR (gst_myaudio_set_caps);
//  base_src_class->decide_allocation =
//    GST_DEBUG_FUNCPTR (gst_myaudio_decide_allocation);
  base_src_class->start = GST_DEBUG_FUNCPTR (gst_myaudio_start);
  base_src_class->stop = GST_DEBUG_FUNCPTR (gst_myaudio_stop);
//  base_src_class->get_times = GST_DEBUG_FUNCPTR (gst_myaudio_get_times);
//  base_src_class->get_size = GST_DEBUG_FUNCPTR (gst_myaudio_get_size);
//  base_src_class->is_seekable = GST_DEBUG_FUNCPTR (gst_myaudio_is_seekable);
//  base_src_class->prepare_seek_segment =
//    GST_DEBUG_FUNCPTR (gst_myaudio_prepare_seek_segment);
//  base_src_class->do_seek = GST_DEBUG_FUNCPTR (gst_myaudio_do_seek);
//  base_src_class->unlock = GST_DEBUG_FUNCPTR (gst_myaudio_unlock);
//  base_src_class->unlock_stop = GST_DEBUG_FUNCPTR (gst_myaudio_unlock_stop);
  base_src_class->query = GST_DEBUG_FUNCPTR (gst_myaudio_query);
//  base_src_class->event = GST_DEBUG_FUNCPTR (gst_myaudio_event);
  base_src_class->create = GST_DEBUG_FUNCPTR (gst_myaudio_create);
//  base_src_class->alloc = GST_DEBUG_FUNCPTR (gst_myaudio_alloc);
  base_src_class->fill = GST_DEBUG_FUNCPTR (gst_myaudio_fill);

  g_object_class_install_property (gobject_class, PROP_LOCATION,
				   g_param_spec_string ("location",
							"File Location",
							"Location of the file to read",
							NULL,
							G_PARAM_READWRITE |
							G_PARAM_STATIC_STRINGS
							|
							GST_PARAM_MUTABLE_READY));


}

static void
gst_myaudio_init (GstMyaudio * myaudio)
{
  printf ("gst_myaudio_init\n");

}

void
gst_myaudio_set_property (GObject * object, guint property_id,
			  const GValue * value, GParamSpec * pspec)
{
  printf ("gst_myaudio_set_property\n");

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "set_property");

  switch (property_id)
    {
    case PROP_LOCATION:
      myaudio->location = g_strdup (g_value_get_string (value));
      g_printf ("!!!!!!!location=%s!!!!!!\n", myaudio->location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

void
gst_myaudio_get_property (GObject * object, guint property_id,
			  GValue * value, GParamSpec * pspec)
{

  printf ("gst_myaudio_get_property\n");

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "get_property");

  switch (property_id)
    {
    case PROP_LOCATION:
      g_value_set_string (value, myaudio->location);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

#if 0
void
gst_myaudio_dispose (GObject * object)
{

  printf ("gst_myaudio_dispose");

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_myaudio_parent_class)->dispose (object);
}
#endif

void
gst_myaudio_finalize (GObject * object)
{

  printf ("gst_myaudio_finalize\n");

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_myaudio_parent_class)->finalize (object);
}

#if 0
/* get caps from subclass */
static GstCaps *
gst_myaudio_get_caps (GstBaseSrc * src, GstCaps * filter)
{
  printf ("gst_myaudio_get_caps\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "get_caps");

  return NULL;
}
#endif

#if 0
/* decide on caps */
static gboolean
gst_myaudio_negotiate (GstBaseSrc * src)
{
  printf ("gst_myaudio_negotiate\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "negotiate");

  return TRUE;
}
#endif

#if 0
/* called if, in negotiation, caps need fixating */
static GstCaps *
gst_myaudio_fixate (GstBaseSrc * src, GstCaps * caps)
{

  printf ("gst_myaudio_fixate\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "fixate");

  return NULL;
}
#endif

#if 0
/* notify the subclass of new caps */
static gboolean
gst_myaudio_set_caps (GstBaseSrc * src, GstCaps * caps)
{

  printf ("gst_myaudio_set_caps\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "set_caps");

  return TRUE;
}
#endif

#if 0
/* setup allocation query */
static gboolean
gst_myaudio_decide_allocation (GstBaseSrc * src, GstQuery * query)
{
  printf ("gst_myaudio_decide_allocation\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "decide_allocation");

  return TRUE;
}
#endif

/* start and stop processing, ideal for opening/closing the resource */
static gboolean
gst_myaudio_start (GstBaseSrc * src)
{
  printf ("gst_myaudio_start\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "start");

  if ((myaudio->fd = open (myaudio->location, O_RDONLY)) < 0)
    {
      perror
	("Ошибка вызова open(чтение) для файла\n");
      return FALSE;
    }

  return TRUE;
}

static gboolean
gst_myaudio_stop (GstBaseSrc * src)
{
  printf ("gst_myaudio_stop\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "stop");

  if (close (myaudio->fd) < 0)
    {
      perror
	("Ошибка вызова close(чтение) для файла\n");
      return FALSE;
    }

  return TRUE;
}

#if 0
/* given a buffer, return start and stop time when it should be pushed
 * out. The base class will sync on the clock using these times. */
static void
gst_myaudio_get_times (GstBaseSrc * src, GstBuffer * buffer,
		       GstClockTime * start, GstClockTime * end)
{
  printf ("gst_myaudio_get_times\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "get_times");

}
#endif

#if 0
/* get the total size of the resource in bytes */
static gboolean
gst_myaudio_get_size (GstBaseSrc * src, guint64 * size)
{

  printf ("gst_myaudio_get_size\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "get_size");

  return TRUE;
}
#endif

#if 0
/* check if the resource is seekable */
static gboolean
gst_myaudio_is_seekable (GstBaseSrc * src)
{
  printf ("gst_myaudio_is_seekable\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "is_seekable");

  return TRUE;
}
#endif

#if 0
/* Prepare the segment on which to perform do_seek(), converting to the
 * current basesrc format. */
static gboolean
gst_myaudio_prepare_seek_segment (GstBaseSrc * src, GstEvent * seek,
				  GstSegment * segment)
{

  printf ("gst_myaudio_prepare_seek_segment\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "prepare_seek_segment");

  return TRUE;
}
#endif

#if 0
/* notify subclasses of a seek */
static gboolean
gst_myaudio_do_seek (GstBaseSrc * src, GstSegment * segment)
{
  printf ("gst_myaudio_do_seek\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "do_seek");

  return TRUE;
}
#endif


#if 0
/* unlock any pending access to the resource. subclasses should unlock
 * any function ASAP. */
static gboolean
gst_myaudio_unlock (GstBaseSrc * src)
{
  printf ("gst_myaudio_unlock\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "unlock");

  return TRUE;
}
#endif

#if 0
/* Clear any pending unlock request, as we succeeded in unlocking */
static gboolean
gst_myaudio_unlock_stop (GstBaseSrc * src)
{
  printf ("gst_myaudio_unlock_stop\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "unlock_stop");

  return TRUE;
}
#endif

/* notify subclasses of a query */
static gboolean
gst_myaudio_query (GstBaseSrc * src, GstQuery * query)
{

  printf ("gst_myaudio_query\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "query");

  return TRUE;
}

#if 0
/* notify subclasses of an event */
static gboolean
gst_myaudio_event (GstBaseSrc * src, GstEvent * event)
{
  printf ("gst_myaudio_event\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "event");

  return TRUE;
}
#endif

/* ask the subclass to create a buffer with offset and size, the default
 * implementation will call alloc and fill. */
static GstFlowReturn
gst_myaudio_create (GstBaseSrc * src, guint64 offset, guint size,
		    GstBuffer ** buf)
{

  printf ("gst_myaudio_create\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "create");


  return GST_FLOW_OK;
}

#if 0
/* ask the subclass to allocate an output buffer. The default implementation
 * will use the negotiated allocator. */
static GstFlowReturn
gst_myaudio_alloc (GstBaseSrc * src, guint64 offset, guint size,
		   GstBuffer ** buf)
{

  printf ("gst_myaudio_alloc\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "alloc");

  return GST_FLOW_OK;
}
#endif


#if 1
/* ask the subclass to fill the buffer with data from offset and size */
static GstFlowReturn
gst_myaudio_fill (GstBaseSrc * src, guint64 offset, guint size,
		  GstBuffer * buf)
{
  printf ("gst_myaudio_fill\n");

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "fill");

  return GST_FLOW_OK;
}

#endif

static gboolean
plugin_init (GstPlugin * plugin)
{
  printf ("plugin_init\n");

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "myaudio", GST_RANK_NONE,
			       GST_TYPE_MYAUDIO);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
		   GST_VERSION_MINOR,
		   myaudio,
		   "FIXME plugin description",
		   plugin_init, VERSION, "LGPL", PACKAGE_NAME,
		   GST_PACKAGE_ORIGIN)
