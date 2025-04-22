// TablaHash.hpp
#ifndef TABLA_HASH_HPP
#define TABLA_HASH_HPP

#include <vector>
#include <string>

enum Estado {
    VACIO,
    OCUPADO,
    BORRADO
};

template <typename K, typename V>
class CeldaHash {
public:
    K clave;
    V valor;
    Estado estado;

    CeldaHash() : estado(VACIO) {}
};

template <typename K, typename V>
class TablaHash {
private:
    std::vector<CeldaHash<K, V>> tabla;
    int tamanio;
    int elementosOcupados;

    // Método para calcular el hash principal
    int funcionHash(const K& clave) const;

    // Método para calcular el salto (para doble hashing)
    int funcionHash2(const K& clave) const;

    // Método para buscar el índice de una clave
    int buscarIndice(const K& clave) const;

    // Método para redimensionar la tabla
    void rehash();

public:
    explicit TablaHash(int tamInicial = 10);

    bool insertar(const K& clave, const V& valor);
    bool buscar(const K& clave, V& valorSalida) const;
    bool eliminar(const K& clave);
    void mostrar() const;
    [[nodiscard]] int obtenerTamanio() const;
    [[nodiscard]] int obtenerElementosOcupados() const;
};

// Incluimos la implementación de la plantilla
#include "TablaHash.cpp"

#endif // TABLA_HASH_HPP