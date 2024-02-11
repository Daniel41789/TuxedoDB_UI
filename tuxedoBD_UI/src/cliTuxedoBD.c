#include <stdio.h>
#include <string.h>
#include <atmi.h>
#include <fml32.h>
#include "biblioBD.fml.h"

typedef struct hospital{
    int idPaciente;
    char nombre[20];
    char apellido[20];
    char telefono[10];
}hospital;

int main(int argc, char **argv)
{
    int ivL_resLlamd;
    long ivL_tamLongt;
    float fvL_recValor;
    FBFR32 *fbfr;
    FBFR32 *recv;
    FLDLEN32 flen;
    char msgbuf[64];

    hospital hos;

    printf("\n*************************************************\n");
    printf("id del Paciente :"); scanf("%d", &hos.idPaciente);
    printf("Nombre          :"); scanf("%s", hos.nombre);
    printf("Apellido        :"); scanf("%s", hos.apellido);
    printf("Telefono        :"); scanf("%s", hos.telefono);

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
    if(Fadd32 (fbfr, IDPACIENTE, (char *)&hos.idPaciente, 0) < 0){
        printf ("\n\tError insertando campo FML (ID DEL PACIENTE)");
        tpfree((char*)fbfr);
        tpterm();
        return (0);
    }

    if(Fadd32 (fbfr, NOMBRE, hos.nombre, 0) < 0){
        printf ("\n\tError insertando campo FML (NOMBRE)");
        tpfree((char*)fbfr);
        tpterm();
        return (0);
    }

    if(Fadd32 (fbfr, APELLIDO, hos.apellido, 0) < 0){
        printf ("\n\tError insertando campo FML (APELLIDO)");
        tpfree((char*)fbfr);
        tpterm();
        return (0);
    }

    if(Fadd32 (fbfr, TELEFONO, hos.telefono, 0) < 0){
        printf ("\n\tError insertando campo FML (TELEFONO)");
        tpfree((char*)fbfr);
        tpterm();
        return (0);
    }

    //Invocamos al servicio    
    printf("Llamada al servicio 'insert_Negocio_FML'\n");
    if(tpcall("insert_Negocio_FML", (char *)fbfr, 0, (char **)&recv, &ivL_tamLongt, 0L) == -1){
        printf("\n Error en la llamada al servicio: tperrno = %d\n", tperrno);
        tpfree((char *)fbfr);
        tpfree((char *)recv);
        tpterm();
        return (1);
    }

    flen = sizeof(msgbuf);
    Fget32(recv, OUTPUT, 0, (char *)msgbuf, &flen);
    printf("Respuesta del servidor: %s\n", msgbuf);

    // Liberamos el buffer y desconectamos de la aplicacion
    printf("Liberamos Buffer y desconectamos de la aplicacion\n");
    tpfree((char *)fbfr);
    tpfree((char *)recv);
    tpterm();
    return 0;
}




