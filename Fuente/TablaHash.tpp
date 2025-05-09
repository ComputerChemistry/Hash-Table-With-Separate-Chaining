/**
 * @file TablaHash.tpp
 * @brief Implementaciones de los métodos de la plantilla de clase TablaHash.
 *
 * Este archivo contiene la lógica detrás de las operaciones de la tabla hash,
 * como la inserción, búsqueda, eliminación, y la gestión de la memoria
 * y las colisiones mediante encadenamiento separado.
 * Las declaraciones de estos métodos se encuentran en "Cabeceras/TablaHash.hpp".
 */

#include "../Cabeceras/TablaHash.hpp" // Declaraciones de la plantilla TablaHash
#include <cstdint>                   // Para uint64_t
#include <stdexcept>                 // Para std::invalid_argument
#include <iostream>                  // Para std::cout en MostrarTabla

// Implementación del Constructor
template<typename Clave, typename Valor, typename Hash>
TablaHash<Clave, Valor, Hash>::TablaHash(size_t capacidad_Inicial)
    : tam_Tabla(capacidad_Inicial), num_Elementos(0), cubos(nullptr), funcionHash() {
    if (tam_Tabla == 0) {
        // Lanzar excepción si la capacidad inicial es inválida.
        throw std::invalid_argument("La capacidad de la tabla no puede ser 0.");
    }
    // Intentar asignar memoria para el array de cubetas.
    // std::bad_alloc se lanzará aquí si la asignación falla.
    cubos = new TablaHashNodo<Clave, Valor> *[tam_Tabla];
    // Inicializar todas las cubetas a nullptr.
    for (size_t i = 0; i < tam_Tabla; i++) {
        cubos[i] = nullptr;
    }
}

// Implementación del Destructor
template<typename Clave, typename Valor, typename Hash>
TablaHash<Clave, Valor, Hash>::~TablaHash() {
    // Recorrer cada cubeta.
    for (size_t i = 0; i < tam_Tabla; i++) {
        TablaHashNodo<Clave, Valor> *actual = cubos[i];
        // Eliminar todos los nodos en la lista enlazada de la cubeta actual.
        while (actual != nullptr) {
            TablaHashNodo<Clave, Valor> *nodo_a_eliminar = actual;
            actual = actual->siguiente;
            delete nodo_a_eliminar;
        }
        cubos[i] = nullptr; // Asegurar que el puntero de la cubeta quede nulo.
    }
    // Eliminar el array de cubetas.
    delete[] cubos;
}

// Implementación de obtenerIndice
template<typename Clave, typename Valor, typename Hash>
size_t TablaHash<Clave, Valor, Hash>::obtenerIndice(const Clave &clave) const {
    // Calcular el hash usando el functor proporcionado.
    uint64_t valor_hash = funcionHash(clave);
    if (tam_Tabla == 0) {
        // Esto no debería ocurrir si el constructor valida la capacidad,
        // pero es una buena salvaguarda.
        throw std::logic_error("La capacidad de la tabla es 0 en obtenerIndice, lo cual no debería ser posible.");
    }
    // Convertir el hash a un índice dentro del rango de la tabla.
    size_t indice = valor_hash % tam_Tabla;
    return indice;
}

// Implementación de Insertar
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Insertar(const Clave &clave, const Valor &valor) {
    // obtenerIndice puede lanzar std::invalid_argument si tam_Tabla es 0 (aunque protegido por constructor).
    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];

    // Buscar si la clave ya existe en la cubeta.
    while (actual != nullptr) {
        if (actual->clave == clave) {
            actual->valor = valor; // Actualizar valor si la clave existe.
            return false;          // Indicar que se actualizó, no se insertó nuevo.
        }
        actual = actual->siguiente;
    }

    // Si la clave no existe, crear un nuevo nodo e insertarlo al principio de la lista.
    // std::bad_alloc se lanzará aquí si la asignación de nuevo_nodo falla.
    TablaHashNodo<Clave, Valor> *nuevo_nodo = new TablaHashNodo<Clave, Valor>(clave, valor);
    nuevo_nodo->siguiente = cubos[indice];
    cubos[indice] = nuevo_nodo;
    num_Elementos++;
    return true; // Indicar que se insertó un nuevo elemento.
}

// Implementación de Buscar
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Buscar(const Clave &clave, Valor &valor_encontrado) const {
    // obtenerIndice puede lanzar std::invalid_argument si tam_Tabla es 0.
    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];

    // Recorrer la lista enlazada de la cubeta.
    while (actual != nullptr) {
        if (actual->clave == clave) {
            valor_encontrado = actual->valor; // Clave encontrada, copiar valor.
            return true;
        }
        actual = actual->siguiente;
    }
    return false; // Clave no encontrada.
}

// Implementación de Eliminar
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Eliminar(const Clave &clave) {
    // obtenerIndice puede lanzar std::invalid_argument si tam_Tabla es 0.
    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];
    TablaHashNodo<Clave, Valor> *anterior = nullptr;

    // Recorrer la lista enlazada de la cubeta.
    while (actual != nullptr) {
        if (actual->clave == clave) {
            // Clave encontrada, proceder a eliminar el nodo.
            if (anterior == nullptr) {
                // El nodo a eliminar es el primero de la lista.
                cubos[indice] = actual->siguiente;
            } else {
                // El nodo a eliminar está en medio o al final de la lista.
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            num_Elementos--;
            return true; // Elemento eliminado.
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false; // Clave no encontrada, nada que eliminar.
}

// Implementación de MostrarTabla
template<typename Clave, typename Valor, typename Hash>
void TablaHash<Clave, Valor, Hash>::MostrarTabla() const {
    std::cout << "\n--- Detalles de la Tabla Hash ---" << std::endl;
    std::cout << "Capacidad: " << this->tam_Tabla
              << ", Elementos: " << this->num_Elementos
              << std::endl;

    // Calcular y mostrar el factor de carga.
    if (this->tam_Tabla > 0) {
        std::cout << "Factor de Carga: "
                  << static_cast<double>(this->num_Elementos) / this->tam_Tabla
                  << std::endl;
    } else {
        std::cout << "Factor de Carga: N/A (capacidad es 0)" << std::endl;
    }
    std::cout << "----------------------------------------------------------" << std::endl;

    // Recorrer cada cubeta e imprimir su contenido.
    for (size_t i = 0; i < this->tam_Tabla; ++i) {
        std::cout << "Cubeta [" << i << "]:";
        TablaHashNodo<Clave, Valor>* actual = this->cubos[i];
        if (actual == nullptr) {
            std::cout << " [VACIA]" << std::endl;
        } else {
            std::cout << std::endl; // Nueva línea para los elementos de la cubeta.
            while (actual != nullptr) {
                // Calcular hash para mostrar (con semilla 0 por consistencia).
                uint64_t hashValor = this->funcionHash(actual->clave, 0);
                std::cout << "  -> Clave: \"" << actual->clave << "\""
                          << ", Valor: " << actual->valor
                          << ", Hash (seed 0): " << hashValor
                          << ", Indice Esperado (Hash % Capacidad): " 
                          << (this->tam_Tabla > 0 ? (hashValor % this->tam_Tabla) : 0);

                // Comprobar si el índice esperado coincide con el actual.
                if (this->tam_Tabla > 0 && (hashValor % this->tam_Tabla) != i) {
                    std::cout << " [ALERTA: Indice no coincide!]";
                } else if (this->tam_Tabla == 0 && i != 0) { // Caso extremo, tam_Tabla es 0.
                    std::cout << " [ALERTA: Capacidad es 0, índice no aplicable]";
                }
                std::cout << std::endl;
                actual = actual->siguiente;
            }
        }
    }
    std::cout << "----------------------------------------------------------" << std::endl;
}