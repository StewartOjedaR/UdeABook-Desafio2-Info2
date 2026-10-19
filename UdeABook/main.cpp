#include <iostream>
#include "sistema.h"

using namespace std;


    int main() {
        // Inicializamos el gestor principal
        char rutaMatriz[]="matriz_adyacencia.txt";
        char rutaBaseUsuarios[]="usuarios.txt";
        char rutaBaseMensajes[]="mensajes.txt";
        char rutaBaseFotos[]="fotos.txt";
        Sistema redSocial(rutaMatriz,rutaBaseUsuarios,rutaBaseMensajes,rutaBaseFotos);
        redSocial.menuInicio();
        if (redSocial.getSesionIniciada()){
            redSocial.menuPrincipal();
        }


/*        // Creamos al primer usuario (su ID será 0 automáticamente)
        redSocial.registrarNuevoUsuario("stewart", "mi_password_123");
        redSocial.registrarNuevoUsuario();
        // Si registras un segundo usuario, su ID será 1 y la matriz de adyacencia crecerá a 2x2 bits
        redSocial.registrarNuevoUsuario("luz_mery", "clave_segura");
        redSocial.registrarNuevoUsuario();
        // Ahora ya puedes gestionar amistades entre el usuario 0 y el usuario 1
        redSocial.agregarAmistad(0, 1);
*/
        return 0;
    }

