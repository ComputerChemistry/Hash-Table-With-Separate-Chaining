/**
 * @file Hasher.hpp
 * @brief Define functores para calcular hashes de 64 bits para strings y enteros.
 *
 * Este archivo proporciona dos estructuras, HasherString64 y HasherInt64,
 * que actúan como functores para generar valores hash utilizando el algoritmo xxh64.
 * Están diseñadas para ser utilizadas con la clase TablaHash u otras estructuras de datos
 * que requieran funciones hash personalizadas.
 */

#ifndef HASHER_HPP
#define HASHER_HPP

#include <cstdint>
#include <string>
#include <stdexcept> // Incluido para std::runtime_error, aunque no se usa directamente aquí.

#include "xxh64.hpp"

/**
 * @struct HasherString64
 * @brief Functor para calcular el hash de 64 bits de un std::string usando xxh64.
 */
struct HasherString64 {
    /**
     * @brief Calcula el valor hash de 64 bits para una cadena dada.
     * @param k La cadena de entrada (std::string) para la cual calcular el hash.
     * @param seed Un valor opcional de semilla para el algoritmo hash. Por defecto es 0.
     * @return El valor hash de 64 bits calculado.
     * @note Esta función depende de la implementación de xxh64::hash.
     *       No lanza excepciones directamente, pero el comportamiento con cadenas inválidas
     *       o problemas de memoria en `k.data()` depende de `std::string` y `xxh64`.
     */
    uint64_t operator()(const std::string &k, uint64_t seed = 0) const {
        // k.data() podría teóricamente devolver nullptr si la cadena está en un estado inválido extremo,
        // pero std::string normalmente garantiza un puntero válido incluso para cadenas vacías (a un terminador nulo).
        // xxh64::hash debería manejar correctamente k.length() == 0.
        return xxh64::hash(k.data(), k.length(), seed);
    }
};

/**
 * @struct HasherInt64
 * @brief Functor para calcular el hash de 64 bits de un entero usando xxh64.
 */
struct HasherInt64 {
    /**
     * @brief Calcula el valor hash de 64 bits para un entero dado.
     * @param k El entero de entrada para el cual calcular el hash.
     * @param seed Un valor opcional de semilla para el algoritmo hash. Por defecto es 0.
     * @return El valor hash de 64 bits calculado.
     * @note Esta función depende de la implementación de xxh64::hash.
     *       Interpreta los bytes del entero como datos para el hash.
     */
    uint64_t operator()(int k, uint64_t seed = 0) const {
        // reinterpret_cast se usa para tratar los bytes del entero como una secuencia de caracteres.
        // Esto es una práctica común para funciones hash genéricas.
        // sizeof(k) asegura que se procese el número correcto de bytes para el tipo int.
        return xxh64::hash(reinterpret_cast<const char *>(&k), sizeof(k), seed);
    }
};

#endif //HASHER_HPP