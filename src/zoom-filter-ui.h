#ifndef ZOOM_FILTER_UI_H
#define ZOOM_FILTER_UI_H

#include <obs-module.h>

obs_properties_t *zoom_filter_get_properties(void *data);
void zoom_filter_get_defaults(obs_data_t *settings);

#endif
