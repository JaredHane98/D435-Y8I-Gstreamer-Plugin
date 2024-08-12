#ifndef __GSTRSDEINTERLACE_HPP__
#define __GSTRSDEINTERLACE_HPP__

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

G_BEGIN_DECLS

#define GST_TYPE_RSDEINTERLACE (gst_rsdeinterlace_get_type())

#define GST_RSDEINTERLACE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_RSDEINTERLACE,GstRsDeinterlace))

#define GST_RSDEINTERLACE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RSDEINTERLACE,GstRsDeinterlaceClass))

#define GST_IS_RSDEINTERLACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RSDEINTERLACE))

#define GST_IS_RSDEINTERLACE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RSDEINTERLACE))

#define GST_RSDEINTERLACE_CAST(obj)  ((GstRsDeinterlace*)(obj))


typedef struct _GstRsDeinterlace GstRsDeinterlace;
typedef struct _GstRsDeinterlaceClass GstRsDeinterlaceClass;
typedef struct _V4L2FormatInterface V4L2FormatInterface;


struct _V4L2FormatInterface
{
    GstElement *v4l2src = NULL;
};

struct _GstRsDeinterlace
{
    GstVideoFilter element;
    GstElement *v4l2src;
};

struct _GstRsDeinterlaceClass
{
    GstVideoFilterClass parent_class;
};

G_GNUC_INTERNAL GType gst_rsdeinterlace_get_type (void);

G_END_DECLS

#endif // __GSTRSDEINTERLACE_HPP__
