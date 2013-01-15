#include "mty_gui.h"
#include <windows.h>
typedef struct{
	GtkTextBuffer *tar_buffer;
	GtkTextBuffer *log_buffer;
}SearchArgs;

void search(GtkWidget *btn, gpointer data){
	SearchArgs *args = data;
	GtkTextBuffer *tar_buffer = args->tar_buffer;
	GtkTextBuffer *log_buffer = args->log_buffer;
	GtkTextIter start, end;
	gchar *tar, *log;
	size_t log_len = 0;
	
	/* WRITE TARGET.txt */
	gtk_text_buffer_get_bounds(tar_buffer, &start, &end);
	tar = gtk_text_buffer_get_text(tar_buffer, &start, &end, FALSE);
	if(writeFileFromUTF8ToEncoding("target.txt", tar, "Shift-JIS")){
		fprintf(stderr, "Couldn't write target.txt, using old target.txt\n");
		//TODO: Properly alert the user that target.txt couldn't be written.
	}
	
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = "open";
	ShExecInfo.lpFile = "mty_cl.exe";		
	ShExecInfo.lpParameters = "";	
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;	
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	
	log = (gchar *)readFileFromEncodingToUTF8("log.txt", &log_len, "Shift-JIS");
	gtk_text_buffer_set_text(log_buffer, log, -1);
	
	while(gtk_events_pending()) gtk_main_iteration();
}


int main( int argc, char *argv[]){
	GtkWidget/*Windows*/ 	*window;
	GtkWidget/*Containers*/ *pane;
	GtkWidget/*Widgets*/	*start_btn;
	GdkColor bg;
	bg.red = 0x8000;
	bg.green = 0x9000;
	bg.blue = 0x8000;
	
	//Log
	GtkWidget/*Containers*/	*log_view, *log_box;
	GtkWidget/*Widgets*/	*log_scroll, *log_label;
	GtkTextBuffer *log_buffer;
	GtkTextIter log_iter;
	gchar *log;
	gsize log_len;

	//Target
	GtkWidget/*Containers*/	*tar_view, *tar_box;
	GtkWidget/*Widgets*/	*tar_scroll, *tar_label;
	GtkTextBuffer *tar_buffer;
	GtkTextIter tar_iter;
	gchar *tar;
	gsize tar_len;
	
	gtk_init(&argc, &argv);
	
	/* SET UP TOPLEVEL WINDOW */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 768, 480);
	gtk_window_set_title(GTK_WINDOW(window), "MTY CL");
	gtk_container_set_border_width(GTK_CONTAINER(window), 5);
	GTK_WINDOW(window)->allow_shrink = TRUE;
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &bg);
	g_signal_connect_swapped(G_OBJECT(window), "destroy",
		G_CALLBACK(gtk_main_quit), G_OBJECT(window));
	
	
	/* SET UP TEXT AREAS */
	tar_box = gtk_vbox_new(FALSE, 0);
	log_box = gtk_vbox_new(FALSE, 0);
	
	//Name the areas with labels
	tar_label = gtk_label_new("Target:");
	log_label = gtk_label_new("Log:");
	
	//Align labels left
	gtk_misc_set_alignment(GTK_MISC(tar_label), 0.0f, 0.0f);
	gtk_misc_set_alignment(GTK_MISC(log_label), 0.0f, 0.0f);
	
	gtk_box_pack_start(GTK_BOX(tar_box), tar_label, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(log_box), log_label, FALSE, FALSE, 0);
	
	//Use source buffer and source view since these have undo/redo functionality
	//which gtk+'s textview didn't.
	log_buffer = (GtkTextBuffer *)gtk_source_buffer_new(NULL);
	tar_buffer = (GtkTextBuffer *)gtk_source_buffer_new(NULL);
	
	tar_view = gtk_source_view_new_with_buffer((GtkSourceBuffer *)tar_buffer);
	log_view = gtk_source_view_new_with_buffer((GtkSourceBuffer *)log_buffer);
	
	//Set font of text areas
	//I personally detest nonmonospace when displaying data/text with patterns.
	setMonospace(tar_view);
	setMonospace(log_view);
	
	//Add the text fields into "scrolled windows",
	//which are actually just normal widgets and not windows as far as i can tell.
	tar_scroll = gtk_scrolled_window_new(NULL, NULL);
	log_scroll = gtk_scrolled_window_new(NULL, NULL);
	
	gtk_container_add(GTK_CONTAINER(tar_scroll), tar_view);
	gtk_container_add(GTK_CONTAINER(log_scroll), log_view);
	
	gtk_box_pack_start(GTK_BOX(tar_box), tar_scroll, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(log_box), log_scroll, TRUE, TRUE, 0);
	
	//Read relevant text and put it in the text areas.
	log = (gchar *)readFileFromEncodingToUTF8("log.txt", &log_len, "Shift-JIS");
	tar = (gchar *)readFileFromEncodingToUTF8("target.txt", &tar_len, "Shift-JIS");
	
	gtk_text_buffer_get_iter_at_offset(log_buffer, &log_iter, 0);
	gtk_text_buffer_get_iter_at_offset(tar_buffer, &tar_iter, 0);
	
	gtk_text_buffer_insert(log_buffer, &log_iter, log, -1);
	gtk_text_buffer_insert(tar_buffer, &tar_iter, tar, -1);
	
	//The log shouldn't be editable. I don't plan on implementing any editing tools such as saving for the log.
	gtk_text_view_set_editable(GTK_TEXT_VIEW(log_view), FALSE);
	
	
	/* SET UP START BUTTON */
	start_btn = gtk_button_new_with_label("Start search");
	gtk_widget_set_size_request(start_btn, 80, 35);
	SearchArgs *start_btn_args = calloc(sizeof(SearchArgs),1);
	start_btn_args->log_buffer = log_buffer;
	start_btn_args->tar_buffer = tar_buffer;
	g_signal_connect(start_btn, "clicked", G_CALLBACK(search), start_btn_args);
	gtk_box_pack_start(GTK_BOX(tar_box), start_btn, FALSE, FALSE, 0);
	
	
	/* SET UP PANE */
	//I'm using a pane instead of a hbox since automatic sizing between scrolledwindows is
	//not identical to between the text views alone... text views alone would, when competing
	//for space, let the one with the widest text get the most space. if you wrap them in
	//scrolled windows, however, this no longer works and they split it 50/50 for an even
	//uglier interface.
	//it also lets the user edit the division, which (in retrospect) is reason enough...
	pane = gtk_hpaned_new();
	
	//Without this, target view would get unresonable small by default
	//Unfortunately, despite being a "request", it makes it a fixed minimum.
	gtk_widget_set_size_request(tar_box, 250, -1);
	
	gtk_paned_pack1(GTK_PANED(pane), tar_box, FALSE, FALSE);
	gtk_paned_pack2(GTK_PANED(pane), log_box, TRUE, FALSE);
	
	
	/* SHOW */
	gtk_container_add(GTK_CONTAINER(window), pane);
	
	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}