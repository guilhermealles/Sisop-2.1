#include <gtk/gtk.h>

/* Para rodar: gcc -o simple testeG.c `pkg-config --libs --cflags gtk+-2.0`  */

GtkWidget *entry1;


// callback botao
void get_value(GtkWidget *widget, gpointer data) {

	const gchar *entry_text1;
    GtkWidget* ld = (GtkWidget*)data;

    entry_text1 = gtk_entry_get_text(GTK_ENTRY(entry1));
    g_print ("Mensage:%s\n", entry_text1);

	// Send message to the client app

}


int main(int argc, char *argv[]) {

	GtkWidget *window;
	GtkWidget *table;
	GtkWidget *label1;

	GtkWidget *button;
	GtkWidget *halign;

	gtk_init(&argc, &argv);

	// janela

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_title(GTK_WINDOW(window), "Earth Chat");
	gtk_container_set_border_width(GTK_CONTAINER(window), 20);

	table = gtk_table_new(3, 2, FALSE);
	gtk_container_add(GTK_CONTAINER(window), table);

	//  label e campo
	label1 = gtk_label_new("Command/Message");
	entry1 = gtk_entry_new();

	gtk_table_attach(GTK_TABLE(table), label1, 0, 1, 2, 3,
	  GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	gtk_table_attach(GTK_TABLE(table), entry1, 1, 2, 2, 3,
	  GTK_FILL | GTK_SHRINK, GTK_FILL | GTK_SHRINK, 5, 5);

	// botao

	halign = gtk_alignment_new(0, 0, 0, 0);
  	gtk_container_add(GTK_CONTAINER(table), halign);

	button = gtk_button_new_with_label("Enviar");
  	gtk_widget_set_size_request(button, 80, 30);

	gtk_container_add(GTK_CONTAINER(halign), button);

	g_signal_connect(button, "clicked", G_CALLBACK(get_value), NULL);


	// finalizacao

	gtk_widget_show_all(window);

	g_signal_connect(window, "destroy",
	  G_CALLBACK(gtk_main_quit), &entry1);

	gtk_main();

	return 0;
}
