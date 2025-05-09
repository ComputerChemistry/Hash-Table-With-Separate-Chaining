/**
 * @file TablaHash.hpp
 * @brief Define una estructura de tabla hash genérica con encadenamiento separado.
 *
 * Este archivo contiene la definición de la plantilla de clase `TablaHash`
 * y la estructura auxiliar `TablaHashNodo`. La tabla hash permite almacenar
 * pares clave-valor y utiliza un functor de hash personalizable.
 */

#ifndef TABLAHASH_HPP
#define TABLAHASH_HPP

#include <cstddef> // Para size_t
#include <stdexcept> // Para std::invalid_argument, std::bad_alloc (potencialmente)
#include <iostream>  // Para MostrarTabla

/**
 * @struct TablaHashNodo
 * @brief Representa un nodo en la lista enlazada de una cubeta de la tabla hash.
 *
 * Cada nodo almacena una clave, un valor y un puntero al siguiente nodo
 * en la misma cubeta, implementando así el encadenamiento separado.
 *
 * @tparam Clave Tipo de la clave.
 * @tparam Valor Tipo del valor asociado a la clave.
 */
template<typename Clave, typename Valor>
struct TablaHashNodo {
    Clave clave; ///< La clave almacenada en el nodo.
    Valor valor; ///< El valor asociado a la clave.
    TablaHashNodo *siguiente = nullptr; ///< Puntero al siguiente nodo en la lista enlazada (cubeta).

    /**
     * @brief Constructor del nodo de la tabla hash.
     * @param k La clave a almacenar.
     * @param v El valor a almacenar.
     */
    TablaHashNodo(const Clave &k, const Valor &v): clave(k), valor(v) {
    }
};

/**
 * @class TablaHash
 * @brief Implementación de una tabla hash con encadenamiento separado.
 *
 * Esta clase de plantilla permite almacenar pares clave-valor. Utiliza un
 * array de punteros a nodos (`TablaHashNodo`), donde cada posición del array
 * (cubeta) puede contener una lista enlazada de nodos si ocurren colisiones.
 *
 * @tparam Clave El tipo de las claves a almacenar.
 * @tparam Valor El tipo de los valores asociados a las claves.
 * @tparam Hash Un functor que toma una `Clave` y devuelve un `uint64_t` como valor hash.
 */
template<typename Clave, typename Valor, typename Hash>
class TablaHash {
public:
    /**
     * @brief Constructor de la tabla hash.
     * @param capacidad_Inicial La capacidad inicial de la tabla (número de cubetas).
     *                          Debe ser mayor que 0. Por defecto es 101.
     * @throw std::invalid_argument Si `capacidad_Inicial` es 0.
     * @throw std::bad_alloc Si falla la asignación de memoria para las cubetas.
     */
    explicit TablaHash(size_t capacidad_Inicial = 101);

    /**
     * @brief Destructor de la tabla hash.
     * Libera toda la memoria asignada para los nodos y las cubetas.
     */
    ~TablaHash();

    /**
     * @brief Inserta un par clave-valor en la tabla hash.
     * Si la clave ya existe, actualiza su valor.
     * @param clave La clave a insertar.
     * @param valor El valor asociado a la clave.
     * @return `true` si la clave es nueva y se insertó, `false` si la clave ya existía y se actualizó el valor.
     * @throw std::invalid_argument Si la capacidad de la tabla es 0 (propaga desde `obtenerIndice`).
     * @throw std::bad_alloc Si falla la asignación de memoria para un nuevo nodo.
     */
    bool Insertar(const Clave &clave, const Valor &valor);

    /**
     * @brief Busca una clave en la tabla hash.
     * @param clave La clave a buscar.
     * @param valor_encontrado Referencia donde se almacenará el valor si la clave se encuentra.
     * @return `true` si la clave se encontró, `false` en caso contrario.
     * @throw std::invalid_argument Si la capacidad de la tabla es 0 (propaga desde `obtenerIndice`).
     */
    bool Buscar(const Clave &clave, Valor &valor_encontrado) const;

    /**
     * @brief Elimina un par clave-valor de la tabla hash.
     * @param clave La clave del elemento a eliminar.
     * @return `true` si la clave se encontró y eliminó, `false` en caso contrario.
     * @throw std::invalid_argument Si la capacidad de la tabla es 0 (propaga desde `obtenerIndice`).
     */
    bool Eliminar(const Clave &clave);

    /**
     * @brief Obtiene el número actual de elementos en la tabla hash.
     * @return El número de elementos.
     */
    size_t ObtenerNumElementos() const;

    /**
     * @brief Obtiene la capacidad actual de la tabla hash (número de cubetas).
     * @return La capacidad de la tabla.
     */
    size_t ObtenerCapacidad() const;

    /**
     * @brief Muestra el contenido y detalles de la tabla hash en la consola.
     * Útil para depuración y visualización del estado interno de la tabla.
     * Imprime la capacidad, número de elementos, factor de carga y el contenido de cada cubeta.
     * @note Requiere que los tipos Clave y Valor sean imprimibles con `std::cout`.
     */
    void MostrarTabla() const;

private:
    TablaHashNodo<Clave, Valor> **cubos; ///< Array de punteros a las cubetas (listas enlazadas).
    size_t tam_Tabla;                    ///< Capacidad de la tabla (número de cubetas).
    size_t num_Elementos;                ///< Número actual de elementos almacenados.
    Hash funcionHash;                    ///< Instancia del functor de hash.

    /**
     * @brief Calcula el índice de la cubeta para una clave dada.
     * @param clave La clave para la cual calcular el índice.
     * @return El índice de la cubeta (0 <= indice < tam_Tabla).
     * @throw std::invalid_argument Si `tam_Tabla` es 0.
     */
    size_t obtenerIndice(const Clave &clave) const;
};

// Implementaciones de métodos inline (o definidos en el mismo header para plantillas)

/**
 * @brief Obtiene el número actual de elementos en la tabla hash.
 * @tparam Clave El tipo de las claves.
 * @tparam Valor El tipo de los valores.
 * @tparam Hash El functor de hash.
 * @return El número de elementos.
 */
template<typename Clave, typename Valor, typename Hash>
size_t TablaHash<Clave, Valor, Hash>::ObtenerNumElementos() const {
    return num_Elementos;
}

/**
 * @brief Obtiene la capacidad actual de la tabla hash (número de cubetas).
 * @tparam Clave El tipo de las claves.
 * @tparam Valor El tipo de los valores.
 * @tparam Hash El functor de hash.
 * @return La capacidad de la tabla.
 */
template<typename Clave, typename Valor, typename Hash>
size_t TablaHash<Clave, Valor, Hash>::ObtenerCapacidad() const {
    return tam_Tabla;
}

// Inclusión del archivo de implementación para las plantillas
#include "../Fuente/TablaHash.tpp"

#endif //TABLAHASH_HPP