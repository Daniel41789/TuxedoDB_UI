#include <gtk/gtk.h>
#include <stdio.h>
#include <atmi.h>
#include <fml32.h>
#include "biblioBD.fml.h"

typedef struct hospital {
    int idDoctor;
    char nombre[20];
    char apellido[20];
    char especialidad[20];
} hospital;

// Declaración de funciones
void btnSubmitCallback(GtkWidget *widget, gpointer data);
void handleServiceCall(const gchar *doctorName);

int main(int argc, char **argv) {
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *button;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Interfaz de Usuario");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    label = gtk_label_new("Doctor:");
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

    entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

    button = gtk_button_new_with_label("Enviar");
    g_signal_connect(button, "clicked", G_CALLBACK(btnSubmitCallback), (gpointer)entry);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

void btnSubmitCallback(GtkWidget *widget, gpointer data) {
    GtkWidget *entry = (GtkWidget *)data;
    const gchar *zaL_hosNombre = gtk_entry_get_text(GTK_ENTRY(entry));

    g_print("Doctor: %s\n", zaL_hosNombre);

    handleServiceCall(zaL_hosNombre);
}

void handleServiceCall(const gchar *doctorName) {
    FBFR32 *fbfr;
    FBFR32 *recv;
    long ivL_tamLongt;
    int ivL_numOcurr;
    hospital *spL_hosRegis;

    if(tpinit((TPINIT *) NULL) == -1) {
        g_print("Error en la conexión, tperrno = %d \n", tperrno);
        return;
    }

    if((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL) {
        g_print("Error reservando espacio para Buffer fbfrn");
        tpterm();
        return;
    }

    if((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL) {
        g_print("Error Reservando espacio para Buffer recv\n");
        tpterm();
        return;
    }

    if(Fadd32(fbfr, NOMBRE, doctorName, 0) < 0) {
        g_print("\tError insertando campo FML (NOMBRE)\n");
        tpfree((char *)fbfr);
        tpterm();
        return;
    }

    if(tpcall("select_Negocio_FML", (char *)fbfr, 0, (char **)&recv, &ivL_tamLongt, 0L) == -1) {
        g_print("Error en la llamada al servicio: tperrno = %d\n", tperrno);
        tpfree((char *)fbfr);
        tpfree((char *)recv);
        tpterm();
        return;
    }

    if((ivL_numOcurr = Foccur32(recv, NOMBRE)) < 0) {
        g_print("Error en Foccur32\n");
        tpfree((char *)fbfr);
        tpfree((char *)recv);
        tpterm();
        return;
    }

    spL_hosRegis = (hospital *)malloc(sizeof(hospital) * ivL_numOcurr);

    g_print("Respuesta del servidor para el doctor: %s\n", doctorName);
    int i;
    for(i = 0; i < ivL_numOcurr; i++) {
        Fget32(recv, IDDOCTOR, i, (char *)&spL_hosRegis[i].idDoctor, 0);
        Fget32(recv, NOMBRE, i, (char *)spL_hosRegis[i].nombre, 0);
        Fget32(recv, APELLIDO, i, (char *)spL_hosRegis[i].apellido, 0);
        Fget32(recv, ESPECIALIDAD, i, (char *)spL_hosRegis[i].especialidad, 0);

        g_print("Registro %d: ID Doctor = %d, Nombre = %s, Apellido = %s, Especialidad = %s\n",
                i + 1, spL_hosRegis[i].idDoctor, spL_hosRegis[i].nombre, spL_hosRegis[i].apellido, spL_hosRegis[i].especialidad);
    }

    tpfree((char *)fbfr);
    tpfree((char *)recv);
    tpterm();
}

