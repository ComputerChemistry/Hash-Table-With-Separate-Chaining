/**
 * @file main.cpp
 * @brief Archivo principal que contiene pruebas para las clases Hasher y TablaHash.
 *
 * Este archivo implementa una serie de pruebas para verificar la correcta funcionalidad
 * de las funciones hash para strings y enteros, así como las operaciones CRUD (Crear, Leer, Actualizar, Eliminar)
 * de la TablaHash, incluyendo el manejo de colisiones.
 */

#include <iostream>
#include <string>
#include <vector>
#include "Cabeceras/xxh64.hpp"     // Asumo que estas rutas son correctas desde main.cpp
#include "Cabeceras/Hasher.hpp"    // o que tu configuración de includes lo maneja.
#include "Cabeceras/TablaHash.hpp"

/**
 * @brief Función principal que ejecuta las pruebas de la Tabla Hash y las funciones Hasher.
 * @return 0 si todas las pruebas se ejecutan correctamente.
 */
int main() {
    // Pruebas de Hasher (como las tenías)
    std::cout << "Probando funciones Hash" << std::endl;
    std::cout << std::endl;
    HasherString64 stringHasher;
    std::vector<std::string> stringsPrueba = {
        "hola", "mundo", "ballena", "hashing", "", "IreneGOD",
    };
    std::cout << "\nProbando HasherString64" << std::endl;
    for (const std::string &str: stringsPrueba) {
        uint64_t hashValor = stringHasher(str);
        uint64_t hashValorConSemilla = stringHasher(str, 031001); // Semilla octal 031001 = decimal 12801
        std::cout << "Clave: \"" << str << "\"" << std::endl;
        std::cout << " Hash (seed = 0): " << hashValor << std::endl;
        std::cout << " Hash (seed = " << 031001 << "): " << hashValorConSemilla << std::endl;
    }
    HasherInt64 intHasher;
    std::vector<int> intsPrueba = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
    };
    std::cout << "\nProbando HasherInt64" << std::endl;
    for (int num: intsPrueba) {
        uint64_t hashValor = intHasher(num);
        uint64_t hashValorConSemilla = intHasher(num, 031001);
        std::cout << "Clave: \"" << num << "\"" << std::endl;
        std::cout << " Hash (seed = 0): " << hashValor << std::endl;
        std::cout << " Hash (seed = " << 031001 << "): " << hashValorConSemilla << std::endl;
    }

    std::cout << "\n/---------------------------------------------/" << std::endl;
    std::cout << "\n Probando getters" << std::endl;
    TablaHash<std::string, int, HasherString64> miTabla(10); // Reducí capacidad para ver colisiones más fácil
    std::cout << "\nInstancia de TablaHash::<std::string, int> creada (miTabla)." << std::endl;
    std::cout << "Capacidad inicial: 10, Capacidad obtenida: " << miTabla.ObtenerCapacidad() << std::endl;
    std::cout << "Numero de elementos inicial: 0, Numero de elementos obtenidos: " << miTabla.ObtenerNumElementos() <<
            std::endl;
    std::cout << "\n === ESTADO INICIAL DE miTabla ===" << std::endl;
    miTabla.MostrarTabla();


    std::cout << "\n/---------------------------------------------/" << std::endl;
    std::cout << "\n Probando Insertar" << std::endl;
    bool resultadoOperacion;

    resultadoOperacion = miTabla.Insertar("hola", 100);
    std::cout << "\nInsertando (\"hola\",100): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    resultadoOperacion = miTabla.Insertar("mundo", 200);
    std::cout << "Insertando (\"mundo\", 200): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    resultadoOperacion = miTabla.Insertar("adios", 300);
    std::cout << "Insertando (\"adios\", 300): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    resultadoOperacion = miTabla.Insertar("prueba", 400); // Otro elemento
    std::cout << "Insertando (\"prueba\", 400): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    resultadoOperacion = miTabla.Insertar("hello", 500); // Y otro más para forzar posibles colisiones con capacidad 10
    std::cout << "Insertando (\"hello\", 500): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
            
    std::cout << "\n === ESTADO DE miTabla DESPUES DE VARIAS INSERCIONES ===" << std::endl;
    miTabla.MostrarTabla();

    resultadoOperacion = miTabla.Insertar("hola", 101); // Actualizando "hola"
    std::cout << "\nInsertando (\"hola\", 101) de nuevo: " << (resultadoOperacion
                                                                 ? "Exito (nuevo)"
                                                                 : "Fallo (ya existe/actualizado)") << ". Elementos: "
            << miTabla.ObtenerNumElementos() << std::endl;
            
    std::cout << "\n === ESTADO DE miTabla DESPUES DE ACTUALIZAR 'hola' ===" << std::endl;
    miTabla.MostrarTabla();


    std::cout << "\n/---------------------------------------------/" << std::endl;
    std::cout << "\n Probando Buscar en miTabla (antes de eliminar)" << std::endl;
    int valorEncontradoInt;

    resultadoOperacion = miTabla.Buscar("hola", valorEncontradoInt);
    std::cout << "Buscando \"hola\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
        ? ", Valor: " + std::to_string(valorEncontradoInt)
        : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("mundo", valorEncontradoInt);
    std::cout << "Buscando \"mundo\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
        ? ", Valor: " + std::to_string(valorEncontradoInt)
        : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("adios", valorEncontradoInt);
    std::cout << "Buscando \"adios\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << (resultadoOperacion
        ? ", Valor: " + std::to_string(valorEncontradoInt)
        : "") << std::endl;
    resultadoOperacion = miTabla.Buscar("inexistente", valorEncontradoInt);
    std::cout << "Buscando \"inexistente\": " << (resultadoOperacion ? "Encontrado" : "No encontrado") << std::endl;
    

    std::cout << "\n/---------------------------------------------/" << std::endl;
    std::cout << "\n Probando Eliminar en miTabla" << std::endl;
    // Estado actual de miTabla: {"hola":101, "mundo":200, "adios":300, "prueba":400, "hello":500}, num_Elementos = 5

    resultadoOperacion = miTabla.Eliminar("fantasma");
    std::cout << "Eliminando \"fantasma\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;

    resultadoOperacion = miTabla.Eliminar("mundo");
    std::cout << "Eliminando \"mundo\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE miTabla DESPUES DE ELIMINAR 'mundo' ===" << std::endl;
    miTabla.MostrarTabla();
            
    resultadoOperacion = miTabla.Buscar("mundo", valorEncontradoInt); 
    std::cout << "Buscando \"mundo\" post-eliminacion: " << (resultadoOperacion ? "Encontrado" : "No encontrado") <<
            std::endl; 

    resultadoOperacion = miTabla.Eliminar("hola");
    std::cout << "Eliminando \"hola\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl; 
    std::cout << "\n === ESTADO DE miTabla DESPUES DE ELIMINAR 'hola' ===" << std::endl;
    miTabla.MostrarTabla();

    resultadoOperacion = miTabla.Eliminar("adios");
    std::cout << "Eliminando \"adios\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl; 
    resultadoOperacion = miTabla.Eliminar("prueba");
    std::cout << "Eliminando \"prueba\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl; 
    resultadoOperacion = miTabla.Eliminar("hello");
    std::cout << "Eliminando \"hello\": " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl; 
            
    std::cout << "\n === ESTADO DE miTabla DESPUES DE VACIARLA COMPLETAMENTE ===" << std::endl;
    miTabla.MostrarTabla();

    resultadoOperacion = miTabla.Eliminar("hola"); // Intentar eliminar de tabla vacía
    std::cout << "Eliminando \"hola\" (de tabla vacia): " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
            << ". Elementos: " << miTabla.ObtenerNumElementos() << std::endl;


    std::cout << "\n--- Pruebas de TablaHash<int, double> ---" << std::endl;
    TablaHash<int, double, HasherInt64> tablaNumerica(3); // Capacidad muy pequeña para forzar colisiones
    std::cout << "Tabla numerica creada. Capacidad: " << tablaNumerica.ObtenerCapacidad() << ", Elementos: " <<
            tablaNumerica.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO INICIAL DE tablaNumerica ===" << std::endl;
    tablaNumerica.MostrarTabla();

    resultadoOperacion = tablaNumerica.Insertar(10, 3.14);
    std::cout << "Insertando (10, 3.14): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    resultadoOperacion = tablaNumerica.Insertar(25, 0.577);
    std::cout << "Insertando (25, 0.577): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    resultadoOperacion = tablaNumerica.Insertar(13, 1.0); // Posible colisión con 10 (10%3=1, 13%3=1)
    std::cout << "Insertando (13, 1.0): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") <<
            ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    
    // Limpieza de tabla numérica para pruebas de colisión más claras
    // resultadoOperacion = tablaNumerica.Insertar(2, 2.71); // Posible colisión con 25 (25%3=1, 2%3=2) - No, 25%3=1, 2%3=2. No colisiona aquí.
                                                        // 10%3 = 1, 13%3 = 1. 25%3=1.
                                                        // Probemos 10, 13, 1 (todos -> índice 1), y 2 (índice 2), 5 (índice 2)
    // Vaciado de tabla para prueba específica de colisiones
    while(tablaNumerica.ObtenerNumElementos() > 0) { // Simple bucle para vaciarla si es necesario
        if(!tablaNumerica.Eliminar(10)) // Intenta eliminar elementos conocidos, ajusta según lo que hayas insertado
           if(!tablaNumerica.Eliminar(25))
              tablaNumerica.Eliminar(13); // Si falla uno, prueba otro. Esto es rudimentario.
    }


    std::cout << "Tabla numerica vaciada para prueba de colision. Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    resultadoOperacion = tablaNumerica.Insertar(1, 1.0); // 1 % 3 = 1
    std::cout << "Insertando (1, 1.0): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    resultadoOperacion = tablaNumerica.Insertar(4, 4.0); // 4 % 3 = 1 (colisión con 1)
    std::cout << "Insertando (4, 4.0): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    resultadoOperacion = tablaNumerica.Insertar(7, 7.0); // 7 % 3 = 1 (colisión con 1, 4)
    std::cout << "Insertando (7, 7.0): " << (resultadoOperacion ? "Exito (nuevo)" : "Fallo (ya existe)") << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    // Ahora todos los elementos (1,4,7) deberían estar en la cubeta 1. Capacidad es 3.
    // Todos los elementos (3) están en 1 bucket, FC = 3/3 = 1.0
            
    std::cout << "\n === ESTADO DE tablaNumerica DESPUES DE INSERCIONES (con colisiones) ===" << std::endl;
    tablaNumerica.MostrarTabla();

    std::cout << "\n Probando Eliminar en tablaNumerica (con colisiones)" << std::endl;
    // Eliminar el del medio de la cadena de colisión (4)
    resultadoOperacion = tablaNumerica.Eliminar(4);
    std::cout << "Eliminando 4 (del medio de la cadena): " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
              << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE tablaNumerica DESPUES DE ELIMINAR 4 ===" << std::endl;
    tablaNumerica.MostrarTabla(); // Deberían quedar 1 y 7 en la cubeta 1

    // Eliminar la cabeza de la cadena de colisión (7, ya que insertamos al principio)
    resultadoOperacion = tablaNumerica.Eliminar(7);
    std::cout << "Eliminando 7 (cabeza de la cadena): " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
              << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE tablaNumerica DESPUES DE ELIMINAR 7 ===" << std::endl;
    tablaNumerica.MostrarTabla(); // Debería quedar 1 en la cubeta 1

    // Eliminar el último de la cadena (1)
    resultadoOperacion = tablaNumerica.Eliminar(1);
    std::cout << "Eliminando 1 (ultimo de la cadena): " << (resultadoOperacion ? "Exito" : "Fallo (no existe)")
              << ". Elementos: " << tablaNumerica.ObtenerNumElementos() << std::endl;
    std::cout << "\n === ESTADO DE tablaNumerica DESPUES DE ELIMINAR 1 ===" << std::endl;
    tablaNumerica.MostrarTabla(); // Debería estar vacía

    // Este es el final de tus pruebas originales, el resto son solo las llamadas a MostrarTabla que ya estaban
    // miTabla.MostrarTabla(); // Esta ya está vacía
    // tablaNumerica.MostrarTabla(); // Esta ya está vacía

    std::cout << "\n\n--- Pruebas Completas ---" << std::endl;
    return 0;
}