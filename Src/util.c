#include "mty_gui.h"
#include <string.h>

uint8_t *readFile(const char *filename, size_t *len){
	FILE *file;
	uint8_t *buffer;
	uint64_t fileLen;

	//Open file
	file = fopen(filename, "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s", filename);
		return NULL;
	}

	//Get file length
	fseek(file, 0, SEEK_END);
	fileLen=ftell(file);
	*len = fileLen;
	fseek(file, 0, SEEK_SET);

	//Allocate memory
	buffer=(uint8_t *)calloc(1,fileLen+1);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
        fclose(file);
		return NULL;
	}
	//Read file contents into buffer
	fread(buffer, fileLen, 1, file);
	buffer[fileLen]='\0';
	fclose(file);
	return buffer;
}

gchar* my_convert(gchar* raw, gssize len, const char* TO, const char* FROM, gsize* bread, gsize* bwrite)
{
	GError *error = NULL;
	gchar* result;
	// Split into lines
	gchar** lines = g_strsplit_set(raw, "\n", -1);
	int n_lines = g_strv_length(lines);
	
	gchar* converted[n_lines+1];
	converted[n_lines] = NULL;
	
	// Try to convert each line
	int i = 0;
	for(; i < n_lines; i++)
	{
		gchar* rawline = lines[i];
		gsize lread;
		gsize lwrite;
		gsize ilen = strlen(rawline);
		converted[i] = g_convert_with_fallback(rawline, -1, TO, FROM, NULL, &lread, &lwrite, &error);
		
		// Failed to convert
		if(error)
		{
			int j = 0;
			// Forward to the password
			while(lines[i][j]!=' ' && j < ilen)
			{
				j++;
			}
			//replace it with only spaces.
			while(lines[i][j]!='\t' && j < ilen)
			{
				lines[i][j++] = ' ';
			}
			g_error_free(error);
			error = NULL;
			// Try to convert again
			converted[i] = g_convert_with_fallback(rawline, -1, TO, FROM, NULL, &lread, &lwrite, &error);
			if(error){
				fprintf(stderr,"Error:\n%s\n",error->message);
				g_error_free(error);
				// Failed again, so ignore this line
				converted[i] = g_strnfill(1, '\0');
			}
		}
	}
	
	result = g_strjoinv( "\n", converted );
	for(i = 0; i < n_lines; i++)
	{
		g_free(converted[i]);
	}
	g_strfreev(lines);
	
	return result;
}

uint8_t *readFileFromEncodingToUTF8(const char *filename, gsize *len, const char *input_encoding){
	size_t flen = 0;
	gchar *raw = (char *)readFile(filename, &flen);
	gsize bread = 0;
	gsize bwrit = 0;
	gchar *utf8 = my_convert(raw, -1, "UTF-8", input_encoding, &bread, &bwrit);
	*len = bwrit;
	free(raw);
	return (uint8_t *)utf8;
}

int32_t writeFileFromUTF8ToEncoding(const char *filename, gchar *data, const char *output_encoding){
	GError *error = NULL;
	gsize bread = 0;
	gsize bwrit = 0;
	gchar *encode = g_convert(data, -1, output_encoding, "UTF-8", &bread, &bwrit, &error);
	if(error){
		fprintf(stderr,"Error:\n%s\n",error->message);
		g_error_free(error);
		return 1;
	}
	
	//Hack to get around glib duplicating newlines during conversion
	gchar** lines = g_strsplit_set(encode, "\n", -1);
	g_free(encode);
	encode = g_strjoinv( "\n", lines);
	g_strfreev(lines);
	
	
	int32_t success = writeFile(filename, (uint8_t *)encode, bwrit);
	free(encode);
	return success;
}

int32_t writeFile(const char *filename, uint8_t *data, size_t len){
	FILE *fp;
	
	fp = fopen(filename, "w+");
	if(!fp) return -1;
	
	fwrite(data, 1, len, fp);
	
	fclose(fp);
	
	return 0;
}

void setMonospace(GtkWidget *widget){
	PangoFontDescription *font_desc;
	font_desc = pango_font_description_from_string("monospace");
	gtk_widget_modify_font(widget, font_desc);
	pango_font_description_free(font_desc);  
}