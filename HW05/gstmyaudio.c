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
static void gst_myaudio_finalize (GObject * object);
static gboolean gst_myaudio_start (GstBaseSrc * src);
static gboolean gst_myaudio_stop (GstBaseSrc * src);
static GstFlowReturn gst_myaudio_fill (GstBaseSrc * src, guint64 offset,
				       guint size, GstBuffer * buf);

enum
{
  PROP_0,
  PROP_LOCATION
};

/* pad templates */
static GstStaticPadTemplate gst_myaudio_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
			 GST_PAD_SRC,
			 GST_PAD_ALWAYS,
			 GST_STATIC_CAPS
			 ("audio/x-raw,format=S16LE,rate=[1,max],"
			  "channels=[1,max],layout=interleaved"));

/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstMyaudio, gst_myaudio, GST_TYPE_BASE_SRC,
			 GST_DEBUG_CATEGORY_INIT (gst_myaudio_debug_category,
						  "myaudio", 0,
						  "debug category for myaudio element"))
     static void gst_myaudio_class_init (GstMyaudioClass * klass)
{

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
  gobject_class->finalize = gst_myaudio_finalize;
  base_src_class->start = GST_DEBUG_FUNCPTR (gst_myaudio_start);
  base_src_class->stop = GST_DEBUG_FUNCPTR (gst_myaudio_stop);
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void
gst_myaudio_init (GstMyaudio * myaudio)
{
}

#pragma GCC diagnostic pop

void
gst_myaudio_set_property (GObject * object, guint property_id,
			  const GValue * value, GParamSpec * pspec)
{

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "set_property");

  switch (property_id)
    {
    case PROP_LOCATION:
      myaudio->location = g_strdup (g_value_get_string (value));
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

void
gst_myaudio_finalize (GObject * object)
{

  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_myaudio_parent_class)->finalize (object);
}

/* start and stop processing, ideal for opening/closing the resource */
static gboolean
gst_myaudio_start (GstBaseSrc * src)
{

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "start");

  if ((myaudio->fd = open (myaudio->location, O_RDONLY)) < 0)
    {
      GST_ERROR_OBJECT
	(src, "Ошибка вызова open(чтение) для файла\n");
      return FALSE;
    }

  return TRUE;
}

static gboolean
gst_myaudio_stop (GstBaseSrc * src)
{

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "stop");

  if (close (myaudio->fd) < 0)
    {
      GST_ERROR_OBJECT
	(src, "Ошибка вызова close(чтение) для файла\n");
      return FALSE;
    }

  return TRUE;
}


/* ask the subclass to fill the buffer with data from offset and size */
static GstFlowReturn
gst_myaudio_fill (GstBaseSrc * basesrc, guint64 offset, guint size,
		  GstBuffer * buf)
{

  GstMyaudio *src = GST_MYAUDIO (basesrc);

  GstMapInfo info;

  if ((int) offset == 0)
    lseek (src->fd, sizeof (header_struct_type), SEEK_SET);
  else
    lseek (src->fd, offset, SEEK_SET);

  gst_buffer_map (buf, &info, GST_MAP_WRITE);

  guint8 *data = info.data;

  int ret = read (src->fd, data, size);

  if (G_UNLIKELY (ret == 0))
    {
      GST_DEBUG ("EOS");
      return GST_FLOW_EOS;
    }

  gst_buffer_unmap (buf, &info);

  return GST_FLOW_OK;

}

static gboolean
plugin_init (GstPlugin * plugin)
{

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
