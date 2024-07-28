#include "gstrsdeinterlace.hpp"

#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>
#include <gst/video/gstvideofilter.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>



#define GST_CAT_DEFAULT gst_rsdeinterlace_debug
GST_DEBUG_CATEGORY_STATIC (gst_rsdeinterlace_debug);
GST_DEBUG_CATEGORY_STATIC (CAT_PERFORMANCE);


#define UNUSED_ARG(x) (void)x

#define PACKAGE "rsdeinterlace"
#define VERSION "1.0"
#define LICENSE "Proprietary"
#define DESCRIPTION "Plugin to add support for Y8I format on D435 Camera"
#define BINARY_PACKAGE "Plugin to add support for Y8I format on D435 Camera"
#define URL "NONE"











static GstCaps *gst_rsdeinterlace_transform_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * filter);

static GstCaps *gst_rsdeinterlace_fixate_caps (GstBaseTransform * base, GstPadDirection direction, GstCaps * caps, GstCaps * othercaps);

static gboolean gst_rsdeinterlace_src_event (GstBaseTransform * trans, GstEvent * event);

static gboolean gst_rsdeinterlace_set_info (GstVideoFilter * filter, GstCaps * in, GstVideoInfo * in_info, GstCaps * out, GstVideoInfo * out_info);

static GstFlowReturn gst_rsdeinterlace_transform_frame (GstVideoFilter * filter, GstVideoFrame * in_frame, GstVideoFrame * out_frame);




#define gst_rsdeinterlace_parent_class parent_class
G_DEFINE_TYPE (GstRsDeinterlace, gst_rsdeinterlace, GST_TYPE_VIDEO_FILTER);

static void gst_rsdeinterlace_class_init(GstRsDeinterlaceClass* klass)
{
    GstElementClass *element_class = (GstElementClass *) klass;
    GstBaseTransformClass *trans_class = (GstBaseTransformClass *) klass;
    GstVideoFilterClass *filter_class = (GstVideoFilterClass *) klass;


    trans_class->transform_caps = GST_DEBUG_FUNCPTR (gst_rsdeinterlace_transform_caps); 
    trans_class->fixate_caps = GST_DEBUG_FUNCPTR (gst_rsdeinterlace_fixate_caps); 
    trans_class->src_event = GST_DEBUG_FUNCPTR (gst_rsdeinterlace_src_event); 
    filter_class->set_info = GST_DEBUG_FUNCPTR (gst_rsdeinterlace_set_info); 
    filter_class->transform_frame = GST_DEBUG_FUNCPTR (gst_rsdeinterlace_transform_frame); 


    gst_element_class_add_pad_template(element_class, gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS, 
                                        gst_caps_from_string("video/x-raw, format=(string)GRAY8, width=(int) [ 1, max ], height=(int) [ 1, max ]")));

    gst_element_class_add_pad_template(element_class, gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS, 
                                        gst_caps_from_string("video/x-raw, format=(string)GRAY8, width=(int) [ 1, max ], height=(int) [ 1, max ]")));

    
    gst_element_class_set_details_simple (element_class,
      "Y8I RealSense plugin",
      "Y8I RealSense Plugin",
      "Converts Y8I to GRAY8"
      "",
      "Jared Hane");

}


static void gst_rsdeinterlace_init (GstRsDeinterlace * rsdeinterlace)
{
    /*nothing really to do here*/
    rsdeinterlace->v4l2src = NULL;
}



#define DEBUG_DEINTERLACE 0 


static GstElement* gst_find_v4l2_element(GstBin* bin)
{
    gboolean done = FALSE;
    GValue data = G_VALUE_INIT;
    GstIterator* iterator = gst_bin_iterate_elements(bin);
    GstElement *element;
    GstPluginFeature *feature;

    while(!done)
    {
        switch(gst_iterator_next(iterator, &data))
        {
            case GST_ITERATOR_OK:
                element = (GstElement*)g_value_get_object (&data);
                feature = GST_PLUGIN_FEATURE (gst_element_get_factory (element));
                if(strcmp("v4l2src", gst_plugin_feature_get_name(feature)) == 0)
                {
                    gst_iterator_free(iterator);
                    return element;
                }
                g_value_reset (&data);
                break;
            case GST_ITERATOR_RESYNC: 
                gst_iterator_resync (iterator);
                break;
            case GST_ITERATOR_ERROR:
            case GST_ITERATOR_DONE: 
                done = TRUE;
                break;
        }       
    }
    gst_iterator_free (iterator);
    return NULL;
}

static void gst_v4l2_setup_callbacks(GstRsDeinterlace* rsdeinterlace, GstElement* bin_element)
{
    GstElement* v4l2_element = gst_find_v4l2_element(GST_BIN_CAST(bin_element));
    if(v4l2_element)
        rsdeinterlace->v4l2src = v4l2_element;
}



static GstCaps *gst_rsdeinterlace_transform_caps (GstBaseTransform * trans, GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
    GstCaps *ret;
    GstStructure *structure;
    GstCapsFeatures *features;
    gint i, n;

    GstRsDeinterlace * rsdeinterlace = GST_RSDEINTERLACE(trans);


    if(!rsdeinterlace->v4l2src)
    {
        GstElement* parent = GST_ELEMENT_PARENT(GST_ELEMENT(trans));
        if(GST_IS_BIN(parent))
            gst_v4l2_setup_callbacks(rsdeinterlace, parent); 
    }

    GST_DEBUG_OBJECT (trans, "Transforming caps %" GST_PTR_FORMAT " in direction %s", caps, (direction == GST_PAD_SINK) ? "sink" : "src");

    ret = gst_caps_new_empty ();
    n = gst_caps_get_size (caps);
    for (i = 0; i < n; i++)
    {
        structure = gst_caps_get_structure (caps, i);
        features = gst_caps_get_features(caps, i);

        if (i > 0 && gst_caps_is_subset_structure_full (ret, structure, features))
            continue;
        
        structure = gst_structure_copy (structure);

        if (!gst_caps_features_is_any (features) && gst_caps_features_is_equal (features, GST_CAPS_FEATURES_MEMORY_SYSTEM_MEMORY))
        {
            gst_structure_set (structure, "width", GST_TYPE_INT_RANGE, 1, G_MAXINT, "height", GST_TYPE_INT_RANGE, 1, G_MAXINT, NULL);

            if (gst_structure_has_field (structure, "pixel-aspect-ratio")) 
                gst_structure_set (structure, "pixel-aspect-ratio", GST_TYPE_FRACTION_RANGE, 1, G_MAXINT, G_MAXINT, 1, NULL);
        }
        gst_caps_append_structure_full (ret, structure, gst_caps_features_copy (features));
    }

    if (filter) 
    {
        GstCaps *intersection;
        intersection = gst_caps_intersect_full (filter, ret, GST_CAPS_INTERSECT_FIRST);
        gst_caps_unref (ret);
        ret = intersection;
    }

    GST_DEBUG_OBJECT (trans, "returning caps: %" GST_PTR_FORMAT, ret);
    
    return ret;
}

static gboolean gst_rsdeinterlace_src_event (GstBaseTransform * trans, GstEvent * event)
{
    /*nothing to do here for now*/
    gboolean ret;
    ret = GST_BASE_TRANSFORM_CLASS (parent_class)->src_event (trans, event);
    return ret;
}



static GstCaps *gst_rsdeinterlace_fixate_caps (GstBaseTransform * base, GstPadDirection direction, GstCaps * caps, GstCaps * othercaps)
{
    GstStructure *ins, *outs;
    gint from_width, from_height, to_width, to_height, fixed_width_difference, fixed_height_difference;

    othercaps = gst_caps_truncate (othercaps);
    othercaps = gst_caps_make_writable (othercaps);

    GST_DEBUG_OBJECT (base, "trying to fixate othercaps %" GST_PTR_FORMAT " based on caps %" GST_PTR_FORMAT, othercaps, caps);

    ins = gst_caps_get_structure (caps, 0);
    outs = gst_caps_get_structure (othercaps, 0);

    /* Always set the pixel-aspect-ratio to be 1/1  */
    gst_structure_set (outs, "pixel-aspect-ratio", GST_TYPE_FRACTION, 1, 1, NULL);

    gst_structure_get_int (ins, "width", &from_width);
    gst_structure_get_int (ins, "height", &from_height);

    gst_structure_get_int (outs, "width", &to_width);
    gst_structure_get_int (outs, "height", &to_height);


    /* If dimensions are already fixed ensure the output dimensions are suitable */
    if(to_width && to_height) 
    {
        GST_DEBUG_OBJECT (base, "dimensions  %dx%d already fixed, checking them", to_width, to_height);

        fixed_width_difference = from_width * 2 - to_width;
        fixed_height_difference = from_height - to_height;

        if(fixed_width_difference != 0 || fixed_height_difference != 0)
        {
            to_width = from_width * 2;
            to_height = from_height;
            gst_structure_set(outs, "width", G_TYPE_INT, to_width, "height", G_TYPE_INT, to_height, NULL);
        }
        goto done;
    }
    else
    {
        GST_DEBUG_OBJECT (base, "dimensions not fixed. Fixating them");
        to_width = from_width * 2;
        to_height = from_height;
        gst_structure_set(outs, "width", G_TYPE_INT, to_width, "height", G_TYPE_INT, to_height, NULL);
        goto done;
    }


done:
    GST_DEBUG_OBJECT (base, "fixated othercaps to %" GST_PTR_FORMAT, othercaps);
    return othercaps;
}


static gboolean update_v4l2_format(GstRsDeinterlace * rsdeinterlace, const gint width, const gint height)
{
    gint fd;
    g_object_get(G_OBJECT(rsdeinterlace->v4l2src), "device-fd", &fd, NULL);
    if(fd)
    {
        struct v4l2_format format;
        memset(&format, 0, sizeof(struct v4l2_format));
        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width = width;
        format.fmt.pix.height = height;
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_Y8I;
        format.fmt.pix.field = V4L2_FIELD_NONE;

        if(ioctl(fd, VIDIOC_S_FMT, &format) < 0)
            return FALSE;
        return TRUE;
    }
    return FALSE;
}


static gboolean gst_rsdeinterlace_set_info (GstVideoFilter * filter, GstCaps * in, GstVideoInfo * in_info, GstCaps * out, GstVideoInfo * out_info)
{
    GstBaseTransform * base = GST_BASE_TRANSFORM(filter);
    GstRsDeinterlace * rsdeinterlace = GST_RSDEINTERLACE(base);

    if(!rsdeinterlace->v4l2src)
    {
        GST_ELEMENT_ERROR (filter, CORE, NEGOTIATION, (NULL), ("Error failed to find v4l2src in pipeline."));
        return FALSE;
    }
    else
    {
        gst_base_transform_set_passthrough (base, FALSE);

        if(!update_v4l2_format(rsdeinterlace, GST_VIDEO_INFO_WIDTH(in_info), GST_VIDEO_INFO_HEIGHT(in_info)))
        {
            GST_ELEMENT_ERROR (base, CORE, NEGOTIATION, (NULL), ("Failed to set v4l2 format."));
            return FALSE;
        }
        else
            return TRUE;
    }
}


static GstFlowReturn gst_rsdeinterlace_transform_frame (GstVideoFilter * filter, GstVideoFrame * in_frame, GstVideoFrame * out_frame)
{
    guint8 *input_image_data = (guint8*)GST_VIDEO_FRAME_PLANE_DATA(in_frame, 0);
    guint8 *left_image_ptr   = (guint8*)GST_VIDEO_FRAME_PLANE_DATA(out_frame, 0);
    guint8 *right_image_ptr  = left_image_ptr + GST_VIDEO_FRAME_PLANE_STRIDE(in_frame, 0);

    for(gint i = 0; i < GST_VIDEO_FRAME_HEIGHT(in_frame); i++)
    {
        for(gint t = 0; t < GST_VIDEO_FRAME_WIDTH(in_frame); t++)
        {
            *(left_image_ptr++) = *(input_image_data++);
            *(right_image_ptr++) = *(input_image_data++);
        }
        left_image_ptr += GST_VIDEO_FRAME_PLANE_STRIDE(in_frame, 0);
        right_image_ptr +=  GST_VIDEO_FRAME_PLANE_STRIDE(in_frame, 0);
    }

    return GST_FLOW_OK;
}



static gboolean rsdeinterlace_plugin_init (GstPlugin * plugin)
{
    GST_DEBUG_CATEGORY_INIT (gst_rsdeinterlace_debug, "rsdeinterlace", 0, "rsdeinterlace plugin");

    GST_DEBUG_CATEGORY_GET (CAT_PERFORMANCE, "GST_PERFORMANCE");

    return gst_element_register (plugin, "rsdeinterlace", GST_RANK_PRIMARY, GST_TYPE_RSDEINTERLACE);
}


GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    rsdeinterlace,
    DESCRIPTION, rsdeinterlace_plugin_init, VERSION, LICENSE, BINARY_PACKAGE, URL)

