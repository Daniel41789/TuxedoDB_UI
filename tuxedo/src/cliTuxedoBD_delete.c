#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include "biblioBD.fml.h"

typedef struct hospital{
    int idEspecialidad;
} hospital;

int main(int argc, char **argv)
{
    int ivL_resLlamd;
    long ivL_tamLongt;
    FBFR32 *fbfr;
    FLDLEN32 flen;
    FBFR32 *recv;
    char msgbuf[64];
    hospital hos;

    printf("*************************************************\n");
    printf("Id de Especialidad : "); scanf("%d", &hos.idEspecialidad); // Ingresar el ID de la habitación a actualizar
    printf("*************************************************\n");

    printf("Conectamos con la aplicación");
    if(tpinit((TPINIT *) NULL) == -1){
        printf("Error en la conexión, tperrno = %d \n", tperrno);
        return(1);
    }

    //Reservamos espacio para el buffer FML
    printf("Reservamos espacio para los buffers FML\n");
    if ((fbfr = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error reservando espacio para Buffer fbfrn");
        tpterm();
        return(1);
    }

    if ((recv = (FBFR32 *) tpalloc("FML32", NULL, 1024)) == NULL){
        printf("Error Reservando espacio para Buffer recv\n");
        tpterm();
        return(1);
    }

    /*Manejo del Buffer FML*/
    printf ("\nInsertamos datos en buffer FML.");
    if(Fadd32 (fbfr, IDESPECIALIDAD, (char *)&hos.idEspecialidad, 0) < 0){
        printf ("\n\tError insertando campo FML (ID DE ESPECIALIDAD)");
        tpfree((char*)fbfr);
        tpterm();
        return (0);
    }

    //Invocamos al servicio    
    printf("Llamada al servicio 'delete_Negocio_FML'\n");
    if(tpcall("delete_Negocio_FML", (char *)fbfr, 0, (char **)&recv, &ivL_tamLongt, 0L) == -1){
        printf("\n Error en la llamada al servicio: tperrno = %d\n", tperrno);
        tpfree((char *)fbfr);
        tpfree((char *)recv);
        tpterm();
        return (1);
    }

    printf("Servicio exitoso!\n");

    flen = sizeof(msgbuf);
    Fget32(recv, OUTPUT, 0, (char *)msgbuf, &flen);
    printf("Respuesta del servidor: %s\n", msgbuf);

    tpfree((char *)fbfr);
    tpfree((char *)recv);
    tpterm();
    return 0;
}
