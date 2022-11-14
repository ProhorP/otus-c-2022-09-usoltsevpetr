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
#include <gst/audio/gstaudiosrc.h>
#include "gstmyaudio.h"
#include <stdio.h>

GST_DEBUG_CATEGORY_STATIC (gst_myaudio_debug_category);
#define GST_CAT_DEFAULT gst_myaudio_debug_category

/* prototypes */


static void gst_myaudio_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_myaudio_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_myaudio_dispose (GObject * object);
static void gst_myaudio_finalize (GObject * object);

static gboolean gst_myaudio_open (GstAudioSrc * src);
static gboolean gst_myaudio_prepare (GstAudioSrc * src,
    GstAudioRingBufferSpec * spec);
static gboolean gst_myaudio_unprepare (GstAudioSrc * src);
static gboolean gst_myaudio_close (GstAudioSrc * src);
static guint gst_myaudio_read (GstAudioSrc * src, gpointer data, guint length,
    GstClockTime * timestamp);
static guint gst_myaudio_delay (GstAudioSrc * src);
static void gst_myaudio_reset (GstAudioSrc * src);

//static gboolean gst_myaudio_start (GstAudioSrc * src);
//static gboolean gst_myaudio_stop (GstAudioSrc * src);


enum
{
  PROP_0
};

/* pad templates */

/* FIXME add/remove the formats that you want to support */
static GstStaticPadTemplate gst_myaudio_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw,format=S16LE,rate=[1,max],"
      "channels=[1,max],layout=interleaved")
    );


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstMyaudio, gst_myaudio, GST_TYPE_AUDIO_SRC,
  GST_DEBUG_CATEGORY_INIT (gst_myaudio_debug_category, "myaudio", 0,
  "debug category for myaudio element"));

static void
gst_myaudio_class_init (GstMyaudioClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstAudioSrcClass *audio_src_class = GST_AUDIO_SRC_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_static_pad_template (GST_ELEMENT_CLASS(klass),
      &gst_myaudio_src_template);

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
      "FIXME Long name", "Generic", "FIXME Description",
      "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_myaudio_set_property;
  gobject_class->get_property = gst_myaudio_get_property;
  gobject_class->dispose = gst_myaudio_dispose;
  gobject_class->finalize = gst_myaudio_finalize;
  audio_src_class->open = GST_DEBUG_FUNCPTR (gst_myaudio_open);
  audio_src_class->prepare = GST_DEBUG_FUNCPTR (gst_myaudio_prepare);
  audio_src_class->unprepare = GST_DEBUG_FUNCPTR (gst_myaudio_unprepare);
  audio_src_class->close = GST_DEBUG_FUNCPTR (gst_myaudio_close);
  audio_src_class->read = GST_DEBUG_FUNCPTR (gst_myaudio_read);
  audio_src_class->delay = GST_DEBUG_FUNCPTR (gst_myaudio_delay);
  audio_src_class->reset = GST_DEBUG_FUNCPTR (gst_myaudio_reset);

//audio_src_class->start = GST_DEBUG_FUNCPTR (gst_myaudio_start);  
//audio_src_class->stop = GST_DEBUG_FUNCPTR (gst_myaudio_stop);  
}

static void
gst_myaudio_init (GstMyaudio *myaudio)
{
}

void
gst_myaudio_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "set_property");

  switch (property_id) {
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

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_myaudio_dispose (GObject * object)
{
  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_myaudio_parent_class)->dispose (object);
}

void
gst_myaudio_finalize (GObject * object)
{
  GstMyaudio *myaudio = GST_MYAUDIO (object);

  GST_DEBUG_OBJECT (myaudio, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_myaudio_parent_class)->finalize (object);
}

/* open the device with given specs */
static gboolean
gst_myaudio_open (GstAudioSrc * src)
{

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "open");

  return TRUE;
}

#if 0
static gboolean
gst_myaudio_start (GstAudioSrc * src)
{

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "start");

  return TRUE;
}

static gboolean
gst_myaudio_stop (GstAudioSrc * src)
{

  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "stop");

  return TRUE;
}
#endif


/* prepare resources and state to operate with the given specs */
static gboolean
gst_myaudio_prepare (GstAudioSrc * src, GstAudioRingBufferSpec * spec)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "prepare");

  return TRUE;
}

/* undo anything that was done in prepare() */
static gboolean
gst_myaudio_unprepare (GstAudioSrc * src)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "unprepare");

  return TRUE;
}

/* close the device */
static gboolean
gst_myaudio_close (GstAudioSrc * src)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "close");

  return TRUE;
}

/* read samples from the device */
static guint
gst_myaudio_read (GstAudioSrc * src, gpointer data, guint length,
    GstClockTime * timestamp)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "read");

  return 0;
}

/* get number of samples queued in the device */
static guint
gst_myaudio_delay (GstAudioSrc * src)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "delay");

  return 0;
}

/* reset the audio device, unblock from a write */
static void
gst_myaudio_reset (GstAudioSrc * src)
{
  GstMyaudio *myaudio = GST_MYAUDIO (src);

  GST_DEBUG_OBJECT (myaudio, "reset");

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
#define VERSION "0.0.1"
#endif
#ifndef PACKAGE
#define PACKAGE "myaudio"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "myaudio"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "https://github.com/ProhorP/otus-c-2022-09-usoltsevpetr/tree/master/HW05/"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myaudio,
    "myaudio description",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)

