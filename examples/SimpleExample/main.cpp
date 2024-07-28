#include <gst/gst.h>
#include <cassert>
#include <string>
#include <iostream>





struct Pipeline
{
    GstElement* pipe;
    GstElement* src; 
    GstElement* rsdeinterlace;
    GstElement* videoconvert;
    GstElement* autovideosink;
};




GstElement* CreateElement(const std::string& element, const std::string& elementName, GstElement* pipe)
{
    GstElement* output;

    if (G_UNLIKELY (!(output = (gst_element_factory_make (element.c_str(), element_name.c_str()))))) 
        return NULL;
    if (G_UNLIKELY (!gst_bin_add (GST_BIN_CAST (pipe), output))) 
    {
        gst_object_unref(G_OBJECT(output));
        return NULL;
    } 
    return output;
}





int main(int argc, char* argv[])
{
    Pipeline pipeline;

    gst_init(NULL, NULL);


    pipeline.pipe = gst_pipeline_new ("Example_Pipeline");
    pipeline.src = CreateElement("v4l2src", "source", pipeline.pipe);
    pipeline.rsdeinterlace = CreateElement("rsdeinterlace", "rsdeinterlace", pipeline.pipe);
    pipeline.videoconvert = CreateElement("videoconvert", "converter", pipeline.pipe);
    pipeline.autovideosink = CreateElement("autovideosink", "sink", pipeline.pipe);

    assert(pipeline.pipe && pipeline.src && pipeline.rsdeinterlace && pipeline.videoconvert && pipeline.autovideosink);



    




}