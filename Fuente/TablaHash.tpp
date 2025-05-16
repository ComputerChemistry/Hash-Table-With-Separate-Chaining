/**
 * @file TablaHash.tpp
 * @brief Implementaciones de los métodos de la plantilla de clase TablaHash.
 *
 * Este archivo contiene la lógica detrás de las operaciones de la tabla hash,
 * como la inserción, búsqueda, eliminación, y la gestión de la memoria
 * y las colisiones mediante encadenamiento separado.
 * Las declaraciones de estos métodos se encuentran en "Cabeceras/TablaHash.hpp".
 */

#include "../Cabeceras/TablaHash.hpp"
#include <cstdint>
#include <stdexcept>
#include <iostream>

/**
 * @brief Constructor de la tabla hash.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @param capacidad_Inicial Capacidad inicial con la que se creará la tabla hash.
 * @throw std::invalid_argument Si `capacidad_Inicial` es 0.
 */
template<typename Clave, typename Valor, typename Hash>
TablaHash<Clave, Valor, Hash>::TablaHash(size_t capacidad_Inicial)
    : tam_Tabla(capacidad_Inicial), num_Elementos(0), cubos(nullptr), funcionHash() {
    if (tam_Tabla == 0) {
        throw std::invalid_argument("La capacidad de la tabla no puede ser 0.");
    }

    cubos = new TablaHashNodo<Clave, Valor> *[tam_Tabla];

    for (size_t i = 0; i < tam_Tabla; i++) {
        cubos[i] = nullptr;
    }
}

/**
 * @brief Destructor de la tabla hash.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @note Libera toda la memoria dinámica utilizada por los nodos y la tabla de cubetas.
 */
template<typename Clave, typename Valor, typename Hash>
TablaHash<Clave, Valor, Hash>::~TablaHash() {
    for (size_t i = 0; i < tam_Tabla; i++) {
        TablaHashNodo<Clave, Valor> *actual = cubos[i];

        while (actual != nullptr) {
            TablaHashNodo<Clave, Valor> *nodo_a_eliminar = actual;
            actual = actual->siguiente;
            delete nodo_a_eliminar;
        }
        cubos[i] = nullptr;
    }

    delete[] cubos;
}

/**
 * @brief Calcula el índice en la tabla para una clave dada.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @param clave La clave para la cual calcular el índice.
 * @return size_t El índice calculado para la clave.
 * @throw std::logic_error Si la capacidad de la tabla (`tam_Tabla`) es 0.
 */
template<typename Clave, typename Valor, typename Hash>
size_t TablaHash<Clave, Valor, Hash>::obtenerIndice(const Clave &clave) const {
    uint64_t valor_hash = funcionHash(clave);
    if (tam_Tabla == 0) {
        throw std::logic_error("La capacidad de la tabla es 0 en obtenerIndice, lo cual no debería ser posible.");
    }

    size_t indice = valor_hash % tam_Tabla;
    return indice;
}

/**
 * @brief Inserta un par clave-valor en la tabla hash.
 * @details Si la clave ya existe, actualiza su valor. Si el factor de carga
 *          supera un umbral predefinido (0.75), se redimensiona la tabla.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @param clave La clave del elemento a insertar.
 * @param valor El valor asociado a la clave.
 * @return bool `true` si el elemento fue insertado como nuevo, `false` si la clave
 *         ya existía y su valor fue actualizado.
 */
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Insertar(const Clave &clave, const Valor &valor) {
    const double FACTOR_CARGA_MAXIMO_REHASH = 0.75;

    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];


    while (actual != nullptr) {
        if (actual->clave == clave) {
            actual->valor = valor; // Actualiza el valor si la clave ya existe
            return false; // Indica que la clave ya existía
        }
        actual = actual->siguiente;
    }

    // La clave no existe, insertar nuevo nodo al principio de la lista enlazada
    TablaHashNodo<Clave, Valor> *nuevo_nodo = new TablaHashNodo<Clave, Valor>(clave, valor);
    nuevo_nodo->siguiente = cubos[indice];
    cubos[indice] = nuevo_nodo;
    num_Elementos++;

    // Comprobar si es necesario redimensionar
    if (tam_Tabla > 0 && (static_cast<double>(num_Elementos) / tam_Tabla > FACTOR_CARGA_MAXIMO_REHASH)) {
        Redimensionar();
    }

    return true; // Indica que se insertó un nuevo elemento
}

/**
 * @brief Busca un elemento por su clave en la tabla hash.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @param clave La clave del elemento a buscar.
 * @param valor_encontrado Referencia donde se almacenará el valor si la clave es encontrada.
 * @return bool `true` si la clave fue encontrada y `valor_encontrado` ha sido actualizado,
 *         `false` en caso contrario.
 */
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Buscar(const Clave &clave, Valor &valor_encontrado) const {
    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];


    while (actual != nullptr) {
        if (actual->clave == clave) {
            valor_encontrado = actual->valor;
            return true;
        }
        actual = actual->siguiente;
    }
    return false; // Clave no encontrada
}

/**
 * @brief Elimina un elemento de la tabla hash por su clave.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @param clave La clave del elemento a eliminar.
 * @return bool `true` si el elemento fue encontrado y eliminado, `false` si la clave no existía.
 */
template<typename Clave, typename Valor, typename Hash>
bool TablaHash<Clave, Valor, Hash>::Eliminar(const Clave &clave) {
    size_t indice = obtenerIndice(clave);
    TablaHashNodo<Clave, Valor> *actual = cubos[indice];
    TablaHashNodo<Clave, Valor> *anterior = nullptr;


    while (actual != nullptr) {
        if (actual->clave == clave) {
            if (anterior == nullptr) {
                // El nodo a eliminar es el primero en la lista
                cubos[indice] = actual->siguiente;
            } else {
                // El nodo a eliminar está en medio o al final de la lista
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            num_Elementos--;
            return true; // Elemento encontrado y eliminado
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false; // Clave no encontrada
}

/**
 * @brief Muestra el contenido y estado actual de la tabla hash en la consola.
 * @details Imprime la capacidad, número de elementos, factor de carga, y el contenido
 *          de cada cubeta, incluyendo las claves, valores, y sus hashes.
 *          También indica si el índice esperado de un elemento coincide con su cubeta actual.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @note Esta función es principalmente para depuración y visualización.
 */
template<typename Clave, typename Valor, typename Hash>
void TablaHash<Clave, Valor, Hash>::MostrarTabla() const {
    std::cout << "\n--- Detalles de la Tabla Hash ---" << std::endl;
    std::cout << "Capacidad: " << this->tam_Tabla
            << ", Elementos: " << this->num_Elementos
            << std::endl;

    if (this->tam_Tabla > 0) {
        std::cout << "Factor de Carga: "
                << static_cast<double>(this->num_Elementos) / this->tam_Tabla
                << std::endl;
    } else {
        std::cout << "Factor de Carga: N/A (capacidad es 0)" << std::endl;
    }
    std::cout << "----------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < this->tam_Tabla; ++i) {
        std::cout << "Cubeta [" << i << "]:";
        TablaHashNodo<Clave, Valor> *actual = this->cubos[i];
        if (actual == nullptr) {
            std::cout << " [VACIA]" << std::endl;
        } else {
            std::cout << std::endl;
            while (actual != nullptr) {
                // Asumiendo que funcionHash puede tomar un segundo argumento opcional (seed)
                // o que existe una sobrecarga. Si no, ajustar esta línea.
                // Para el propósito de MostrarTabla, un hash consistente es suficiente.
                // Si funcionHash es std::hash, no toma un segundo argumento.
                // En este caso, usaremos funcionHash(actual->clave) directamente.
                uint64_t hashValor = this->funcionHash(actual->clave);
                std::cout << "  -> Clave: \"" << actual->clave << "\""
                        << ", Valor: " << actual->valor
                        << ", Hash: " << hashValor // Simplificado para generalidad
                        << ", Indice Esperado (Hash % Capacidad): "
                        << (this->tam_Tabla > 0 ? (hashValor % this->tam_Tabla) : 0);

                if (this->tam_Tabla > 0 && (hashValor % this->tam_Tabla) != i) {
                    std::cout << " [ALERTA: Indice no coincide!]";
                } else if (this->tam_Tabla == 0 && i != 0) {
                    std::cout << " [ALERTA: Capacidad es 0, índice no aplicable]";
                }
                std::cout << std::endl;
                actual = actual->siguiente;
            }
        }
    }
    std::cout << "----------------------------------------------------------" << std::endl;
}

/**
 * @brief Redimensiona la tabla hash, generalmente duplicando su capacidad.
 * @details Esta operación se activa cuando el factor de carga excede un umbral.
 *          Crea una nueva tabla con mayor capacidad, y todos los elementos existentes
 *          son reinsertados en la nueva tabla de acuerdo a sus nuevos índices hash.
 *          La tabla anterior es luego eliminada.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 * @note Esta es una operación costosa ya que implica rehashear y mover todos los elementos.
 */
template<typename Clave, typename Valor, typename Hash>
void TablaHash<Clave, Valor, Hash>::Redimensionar() {
    size_t capacidad_Anterior = this->tam_Tabla;
    TablaHashNodo<Clave, Valor> **cubos_Anteriores = this->cubos;

    // Duplica la capacidad. Si la capacidad anterior era 0, la establece a un valor por defecto (ej. 10).
    this->tam_Tabla = (capacidad_Anterior == 0) ? 10 : capacidad_Anterior * 2;

    std::cout << "\n*** REDIMENSIONANDO TABLA HASH ***" << std::endl;
    std::cout << "    Capacidad Antigua: " << capacidad_Anterior << ", Nueva Capacidad: " << this->tam_Tabla <<
            std::endl;

    this->cubos = new TablaHashNodo<Clave, Valor> *[this->tam_Tabla];
    for (size_t i = 0; i < this->tam_Tabla; ++i) {
        this->cubos[i] = nullptr;
    }
    this->num_Elementos = 0; // Se reseteará al reinsertar los elementos

    std::cout << "    Redimensionar: num_Elementos reseteado a 0. Empezando reinsercion." << std::endl;

    // Reinsertar todos los elementos de la tabla vieja a la nueva
    for (size_t i = 0; i < capacidad_Anterior; ++i) {
        TablaHashNodo<Clave, Valor> *actual_viejo = cubos_Anteriores[i];
        while (actual_viejo != nullptr) {
            // Insertar usará la nueva tam_Tabla y la misma funcionHash
            this->Insertar(actual_viejo->clave, actual_viejo->valor);
            actual_viejo = actual_viejo->siguiente;
        }
    }
    std::cout << "    Redimensionar: Reinsercion finalizada. num_Elementos final dentro de Redimensionar: " << this->
            num_Elementos << std::endl;

    // Eliminar la tabla vieja
    for (size_t i = 0; i < capacidad_Anterior; ++i) {
        TablaHashNodo<Clave, Valor> *actual_a_borrar = cubos_Anteriores[i];
        while (actual_a_borrar != nullptr) {
            TablaHashNodo<Clave, Valor> *siguiente_a_borrar = actual_a_borrar->siguiente;
            delete actual_a_borrar;
            actual_a_borrar = siguiente_a_borrar;
        }
    }
    delete[] cubos_Anteriores;

    std::cout << "*** REDIMENSIONAMIENTO COMPLETO. Elementos FINALES: " << this->num_Elementos << " ***" << std::endl;
}

/**
 * @brief Elimina todos los elementos de la tabla hash.
 * @details Libera la memoria de todos los nodos almacenados y resetea el número
 *          de elementos a cero. La capacidad de la tabla permanece sin cambios.
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor.
 * @tparam Hash Functor para calcular el hash de las claves.
 */
template<typename Clave, typename Valor, typename Hash>
void TablaHash<Clave, Valor, Hash>::Vaciar() {
    for (size_t i = 0; i < this->tam_Tabla; ++i) {
        TablaHashNodo<Clave, Valor> *actual = this->cubos[i];
        while (actual != nullptr) {
            TablaHashNodo<Clave, Valor> *nodo_a_eliminar = actual;
            actual = actual->siguiente;
            delete nodo_a_eliminar;
        }
        this->cubos[i] = nullptr; // Importante: resetear el puntero de la cubeta
    }
    this->num_Elementos = 0;
}
