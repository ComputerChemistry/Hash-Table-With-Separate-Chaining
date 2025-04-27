// TablaHash.cpp
#ifndef TABLA_HASH_CPP
#define TABLA_HASH_CPP

#include "../Headers/TablaHash.hpp"
#include <iostream>
#include <functional>

template <typename K, typename V>
TablaHash<K, V>::TablaHash(int tamInicial) : tamanio(tamInicial), elementosOcupados(0) {
    // Asegurar que el tamaño sea un número primo para reducir colisiones
    bool esPrimo = false;
    while (!esPrimo) {
        esPrimo = true;
        for (int i = 2; i <= tamInicial / 2; i++) {
            if (tamInicial % i == 0) {
                esPrimo = false;
                tamInicial++;
                break;
            }
        }
    }
    tamanio = tamInicial;
    tabla.resize(tamanio);
}

template <typename K, typename V>
int TablaHash<K, V>::funcionHash(const K& clave) const {
    return std::hash<K>{}(clave) % tamanio;
}

template <typename K, typename V>
int TablaHash<K, V>::funcionHash2(const K& clave) const {
    // Segundo hash para doble hashing
    // Usamos un número primo menor que tamanio para el salto
    int primo = tamanio - 2;
    return (primo - (std::hash<K>{}(clave) % primo));
}

template <typename K, typename V>
int TablaHash<K, V>::buscarIndice(const K& clave) const {
    const int indiceInicial = funcionHash(clave);
    const int salto = funcionHash2(clave);
    int indice = indiceInicial;
    int i = 0;

    while (i < tamanio) {
        if (tabla[indice].estado == VACIO) {
            return -1; // No se encontró
        }

        if (tabla[indice].estado == OCUPADO && tabla[indice].clave == clave) {
            return indice; // Se encontró
        }

        // Doble hashing para reducir agrupamientos
        i++;
        indice = (indiceInicial + i * salto) % tamanio;
    }

    return -1; // Tabla llena y no se encontró
}

template <typename K, typename V>
void TablaHash<K, V>::rehash() {
    std::vector<CeldaHash<K, V>> tablaTemporal = tabla;
    const int tamanioAnterior = tamanio;

    // Calcular nuevo tamaño primo
    tamanio = tamanio * 2 + 1;
    bool esPrimo = false;
    while (!esPrimo) {
        esPrimo = true;
        for (int i = 2; i <= tamanio / 2; i++) {
            if (tamanio % i == 0) {
                esPrimo = false;
                tamanio += 2;
                break;
            }
        }
    }

    tabla.clear();
    tabla.resize(tamanio);
    elementosOcupados = 0;

    // Reinsertar elementos
    for (int i = 0; i < tamanioAnterior; i++) {
        if (tablaTemporal[i].estado == OCUPADO) {
            insertar(tablaTemporal[i].clave, tablaTemporal[i].valor);
        }
    }
}

template <typename K, typename V>
bool TablaHash<K, V>::insertar(const K& clave, const V& valor) {
    // Verificar factor de carga
    if (const float factorCarga = static_cast<float>(elementosOcupados) / tamanio; factorCarga >= 0.7) {
        rehash();
    }

    const int indiceInicial = funcionHash(clave);
    const int salto = funcionHash2(clave);
    int indice = indiceInicial;
    int indiceBorrado = -1;
    int i = 0;

    while (i < tamanio) {
        if (tabla[indice].estado == VACIO) {
            // Encontró una celda vacía
            if (indiceBorrado != -1) {
                indice = indiceBorrado; // Usar la celda marcada como borrada
            }
            tabla[indice].clave = clave;
            tabla[indice].valor = valor;
            tabla[indice].estado = OCUPADO;
            elementosOcupados++;
            return true;
        } else if (tabla[indice].estado == BORRADO) {
            // Guardar el primer índice con estado BORRADO
            if (indiceBorrado == -1) {
                indiceBorrado = indice;
            }
        } else if (tabla[indice].clave == clave) {
            // Actualizar valor si la clave ya existe
            tabla[indice].valor = valor;
            return true;
        }

        // Doble hashing para el siguiente índice
        i++;
        indice = (indiceInicial + i * salto) % tamanio;
    }

    return false; // Tabla llena
}

template <typename K, typename V>
bool TablaHash<K, V>::buscar(const K& clave, V& valorSalida) const {
    int indice = buscarIndice(clave);
    if (indice != -1) {
        valorSalida = tabla[indice].valor;
        return true;
    }
    return false;
}

template <typename K, typename V>
bool TablaHash<K, V>::eliminar(const K& clave) {
    int indice = buscarIndice(clave);
    if (indice != -1) {
        tabla[indice].estado = BORRADO;
        return true;
    }
    return false;
}

template <typename K, typename V>
void TablaHash<K, V>::mostrar() const {
    std::cout << "\n===== TABLA HASH =====\n";
    std::cout << "Índice\tEstado\tClave\tValor\n";
    for (int i = 0; i < tamanio; i++) {
        std::cout << i << "\t";
        switch (tabla[i].estado) {
            case VACIO:
                std::cout << "VACÍO\t-\t-";
                break;
            case OCUPADO:
                std::cout << "OCUPADO\t" << tabla[i].clave << "\t" << tabla[i].valor;
                break;
            case BORRADO:
                std::cout << "BORRADO\t-\t-";
                break;
            default: ;
        }
        std::cout << std::endl;
    }
    std::cout << "=====================\n";
    std::cout << "Elementos ocupados: " << elementosOcupados << "/" << tamanio << std::endl;
    std::cout << "Factor de carga: " << static_cast<float>(elementosOcupados) / tamanio << std::endl;
}

template <typename K, typename V>
int TablaHash<K, V>::obtenerTamanio() const {
    return tamanio;
}

template <typename K, typename V>
int TablaHash<K, V>::obtenerElementosOcupados() const {
    return elementosOcupados;
}

#endif // TABLA_HASH_CPP
