#include <obs-module.h>
#include "plugin-support.h"
#include "zoom-filter-ui.h"

#define S_SCALE_FACTOR "scale_factor"

obs_properties_t *zoom_filter_get_properties(void *data)
{
    UNUSED_PARAMETER(data);
    obs_properties_t *props = obs_properties_create();
    obs_properties_add_float_slider(props, S_SCALE_FACTOR, 
        obs_module_text("ScaleFactor"), 1.0, 5.0, 0.1);
    return props;
}

void zoom_filter_get_defaults(obs_data_t *settings)
{
    obs_data_set_default_double(settings, S_SCALE_FACTOR, 1.0);
}
