#include <gtk/gtk.h>
#include <stdio.h>
#include <atmi.h>
#include <fml32.h>
#include "biblioBD.fml.h"

typedef struct hospital {
    int idPaciente;
    char nombre[20];
    char apellido[20];
    char telefono[10];
} hospital;

GtkWidget *entryId;
GtkWidget *entryNombre;
GtkWidget *entryApellido;
GtkWidget *entryTelefono;

// Declaración de funciones
void btnSubmitCallback(GtkWidget *widget, gpointer data);
void handleServiceCallPaciente(hospital *hos);

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *labelId;
    GtkWidget *labelNombre;
    GtkWidget *labelApellido;
    GtkWidget *labelTelefono;
    GtkWidget *button;

    // Inicialización de GTK
    gtk_init(&argc, &argv);

    // Creación de la ventana principal
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interfaz de Usuario");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Creación del contenedor
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Etiqueta y campo de entrada para el ID del paciente
    labelId = gtk_label_new("ID del Paciente:");
    gtk_box_pack_start(GTK_BOX(box), labelId, FALSE, FALSE, 0);
    entryId = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entryId, FALSE, FALSE, 0);

    // Etiqueta y campo de entrada para el nombre del paciente
    labelNombre = gtk_label_new("Nombre:");
    gtk_box_pack_start(GTK_BOX(box), labelNombre, FALSE, FALSE, 0);
    entryNombre = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entryNombre, FALSE, FALSE, 0);

    // Etiqueta y campo de entrada para el apellido del paciente
    labelApellido = gtk_label_new("Apellido:");
    gtk_box_pack_start(GTK_BOX(box), labelApellido, FALSE, FALSE, 0);
    entryApellido = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entryApellido, FALSE, FALSE, 0);

    // Etiqueta y campo de entrada para el teléfono del paciente
    labelTelefono = gtk_label_new("Teléfono:");
    gtk_box_pack_start(GTK_BOX(box), labelTelefono, FALSE, FALSE, 0);
    entryTelefono = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entryTelefono, FALSE, FALSE, 0);

    // Botón de envío
    button = gtk_button_new_with_label("Enviar");
    g_signal_connect(button, "clicked", G_CALLBACK(btnSubmitCallback), 
                     (gpointer)window); // Pasamos la ventana como datos
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    // Mostramos todos los widgets
    gtk_widget_show_all(window);

    // Iniciamos el bucle principal de GTK
    gtk_main();

    return 0;
}

// Callback para el botón de envío
void btnSubmitCallback(GtkWidget *widget, gpointer data) {
    GtkWidget *window = (GtkWidget *)data; // Recuperamos la ventana
    const gchar *id_str = gtk_entry_get_text(GTK_ENTRY(entryId));
    const gchar *nombre = gtk_entry_get_text(GTK_ENTRY(entryNombre));
    const gchar *apellido = gtk_entry_get_text(GTK_ENTRY(entryApellido));
    const gchar *telefono = gtk_entry_get_text(GTK_ENTRY(entryTelefono));

    // Mensaje de depuración
    g_print("ID del Paciente: %s\n", id_str);
    g_print("Nombre: %s\n", nombre);
    g_print("Apellido: %s\n", apellido);
    g_print("Teléfono: %s\n", telefono);

    // Creamos una estructura de paciente con los datos ingresados
    hospital hos;
    hos.idPaciente = atoi(id_str); // Convertimos el ID a entero
    strncpy(hos.nombre, nombre, sizeof(hos.nombre));
    strncpy(hos.apellido, apellido, sizeof(hos.apellido));
    strncpy(hos.telefono, telefono, sizeof(hos.telefono));

    // Llamamos a la función para manejar la llamada al servicio Tuxedo
    handleServiceCallPaciente(&hos);

    // Cerramos la ventana
    gtk_widget_destroy(window);
}

// Función para manejar la llamada al servicio Tuxedo para paciente
void handleServiceCallPaciente(hospital *hos) {
    // Aquí va el código para el servicio de paciente
    g_print("Llamada al servicio de paciente\n");

    FBFR32 *fbfr;
    FBFR32 *recv;
    long ivL_tamLongt;
    char msgbuf[64];
    FLDLEN32 flen;

    if(tpinit((TPINIT *) NULL) == -1) {
        g_print("Error en la conexión, tperrno = %d \n", tperrno);
        return;
    }

    //Reservamos espacio para el buffer FML
    if ((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        g_print("Error reservando espacio para Buffer fbfrn");
        tpterm();
        return;
    }

    if ((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        g_print("Error Reservando espacio para Buffer recv\n");
        tpterm();
        return;
    }

    // Insertamos datos en el buffer FML
    if(Fadd32(fbfr, IDPACIENTE, (char *)&(hos->idPaciente), 0) < 0){
        g_print("\tError insertando campo FML (ID DEL PACIENTE)\n");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    if(Fadd32(fbfr, NOMBRE, hos->nombre, 0) < 0){
        g_print("\tError insertando campo FML (NOMBRE)\n");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    if(Fadd32(fbfr, APELLIDO, hos->apellido, 0) < 0){
        g_print("\tError insertando campo FML (APELLIDO)\n");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    if(Fadd32(fbfr, TELEFONO, hos->telefono, 0) < 0){
        g_print("\tError insertando campo FML (TELEFONO)\n");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    // Llamada al servicio 'insert_Negocio_FML'
    if(tpcall("insert_Negocio_FML", (char *)fbfr, 0, (char **)&recv, &ivL_tamLongt, 0L) == -1){
        g_print("Error en la llamada al servicio: tperrno = %d\n", tperrno);
        tpfree((char *)fbfr);
        tpfree((char *)recv);
        tpterm();
        return;
    }

    flen = sizeof(msgbuf);
    Fget32(recv, OUTPUT, 0, (char *)msgbuf, &flen);
    g_print("Respuesta del servidor: %s\n", msgbuf);

    // Liberamos el buffer y desconectamos de la aplicación
    tpfree((char *)fbfr);
    tpfree((char *)recv);
    tpterm();
}
