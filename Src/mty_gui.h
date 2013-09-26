#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourceview.h>
#include <glib.h>

#define gtk_update while(gtk_events_pending()) gtk_main_iteration();

uint8_t *readFile(const char *filename, size_t *len);
int32_t  writeFile(const char *filename, uint8_t *data, size_t len);
uint8_t *readFileFromEncodingToUTF8(const char *filename, gsize *len, const char *input_encoding);
int32_t  writeFileFromUTF8ToEncoding(const char *filename, gchar *data, const char *output_encoding);
void setMonospace(GtkWidget *widget);
