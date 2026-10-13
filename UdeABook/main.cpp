#include <iostream>
#include "sistema.h"

using namespace std;


    int main() {
        // Inicializamos el gestor principal
        Sistema redSocial("C:/Users/ojeda/OneDrive/Documentos/Desafio2/matriz_adyacencia.dat","C:/Users/ojeda/OneDrive/Documentos/Desafio2/usuarios.dat","C:/Users/ojeda/OneDrive/Documentos/Desafio2/mensajes.dat");
        redSocial.menuInicio();
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

