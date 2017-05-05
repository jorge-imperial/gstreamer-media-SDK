/*
 *  Copyright (C) 2011-2013 Intel Corporation
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@intel.com>
 *  Copyright (C) 2016 Intel Corporation
 *    Author: Ishmael Visayana Sameen <ishmael.visayana.sameen@intel.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */

#ifndef GST_MFX_DECODER_H
#define GST_MFX_DECODER_H

#include <mfxplugin.h>
#include "gstmfxsurface.h"
#include "gstmfxtaskaggregator.h"
#include "gstmfxprofile.h"
#include "gstmfxfilter.h"

G_BEGIN_DECLS

#define GST_TYPE_MFX_DECODER (gst_mfx_decoder_get_type ())

#define GST_MFX_DECODER(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_MFX_DECODER, GstMfxDecoder))

#define GST_MFX_IS_DECODER(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_MFX_DECODER))

#define GST_MFX_DECODER_CAST(display) \
    ((GstMfxDecoder *)(display))

#define GST_MFX_DECODER_GET_PRIVATE(display) \
    (GST_MFX_DECODER_CAST (display)->priv)

#define GST_MFX_DECODER_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_MFX_DECODER, GstMfxDecoderClass))

#define GST_MFX_IS_DECODER_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_MFX_DECODER))

#define GST_MFX_DECODER_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_MFX_DECODER, GstMfxDecoderClass))

/**
* GST_MFX_DECODER_GET_CLASS_TYPE:
* @decoder: a #GstMfxDecoder
*
* Returns the #decoder class type
* This is an internal macro that does not do any run-time type check.
*/
#undef  GST_MFX_DECODER_GET_CLASS_TYPE
#define GST_MFX_DECODER_GET_CLASS_TYPE(decoder) \
    GST_MFX_DECODER_GET_CLASS (GST_MFX_DECODER (decoder))


typedef struct _GstMfxDecoder GstMfxDecoder;
typedef struct _GstMfxDecoderClass GstMfxDecoderClass;

struct _GstMfxDecoder
{
	/*< private > */
	GstObject parent_instance;

	GstMfxTaskAggregator *aggregator;
	GstMfxTask *decode;
	GstMfxProfile profile;
	GstMfxSurfacePool *pool;
	GstMfxFilter *filter;
	GByteArray *bitstream;
	GByteArray *codec_data;

	GQueue decoded_frames;
	GQueue pending_frames;
	GQueue discarded_frames;

	mfxSession session;
	mfxVideoParam params;
	mfxFrameAllocRequest request;
	mfxBitstream bs;
	mfxPluginUID plugin_uid;

	GstVideoInfo info;
	gboolean inited;
	gboolean was_reset;
	gboolean has_ready_frames;
	gboolean memtype_is_system;
	gboolean enable_csc;
	gboolean enable_deinterlace;
	gboolean skip_corrupted_frames;
	gboolean can_double_deinterlace;
	guint num_partial_frames;

	/* For special double frame rate deinterlacing case */
	GstClockTime current_pts;
	GstClockTime duration;
	GstClockTime pts_offset;
};

//TODO: cleanup.
struct _GstMfxDecoderClass
{
	/*< private >*/
	GstObjectClass parent_class;

	/*< protected >*/
	//guint display_type;

	/*< public >*/
	//void(*init)            (GstMfxDecoder * decoder);
};

/**
* GstMfxDecoderStatus:
* @GST_MFX_DECODER_STATUS_SUCCESS: Success.
* @GST_MFX_DECODER_STATUS_FLUSHED: Decoder is flushed.
* @GST_MFX_DECODER_STATUS_ERROR_ALLOCATION_FAILED: No memory left.
* @GST_MFX_DECODER_STATUS_ERROR_INIT_FAILED: Decoder initialization failure.
* @GST_MFX_DECODER_STATUS_ERROR_UNSUPPORTED_CODEC: Unsupported codec.
* @GST_MFX_DECODER_STATUS_ERROR_MORE_DATA: Not enough input data to decode.
* @GST_MFX_DECODER_STATUS_ERROR_MORE_SURFACE: No surface left to hold the decoded picture.
* @GST_MFX_DECODER_STATUS_ERROR_INVALID_SURFACE: Invalid surface.
* @GST_MFX_DECODER_STATUS_ERROR_BITSTREAM_PARSER: Invalid or unsupported bitstream data.
* @GST_MFX_DECODER_STATUS_ERROR_UNSUPPORTED_PROFILE: Unsupported codec profile.
* @GST_MFX_DECODER_STATUS_ERROR_UNSUPPORTED_CHROMA_FORMAT: Unsupported chroma format.
* @GST_MFX_DECODER_STATUS_ERROR_INVALID_PARAMETER: Unsupported parameter.
* @GST_MFX_DECODER_STATUS_ERROR_UNKNOWN: Unknown error.
*
* Decoder status for gst_mfx_decoder_get_surface().
*/
typedef enum {
  GST_MFX_DECODER_STATUS_SUCCESS = 0,
  GST_MFX_DECODER_STATUS_FLUSHED,
  GST_MFX_DECODER_STATUS_ERROR_ALLOCATION_FAILED,
  GST_MFX_DECODER_STATUS_ERROR_INIT_FAILED,
  GST_MFX_DECODER_STATUS_ERROR_UNSUPPORTED_CODEC,
  GST_MFX_DECODER_STATUS_ERROR_MORE_DATA,
  GST_MFX_DECODER_STATUS_ERROR_MORE_SURFACE,
  GST_MFX_DECODER_STATUS_ERROR_INVALID_SURFACE,
  GST_MFX_DECODER_STATUS_ERROR_BITSTREAM_PARSER,
  GST_MFX_DECODER_STATUS_ERROR_UNSUPPORTED_PROFILE,
  GST_MFX_DECODER_STATUS_ERROR_INVALID_PARAMETER,
  GST_MFX_DECODER_STATUS_ERROR_UNKNOWN = -1
} GstMfxDecoderStatus;

GstMfxDecoder *
gst_mfx_decoder_new (GstMfxDecoder * decoder, GstMfxTaskAggregator * aggregator,
    GstMfxProfile profile, const GstVideoInfo * info, mfxU16 async_depth,
    gboolean live_mode);

GType
gst_mfx_decoder_get_type(void) G_GNUC_CONST;

GstMfxDecoder *
gst_mfx_decoder_ref (GstMfxDecoder * decoder);

void
gst_mfx_decoder_unref (GstMfxDecoder * decoder);

void
gst_mfx_decoder_replace (GstMfxDecoder ** old_decoder_ptr,
    GstMfxDecoder * new_decoder);

GstMfxProfile
gst_mfx_decoder_get_profile (GstMfxDecoder * decoder);

gboolean
gst_mfx_decoder_get_decoded_frames (GstMfxDecoder * decoder,
    GstVideoCodecFrame ** out_frame);

GstVideoCodecFrame *
gst_mfx_decoder_get_discarded_frame (GstMfxDecoder * decoder);

GstVideoInfo *
gst_mfx_decoder_get_video_info (GstMfxDecoder * decoder);

void
gst_mfx_decoder_skip_corrupted_frames (GstMfxDecoder * decoder);

void
gst_mfx_decoder_should_use_video_memory (GstMfxDecoder * decoder,
    gboolean memtype_is_video);

void
gst_mfx_decoder_reset (GstMfxDecoder * decoder);

GstMfxDecoderStatus
gst_mfx_decoder_decode (GstMfxDecoder * decoder,
    GstVideoCodecFrame * frame);

GstMfxDecoderStatus
gst_mfx_decoder_flush (GstMfxDecoder * decoder);

G_END_DECLS

#endif /* GST_MFX_DECODER_H */
