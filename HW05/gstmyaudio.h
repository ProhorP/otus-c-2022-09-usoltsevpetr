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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_MYAUDIO_H_
#define _GST_MYAUDIO_H_

#include <gst/base/gstbasesrc.h>

G_BEGIN_DECLS
#define GST_TYPE_MYAUDIO   (gst_myaudio_get_type())
#define GST_MYAUDIO(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MYAUDIO,GstMyaudio))
#define GST_MYAUDIO_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MYAUDIO,GstMyaudioClass))
#define GST_IS_MYAUDIO(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MYAUDIO))
#define GST_IS_MYAUDIO_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MYAUDIO))
typedef struct _GstMyaudio GstMyaudio;
typedef struct _GstMyaudioClass GstMyaudioClass;

struct _GstMyaudio
{
  GstBaseSrc base_myaudio;

  gchar *location;
  int fd;

};

#define HEADER_SIZE 44

typedef unsigned char byte;

union int_data
{
  int int_value;
  byte int_bytes[4];
};

union short_data
{
  short short_value;
  byte short_bytes[2];
};

struct header_struct
{
  char chunk_id[4];
  union int_data chunk_size;
  char format[4];
  char subchunk1_id[4];
  union int_data subchunk1_size;
  union short_data audio_format;
  union short_data num_channels;
  union int_data sample_rate;
  union int_data byte_rate;
  union short_data block_align;
  union short_data bits_per_sample;
  char subchunk2_id[4];
  union int_data subchunk2_size;
} header_struct_type;

struct _GstMyaudioClass
{
  GstBaseSrcClass base_myaudio_class;
};

GType gst_myaudio_get_type (void);

G_END_DECLS
#endif
