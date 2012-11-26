#include "mty_gui.h"

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

uint8_t *readFileFromEncodingToUTF8(const char *filename, gsize *len, const char *input_encoding){
	GError *error = NULL;
	size_t flen = 0;
	gchar *raw = (char *)readFile(filename, &flen);
	gsize bread = 0;
	gsize bwrit = 0;
	gchar *utf8 = g_convert(raw, -1, "UTF-8", input_encoding, &bread, &bwrit, &error);
	*len = bwrit;
	free(raw);
	if(error){
		fprintf(stderr,"Error:\n%s\n",error->message);
	}
	return (uint8_t *)utf8;
}

int32_t writeFileFromUTF8ToEncoding(const char *filename, gchar *data, const char *output_encoding){
	GError *error = NULL;
	gsize bread = 0;
	gsize bwrit = 0;
	gchar *encode = g_convert(data, -1, output_encoding, "UTF-8", &bread, &bwrit, &error);
	if(error){
		fprintf(stderr,"Error:\n%s\n",error->message);
		return 1;
	}
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