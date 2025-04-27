// main.cpp
#include <iostream>
#include <string>
#include "../Headers/TablaHash.hpp"

using namespace std;

int main() {
    TablaHash<int, string> tabla;
    int opcion, clave;
    string valor;

    do {
        cout << "\n=== MENU TABLA HASH ===\n";
        cout << "1. Insertar elemento\n";
        cout << "2. Buscar elemento\n";
        cout << "3. Eliminar elemento\n";
        cout << "4. Mostrar tabla\n";
        cout << "5. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                cout << "Ingrese clave (entero): ";
                cin >> clave;
                cout << "Ingrese valor (texto): ";
                cin.ignore();
                getline(cin, valor);

                if (tabla.insertar(clave, valor)) {
                    cout << "Elemento insertado correctamente." << endl;
                } else {
                    cout << "Error al insertar elemento." << endl;
                }
                break;

            case 2:
                cout << "Ingrese clave a buscar: ";
                cin >> clave;

                if (tabla.buscar(clave, valor)) {
                    cout << "Encontrado: [" << clave << "] = " << valor << endl;
                } else {
                    cout << "Clave no encontrada." << endl;
                }
                break;

            case 3:
                cout << "Ingrese clave a eliminar: ";
                cin >> clave;

                if (tabla.eliminar(clave)) {
                    cout << "Elemento eliminado correctamente." << endl;
                } else {
                    cout << "Clave no encontrada." << endl;
                }
                break;

            case 4:
                tabla.mostrar();
                break;

            case 5:
                cout << "Saliendo del programa..." << endl;
                break;

            default:
                cout << "Opcion invalida. Intente de nuevo." << endl;
        }
    } while (opcion != 5);

    return 0;
}
